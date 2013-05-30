#include "DFS_server.hpp"
#include "LFS_operations.hpp"
#include "fuse_operations.hpp"

#include <pthread.h>
#include <csignal>

using namespace std;

pthread_t sThread;
pthread_t fThread;
bool started = false;

static void killall(int signal) {
    cerr << "Caught Signal" << endl;
    if (!started)
        return;
    started = false;

    pthread_kill(sThread, 15);
    pthread_kill(fThread, 15);
    pthread_exit(NULL);
    exit(1);
}

int main(int argc, char *argv[])
{
    umask(0);

    int rc;

    cerr << "Setting up signal handler..." << endl;
    /*if (signal(SIGINT, killall) == SIG_ERR) {
        cerr << "ERROR: Unable to set signal handler." << endl;
        return EXIT_FAILURE;
    }*/

    ArgStruct * args = new ArgStruct;
    args->argc = argc;
    args->argv = argv;

    cerr << "Starting Thrift server thread..." << endl;
    rc = pthread_create(&sThread, NULL, &startServer, (void *) args);
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

    started = true;

    //startServer(9090);
    
    void * val = NULL;
    pthread_join(sThread, &val);
    pthread_join(fThread, &val);

    cerr << "Done..." << endl;

    return 0;
}
 
