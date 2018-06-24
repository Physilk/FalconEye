#include "WorkerThreadRunnable.h"
#include "ThreadManager.h"

namespace Threading
{

void TWorkerThreadRunnable::operator()()
{
    std::unique_lock<mutex> lck(threadManager->mtx);
    TJob_ptr job;
    while (job.get() == nullptr && !threadManager->FStopThreadBools[threadNumber])
    {
        job = threadManager->FJobQueue.GetNextJob();
        //no current job, we wait
        if (job == nullptr)
        {
            if (!lck.owns_lock())
            {
                lck.lock();
            }
            threadManager->wait_variable.wait(lck);
            job = threadManager->FJobQueue.GetNextJob();
            lck.unlock();
        }
        if (job.get())
        {
            job->Run();
            job = nullptr;
        }
    }
    

}

}//END namespace Threading
