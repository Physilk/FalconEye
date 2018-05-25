#include "WorkerThreadRunnable.h"

namespace Threading
{

void TWorkerThreadRunnable::operator()()
{
    while(!FStopWhenPossible->load())
    {
        TJob_ptr job = FJobQueue->GetNextJob();

        if (job.get())
        {
            job->Run();
        }
    }
}

}//END namespace Threading
