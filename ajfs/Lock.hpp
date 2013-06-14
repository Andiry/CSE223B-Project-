#ifndef LOCK_HPP_INC
#define LOCK_HPP_INC 1

#include <map>
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

        std::ostream& print(std::ostream& out) const;

        bool readLock(const DFS::HostID& host);
        bool readUnlock(const DFS::HostID& host);
        bool writeLock(const DFS::HostID& host);
        bool writeUnlock(const DFS::HostID& host);
        bool unlock(const DFS::HostID& host);

        bool readLocked() const;
        bool writeLocked() const;
        bool locked() const;

    private:
        State state_;
        unsigned readCount_;
        pthread_mutex_t mutex_;
        std::map<DFS::HostID, unsigned> hosts_;
};

inline std::ostream& operator<<(std::ostream& stream, const Lock& lock) {
    return lock.print(stream);
}

#endif
