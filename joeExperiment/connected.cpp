#include "DFS_server.hpp"
#include "LFS_operations.hpp"
#include "fuse_operations.hpp"
#include "lockmanager.hpp"

#include "Host.hpp"

#include <pthread.h>
#include <csignal>
#include <map>

using namespace std;

pthread_t sThread;
pthread_t fThread;
pthread_t lThread;

pthread_t mainThread;

static void killall(int signal) {
    pthread_t self = pthread_self();
    if (self == mainThread) {

    }
    else if (self == sThread) {
        DFSServer::stop();
        return;
    }
    else if (self == fThread) {
        FUSEService::stop();
        cerr << endl << "\t == "
             << "HINT: Do an \"ls\" in the mounted directory to exit."
             << " ==" << endl;
        return;
    }
    else if (self == lThread) {
        LockManager::stop();
        return;
    }
    else {
        cerr << "ERROR: Caught signal in an unknown thread...?" << endl;
        pthread_exit(NULL);
        return;
    }

    pthread_kill(sThread, signal);
    pthread_kill(fThread, signal);
    pthread_kill(lThread, signal);
}

void usage(string script) {
    cerr << "Usage: " << script <<
                         " MOUNT_POINT BACKUP_DIR" <<
                         " LOCAL_IP LOCAL_PORT" <<
                         " [REMOTE_IP REMOTE_PORT]" << endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    int rc;
    umask(0);

    mainThread = pthread_self();

    HostMap_t hostMap;
    pthread_mutex_t hostLock;

    if (argc != 5 && argc != 7)
        usage(argv[0]);

    char * mountPoint(argv[1]);
    string backupPath(argv[2]);
    string localIP(argv[3]);
    int16_t port = atoi(argv[4]);
    cerr << "Mount Point:\t" << mountPoint << endl;
    cerr << "Backup Point:\t" << backupPath << endl;
    cerr << "Local IP/Port:\t" << localIP << ":" << port << endl;

    Host me(localIP, port, Host::State::ME);
    hostMap[me.id_] = me;

    if (argc == 7) {
        string remoteIP(argv[5]);
        int16_t remotePort = atoi(argv[6]);

        Host remoteHost(remoteIP, remotePort, Host::State::ALIVE);
        hostMap[remoteHost.id_] = remoteHost;

        cerr << "Remote IP:\t" << localIP << endl;
        cerr << "Remote Port:\t" << port << endl;
    }

    cerr << endl;
    cerr << "Setting up signal handler..." << endl;
    if (signal(SIGINT, killall) == SIG_ERR) {
        cerr << "ERROR: Unable to set signal handler." << endl;
        return EXIT_FAILURE;
    }

    FUSEService::ArgStruct * args = new FUSEService::ArgStruct;

    args->argc = 4;
    args->argv = (char **) (new char *[4]);
    args->backupPath = backupPath;

    initializer_list<char*> newArgv = initializer_list<char *>(
        {argv[0], "-f", "-s", mountPoint});
    copy(newArgv.begin(), newArgv.end(), args->argv);

    cerr << "Starting Thrift server thread..." << endl;
    rc = pthread_create(&sThread, NULL, &DFSServer::start, (void *) (size_t) port);
    if (rc) {
        cerr << "ERROR: return code from pthread_create() is " << rc << endl;
        exit(-1);
    }

    cerr << "Starting FUSE thread..." << endl;
    rc = pthread_create(&fThread, NULL, &FUSEService::start, (void *) args);
    if (rc) {
        cerr << "ERROR: return code from pthread_create() is " << rc << endl;
        exit(-1);
    }

    cerr << "Starting Lock Manager thread..." << endl;
    LockManager::LMArgs lmArgs(me, hostMap, hostLock);
    rc = pthread_create(&lThread, NULL, &LockManager::start, (void *) &lmArgs);
    if (rc) {
        cerr << "ERROR: return code from pthread_create() is " << rc << endl;
        exit(-1);
    }

    void * val = NULL;
    pthread_join(sThread, &val);
    pthread_join(fThread, &val);
    pthread_join(lThread, &val);

    cerr << "Done!" << endl;

    return 0;
}
 
