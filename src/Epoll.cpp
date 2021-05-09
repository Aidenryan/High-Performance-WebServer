#include "Epoll.h"
#include "ThreadPool.h"
#include "HttpRequest.h"
#include <assert.h>
#include <unistd.h> //close
#include <string.h> //strerror
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
    int eventsNum = ::epoll_wait(mEpollFd, &*mEvents.begin(), static_cast<int>(mEvents.size()), timeOutMs);
    if(eventsNum < 0)
    {
        printf("[Epoll::wait] epoll : %s\n", strerror(errno));
    }
    else if (eventsNum == 0)
    {
        //没有事件发生
    }
    
    return eventsNum;
} 

void Epoll::dealEvent(int listenfd, std::shared_ptr<ThreadPool> &threadpool, int eventsNum)
{
    assert(eventsNum > 0);

    for(int i(0); i < eventsNum; ++i)
    {
        auto req = static_cast<HttpRequest*>(mEvents[i].data.ptr);
        int fd = req->fd();

        if(fd == listenfd)
        {
            mNewConnection();
        }
        else
        {
            //排除错误事件                                      读写端都被关闭
            if((mEvents[i].events & EPOLLERR) || (mEvents[i].events & EPOLLHUP) || (! mEvents[i].events & EPOLLIN))
            {
                req->setNoWorking();
                mCloseConnection(req);
            }
            else if(mEvents[i].events & EPOLLIN) //读事件
            {
                req->setWorking();
                threadpool->pushJob(std::bind(mRequest, req));
            }
            else if(mEvents[i].events & EPOLLOUT) //写事件
            {
                req->setWorking();
                threadpool->pushJob(std::bind(mResponse, req));
            }
            else    //不知道上面事件
            {
                printf("[Epoll::handleEvent] unexpected event\n");
            }
            
        }
        
    }

    return;
}

