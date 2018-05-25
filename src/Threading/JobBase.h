#pragma once

#include <atomic>
#include <memory>

using std::atomic;

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
    TJobBase() : FStatus(EJobStatus::None) {}
    TJobBase(const TJobBase &) = default;
    TJobBase(TJobBase &&) = default;
    ~TJobBase() = default;
    
    atomic<EJobStatus>  FStatus;
    
    virtual int VirtualExecute() = 0;
public:
    
    EJobStatus Run();
};

typedef  std::shared_ptr<TJobBase> TJob_ptr;


}//END namespace Threading
