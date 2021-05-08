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
}