
#include "LockManager.hpp"

using namespace std;
using namespace LockManager;

namespace LockManager {
    bool dead_ = false;
}

void nanoSleep(time_t seconds, long nanoseconds) {
    struct timespec tim, tim2;
    tim.tv_sec = seconds;
    tim.tv_nsec = nanoseconds;

    if(nanosleep(&tim , &tim2) < 0 ) {
        cerr << "Lock Manager interrupted..." << endl;
        pthread_exit(NULL);
    }
}

void LockManager::stop() {
    dead_ = true;
    pthread_exit(NULL);
}

void * LockManager::start(void * arg) {
    GlobalBucket * globals = static_cast<GlobalBucket *>(arg);

    while (!dead_) {
        int numDead = 0;
        int numAlive = 0;
        pthread_mutex_lock(&(globals->hostLock_));
        for (HostMap_t::iterator iter = globals->hostMap_.begin();
                iter != globals->hostMap_.end();
                ++iter) {
            //cerr << "Checking " << iter->second.identifier() << endl;
            if (iter->second.state_ == Host::State::ME) {
                //cerr << "Ignoring myself " << iter->second.identifier() << endl;
                ++numAlive;
            //} else if (iter->second.state_ == Host::State::JOINING) {
            //    cerr << "Ignoring joining host " << iter->second.identifier() << endl;
            //    // Do nothing?
            } else if (iter->second.state_ == Host::State::DEAD) {
                //cerr << "Ignoring dead host " << iter->second.identifier() << endl;
                ++numDead;
            } else if (iter->second.state_ == Host::State::UNKNOWN) {
                cerr << "Ignoring unknown host " << iter->second.identifier() << endl;
                // Skip these...
            } else if (iter->second.ping()) { // ALIVE
                ++numAlive;
            }
            else { // ping failed
                cerr << "Discovered dead host " << iter->second.identifier() << endl;
                ++numDead;
                iter->second.state_ = Host::State::DEAD;

                // Kill locks for host
                globals->locks_.unlockAll(iter->second.id_);
            }
        }
        pthread_mutex_unlock(&(globals->hostLock_));

        if (numDead >= numAlive) {
            cerr << "ALERT: NO QUORUM DETECTED... EXITING." << endl;
            globals->killall_();
            break;
        }

        nanoSleep(SLEEP_SECONDS, SLEEP_NANOSECONDS);
    }

    cerr << "Lock Manager done!" << endl;

    return NULL;
}

