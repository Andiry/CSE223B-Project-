#ifndef GLOBALS_HPP_INC
#define GLOBALS_HPP_INC 1

#include "DFS.h"
#include "Host.hpp"
#include "LockSet.hpp"

typedef std::map<DFS::HostID, Host> HostMap_t;

struct GlobalBucket {
    LockSet locks_;
    DFS::HostID me_;
    HostMap_t hostMap_;
    pthread_mutex_t hostLock_;
    void (*killall_)(void);
   
    GlobalBucket(const std::string& hostname, int16_t port, void (*killall)(void)) {
        me_.hostname = hostname;
        me_.port     = port;
        killall_     = killall;
        pthread_mutex_init(&hostLock_, NULL);
    }
};

#endif
