#include "Timer.h"
#include "HttpRequest.h"

using namespace lcx;

void TimerManager::addTimer(HttpRequest *req, const int &timeout, const TimeoutCallback &cb)
{
    std::unique_lock<std::mutex> lock(mLock);
    assert(req != nullptr);

    updateTime();
    auto timer = new Timer(mNowTime + MS(timeout), cb);
    mTimerQueue.emplace(timer);

    // 对同一个request连续调用两次addTimer，需要把前一个定时器删除
    if(req->getTimer() != nullptr)
        delTimer(req);
    
    req->setTimer(timer);
}

// 这个函数不必上锁，没有线程安全问题
// 若上锁，反而会因为连续两次上锁造成死锁：dealExpireTimer -> runCallBack -> closeConnection -> delTimer
void TimerManager::delTimer(HttpRequest *req)
{
    assert(req != nullptr);

    auto timer = req->getTimer();
    if(timer == nullptr)
        return;

    // 如果这里写成delete timer，会使priority_queue里的对应指针变成垂悬指针 悬挂指针是由于：指针的指向内存被删除导致
    // 正确的方法是惰性删除
    timer->del();
    req->setTimer(nullptr);

}

void TimerManager::dealExpireTimer()
{
    std::unique_lock<std::mutex> lock(mLock);
    updateTime();

    while(!mTimerQueue.empty())
    {
        auto timer = mTimerQueue.top();
        assert(timer != nullptr);
        if(timer->isDeleted())
        {
            mTimerQueue.pop();
            delete timer;
            continue;
        }

        // 优先队列头部的定时器也没有超时，return
        if(std::chrono::duration_cast<MS>(timer->getExpireTime() - mNowTime).count() > 0 )
        {
            return;
        }

        //超时了
        timer->runCallback();
        mTimerQueue.pop();
        delete timer;
    }
}

// 返回超时时间(优先队列中最早超时时间和当前时间差)
int TimerManager::getNextExpireTime()
{
    std::unique_lock<std::mutex> lock(mLock);
    updateTime();
    int res = -1;

    while (!mTimerQueue.empty())
    {
        auto timer = mTimerQueue.top();
        assert(timer != nullptr);
        if(timer->isDeleted())
        {
            mTimerQueue.pop();
            delete timer;
            continue;
        }

        res = std::chrono::duration_cast<MS>(timer->getExpireTime() - mNowTime).count();
        res = res < 0 ? 0 : res;
        break;
    }

    return res;
    
}