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
bool started = false;

/*
static void killall(int signal) {
    cerr << "Caught Signal" << endl;
    if (!started)
        return;
    started = false;

    pthread_kill(sThread, 15);
    pthread_kill(fThread, 15);
    pthread_kill(lThread, 15);
    pthread_exit(NULL);
    exit(1);
}
*/

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

    HostMap_t hostMap;
    pthread_mutex_t hostLock;
    bool dead = false;

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

    /*cerr << "Setting up signal handler..." << endl;
    if (signal(SIGINT, killall) == SIG_ERR) {
        cerr << "ERROR: Unable to set signal handler." << endl;
        return EXIT_FAILURE;
    }*/

    ArgStruct * args = new ArgStruct;

    args->argc = 4;
    args->argv = (char **) (new char *[4]);
    args->backupPath = backupPath;

    initializer_list<char*> newArgv = initializer_list<char *>(
        {argv[0], "-f", "-s", mountPoint});
    copy(newArgv.begin(), newArgv.end(), args->argv);

    /*
    cerr << args->argv[0] << endl;
    cerr << args->argv[1] << endl;
    cerr << args->argv[2] << endl;
    cerr << args->argv[3] << endl;
    */
    //args->argv = argv;

    cerr << "Starting Thrift server thread..." << endl;
    rc = pthread_create(&sThread, NULL, &startServer, (void *) (size_t) port);
    if (rc) {
        cerr << "ERROR: return code from pthread_create() is " << rc << endl;
        exit(-1);
    }

    cerr << "Starting FUSE thread..." << endl;
    rc = pthread_create(&fThread, NULL, &startFuse, (void *) args);
    if (rc) {
        cerr << "ERROR: return code from pthread_create() is " << rc << endl;
        exit(-1);
    }

    cerr << "Starting Lock Manager thread..." << endl;
    LockManager::LMArgs lmArgs(me, hostMap, hostLock, dead);
    
    rc = pthread_create(&lThread, NULL, &LockManager::start, (void *) &lmArgs);
    if (rc) {
        cerr << "ERROR: return code from pthread_create() is " << rc << endl;
        exit(-1);
    }

    started = true;

    //startServer(9090);
    
    void * val = NULL;
    pthread_join(sThread, &val);
    pthread_join(fThread, &val);
    pthread_join(lThread, &val);

    cerr << "Done..." << endl;

    return 0;
}
 
