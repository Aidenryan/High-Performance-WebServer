#include "HttpServer.h"
#include "ThreadPool.h"
#include "HttpRequest.h"
#include "Epoll.h"
#include "Utils.h"

#include <assert.h>
using namespace lcx;

//注意智能指针的初始化方法
HttpServer::HttpServer(int port, int numThread)
    : mPort(port), mThreadPool(new ThreadPool(numThread)), mListenFd(Utils::createListenFd(port)),
    mListenRequst(new HttpRequest(mListenFd)), mEpoll(new Epoll())
{
    assert(mListenFd >= 0);
}

//分析构函数一定要放这里，如果定义在.h会导致智能指针在析构时候找不到内存大小，这个和前置声明有关系
HttpServer::~HttpServer()
{

}

void HttpServer::run()
{
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
        int timeMs = 1; //TODO

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