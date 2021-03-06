/* AJFS - a simple distributed file system.
 *
 * By Joe DeBlasio and Andiry Xu
 * For UCSD CSE223B, SP13
 * */

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

#define counterCheck() \
/*cerr << "Checking " << globals_->hostMap_[sender].farCounter_ << " " << counter <<  * endl; */\
if (globals_->hostMap_[sender].farCounter_ + 1 != counter) { \
    /*cerr << "Bad value detected! " << globals_->hostMap_[sender].farCounter_ << " " << counter << endl; */\
    globals_->hostMap_[sender].farCounter_ = counter; \
    /*sleep(1);*/\
    return;\
} \
globals_->hostMap_[sender].farCounter_ = counter;

#define counterUpdate() \
++globals_->hostMap_[sender].farCounter_;

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
    announceOperation("lock", sender, file);
    if (checkForDead(sender)) return false;

    if (lockType == LockType::type::WRITE)
        return globals_->locks_.writeLockPath(file, sender);
    else // reading
        return globals_->locks_.readLockPath(file, sender);
}

void DFSHandler::join(std::set<HostID> & _return, const HostID& sender) {
    if (!globals_->joinMaster_) return;
    if (globals_->joining_ != sender) return;

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

    globals_->joinMaster_ = false;

    cerr << "Finished join of " << sender.hostname << ":" << sender.port << endl;
}

void DFSHandler::requestJoinLock(string& _return, const HostID& sender) {
    cerr << "Received JoinLock Request from " << sender.hostname << ":" << sender.port << endl;
    if (globals_->joinLock_) {
        if (globals_->joining_ == sender) {
            _return = globals_->backupPath_;
            cerr << "Reinitialized requestJoinLock of "
                 << sender.hostname << ":" << sender.port
                 << " successfully" << endl;
            return;
        }
        return;
    }
    // If folks have files open for writing, this isn't safe to do right now...
    // TODO - improve this behavior
    else if (globals_->locks_.anyWriteLocked())
        return;

    globals_->joinLock_     = true;
    globals_->joinMaster_   = true;
    globals_->joining_      = sender;

    bool backout = false;
    list<Host *> backoutHosts;
    for (auto& pair : globals_->hostMap_) {
        bool result = pair.second.getJoinLock(sender);
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

bool DFSHandler::getJoinLock(const HostID& sender, const HostID& newServer) {
    if (checkForDead(sender)) return false;
    cerr << "JoinLock requested from " << sender.hostname << ":" << sender.port << endl;
    if (globals_->joinLock_)
        return false;
    globals_->joinLock_ = true;
    globals_->joining_ = newServer;
    return true;
}

void DFSHandler::ffit2ffi(const FUSEFileInfoTransport& ffit, fuse_file_info& ffi) {
    // I hate myself a little bit for having to do this.
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

void DFSHandler::releasedir(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi, const int64_t counter) {
    announceOperation("releasedir", sender, path);
    counterCheck();
    if (checkForDead(sender)) return;

    string cpath = convert(path);
    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    local_releasedir(cpath.c_str(), &ffi, fh);
}

void DFSHandler::mkdir(const HostID& sender, const std::string& path, const int32_t mode, const int64_t counter) {
    announceOperation("mkdir", sender, path);
    counterCheck();
    if (checkForDead(sender)) return;
    string cpath = convert(path);
    local_mkdir(cpath.c_str(), mode);
}

void DFSHandler::unlink(const HostID& sender, const std::string& path, const int64_t counter) {
    announceOperation("unlink", sender, path);
    counterCheck();
    if (checkForDead(sender)) return;
    string cpath = convert(path);
    local_unlink(cpath.c_str());
}

void DFSHandler::rmdir(const HostID& sender, const std::string& path, const int64_t counter) {
    announceOperation("rmdir", sender, path);
    counterCheck();
    if (checkForDead(sender)) return;

    string cpath = convert(path);
    local_rmdir(cpath.c_str());
}

void DFSHandler::symlink(const HostID& sender, const std::string& from, const std::string& to, const int64_t counter) {
    announceOperation("symlink", sender, from, to);
    counterCheck();
    if (checkForDead(sender)) return;
    string cfrom = convert(from);
    string cto = convert(to);
    local_symlink(cfrom.c_str(), cto.c_str());
}

void DFSHandler::rename(const HostID& sender, const std::string& from, const std::string& to, const int64_t counter) {
    announceOperation("rename", sender, from, to);
    counterCheck();
    if (checkForDead(sender)) return;
    string cfrom = convert(from);
    string cto = convert(to);
    local_rename(cfrom.c_str(), cto.c_str());
}

void DFSHandler::link(const HostID& sender, const std::string& from, const std::string& to, const int64_t counter) {
    announceOperation("link", sender, from, to);
    counterCheck();
    if (checkForDead(sender)) return;
    string cfrom = convert(from);
    string cto = convert(to);
    local_link(cfrom.c_str(), cto.c_str());
}

void DFSHandler::chmod(const HostID& sender, const std::string& path, const int32_t mode, const int64_t counter) {
    announceOperation("chmod", sender, path);
    counterCheck();
    if (checkForDead(sender)) return;
    string cpath = convert(path);
    local_chmod(cpath.c_str(), mode);
}

void DFSHandler::chown(const HostID& sender, const std::string& path, const int32_t uid, const int32_t gid, const int64_t counter) {
    announceOperation("chown", sender, path);
    counterCheck();
    if (checkForDead(sender)) return;
    string cpath = convert(path);
    local_chown(cpath.c_str(), uid, gid);
}

void DFSHandler::truncate(const HostID& sender, const std::string& path, const int64_t size, const int64_t counter) {
    announceOperation("truncate", sender, path);
    counterCheck();
    if (checkForDead(sender)) return;
    string cpath = convert(path);
    local_truncate(cpath.c_str(), size);
}

void DFSHandler::ftruncate(const HostID& sender, const std::string& path, const int64_t size, const FUSEFileInfoTransport& fi, const int64_t counter) {
    announceOperation("ftruncate", sender, path);
    counterCheck();
    if (checkForDead(sender)) return;
    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_ftruncate(cpath.c_str(), size, &ffi, fh);
}

void DFSHandler::create(const HostID& sender, const std::string& path, const int32_t mode, const FUSEFileInfoTransport& fi, const int64_t counter) {

    announceOperation("create", sender, path);
    counterCheck();
    if (checkForDead(sender)) return;

    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_create(cpath.c_str(), mode, &ffi, fh);
}

void DFSHandler::write(const HostID& sender, const std::string& path, const std::vector<int8_t> & buf, const int64_t size, const int64_t offset, const FUSEFileInfoTransport& fi, const int64_t counter) {
    announceOperation("write", sender, path);
    counterCheck();
    if (checkForDead(sender)) return;
    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    char * newbuf = (char *) (new char [size]);
    copy(buf.begin(), buf.end(), newbuf);
    string cpath = convert(path);
    local_write(cpath.c_str(), newbuf, size, offset, &ffi, fh);
    delete [] newbuf;
}

void DFSHandler::flush(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi, const int64_t counter) {
    announceOperation("flush", sender, path);
    counterCheck();
    if (checkForDead(sender)) return;
    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_flush(cpath.c_str(), &ffi, fh);
}

void DFSHandler::release(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi, const int64_t counter) {
    announceOperation("release", sender, path);
    counterCheck();
    if (checkForDead(sender)) return;

    fuse_file_info ffi;
    ffit2ffi(fi, ffi);

    // This way we can close invalid filehandles without bad things happening.
    if (!globals_->fhMap_.count(fi.fh))
        return;

    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_release(cpath.c_str(), &ffi, fh);
}

void DFSHandler::flock(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi, const int64_t op, const int64_t counter) {
    announceOperation("flock", sender, path);
    counterCheck();
    if (checkForDead(sender)) return;
    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_flock(cpath.c_str(), &ffi, op, fh);
}

void DFSHandler::fallocate(const HostID& sender, const std::string& path, const int64_t mode, const int64_t offset, const int64_t length, const FUSEFileInfoTransport& fi, const int64_t counter) {
    announceOperation("fallocate", sender, path);
    counterCheck();
    if (checkForDead(sender)) return;
#ifdef HAVE_POSIX_FALLOCATE
    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_fallocate(cpath.c_str(), mode, offset, length, &ffi, fh);
#endif
}

bool DFSHandler::fsync(const HostID& sender, const std::string& path, const int32_t isdatasync, const FUSEFileInfoTransport& fi, const int64_t counter) {
    announceOperation("fsync", sender, path);
    counterUpdate();
    if (checkForDead(sender)) return false;
    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_fsync(cpath.c_str(), isdatasync, &ffi, fh);
    // TODO
    return true;
}

bool DFSHandler::open(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi, const int64_t counter) {
    announceOperation("open", sender, path);
    counterUpdate();
    if (checkForDead(sender)) return false;

    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_open(cpath.c_str(), &ffi, fh);
    return true;
}

bool DFSHandler::opendir(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi, const int64_t counter) {

    announceOperation("opendir", sender, path);
    counterUpdate();
    if (checkForDead(sender)) return false;

    fuse_file_info ffi;
    ffit2ffi(fi, ffi);
    uint64_t& fh(globals_->fhMap_[fi.fh]);
    string cpath = convert(path);
    local_opendir(cpath.c_str(), &ffi, fh);
    return false;
}

void DFSHandler::utimens(const HostID& sender, const string& path, const TimeSpec& atime, const TimeSpec& mtime, const int64_t counter) {
    announceOperation("utimens", sender, path);
    counterCheck();
    if (checkForDead(sender)) return;


    string cpath = convert(path);

    timespec * ts = new timespec[2];
    ts[0].tv_sec  = atime.sec;
    ts[0].tv_nsec = atime.nsec;
    ts[1].tv_sec  = mtime.sec;
    ts[1].tv_nsec = mtime.nsec;

    local_utimens(cpath.c_str(), ts);

    delete [] ts;
}

boost::shared_ptr<TServer> server_;

void DFSServer::stop() {
    server_->stop();
}

void * DFSServer::start(void * arg) {
    GlobalBucket * globals = (GlobalBucket *) arg;
    int port = globals->me_.port;

    cerr << "Starting Thrift server..." << endl;

    boost::shared_ptr<DFSHandler> handler(new DFSHandler(globals));
    boost::shared_ptr<TProcessor> processor(new DFSProcessor(handler));
    boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
    /*
    boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());*/

    //server_.reset(new TSimpleServer(processor, serverTransport, transportFactory, protocolFactory));
    server_.reset(new TNonblockingServer(processor, protocolFactory, port));
    server_->serve();

    cerr << "Thrift server done!" << endl;

    globals->killall_();

    return NULL;
}

