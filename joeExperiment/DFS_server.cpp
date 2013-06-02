// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "DFS_server.hpp"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace ::DFS;
using namespace std;

int global_id;
std::vector < std::pair<std::string, int> > global_backendServerVector;
std::string global_backup_path;

static const string convert(const string path)
{
	return global_backup_path + path;
}

DFSHandler::DFSHandler(int argc, char **argv) {
    // Your initialization goes here
    _id = atoi(argv[4]);

    //FIXME: support indirect path
    if (argv[3][0] == '/') {
	_backup_path = argv[3];
	cout << "backup path at " << _backup_path << endl;
    } else {
	cerr << "Must use direct path for backup! " << argv[3] << endl;
    }

    for (int i = 6; i + 1 < argc; i += 2) {
	string peer_ip(argv[i]);
	int peer_port = atoi(argv[i+1]);
	_backendServerVector.push_back(make_pair(peer_ip, peer_port));
	cout << "Backend server at " << peer_ip << " on port " << peer_port << endl;
    }

    global_id = _id;
    global_backendServerVector = _backendServerVector;
    global_backup_path = _backup_path;

    RsyncWithOtherServers();
}

bool DFSHandler::lock(const std::string& path, const std::string& hostname) {
    // Your implementation goes here
    printf("lock\n");
    return false;
}

bool DFSHandler::unlock(const std::string& path, const std::string& hostname) {
    // Your implementation goes here
    printf("unlock\n");
    return false;
}

void DFSHandler::commit(const int64_t id, const std::string& hostname) {
    // Your implementation goes here
    printf("commit\n");
}

void DFSHandler::Bla() {
    // Your implementation goes here
    printf("Bla\n");
}

void DFSHandler::Ping() {
    // Your implementation goes here
    printf("Ping\n");
}

void DFSHandler::Pong() {
    // Your implementation goes here
    printf("Pong\n");
}

void DFSHandler::dfs_doOperation(const std::string& operation, const std::string& path, const int32_t mode, const int32_t flags) {
    // Your implementation goes here
    string local_path = convert(path);
    struct fuse_file_info *fi = NULL;

    cout << "dfs_doOperation " << operation << " " << local_path << " " << mode_t(mode) << " " << flags << endl;
    if (operation == "create") {
	fi = new(struct fuse_file_info);
	fi->flags = flags;
	local_create(local_path.c_str(), (mode_t)mode, fi);
    }

    if (fi)
	delete fi;
}

void DFSHandler::dfs_remote_opendir(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_opendir\n");
}

void DFSHandler::dfs_remote_readdir(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_readdir\n");
}

void DFSHandler::dfs_remote_releasedir(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_releasedir\n");
}

void DFSHandler::dfs_remote_mkdir(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_mkdir\n");
}

void DFSHandler::dfs_remote_symlink(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_symlink\n");
}

void DFSHandler::dfs_remote_unlink(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_unlink\n");
}

void DFSHandler::dfs_remote_rmdir(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_rmdir\n");
}

void DFSHandler::dfs_remote_rename(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_rename\n");
}

void DFSHandler::dfs_remote_link(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_link\n");
}

void DFSHandler::dfs_remote_chmod(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_chmod\n");
}

void DFSHandler::dfs_remote_chown(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_chown\n");
}

void DFSHandler::dfs_remote_truncate(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_truncate\n");
}

void DFSHandler::dfs_remote_ftruncate(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_ftruncate\n");
}

void DFSHandler::dfs_remote_create(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_create\n");
}

void DFSHandler::dfs_remote_open(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_open\n");
}

void DFSHandler::dfs_remote_write(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_write\n");
}

void DFSHandler::dfs_remote_flush(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_flush\n");
}

void DFSHandler::dfs_remote_release(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_release\n");
}

void DFSHandler::dfs_remote_fsync(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_fsync\n");
}

void DFSHandler::dfs_remote_fallocate(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_fallocate\n");
}

void DFSHandler::dfs_remote_lock(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_lock\n");
}

void DFSHandler::dfs_remote_flock(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_flock\n");
}

void DFSHandler::GetInfo(GetInfoResponse& _return, const std::string& key) {
    if (key == "backup_path") {
	_return.status = DFS_status::OK;
	_return.values.push_back(_backup_path);
    }
    return;
}

/* Get information from other servers */
void DFSHandler::RsyncWithOtherServers(void) {
    std::vector<pair<string, int> >::iterator iter;
    std::string storageServer;
    int storageServerPort;
    GetInfoResponse _return;
    bool retry = true;

    for (iter = _backendServerVector.begin(); iter != _backendServerVector.end(); ++iter) {
	storageServer = iter->first;
	storageServerPort = iter->second;
	cout << "Retrieve data from server " << storageServer << " " << storageServerPort << endl;
	boost::shared_ptr<TSocket> socket(new TSocket(storageServer, storageServerPort));
	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	DFSClient client(protocol);
	socket->setConnTimeout(100);
	socket->setRecvTimeout(100);
	socket->setSendTimeout(100);
	try {
	    transport->open();
	    client.GetInfo(_return, "backup_path");
	    if (_return.status == DFS_status::OK) {
		cout << "Get backup path " << _return.values[0] << " on server " << storageServer << endl;
		std::string backup_path(_return.values[0]);
		std::string command("rsync -ru " + storageServer + ":" + backup_path + "/ " + _backup_path);
		cout << command << endl;
		system(command.c_str());
		retry = false;
	    }

	    transport->close();
	    if (!retry)
		break;
	} catch (TException &tx) {
	    cout << "ERROR: %s" << tx.what() << endl;
	    continue;
	}
    }
    return;
}

/* Run file operations pushed by other servers */
#if 0
void PushData(const string op, const char *path, mode_t mode, struct fuse_file_info fi) {

    switch (op) {
    case "create":
	local_create(path, mode, &fi);
	break;
    default:
	break;
    }
}
#endif

/* Post information to other servers */
void PropagateToOtherServers(const string op, const char *path, mode_t mode, struct fuse_file_info *fi) {
    std::vector<pair<string, int> >::iterator iter;
    std::string storageServer;
    int storageServerPort;

    cout << "Propagate to other servers" << endl;
    for (iter = global_backendServerVector.begin(); iter != global_backendServerVector.end(); ++iter) {
	storageServer = iter->first;
	storageServerPort = iter->second;
	cout << "Post data to server " << storageServer << " " << storageServerPort << endl;
	boost::shared_ptr<TSocket> socket(new TSocket(storageServer, storageServerPort));
	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	DFSClient client(protocol);
	socket->setConnTimeout(100);
	socket->setRecvTimeout(100);
	socket->setSendTimeout(100);
	try {
	    transport->open();
	    client.dfs_doOperation(op, path, mode, fi->flags);

	    transport->close();
	} catch (TException &tx) {
	    cout << "ERROR: %s" << tx.what() << endl;
	    continue;
	}
    }
    return;
}

void * startServer(void * arg) {
    ArgStruct *args = (ArgStruct *) arg;
    int port = atoi(args->argv[5]);

    cerr << "Starting Thrift Server on port " << port << " ..." << endl;

    boost::shared_ptr<DFSHandler> handler(new DFSHandler(args->argc, args->argv));
    boost::shared_ptr<TProcessor> processor(new DFSProcessor(handler));
    boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    server.serve();

    pthread_exit(NULL);
    return NULL;
}

