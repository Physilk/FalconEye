#pragma once

#include "Threading/ThreadManager.h"
#include "Threading/JobBase.h"

namespace FalconEye {

    namespace ThreadingInterface
    {
        void AddJob(Threading::TJob_ptr job);
    }
} // end namespace FalconEye