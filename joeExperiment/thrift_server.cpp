
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

inline string DFSHandler::convert(const string& path) {
    stringstream ss;
    ss << globals_->backupPath_ << path;
	string newPath = ss.str();
    return newPath;
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
    //cerr << "Pinged by " << sender.hostname << ":" << sender.port << endl;
}

void DFSHandler::unlock(const HostID& sender, const std::string& file) {
    announceOperation("unlock", sender, file);
    if (checkForDead(sender)) return;

    globals_->locks_.unlockPath(file, sender);

    printf("unlock\n");
}

void DFSHandler::die(const HostID& sender) {
    //if (checkForDead(sender)) return;
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

bool DFSHandler::lock(const HostID& sender, const std::string& file, const LockType::type lockType) {
    if (checkForDead(sender)) return false;
    announceOperation("lock", sender, file);

    printf("lock\n");

    if (lockType == LockType::type::WRITE)
        return globals_->locks_.writeLockPath(file, sender);
    else // reading
        return globals_->locks_.readLockPath(file, sender);
}

void DFSHandler::join(std::set<HostID> & _return, const HostID& sender) {
    if (checkForDead(sender)) return;

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
    if (globals_->joinLock_)
        return;

    globals_->joinLock_ = true;

    bool backout = false;
    list<Host *> backoutHosts;
    for (auto& pair : globals_->hostMap_) {
        bool result = pair.second.getJoinLock();
        if (!result) {
            backout = true;
            cerr << "getJoinLock() failed on " << pair.second.identifier() << endl;
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

void DFSHandler::ffit2ffi(const FUSEFileInfoTransport& ffit, fuse_file_info& ffi) {
    ffi.flags       = ffit.flags;
    ffi.fh_old      = ffit.fh_old;
    ffi.writepage   = ffit.writepage;
    ffi.direct_io   = ffit.direct_io;
    ffi.keep_cache  = ffit.keep_cache;
    ffi.flush       = ffit.flush;
    ffi.nonseekable = ffit.nonseekable;
    ffi.padding     = ffit.padding;
    ffi.fh          = ffit.fh;
    ffi.lock_owner  = ffit.lock_owner;
}

void DFSHandler::releasedir(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return;

    announceOperation("releasedir", sender, path);

    string cpath = convert(path);
    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    local_releasedir(cpath.c_str(), &ffi, fh);
    printf("releasedir\n");
}

void DFSHandler::mkdir(const HostID& sender, const std::string& path, const int32_t mode) {
    if (checkForDead(sender)) return;
    announceOperation("mkdir", sender, path);
    string cpath = convert(path);
    local_mkdir(cpath.c_str(), mode);
    printf("mkdir\n");
}

void DFSHandler::unlink(const HostID& sender, const std::string& path) {
    if (checkForDead(sender)) return;
    announceOperation("unlink", sender, path);
    string cpath = convert(path);
    local_unlink(cpath.c_str());
    printf("unlink\n");
}

void DFSHandler::rmdir(const HostID& sender, const std::string& path) {
    if (checkForDead(sender)) return;
    announceOperation("rmdir", sender, path);
    string cpath = convert(path);
    local_rmdir(cpath.c_str());
    printf("rmdir\n");
}

void DFSHandler::symlink(const HostID& sender, const std::string& from, const std::string& to) {
    if (checkForDead(sender)) return;
    announceOperation("symlink", sender, from, to);
    string cfrom = convert(from);
    string cto = convert(to);
    local_symlink(cfrom.c_str(), cto.c_str());
    printf("symlink\n");
}

void DFSHandler::rename(const HostID& sender, const std::string& from, const std::string& to) {
    if (checkForDead(sender)) return;
    announceOperation("rename", sender, from, to);
    string cfrom = convert(from);
    string cto = convert(to);
    local_rename(cfrom.c_str(), cto.c_str());
    printf("rename\n");
}

void DFSHandler::link(const HostID& sender, const std::string& from, const std::string& to) {
    if (checkForDead(sender)) return;
    announceOperation("link", sender, from, to);
    string cfrom = convert(from);
    string cto = convert(to);
    local_link(cfrom.c_str(), cto.c_str());
    printf("link\n");
}

void DFSHandler::chmod(const HostID& sender, const std::string& path, const int32_t mode) {
    if (checkForDead(sender)) return;
    announceOperation("chmod", sender, path);
    string cpath = convert(path);
    local_chmod(cpath.c_str(), mode);
    printf("chmod\n");
}

void DFSHandler::chown(const HostID& sender, const std::string& path, const int32_t uid, const int32_t gid) {
    if (checkForDead(sender)) return;
    announceOperation("chown", sender, path);
    string cpath = convert(path);
    local_chown(cpath.c_str(), uid, gid);
    printf("chown\n");
}

void DFSHandler::truncate(const HostID& sender, const std::string& path, const int64_t size) {
    if (checkForDead(sender)) return;
    announceOperation("truncate", sender, path);
    string cpath = convert(path);
    local_truncate(cpath.c_str(), size);
    printf("truncate\n");
}

void DFSHandler::ftruncate(const HostID& sender, const std::string& path, const int64_t size, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return;
    announceOperation("ftruncate", sender, path);
    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_ftruncate(cpath.c_str(), size, &ffi, fh);
    printf("ftruncate\n");
}

void DFSHandler::create(const HostID& sender, const std::string& path, const int32_t mode, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return;

    announceOperation("create", sender, path);

    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_create(cpath.c_str(), mode, &ffi, fh);
    printf("create\n");
}

void DFSHandler::write(const HostID& sender, const std::string& path, const std::vector<int8_t> & buf, const int64_t size, const int64_t offset, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return;
    announceOperation("write", sender, path);
    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    char * newbuf = (char *) (new char [size]);
    copy(buf.begin(), buf.end(), newbuf);
    string cpath = convert(path);
    local_write(cpath.c_str(), newbuf, size, offset, &ffi, fh);
    delete [] newbuf;
    printf("write\n");
}

void DFSHandler::flush(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return;
    announceOperation("flush", sender, path);
    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_flush(cpath.c_str(), &ffi, fh);
    printf("flush\n");
}

void DFSHandler::release(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return;

    announceOperation("release", sender, path);

    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_release(cpath.c_str(), &ffi, fh);
    printf("release\n");
}

void DFSHandler::flock(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi, const int64_t op) {
    if (checkForDead(sender)) return;
    announceOperation("flock", sender, path);
    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_flock(cpath.c_str(), &ffi, op, fh);
    printf("flock\n");
}

void DFSHandler::fallocate(const HostID& sender, const std::string& path, const int64_t mode, const int64_t offset, const int64_t length, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return;
    announceOperation("fallocate", sender, path);
#ifdef HAVE_POSIX_FALLOCATE
    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_fallocate(cpath.c_str(), mode, offset, length, &ffi, fh);
#endif
    printf("fallocate\n");
}

bool DFSHandler::fsync(const HostID& sender, const std::string& path, const int32_t isdatasync, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return false;
    announceOperation("fsync", sender, path);
    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_fsync(cpath.c_str(), isdatasync, &ffi, fh);
    printf("fsync\n");
    // TODO
    return true;
}

bool DFSHandler::open(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return false;

    announceOperation("open", sender, path);

    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_open(cpath.c_str(), &ffi, fh);
    printf("open\n");
    return true;
}

bool DFSHandler::opendir(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi) {
    if (checkForDead(sender)) return false;

    announceOperation("opendir", sender, path);

    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_opendir(cpath.c_str(), &ffi, fh);
    printf("opendir\n");
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

