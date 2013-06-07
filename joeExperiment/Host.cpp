
#include "Host.hpp"

using namespace std;
using namespace DFS;

Host::Host() : state_(UNKNOWN), me_(NULL) {
    // Nothing else to do?
}

Host::Host(const HostID& id, const HostID& me)
    : id_(id), state_(ALIVE), me_(&me) {
    setup();
}

Host::Host(const string& hostname, int16_t port, State state, const HostID& me)
    : state_(state), me_(&me)
{
    id_.hostname = hostname;
    id_.port = port;

    setup();
}

Host::Host(const Host& rhs) : id_(rhs.id_), state_(rhs.state_), me_(rhs.me_) {
    setup();
}

void Host::setup() {
    if (state_ == ME || state_ == UNKNOWN)
        return;

    socket_.reset(new apache::thrift::transport::TSocket(id_.hostname, id_.port));
    transport_.reset(new apache::thrift::transport::TBufferedTransport(socket_));
    protocol_.reset(new apache::thrift::protocol::TBinaryProtocol(transport_));
    client_.reset(new DFSClient(protocol_));

    tryConnect();
}

bool Host::operator<(const Host& rhs) const {
    return (id_.hostname == rhs.id_.hostname) ?
        id_.port < rhs.id_.port : id_.hostname < rhs.id_.hostname;
}

bool Host::tryConnect() {
    if (state_ == ME)
        return true;
    if (state_ == UNKNOWN)
        return false;

    if(transport_->isOpen()) {
        state_ = ALIVE;
        return true;
    }

    cerr << "Trying to open connection to " << identifier() << endl;
    try {
        transport_->open();
    } catch (apache::thrift::TException& tx) {
        state_ = DEAD;
        return false;
    }

    state_ = ALIVE;
    return true;
}

void Host::kill() {
    if (state_ == ME)
        return;

    try {
        if(transport_ && transport_->isOpen())
            transport_->close();
    } catch (apache::thrift::TException& tx) {
        cerr << "Unable to close connection for " << identifier() << endl;
    }
}


#define PRECHECK(ON_ME, ON_FAIL)      \
    if (state_ == ME)      { ON_ME;   } \
    if (state_ == UNKNOWN) { ON_ME;   } \
    if (me_ == NULL)       { ON_FAIL; } \
    if (state_ != ALIVE)   { ON_FAIL; } \
    if (tryConnect()) {               \
        try {
#define POSTCHECK(ON_FAIL)                          \
        } catch (apache::thrift::TException &tx) {  \
            state_ = DEAD;                          \
            { ON_FAIL; }                            \
        }                                           \
    }


bool Host::ping() {
    PRECHECK(return true, return false);
    client_->ping(*me_);
    POSTCHECK(return false);
    return true;
}


void Host::unlock(const string& file) {
    PRECHECK(return, return);
    client_->unlock(*me_, file);
    POSTCHECK(return);
}

void Host::die() {
    cerr << "Telling " << identifier() << " to die." << endl;
    PRECHECK(, return);
    client_->die(*me_);
    POSTCHECK(return);
}

void Host::addServer(const DFS::HostID& newServer) {
    PRECHECK(return, return);
    client_->addServer(*me_, newServer);
    POSTCHECK(return);
}

void Host::releaseJoinLock() {
    PRECHECK(return, return);
    client_->releaseJoinLock(*me_);
    POSTCHECK(return);
}

bool Host::lock(const string& file, LockType::type type) {
    bool ret;
    PRECHECK(return true;, cerr << "First fail" << endl; return false);
    ret = client_->lock(*me_, file, type);
    cerr << "Succeeded to request lock on " << file << " from " << identifier() << endl;
    POSTCHECK(cerr << "Second fail" << endl; return false;);
    return ret;
}

void Host::join(set<DFS::HostID> & _return) {
    PRECHECK(return, return);
    client_->join(_return, *me_);
    POSTCHECK(return);
}

bool Host::requestJoinLock(string& _return) {
    PRECHECK(return false, return false);
    client_->requestJoinLock(_return, *me_);
    if(_return == "")
        return false;
    POSTCHECK(return false);
    return true;
}

bool Host::getJoinLock() {
    PRECHECK(return true, return false);
    return client_->getJoinLock(*me_);
    POSTCHECK(return false);
    return false;
}

void Host::releasedir(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    PRECHECK(return, return);
    client_->releasedir(*me_, path, fi);
    POSTCHECK(return);
}

void Host::mkdir(const string& path, const int32_t mode) {
    PRECHECK(return, return);
    client_->mkdir(*me_, path, mode);
    POSTCHECK(return);
}

void Host::unlink(const string& path) {
    PRECHECK(return, return);
    client_->unlink(*me_, path);
    POSTCHECK(return);
}

void Host::rmdir(const string& path) {
    PRECHECK(return, return);
    client_->rmdir(*me_, path);
    POSTCHECK(return);
}

void Host::symlink(const string& from, const string& to) {
    PRECHECK(return, return);
    client_->symlink(*me_, from, to);
    POSTCHECK(return);
}

void Host::rename(const string& from, const string& to) {
    PRECHECK(return, return);
    client_->rename(*me_, from, to);
    POSTCHECK(return);
}

void Host::link(const string& from, const string& to) {
    PRECHECK(return, return);
    client_->link(*me_, from, to);
    POSTCHECK(return);
}

void Host::chmod(const string& path, const int32_t mode) {
    PRECHECK(return, return);
    client_->chmod(*me_, path, mode);
    POSTCHECK(return);
}

void Host::chown(const string& path, const int32_t uid, const int32_t gid) {
    PRECHECK(return, return);
    client_->chown(*me_, path, uid, gid);
    POSTCHECK(return);
}

void Host::truncate(const string& path, const int64_t size) {
    PRECHECK(return, return);
    client_->truncate(*me_, path, size);
    POSTCHECK(return);
}

void Host::ftruncate(const string& path, const int64_t size, const DFS::FUSEFileInfoTransport& fi) {
    PRECHECK(return, return);
    client_->ftruncate(*me_, path, size, fi);
    POSTCHECK(return);
}

void Host::create(const string& path, const int32_t mode, const DFS::FUSEFileInfoTransport& fi) {
    PRECHECK(return, return);
    client_->create(*me_, path, mode, fi);
    POSTCHECK(return);
}

void Host::write(const string& path, const vector<int8_t> & buf, const int64_t size, const int64_t offset, const DFS::FUSEFileInfoTransport& fi) {
    PRECHECK(return, return);
    client_->write(*me_, path, buf, size, offset, fi);
    POSTCHECK(return);
}

void Host::flush(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    PRECHECK(return, return);
    client_->flush(*me_, path, fi);
    POSTCHECK(return);
}

void Host::release(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    PRECHECK(return, return);
    client_->release(*me_, path, fi);
    POSTCHECK(return);
}

void Host::flock(const string& path, const DFS::FUSEFileInfoTransport& fi, const int64_t op) {
    PRECHECK(return, return);
    client_->flock(*me_, path, fi, op);
    POSTCHECK(return);
}

void Host::fallocate(const string& path, const int64_t mode, const int64_t offset, const int64_t length, const DFS::FUSEFileInfoTransport& fi) {
    PRECHECK(return, return);
    client_->fallocate(*me_, path, mode, offset, length, fi);
    POSTCHECK(return);
}

bool Host::fsync(const string& path, const int32_t isdatasync, const DFS::FUSEFileInfoTransport& fi) {
    PRECHECK(return true, return false);
    return client_->fsync(*me_, path, isdatasync, fi);
    POSTCHECK(return false);
    return false;
}

bool Host::open(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    PRECHECK(return true, return false);
    return client_->open(*me_, path, fi);
    POSTCHECK(return false);
    return false;
}

bool Host::opendir(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    PRECHECK(return true, return false);
    return client_->opendir(*me_, path, fi);
    POSTCHECK(return false);
    return false;
}


