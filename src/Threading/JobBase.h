#pragma once

#include <atomic>
#include <memory>
#include <future>

using std::atomic;
using std::promise;
using std::future;

namespace Threading
{

enum EJobStatus
{
    None            = 0
    ,Started        = 1     //0001
    ,Finished       = 2     //0010
    ,Finished_error = 4     //0110
};

class TJobBase
{
protected:
    TJobBase() : FStatus(EJobStatus::None) { FEndFuture = FEndPromise.get_future(); }
    TJobBase(const TJobBase &) = default;
    TJobBase(TJobBase &&) = default;
public:
    virtual ~TJobBase() = default;

private:
    atomic<EJobStatus>  FStatus;
    promise<void> FEndPromise;
    future<void> FEndFuture;
    
    float FExecutionTime;

    virtual int Init() { return 0; };
    virtual int VirtualExecute() = 0;
public:
    
    EJobStatus Run();
    
    void WaitFinish();
    EJobStatus GetStatus() const { return FStatus.load(); }
    float GetExecutionTime() const { return FExecutionTime; }
};

typedef  std::shared_ptr<TJobBase> TJob_ptr;


}//END namespace Threading
