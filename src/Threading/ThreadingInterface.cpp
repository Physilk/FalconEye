#include "ThreadingInterface.h"
#include "Threading/ThreadManager.h"

namespace FalconEye {

    namespace ThreadingInterface
    {
        static Threading::TThreadManager threadManager;

        void AddJob(Threading::TJob_ptr job)
        {
            threadManager.AddJob(job);
        }
    }    
}