#pragma once

#include <thread>
#include <vector>
#include <atomic>
#include <string>
#include <condition_variable>
#include "JobQueue.h"

using std::thread;
using std::vector;
using std::atomic;
using std::string;
using std::mutex;
using std::condition_variable;

namespace Threading
{

typedef std::shared_ptr<atomic<bool>> AtomicBool_ptr;
class TThreadManager
{
private:
    TJobQueue FJobQueue;
    vector<bool> FStopThreadBools;
    vector<thread> FThreads;
    mutex mtx;
    condition_variable wait_variable;

    bool FInitialized;
    bool FFinishAllJobBeforeExiting;

    friend class TWorkerThreadRunnable;
public:
    TThreadManager()
    : FJobQueue()
    , FStopThreadBools()
    , FThreads()
    , FInitialized(false)
    , FFinishAllJobBeforeExiting(true)
    {
        Init();
    }

    TThreadManager(const TThreadManager &) = delete;
    TThreadManager(TThreadManager &&) = delete;
    ~TThreadManager();

protected:
    void Init();
    void ShutDown();

public:
    void SetFinishAllJobBeforeExiting(bool parBool) { FFinishAllJobBeforeExiting = parBool; }

    void AddJob(TJob_ptr job);
};

}//END namespace Threading
