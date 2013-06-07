
#include "LockSet.hpp"

using namespace std;
using namespace DFS;

static const bool DEBUG = false;

ostream& LockSet::print(std::ostream& out) const {
    for (auto& lockPair: locks_)
        out << "\t" << lockPair.first << ": " << lockPair.second << endl;
    return out;
}

void LockSet::splitPaths(const string& path, vector<string>& paths) {
    const string delim = "/";

    size_t pos = 1;

    paths.push_back("/");

    while ((pos = path.find(delim, pos)) != string::npos) {
        if (DEBUG) cerr << "Found substring of: '" << path.substr(0, pos) << "'" << endl;
        paths.push_back(path.substr(0, pos));

        pos += delim.length();
    }

    string lastPart = path;
    if (lastPart.back() == '/')
        lastPart.erase(--lastPart.end());
    if (lastPart != "" && lastPart != paths.back())
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
    pthread_mutex_lock(&mutex_);
    vector<string> paths;
    splitPaths(path, paths);

    bool backout = false;
    int i = 0;
    for (; i < (int) paths.size(); ++i) {
        bool result;
        if (type == W && i == (paths.size() - 1)) {
            if (DEBUG) cerr << "Trying to get write lock on " << paths[i] << endl;
            result = locks_[paths[i]].writeLock(host);
        } else {
            if (DEBUG) cerr << "Trying to get read lock on " << paths[i] << endl;
            result = locks_[paths[i]].readLock(host);
        }
        if (!result) {
            if (DEBUG) cerr << "...Failed" << endl;
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

    if (DEBUG) cerr << "Locks: " << endl << *this << endl;

    pthread_mutex_unlock(&mutex_);
    return !backout;
}

void LockSet::unlockPath(const string& path, const HostID& host) {
    pthread_mutex_lock(&mutex_);
    vector<string> paths;
    splitPaths(path, paths);

    int i = paths.size() - 1;
    for (; i >= 0; --i) {
        locks_[paths[i]].unlock(host);
        if (!locks_[paths[i]].locked())
            locks_.erase(paths[i]);
    }
    if (DEBUG) cerr << "Locks: " << endl << *this << endl;
    pthread_mutex_unlock(&mutex_);
}

bool LockSet::writeLockPath(const string& path, const HostID& host) {
    return lockPath(path, host, W); 
}

void LockSet::writeUnlockPath(const string& path, const HostID& host) {
    unlockPath(path, host); 
}

bool LockSet::readLockPath(const string& path, const HostID& host) {
    return lockPath(path, host, R); 
}

void LockSet::readUnlockPath(const string& path, const HostID& host) {
    unlockPath(path, host); 
}

void LockSet::unlockAll(const HostID& host) {
    pthread_mutex_lock(&mutex_);
    set<string>& paths = hostPaths_[host];
    for (set<string>::iterator iter = paths.begin(); iter != paths.end(); ++iter) {
        // ONE of these should work!
        locks_[*iter].unlock(host);
        if (!locks_[*iter].locked())
            locks_.erase(*iter);
    }

    hostPaths_.erase(host);
    pthread_mutex_unlock(&mutex_);
}

