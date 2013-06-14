#include "DFS.h"
#include "thrift_server.hpp"
#include "LFS_operations.hpp"
#include "fuse_operations.hpp"
#include "HostManager.hpp"

#include "Host.hpp"

extern "C" {
    #include <pthread.h>
}

#include <csignal>
#include <map>

using namespace std;
using namespace DFS;

pthread_t sThread;
pthread_t fThread;
pthread_t lThread;

pthread_t mainThread;

bool dead_ = false;

static void killall() {
    if (!dead_)
        pthread_kill(mainThread, SIGTERM);
    dead_ = true;
}

static void gracefulExit(int signal) {
    pthread_t self = pthread_self();
    if (self == mainThread) {
        // This may not be necessary...
        pthread_kill(sThread, signal);
        pthread_kill(fThread, signal);
        pthread_kill(lThread, signal);
    }
    else if (self == sThread) {
        DFSServer::stop();
    }
    else if (self == fThread) {
        FUSEService::stop();
    }
    else if (self == lThread) {
        HostManager::stop();
    }
    else {
        cerr << "ERROR: Caught signal in an unknown thread...?" << endl;
        pthread_exit(NULL);
    }
}

void usage(string script) {
    cerr << "Usage: " << script <<
                         " MOUNT_POINT BACKUP_DIR" <<
                         " LOCAL_IP LOCAL_PORT" <<
                         " [REMOTE_IP REMOTE_PORT]" << endl;
    exit(1);
}

void tryStartThread(pthread_t& thread,
                    const string& name,
                    void *(*start) (void *),
                    void * arg) {
    cerr << "Starting " << name << " thread..." << endl;
    int rc = pthread_create(&thread, NULL, start, arg);
    if (rc) {
        cerr << "ERROR: pthread_create() returned " << rc << endl;
        exit(-1);
    }
}

void syncPaths(const string& rHost, const string& lPath, const string& rPath) {
    cerr << "Trying to rsync paths" << endl;
    string command =
        "rsync -az -e ssh --delete " + rHost + ":" + rPath + "/ " + lPath + "/";
    int ret = system(command.c_str());
    
    //cerr << "I WOULD run: " << endl;
    //cerr << command << endl;
    //int ret = 0;
    //sleep(2);

    if (ret) {
        cerr << "ERROR: Unable to rsync. Returned " << ret << endl;
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    umask(0);

    mainThread = pthread_self();

    if (argc != 5 && argc != 7)
        usage(argv[0]);

    char * mountPoint(argv[1]);
    string backupPath(argv[2]);
    string localIP(argv[3]);
    int16_t port = atoi(argv[4]);

    cerr << "Mount Point:\t" << mountPoint << endl;
    cerr << "Backup Point:\t" << backupPath << endl;
    cerr << "Local IP/Port:\t" << localIP << ":" << port << endl;

    GlobalBucket globals(localIP, port, &killall); 

    // Determine full backup path.
    char *bup = realpath(backupPath.c_str(), NULL);
    globals.backupPath_ = bup;
    free(bup);
    if (globals.backupPath_.back() == '/')
        globals.backupPath_.erase(--globals.backupPath_.end());

    // Add the host entry for myself (needed when we give host sets away)
    HostID meID; meID.hostname = localIP; meID.port = port;
    Host me(localIP, port, Host::State::ME, meID);
    globals.hostMap_[me.id_] = me;


    HostID remoteHostID;
    if (argc == 7) {
        string remoteIP(argv[5]);
        int16_t remotePort = atoi(argv[6]);

        cerr << "Remote IP/Port:\t" << remoteIP << ":" << remotePort << endl;

        Host remoteHost(remoteIP, remotePort, Host::State::ALIVE, meID);
        globals.hostMap_[remoteHost.id_] = remoteHost;
        remoteHostID = remoteHost.id_;

        string remotePath;
        if (!remoteHost.requestJoinLock(remotePath)) {
            cerr << "ERROR: Unable to capture join lock. Exiting." << endl;
            exit(1);
        }
        cerr << "Retrieved remote path of '" << remotePath << "'" << endl;

        syncPaths(remoteIP, globals.backupPath_, remotePath);
    }


    cerr << endl;

    // Setup signal handler
    cerr << "Setting up signal handler..." << endl;
    if (signal(SIGINT, gracefulExit) == SIG_ERR) {
        cerr << "ERROR: Unable to set signal handler." << endl;
        return EXIT_FAILURE;
    }
    if (signal(SIGTERM, gracefulExit) == SIG_ERR) {
        cerr << "ERROR: Unable to set signal handler." << endl;
        return EXIT_FAILURE;
    }

    // FUSE Argument Preparation
    FUSEService::ArgStruct * fuseArgs = new FUSEService::ArgStruct;
    fuseArgs->argc       = 4;
    fuseArgs->argv       = (char **) (new char *[4]);
    fuseArgs->globals    = &globals;

    initializer_list<char*> newArgv = initializer_list<char *>(
            {argv[0], const_cast<char *>("-f"), const_cast<char *>("-s"), mountPoint} );
    copy(newArgv.begin(), newArgv.end(), fuseArgs->argv);

    // Start threads
    tryStartThread(sThread, "Thrift",  &DFSServer::start,   (void *) &globals);
    tryStartThread(fThread, "FUSE",    &FUSEService::start, (void *) fuseArgs);
    tryStartThread(lThread, "LockMgr", &HostManager::start, (void *) &globals);

    // Complete the join() operation, if needed.
    if (!dead_ && remoteHostID.hostname != "") {
        // Wait for socket to open fully.
        sleep(1);

        // Join and retrieve new hosts
        set<HostID> newHosts;
        globals.hostMap_[remoteHostID].join(newHosts);
        if (newHosts.size() == 0) {
            cerr << "ERROR: Unable to receive hosts from other nodes." << endl;
            globals.killall_();
        }
        // Add new hosts
        else {
            pthread_mutex_lock(&globals.hostLock_);
            for(auto &newHostID : newHosts) {
                if (globals.me_ == newHostID)
                    continue;
                Host newHost(newHostID, globals.me_);
                globals.hostMap_[newHostID] = newHost;
            }
            pthread_mutex_unlock(&globals.hostLock_);
        }
    }

    // Join threads
    void * val = NULL;
    pthread_join(sThread, &val);
    pthread_join(fThread, &val);
    pthread_join(lThread, &val);
    
    cerr << "Done!" << endl;

    return 0;
}
 
