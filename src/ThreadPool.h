#pragma once

#include <mutex>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>

namespace lcx{

class ThreadPool{
public:
    using JobFunction = std::function<void()>;

    ThreadPool(int numWokers);
    ~ThreadPool();

    void pushJob(const JobFunction &job);
private:

    bool mStop;
    std::vector<std::thread> mThreads; //线程池中线程数目
    std::mutex mLock;       // 互斥锁
    std::condition_variable mCond;  //条件变量
    std::queue<JobFunction> mJobs;  //任务

};


} // namespace lcx

