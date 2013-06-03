
#include "thrift_server.hpp"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace ::DFS;
using namespace std;

DFSHandler::DFSHandler(GlobalBucket * globals) : globals_(globals) {
    // Your initialization goes here
}

bool DFSHandler::checkForDead(const HostID& sender) {
    if (globals_->hostMap_[sender].state_ == Host::State::DEAD) {
        globals_->hostMap_[sender].die();
        return true;
    }
    return false;
}

void DFSHandler::ping(const HostID& sender) {
    if (checkForDead(sender)) return;
    cerr << "Pinged by " << sender.hostname << ":" << sender.port << endl;
}

void DFSHandler::unlock(const HostID& sender, const std::string& file) {
    if (checkForDead(sender)) return;
    printf("unlock\n");
    // TODO
    // Is this even necessary? Inherent in close()?
    cerr << "ERROR: Someone called Unlock?" << endl;
    globals_->killall_();
}

void DFSHandler::die(const HostID& sender) {
    if (checkForDead(sender)) return;
    cerr << "Received die request from " << sender.hostname << ":" << sender.port << endl;
    globals_->killall_();
}

void DFSHandler::addServer(const HostID& sender, const HostID& newServer) {
    if (checkForDead(sender)) return;
    cerr << "Received addServer request for "
         << newServer.hostname << ":" << newServer.port
         << " from " << sender.hostname << ":" << sender.port << endl;

    Host newHost(newServer, globals_->me_);
    globals_->hostMap_[newServer] = newHost;
}

void DFSHandler::releaseJoinLock(const HostID& sender) {
    if (checkForDead(sender)) return;
    cerr << "Released JoinLock from " << sender.hostname << ":" << sender.port << endl;
    globals_->joinLock_ = false;
}

void DFSHandler::lock(const HostID& sender, const std::string& file) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("lock\n");
    // TODO
    // Is this even necessary? Inherent in open()?
    cerr << "ERROR: Someone called Lock?" << endl;
    globals_->killall_();
}

void DFSHandler::join(std::set<HostID> & _return, const HostID& sender) {
    if (checkForDead(sender)) return;
    // Your implementation goes here

    // TODO: Finish this...?

    // Call addServer on all of the hosts.
    pthread_mutex_lock(&globals_->hostLock_);
    for (auto& pair : globals_->hostMap_)
        pair.second.addServer(sender);
    addServer(globals_->me_, sender);
    //pthread_mutex_unlock(&globals_->hostLock_);

    // Call releaseJoinLock on all of the hosts.
    //pthread_mutex_lock(&globals_->hostLock_);
    for (auto& pair : globals_->hostMap_)
        pair.second.releaseJoinLock();
    globals_->joinLock_ = false;
    //pthread_mutex_unlock(&globals_->hostLock_);

    // Add in all of our hosts for the recipient.
    //pthread_mutex_lock(&globals_->hostLock_);
    for (auto& pair : globals_->hostMap_)
        _return.insert(pair.second.id_);
    pthread_mutex_unlock(&globals_->hostLock_);

    cerr << "Finished joined of " << sender.hostname << ":" << sender.port << endl;
}

void DFSHandler::requestJoinLock(string& _return, const HostID& sender) {
    if (checkForDead(sender)) return;
    if (globals_->joinLock_)
        return;

    globals_->joinLock_ = true;

    bool backout = false;
    list<Host *> backoutHosts;
    for (auto& pair : globals_->hostMap_) {
        bool result = pair.second.getJoinLock();
        if (!result) {
            backout = true;
            cerr << "getJoinLock() failed on "
                 << pair.first.hostname << ":" << pair.first.port << endl;
            break;
        }
        backoutHosts.push_front(&pair.second);
    }

    if (backout) {
        for (auto& host : backoutHosts)
            host->releaseJoinLock();
        globals_->joinLock_ = false;
        cerr << "Backed out of requestJoinLock from "
             << sender.hostname << ":" << sender.port << endl;
        return;
    }
    
    _return = globals_->backupPath_;
    cerr << "Finished requestJoinLock of "
         << sender.hostname << ":" << sender.port
         << " successfully" << endl;
}

bool DFSHandler::getJoinLock(const HostID& sender) {
    if (checkForDead(sender)) return false;
    cerr << "JoinLock requested from " << sender.hostname << ":" << sender.port << endl;
    if (globals_->joinLock_)
        return false;
    globals_->joinLock_ = true;
    return true;
}

void DFSHandler::releasedir(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("releasedir\n");
    // TODO
}

void DFSHandler::mkdir(const HostID& sender, const std::string& path, const int32_t mode) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("mkdir\n");
    // TODO
}

void DFSHandler::unlink(const HostID& sender, const std::string& path) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("unlink\n");
    // TODO
}

void DFSHandler::rmdir(const HostID& sender, const std::string& path) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("rmdir\n");
    // TODO
}

void DFSHandler::symlink(const HostID& sender, const std::string& from, const std::string& to) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("symlink\n");
    // TODO
}

void DFSHandler::rename(const HostID& sender, const std::string& from, const std::string& to) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("rename\n");
    // TODO
}

void DFSHandler::link(const HostID& sender, const std::string& from, const std::string& to) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("link\n");
    // TODO
}

void DFSHandler::chmod(const HostID& sender, const std::string& path, const int32_t mode) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("chmod\n");
    // TODO
}

void DFSHandler::chown(const HostID& sender, const std::string& path, const int32_t uid, const int32_t gid) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("chown\n");
    // TODO
}

void DFSHandler::truncate(const HostID& sender, const std::string& path, const int64_t size) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("truncate\n");
    // TODO
}

void DFSHandler::ftruncate(const HostID& sender, const std::string& path, const int64_t size, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("ftruncate\n");
    // TODO
}

void DFSHandler::create(const HostID& sender, const std::string& path, const int32_t mode, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("create\n");
    // TODO
}

void DFSHandler::write(const HostID& sender, const std::string& path, const std::vector<int8_t> & buf, const int64_t size, const int64_t offset, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("write\n");
    // TODO
}

void DFSHandler::flush(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("flush\n");
    // TODO
}

void DFSHandler::release(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("release\n");
    // TODO
}

void DFSHandler::flock(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi, const int64_t op) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("flock\n");
    // TODO
}

void DFSHandler::fallocate(const HostID& sender, const std::string& path, const int64_t mode, const int64_t offset, const int64_t length, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return;
    // Your implementation goes here
    printf("fallocate\n");
    // TODO
}

bool DFSHandler::fsync(const HostID& sender, const std::string& path, const int32_t isdatasync, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return false;
    // Your implementation goes here
    printf("fsync\n");
    // TODO
    return false;
}

bool DFSHandler::open(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return false;
    // Your implementation goes here
    printf("open\n");
    // TODO
    return false;
}

bool DFSHandler::opendir(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return false;
    // Your implementation goes here
    printf("opendir\n");
    // TODO
    return false;
}

boost::shared_ptr<TSimpleServer> server_;

void DFSServer::stop() {
    server_->stop();
}

void * DFSServer::start(void * arg) {
    GlobalBucket * globals = (GlobalBucket *) arg;
    int port = globals->me_.port;

    cerr << "Starting Thrift server..." << endl;

    boost::shared_ptr<DFSHandler> handler(new DFSHandler(globals));
    boost::shared_ptr<TProcessor> processor(new DFSProcessor(handler));
    boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    server_.reset(new TSimpleServer(processor, serverTransport, transportFactory, protocolFactory));
    server_->serve();

    cerr << "Thrift server done!" << endl;

    return NULL;
}

