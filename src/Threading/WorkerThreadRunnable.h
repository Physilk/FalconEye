#pragma once

#include <atomic>
#include "JobQueue.h"

using std::atomic;

namespace Threading
{

class TWorkerThreadRunnable
{    
private:
    TJobQueue *FJobQueue;
    atomic<bool> *FStopWhenPossible;
    
public:
    TWorkerThreadRunnable() = delete;
    TWorkerThreadRunnable(TJobQueue *parJobQueue, atomic<bool> *parStopAtomic)
    : FJobQueue(parJobQueue)
    , FStopWhenPossible(parStopAtomic)
    {}
    
    ~TWorkerThreadRunnable() = default;
    
    void operator()();
};

}//END namespace Threading
