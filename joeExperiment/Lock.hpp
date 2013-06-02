#ifndef LOCK_HPP_INC
#define LOCK_HPP_INC 1

#include <set>
#include "DFS.h"

extern "C" {
    #include <pthread.h>
}

class Lock {
    public:
        enum State {UNLOCKED, READ, WRITE};

        Lock();
        Lock(const Lock& rhs);
        ~Lock();

        bool readLock(const DFS::HostID& host);
        bool readUnlock(const DFS::HostID& host);
        bool writeLock(const DFS::HostID& host);
        bool writeUnlock(const DFS::HostID& host);

        bool readLocked() const;
        bool writeLocked() const;

    private:
        State state_;
        unsigned readCount_;
        pthread_mutex_t mutex_;
        std::set<DFS::HostID> hosts_;
};

#endif
