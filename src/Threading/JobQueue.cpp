#include "JobQueue.h"

namespace Threading
{

void TJobQueue::AddJob(TJob_ptr job)
{
    FQueueAccessMutex.lock();
    FJobQueue.push(job);
    FQueueAccessMutex.unlock();
}

TJob_ptr TJobQueue::GetNextJob()
{
    TJob_ptr ret = TJob_ptr(nullptr);
    
    FQueueAccessMutex.lock();
    if (FJobQueue.size() > 0)
    {
        ret = FJobQueue.front();
        FJobQueue.pop();
    }
    FQueueAccessMutex.unlock();
    
    return ret;
}


}//END namespace Threading
