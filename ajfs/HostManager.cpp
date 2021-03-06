/* AJFS - a simple distributed file system.
 *
 * By Joe DeBlasio and Andiry Xu
 * For UCSD CSE223B, SP13
 * */

#include "HostManager.hpp"

using namespace std;
using namespace HostManager;

namespace HostManager {
    bool dead_ = false;
    GlobalBucket * globals_ = NULL;
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

void HostManager::stop() {
    dead_ = true;
    for(auto& pair: globals_->hostMap_) {
        pair.second.kill();
    }
    pthread_exit(NULL);
}

#define clearScreen() printf("%c[2J", 27);
void * HostManager::start(void * arg) {
    globals_ = static_cast<GlobalBucket *>(arg);

    unsigned joinLockCount_ = 0;

    while (!dead_) {

        //clearScreen();

        int numDead = 0;
        int numAlive = 0;
        pthread_mutex_lock(&(globals_->hostLock_));
        for (HostMap_t::iterator iter = globals_->hostMap_.begin();
                iter != globals_->hostMap_.end();
                ++iter) {
            //cerr << "Checking " << iter->second.identifier() << endl;
            if (iter->second.state_ == Host::State::ME) {
                //cerr << "Ignoring myself " << iter->second.identifier() << endl;
                ++numAlive;
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
                globals_->locks_.unlockAll(iter->second.id_);
            }
        }
        pthread_mutex_unlock(&(globals_->hostLock_));

        if (numDead >= numAlive) {
            cerr << "ALERT: NO QUORUM DETECTED... EXITING." << endl;
            globals_->killall_();
            break;
        }

        if(globals_->joinLock_)
            ++joinLockCount_;
        else
            joinLockCount_ = 0;

        if (joinLockCount_ >= JOIN_LOCK_COUNT_THRESH) {
            cerr << "ERROR: JoinLockCount timed out... New host died?" << endl;
            pthread_mutex_lock(&globals_->hostLock_);
            if (globals_->joinMaster_) {
                for (auto& pair : globals_->hostMap_)
                    pair.second.releaseJoinLock();
                globals_->hostMap_.erase(globals_->joining_);
            }
            globals_->joinMaster_ = false;
            globals_->joinLock_   = false;
            pthread_mutex_unlock(&globals_->hostLock_);
            joinLockCount_ = 0;
        }

        nanoSleep(SLEEP_SECONDS, SLEEP_NANOSECONDS);
    }

    cerr << "Lock Manager done!" << endl;

    return NULL;
}

