/* AJFS - a simple distributed file system.
 *
 * By Joe DeBlasio and Andiry Xu
 * For UCSD CSE223B, SP13
 * */
#ifndef LOCK_SET_HPP_INC
#define LOCK_SET_HPP_INC 1

extern "C" {
    #include <pthread.h>
}
#include <map>
#include <vector>
#include <string>
#include "Lock.hpp"
#include "DFS.h"

class LockSet {
    public:

        LockSet();
        LockSet(const LockSet& rhs);

        bool writeLockPath(const std::string& path, const DFS::HostID& host);
        void writeUnlockPath(const std::string& path, const DFS::HostID& host);
        bool readLockPath(const std::string& path, const DFS::HostID& host);
        void readUnlockPath(const std::string& path, const DFS::HostID& host);
        void unlockAll(const DFS::HostID& host);
        void unlockPath(const std::string& path, const DFS::HostID& host);

        bool anyWriteLocked() const;

        // member function that allows easy printing for operator<<
        std::ostream& print(std::ostream& out) const;
    private:

        enum LockType { R, W };
        bool lockPath(const std::string& path, const DFS::HostID& host, LockType type);

        std::map<std::string, Lock> locks_;
        std::map<DFS::HostID, std::set<std::string>> hostPaths_;
        pthread_mutex_t mutex_;

        void splitPaths(const std::string& path, std::vector<std::string>& paths);
};

inline std::ostream& operator<<(std::ostream& stream, const LockSet& locks) {
    return locks.print(stream);
}

#endif
