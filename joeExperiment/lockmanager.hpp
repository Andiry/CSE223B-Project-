#ifndef LOCKMANAGER_HPP_INC
#define LOCKMANAGER_HPP_INC 1

#include <map>
#include <pthread.h>
#include <time.h>

#include "Host.hpp"

namespace LockManager {
    const time_t    SLEEP_SECONDS       = 0;
    const long      SLEEP_NANOSECONDS   = 500000000L;

    void * start(void * lockManagerArgs);

    struct LMArgs {
        Host & me_;
        HostMap_t & hostMap_;
        pthread_mutex_t & hostLock_;
        bool & dead_;
    
        LMArgs(Host& me, HostMap_t& hostMap, pthread_mutex_t& hostLock, bool& dead)
            : me_(me), hostMap_(hostMap), hostLock_(hostLock), dead_(dead) { }
    };
}


#endif
