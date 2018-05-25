#pragma once

#include <queue>
#include <mutex>
#include "JobBase.h"

using std::queue;
using std::mutex;

namespace Threading
{

class TJobQueue
{
private:
    queue<TJob_ptr> FJobQueue;
    mutex FQueueAccessMutex;

public:
    TJobQueue() = default;
    ~TJobQueue() = default;

    void AddJob(TJob_ptr job);
    TJob_ptr GetNextJob();

    size_t Size() { return FJobQueue.size(); }
};

}//END namespace Threading
