#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <sys/epoll.h>

namespace lcx{

class HttpRequest;


class Epoll
{
public:
    Epoll();
    ~Epoll();
    int add(int fd, HttpRequest* req, int events);
    int del(int fd, HttpRequest* req, int events);
    int mod(int fd, HttpRequest* req, int events);
    int wait(int timeOutMs);

private:

    using EventList = std::vector<epoll_event>;

    int mEpollFd;
    EventList mEvents;
    
};



} // namespace lcx
