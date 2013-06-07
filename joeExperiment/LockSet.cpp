
#include "LockSet.hpp"

using namespace std;
using namespace DFS;

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
        cerr << "Found substring of: '" << path.substr(0, pos) << "'" << endl;
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
    vector<string> paths;
    splitPaths(path, paths);

    bool backout = false;
    int i = 0;
    for (; i < (int) paths.size(); ++i) {
        bool result;
        if (type == W && i == (paths.size() - 1)) {
            cerr << "Trying to get write lock on " << paths[i] << endl;
            result = locks_[paths[i]].writeLock(host);
        } else {
            cerr << "Trying to get read lock on " << paths[i] << endl;
            result = locks_[paths[i]].readLock(host);
        }
        /*
        cerr << "Trying to get " << (type == R ? "Read" : "Write") << " lock on " << paths[i] << endl;
        bool result = (type == R || i == (paths.size() - 1) ?
                locks_[paths[i]].readLock(host) :
                locks_[paths[i]].writeLock(host) );
                */
        if (!result) {
            cerr << "...Failed" << endl;
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

    cerr << "Locks: " << endl << *this << endl;

    return !backout;
}

bool LockSet::unlockPath(const string& path, const HostID& host, LockType type) {
    vector<string> paths;
    splitPaths(path, paths);

    int i = paths.size() - 1;
    for (; i >= 0; --i) {
        locks_[paths[i]].unlock(host);
    }
    return true;
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
    cerr << "Locks: " << endl << *this << endl;
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

