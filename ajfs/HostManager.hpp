/* AJFS - a simple distributed file system.
 *
 * By Joe DeBlasio and Andiry Xu
 * For UCSD CSE223B, SP13
 * */
#ifndef HOSTMANAGER_HPP_INC
#define HOSTMANAGER_HPP_INC 1

#include <map>

extern "C" {
    #include <pthread.h>
    #include <time.h>
}

#include "GlobalBucket.hpp"
#include "Host.hpp"

namespace HostManager {
    const time_t    SLEEP_SECONDS           = 2;
    const long      SLEEP_NANOSECONDS       = 500000000L;
    const unsigned  JOIN_LOCK_COUNT_THRESH  = 4; // 10 seconds

    void * start(void * globals);
    void stop();
}

#endif
