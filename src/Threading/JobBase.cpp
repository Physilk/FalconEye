#include "JobBase.h"

namespace Threading
{

EJobStatus TJobBase::Run()
{
    if (!(FStatus.load() & (EJobStatus::Finished | EJobStatus::Started) ))
    {
        FStatus.store(EJobStatus::Started);
        int ret_init = Init();
        if (ret_init)
        {
            FStatus.store(EJobStatus::Finished_error);
            FEndPromise.set_value();
            return FStatus;
        }
        
        auto begin = std::chrono::high_resolution_clock::now(); 
        int ret = VirtualExecute();
        FExecutionTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin).count();
        EJobStatus ret_status = EJobStatus::Finished;
        if (ret)
        {
            ret_status = EJobStatus::Finished_error;
        }
        FStatus.store(ret_status);
        FEndPromise.set_value();
    }

    return FStatus;

}

void TJobBase::WaitFinish()
{
    FEndFuture.wait();
}

}//END namespace Threading
