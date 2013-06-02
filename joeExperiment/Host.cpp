
#include "Host.hpp"

using namespace std;
using namespace DFS;

Host::Host() : state_(UNKNOWN), me_(NULL) {
    // Nothing else to do?
}

Host::Host(const HostID& id, const HostID& me)
    : id_(id), state_(JOINING), me_(&me) {
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


#define PRECHECK(ON_ME, ON_FAIL)      \
    if (me_ == NULL)     { ON_FAIL; } \
    if (state_ == ME)    { ON_ME;   } \
    if (state_ != ALIVE) { ON_FAIL; } \
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
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::die() {
    cerr << "Telling " << identifier() << " to die." << endl;
    PRECHECK(return, return);
    client_->die(*me_);
    POSTCHECK(return);
}

void Host::addServer(const DFS::HostID& newServer) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::releaseJoinLock() {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::lock(const string& file) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::join(set<DFS::HostID> & _return) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

bool Host::requestJoinLock() {
    // TODO
    PRECHECK(return false, return false);
    POSTCHECK(return false);
    return false;
}

bool Host::getJoinLock() {
    // TODO
    PRECHECK(return false, return false);
    POSTCHECK(return false);
    return false;
}

void Host::releasedir(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::mkdir(const string& path, const int32_t mode) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::unlink(const string& path) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::rmdir(const string& path) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::symlink(const string& from, const string& to) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::rename(const string& from, const string& to) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::link(const string& from, const string& to) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::chmod(const string& path, const int32_t mode) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::chown(const string& path, const int32_t uid, const int32_t gid) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::truncate(const string& path, const int64_t size) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::ftruncate(const string& path, const int64_t size, const DFS::FUSEFileInfoTransport& fi) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::create(const string& path, const int32_t mode, const DFS::FUSEFileInfoTransport& fi) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::write(const string& path, const vector<int8_t> & buf, const int64_t size, const int64_t offset, const DFS::FUSEFileInfoTransport& fi) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::flush(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::release(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::flock(const string& path, const DFS::FUSEFileInfoTransport& fi, const int64_t op) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

void Host::fallocate(const string& path, const int64_t mode, const int64_t offset, const int64_t length, const DFS::FUSEFileInfoTransport& fi) {
    // TODO
    PRECHECK(return, return);
    POSTCHECK(return);
}

bool Host::fsync(const string& path, const int32_t isdatasync, const DFS::FUSEFileInfoTransport& fi) {
    // TODO
    PRECHECK(return false, return false);
    POSTCHECK(return false);
    return false;
}

bool Host::open(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    // TODO
    PRECHECK(return false, return false);
    POSTCHECK(return false);
    return false;
}

bool Host::opendir(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    // TODO
    PRECHECK(return false, return false);
    POSTCHECK(return false);
    return false;
}


