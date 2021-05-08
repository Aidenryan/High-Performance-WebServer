#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <sys/epoll.h>

#define MAXEVENTS 1024

namespace lcx{

class HttpRequest;
class ThreadPool;

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
    void dealEvent(int listenfd, std::shared_ptr<ThreadPool> &threadpool, int eventsNum);
    inline void setConnection(const NewConnectionCallback &cb){ mNewConnection = cb;}
    inline void setCloseConnection(const CloseConnectionCallback &cb){ mCloseConnection = cb;}
    inline void setRequest(const DealRequestCallback &cb){ mRequest = cb;}
    inline void setResponse(const DealResponseCallback &cb){ mResponse = cb;}

private:
    int mEpollFd;

    using EventVector = std::vector<epoll_event>;
    EventVector mEvents;

    NewConnectionCallback mNewConnection;
    CloseConnectionCallback mCloseConnection;
    DealRequestCallback mRequest;
    DealResponseCallback mResponse;
};



} // namespace lcx
