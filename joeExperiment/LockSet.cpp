
#include "LockSet.hpp"

using namespace std;
using namespace DFS;

void LockSet::splitPaths(const string& path, vector<string>& paths) {
    const string delim = "/";

    size_t last = 0;
    size_t pos = 0;

    while ((pos = path.find(delim, pos)) != string::npos) {
        paths.push_back(path.substr(0, pos - last));

        pos += delim.length();
        last = pos;
    }

    string lastPart = path.substr(0, path.length() - last);
    if (lastPart.back() == '/')
        lastPart.erase(--lastPart.end());

    paths.push_back(lastPart);
}

LockSet::LockSet() {
    int rc = pthread_mutex_init(&mutex_, NULL);
    if (rc) cerr << "ERROR: Unable to init mutex!" << endl;
}

LockSet::LockSet(const LockSet& rhs)
    : locks_(rhs.locks_), hostPaths_(rhs.hostPaths_) {
    int rc = pthread_mutex_init(&mutex_, NULL);
    if (rc) cerr << "ERROR: Unable to init mutex!" << endl;
    cerr << "NOTICE: LockSet duplicated (?)" << endl;
}

bool LockSet::lockPath(const string& path, const HostID& host, LockType type) {
    vector<string> paths;
    splitPaths(path, paths);

    bool backout = false;
    int i = 0;
    for (; i < (int) paths.size(); ++i) {
        bool result = (type == R ?
                locks_[paths[i]].readLock(host) :
                locks_[paths[i]].writeLock(host) );
        if (!result) {
            backout = true;
            --i;
            break;
        }
    }

    if (backout) {
        for (; i >= 0; --i) {
            if (type == R)
                locks_[paths[i]].readUnlock(host);
            else
                locks_[paths[i]].writeUnlock(host);
        }
    }

    return !backout;
}

bool LockSet::unlockPath(const string& path, const HostID& host, LockType type) {
    vector<string> paths;
    splitPaths(path, paths);

    bool fullSuccess = true;
    int i = paths.size() - 1;
    for (; i >= 0; --i) {
        bool result = (type == R ?
                locks_[paths[i]].readUnlock(host) :
                locks_[paths[i]].writeUnlock(host) );
        if (!result) {
            fullSuccess = false;
            --i;
            break;
        }
    }
    return fullSuccess;
}

bool LockSet::writeLockPath(const string& path, const HostID& host) {
    pthread_mutex_lock(&mutex_);
    bool result = lockPath(path, host, W); 
    pthread_mutex_unlock(&mutex_);
    return result;
}

bool LockSet::writeUnlockPath(const string& path, const HostID& host) {
    pthread_mutex_lock(&mutex_);
    bool result = unlockPath(path, host, W); 
    pthread_mutex_unlock(&mutex_);
    return result;
}

bool LockSet::readLockPath(const string& path, const HostID& host) {
    pthread_mutex_lock(&mutex_);
    bool result = lockPath(path, host, R); 
    pthread_mutex_unlock(&mutex_);
    return result;
}

bool LockSet::readUnlockPath(const string& path, const HostID& host) {
    pthread_mutex_lock(&mutex_);
    bool result = unlockPath(path, host, R); 
    pthread_mutex_unlock(&mutex_);
    return result;
}

void LockSet::unlockPath(const string& path, const HostID& host) {
    unlockPath(path, host, R);
    unlockPath(path, host, W);
}

void LockSet::unlockAll(const HostID& host) {
    pthread_mutex_lock(&mutex_);
    set<string>& paths = hostPaths_[host];
    for (set<string>::iterator iter = paths.begin(); iter != paths.end(); ++iter) {
        // ONE of these should work!
        locks_[*iter].readUnlock(host);
        locks_[*iter].writeUnlock(host);
    }

    hostPaths_.erase(host);
    pthread_mutex_unlock(&mutex_);
}

