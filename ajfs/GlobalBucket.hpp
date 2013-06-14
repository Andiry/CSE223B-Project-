#ifndef GLOBALS_HPP_INC
#define GLOBALS_HPP_INC 1

#include "DFS.h"
#include "Host.hpp"
#include "LockSet.hpp"

#include <chrono>
#include <random>
#include <ostream>

extern "C" {
    #include <sys/stat.h>
    #include <semaphore.h>
}

typedef std::map<DFS::HostID, Host> HostMap_t;

class DebugStream : public std::ostream {

};

struct GlobalBucket {
    LockSet locks_;
    DFS::HostID me_;
    HostMap_t hostMap_;
    pthread_mutex_t hostLock_;
    void (*killall_)(void);
    std::mt19937_64 randGen_;

    std::string backupPath_;
    bool joinLock_;
    DFS::HostID joining_;
    bool joinMaster_;

    std::map<uint64_t, uint64_t> fhMap_;

    //DebugStream debug_;
    std::ostream& debug_;
    pthread_cond_t joinCond_;
    pthread_mutex_t joinMutex_;
    // TODO - use these for blocking writes on joinLock
    // TODO - make sure that if requestJoinLock() is not called, but addServer
    //        IS, that the server will be killed off!

    GlobalBucket(const std::string& hostname, int16_t port, void (*killall)(void))
        //: killall_(killall), joinLock_(false), joinMaster_(false) {
        : killall_(killall), joinLock_(false), joinMaster_(false), debug_(std::cerr) {
        me_.hostname = hostname;
        me_.port     = port;
        randGen_.seed(std::chrono::system_clock::now().time_since_epoch().count());
        pthread_mutex_init(&hostLock_, NULL);
    }
};

#endif
