
#include "Lock.hpp"

using namespace std;
using namespace DFS;

Lock::Lock() : state_(UNLOCKED), readCount_(0) {
    int rc = pthread_mutex_init(&mutex_, NULL);
    if (rc) cerr << "ERROR: Unable to init mutex!" << endl;
}

Lock::Lock(const Lock& rhs)
    : state_(rhs.state_), readCount_(rhs.readCount_), hosts_(rhs.hosts_) {
    int rc = pthread_mutex_init(&mutex_, NULL);
    if (rc) cerr << "ERROR: Unable to init mutex!" << endl;
}

Lock::~Lock() {
    int rc = pthread_mutex_destroy(&mutex_);
    if (rc) cerr << "ERROR: Unable to destroy mutex!" << endl;
}

bool Lock::readLock(const HostID& host) {
    pthread_mutex_lock(&mutex_);

    if (state_ == READ || state_ == UNLOCKED) {
        ++readCount_;
        state_ = READ;
        ++hosts_[host];

        pthread_mutex_unlock(&mutex_);
        return true;
    }
    pthread_mutex_unlock(&mutex_);
    return false;
}

bool Lock::readUnlock(const HostID& host) {
    pthread_mutex_lock(&mutex_);

    if (state_ == READ && hosts_[host]) {
        --hosts_[host];

        if (--readCount_ == 0)
            state_ = UNLOCKED;

        pthread_mutex_unlock(&mutex_);
        return true;
    }
    pthread_mutex_unlock(&mutex_);
    return false;
}

bool Lock::unlock(const HostID& host) {
    if (state_ == READ)
        return readUnlock(host);
    if (state_ == WRITE)
        return writeUnlock(host);
    return false;
}

bool Lock::writeLock(const HostID& host) {
    pthread_mutex_lock(&mutex_);

    if (state_ == UNLOCKED || (state_ == WRITE && hosts_[host])) {
        state_ = WRITE;

        ++hosts_[host];

        pthread_mutex_unlock(&mutex_);
        return true;
    }

    pthread_mutex_unlock(&mutex_);
    return false;
}

bool Lock::writeUnlock(const HostID& host) {
    pthread_mutex_lock(&mutex_);

    if (state_ == WRITE && hosts_[host]) {
        state_ = UNLOCKED;
        hosts_.clear();
        pthread_mutex_unlock(&mutex_);
        return true;
    }

    pthread_mutex_unlock(&mutex_);
    return false;
}

bool Lock::readLocked() const {
    return state_ == READ;
}

bool Lock::writeLocked() const {
    return state_ == WRITE;
}
