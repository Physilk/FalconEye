#pragma once

#include <thread>
#include <vector>
#include <atomic>
#include <string>
#include "JobQueue.h"

using std::thread;
using std::vector;
using std::atomic;
using std::string;

namespace Threading
{

typedef std::shared_ptr<atomic<bool>> AtomicBool_ptr;
class TThreadManager
{
private:
    TJobQueue FJobQueue;
    vector<AtomicBool_ptr> FStopThreadAtomics;
    vector<thread> FThreads;

    bool FInitialized;
    bool FFinishAllJobBeforeExiting;
public:
    TThreadManager()
    : FJobQueue()
    , FStopThreadAtomics()
    , FThreads()
    , FInitialized(false)
    , FFinishAllJobBeforeExiting(false)
    {}

    TThreadManager(const TThreadManager &) = delete;
    TThreadManager(TThreadManager &&) = delete;
    ~TThreadManager();

    void Init();
    void ShutDown();

    void SetFinishAllJobBeforeExiting(bool parBool) { FFinishAllJobBeforeExiting = parBool; }

	void AddJob(TJob_ptr job) { FJobQueue.AddJob(job); }
    //void AddDownloadJob(TDownloadJob_ptr job) { FJobQueue.AddJob(job); }
    //void AddDownloadJob(const string &parLink, const string &parFilename) { FJobQueue.AddJob(make_DownloadJob(parLink, parFilename)); }
};

}//END namespace Threading
