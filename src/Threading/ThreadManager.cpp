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

        for(unsigned int i = 0; i < thread_number; ++i)
        {
            FStopThreadAtomics.push_back(AtomicBool_ptr(new atomic<bool>(false)));
            TWorkerThreadRunnable runnable = TWorkerThreadRunnable(&FJobQueue, FStopThreadAtomics.back().get());
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
			FStopThreadAtomics[i]->store(true);
			if (FThreads[i].joinable())
			{
				FThreads[i].join();
			}
        }
        FThreads.empty();
        FStopThreadAtomics.empty();
        FInitialized = false;
    }
}

TThreadManager::~TThreadManager()
{
    ShutDown();
}

}//END namespace Threading
