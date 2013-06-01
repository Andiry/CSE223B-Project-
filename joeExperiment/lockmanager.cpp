
#include "lockmanager.hpp"

using namespace std;
using namespace LockManager;

void nanoSleep(time_t seconds, long nanoseconds, bool & death) {
    struct timespec tim, tim2;
    tim.tv_sec = seconds;
    tim.tv_nsec = nanoseconds;

    if(nanosleep(&tim , &tim2) < 0 ) {
        cerr << "Lock Manager interrupted..." << endl;
        death = true;
        pthread_exit(NULL);
    }
}

void * LockManager::start(void * lockManagerArgs) {
    LMArgs * args = static_cast<LMArgs *>(lockManagerArgs);

    while (true) {
        int numDead = 0;
        int numAlive = 0;
        pthread_mutex_lock(&(args->hostLock_));
        for (HostMap_t::iterator iter = args->hostMap_.begin();
                iter != args->hostMap_.end();
                ++iter) {
            if (iter->second.state_ == Host::State::ME) {
                ++numAlive;
            } else if (iter->second.state_ == Host::State::JOINING) {
                // Do nothing?
            } else if (iter->second.state_ == Host::State::DEAD) {
                ++numDead;
            } else if (iter->second.state_ == Host::State::UNKNOWN) {
                // Skip these...
            } else if (iter->second.ping()) { // ALIVE
                ++numAlive;
            }
            else { // ping failed
                ++numDead;
                iter->second.state_ = Host::State::DEAD;
                // TODO
                // KILL LOCKS FOR HOST
            }
        }
        pthread_mutex_unlock(&(args->hostLock_));

        if (numDead > numAlive) {
            cerr << "ALERT: NO QUORUM DETECTED... EXITING." << endl;
            args->dead_ = true;
            pthread_exit(NULL);
        }

        nanoSleep(SLEEP_SECONDS, SLEEP_NANOSECONDS, args->dead_);
    }

}

