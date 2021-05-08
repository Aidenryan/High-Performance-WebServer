#include "Epoll.h"
#include "ThreadPool.h"

#include <assert.h>
#include <unistd.h> //close
using namespace lcx;

Epoll::Epoll() //::全局调用  EPOLL_CLOEXEC：该文件描述符在子进程复制后关闭
    :mEpollFd(::epoll_create1(EPOLL_CLOEXEC)), mEvents(MAXEVENTS)
{
    assert(mEpollFd >= 0);
}

Epoll::~Epoll()
{
    ::close(mEpollFd);
}

int Epoll::add(int fd, HttpRequest* req, int events)
{
    epoll_event event;
    event.data.ptr = static_cast<void*> (req);
    event.events = events;

    int ret = ::epoll_ctl(mEpollFd, EPOLL_CTL_ADD, fd, &event);
    return ret;
}

int Epoll::del(int fd, HttpRequest* req, int events)
{
    epoll_event event;
    event.data.ptr = static_cast<void*> (req);
    event.events = events;

    int ret = ::epoll_ctl(mEpollFd, EPOLL_CTL_DEL, fd, &event);
    return ret;
}

int Epoll::mod(int fd, HttpRequest* req, int events)
{
    epoll_event event;
    event.data.ptr = static_cast<void*> (req);
    event.events = events;

    int ret = ::epoll_ctl(mEpollFd, EPOLL_CTL_MOD, fd, &event);
    return ret;
}

int Epoll::wait(int timeOutMs)   
{

} 

void Epoll::dealEvent(int listenfd, std::shared_ptr<ThreadPool> &threadpool, int eventsNum)
{

}

