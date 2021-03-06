#include "JobBase.h"

namespace Threading
{

EJobStatus TJobBase::Run()
{
    if (!(FStatus.load() & (EJobStatus::Finished | EJobStatus::Started) ))
    {
        FStatus.store(EJobStatus::Started);
        int ret = VirtualExecute();
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
