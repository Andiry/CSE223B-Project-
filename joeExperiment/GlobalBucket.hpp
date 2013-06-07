#ifndef GLOBALS_HPP_INC
#define GLOBALS_HPP_INC 1

#include "DFS.h"
#include "Host.hpp"
#include "LockSet.hpp"

#include <chrono>
#include <random>
#include <ostream>

#include <sys/stat.h>

typedef std::map<DFS::HostID, Host> HostMap_t;

struct DummyStream : std::ostream {
    //operator<<
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

    DummyStream debug_;
    //std::ostream& debug_;
    

    // fi->fh stores the randomized key
    //
    // In open, create and opendir
    //fi->fh = randVal;
    //uint64_t& fh(globals_->fhMap_[fi->fh]);

    // All other calls
    //uint64_t& fh(globals_->fhMap_[fi->fh]);
    //call
   
    GlobalBucket(const std::string& hostname, int16_t port, void (*killall)(void))
        : killall_(killall), joinLock_(false), joinMaster_(false) {
        //: killall_(killall), joinLock_(false), joinMaster_(false), debug_(std::cerr) {
        me_.hostname = hostname;
        me_.port     = port;
        randGen_.seed(std::chrono::system_clock::now().time_since_epoch().count());
        pthread_mutex_init(&hostLock_, NULL);
    }
};

#endif
