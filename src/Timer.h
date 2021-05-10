#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <chrono>
#include <cassert>

namespace lcx{

using TimeoutCallback = std::function<void()>;
using MS = std::chrono::milliseconds;//时间段使用的单位
using Clock = std::chrono::high_resolution_clock;//当前时钟
using TimeStamp = Clock::time_point;//时间点

class HttpRequest;

class Timer
{
public:
    Timer(const TimeStamp& when, const TimeoutCallback& cb) : 
        mExpireTime(when), mTimeCallback(cb), mDel(false) {}
    ~Timer(){}

    inline void del() { mDel = true; }
    inline bool isDeleted() { return mDel; }
    inline TimeStamp getExpireTime() { return mExpireTime; }
    void runCallback() { mTimeCallback(); }
private:
    bool mDel; //是否删除这个时间点
    TimeoutCallback mTimeCallback;
    TimeStamp mExpireTime; //超时时间
};

// 比较函数，用于priority_queue，时间值最小的在队头
struct TimerCmp
{
    bool operator() (Timer *a, Timer *b) const{
        assert(a != nullptr && b != nullptr);
        return a->getExpireTime() > b->getExpireTime();
    }
};

class TimerManager
{
public:
    TimerManager() : mNowTime(Clock::now()) {}
    ~TimerManager() {}

    inline void updateTime(){ mNowTime = Clock::now(); }

    //  常量指针，不能改变内容
    void addTimer(HttpRequest *req, const int &timeout, const TimeoutCallback &cb);
    void delTimer(HttpRequest *req);
    void dealExpireTimer();
    int getNextExpireTime(); // 返回超时时间(优先队列中最早超时时间和当前时间差)

private:

    using TimerQueue = std::priority_queue<Timer*, std::vector<Timer*>, TimerCmp>;
    TimerQueue mTimerQueue;

    TimeStamp mNowTime;
    std::mutex mLock;
};
 
} // namespace lcx
