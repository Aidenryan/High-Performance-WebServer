#include "HttpServer.h"
#include "ThreadPool.h"
#include "HttpRequest.h"
#include "Epoll.h"
#include "Utils.h"
#include "ThreadPool.h"
#include "HttpResponse.h"

#include <assert.h>
#include <sys/socket.h> //accept
#include <string.h>

using namespace lcx;

//注意智能指针的初始化方法
HttpServer::HttpServer(int port, int numThread)
    : mPort(port), 
    mThreadPool(new ThreadPool(numThread)), 
    mListenFd(Utils::createListenFd(port)),
    mListenRequst(new HttpRequest(mListenFd)), 
    mEpoll(new Epoll()) 
    //TODO
{
    assert(mListenFd >= 0);
}

//分析构函数一定要放这里，如果定义在.h会导致智能指针在析构时候找不到内存大小，这个和前置声明有关系
HttpServer::~HttpServer()
{
}

void HttpServer::run()
{
    /*使用function 对函数进行包装时候
      类普通成员函数比较特殊，需要使用bind函数，并且需要实例化对象，成员函数要加取地址符
    */
    //注册监听文件描述符到epoll,设定为可读事件，ET模式
    mEpoll->add(mListenFd, mListenRequst.get(), (EPOLLIN | EPOLLET));
    //注册新连接回调函数
    mEpoll->setConnection(std::bind(&HttpServer::acceptNewConnection, this));
    //注册连接关闭回调函数
    mEpoll->setCloseConnection(std::bind(&HttpServer::closeConnection, this, std::placeholders::_1));
    //注册请求处理回调函数
    mEpoll->setRequest(std::bind(&HttpServer::dealRequest, this, std::placeholders::_1));
    //注册响应处理回调函数
    mEpoll->setResponse(std::bind(&HttpServer::dealResponse, this, std::placeholders::_1));

    //时间循环监听
    while(1)
    {
        //超时时间
        int timeMs = 100; //TODO

        //等待事件发生
        int eventsNum = mEpoll->wait(timeMs);

        if(eventsNum > 0)
        {
            // 分发事件处理函数
            mEpoll->dealEvent(mListenFd, mThreadPool, eventsNum);
        }

        //TODO
    }
    
}

//ET模式
void HttpServer::acceptNewConnection()
{
    while (1)
    {
        //accept4()函数共有4个参数，相比accept()多了一个flags的参数，用户可以通过此参数直接设置套接字的一些属性，如SOCK_NONBLOCK或者是SOCK_CLOEXEC
        int acceptFd = ::accept4(mListenFd, nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC);
        if(acceptFd == -1)
        {
            if(errno == EAGAIN)
                break;
            
            printf("[HttpServer::__acceptConnection] accept : %s\n", strerror(errno));
            break;
        }

        //为新的连接分配资源
        HttpRequest* httpReq = new HttpRequest(acceptFd);
        //TODO
        // 注册连接套接字到epoll（可读，边缘触发，保证任一时刻只被一个线程处理）
        mEpoll->add(acceptFd, httpReq, (EPOLLIN | EPOLLONESHOT));
    }
    
}
void HttpServer::closeConnection(HttpRequest *req)
{
    int fd = req->fd();
    if(req->isWorking())
    {
        return;
    }
    //TODO
    mEpoll->del(fd, req, 0);

    // 释放该套接字占用的HttpRequest资源，在HttpRequest析构函数中close(fd)
    delete req;
    req = nullptr;

}

//LT模式
void HttpServer::dealRequest(HttpRequest *req)
{
    //TODO
    assert(req != nullptr);
    int fd = req->fd();

    int readErrno;
    int nRead = req->reqRead(&readErrno);

    //客户端断开连接
    if(nRead == 0)
    {
        req->setNoWorking();
        closeConnection(req);
        return;
    }

    // 非EAGAIN错误，断开连接
    if(nRead < 0 && readErrno != EAGAIN)
    {
        req->setNoWorking();
        closeConnection(req);
        return;
    }

    // EAGAIN错误则释放线程使用权，并监听下次可读事件epoll_ -> mod(...)
    if(nRead < 0 && readErrno == EAGAIN)
    {
        req->setNoWorking();
        mEpoll->mod(fd, req, EPOLLIN | EPOLLONESHOT);
        //Todo
        return;
    }

    //如果解析报文出错
    if(!req->parseRequest())
    {
        //发送400报文
        HttpResponse response(400, "", false);
        req->appendOutBuffer(response.makeResponse());

        //关闭连接
        int writeErrno;
        req->reqWrite(&writeErrno);
        req->setNoWorking();
        closeConnection(req);
        return;
    }

    //解析完成
    if(req->finishParse())
    {
        HttpResponse response(200, req->getPath(), req->keepAlive());
        req->appendOutBuffer(response.makeResponse());
        mEpoll->mod(fd, req, (EPOLLIN | EPOLLOUT | EPOLLONESHOT));
    }
}

//LT模式
void HttpServer::dealResponse(HttpRequest *req)
{
    //TODO
    assert(req != nullptr);

    int fd = req->fd();
    int toWrite = req->writeableBytes();

    if(toWrite == 0)
    {
        req->setNoWorking();
        mEpoll->mod(fd, req, (EPOLLIN | EPOLLONESHOT));
        //TODO
        return;
    }

    int writeErrno;
    int ret = req->reqWrite(&writeErrno);

    if(ret < 0 )
    {
        if(writeErrno == EAGAIN)
        {
            mEpoll->mod(fd, req, (EPOLLIN | EPOLLONESHOT | EPOLLOUT));
        }
        else if(writeErrno != EAGAIN) // 非EAGAIN错误，断开连接
        {
            req->setNoWorking();
            closeConnection(req);
        }
        return;
    }

    if(ret == toWrite)
    {
        if(req->keepAlive()) //长1连接
        {
            req->resetParse();
            req->setNoWorking();
            mEpoll->mod(fd, req, (EPOLLIN | EPOLLONESHOT));
            //TODO
        }
        else
        {
            req->setNoWorking();
            closeConnection(req);
        }
        return;
    }

    mEpoll->mod(fd, req, (EPOLLIN | EPOLLONESHOT | EPOLLOUT));
    req->setNoWorking();    //是因为LT模式，所以设置为不工作了吗？我感觉这里是没有写完呀
    //TODO
    return;

}