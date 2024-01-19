#include "ThreadManager.h"
#include "WorkerThreadRunnable.h"

#ifdef DEBUG
#include <iostream>
#endif

namespace Threading
{

void TThreadManager::Init()
{
    if (!FInitialized)
    {
        unsigned int thread_number = thread::hardware_concurrency();
        if (thread_number == 0)
        {
            thread_number = 2; //arbitraire
        }
        #ifdef DEBUG
        std::cout << "Initializing "<< thread_number << " threads\n";
        #endif
        //override 
        thread_number = 25;
        for(unsigned int i = 0; i < thread_number; ++i)
        {
            FStopThreadBools.push_back(false);
            TWorkerThreadRunnable runnable = TWorkerThreadRunnable(this, i);
            FThreads.emplace_back(runnable);
        }
        FInitialized = true;
    }
}

void TThreadManager::ShutDown()
{
    if (FInitialized)
    {
        for (unsigned int i = 0; i < FThreads.size(); ++i)
        {
			if (FFinishAllJobBeforeExiting)
			{
				while (FJobQueue.Size())
				{
					std::this_thread::yield();
				}
			}
            FStopThreadBools[i] = true;
            wait_variable.notify_all();
			if (FThreads[i].joinable())
			{
				FThreads[i].join();
			}
        }
        FThreads.empty();
        FStopThreadBools.empty();
        FInitialized = false;
    }
}

void TThreadManager::AddJob(TJob_ptr job)
{
    std::unique_lock<std::mutex> lck(mtx);
    FJobQueue.AddJob(job);
    wait_variable.notify_one();
}

TThreadManager::~TThreadManager()
{
    ShutDown();
}

}//END namespace Threading
