#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <sys/epoll.h>

#define MAXEVENTS 1024

namespace lcx{

class HttpRequest;

class Epoll
{
public:
    using NewConnectionCallback = std::function<void()>;
    using CloseConnectionCallback = std::function<void(HttpRequest*)>;
    using DealRequestCallback = std::function<void(HttpRequest*)>;
    using DealResponseCallback = std::function<void(HttpRequest*)>;

    Epoll();
    ~Epoll();
    int add(int fd, HttpRequest* req, int events); //注册文件描述符
    int del(int fd, HttpRequest* req, int events);  //删除文件描述符
    int mod(int fd, HttpRequest* req, int events);  //修改文件描述符
    int wait(int timeOutMs);    //等待事件发生

private:
    using EventVector = std::vector<epoll_event>;

    int mEpollFd;
    EventVector mEvents;

    NewConnectionCallback mNewConnection;
    CloseConnectionCallback mCloseConnection;
    DealRequestCallback mRequest;
    DealResponseCallback mResponse;
};



} // namespace lcx
