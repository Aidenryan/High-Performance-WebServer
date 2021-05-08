#include "ThreadPool.h"

using namespace lcx;

ThreadPool::ThreadPool(int numWokers) : mStop(false)
{
    numWokers = (numWokers <= 0) ? 1 : numWokers;//以防设置错误的线程数

    for(int i(0); i < numWokers; ++i)
    {
        mThreads.emplace_back([this]()
        {
            while (1)
            {
                JobFunction func;
                {
                    std::unique_lock<std::mutex> lock(mLock);
                    while (!mStop && mJobs.empty())
                        mCond.wait(lock);
                    
                    if(mStop && mJobs.empty())
                        return;
                    
                    func = mJobs.front();
                    mJobs.pop();
                }

                //如果函数不为空，就执行
                if(func)
                {
                    func();
                }

            }
            
        });
    }
}

ThreadPool::~ThreadPool()
{

}