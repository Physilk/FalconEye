#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>
#include "JobQueue.h"

using std::atomic;

namespace Threading
{

class TThreadManager;

class TWorkerThreadRunnable
{    
private:
    TThreadManager *threadManager;
    unsigned int threadNumber;
public:
    TWorkerThreadRunnable() = delete;
    TWorkerThreadRunnable(TThreadManager *tm, unsigned int nb)
    : threadManager(tm)
    , threadNumber(nb)
    {}
    
    ~TWorkerThreadRunnable() = default;
    
    void operator()();
};

}//END namespace Threading
