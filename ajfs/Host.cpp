
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

    counter_ = 0;
    farCounter_ = 0;

    pthread_mutex_init(&connectLock_, NULL);

    socket_.reset(new apache::thrift::transport::TSocket(id_.hostname, id_.port));
    transport_.reset(new apache::thrift::transport::TFramedTransport(socket_));
    protocol_.reset(new apache::thrift::protocol::TBinaryProtocol(transport_));
    client_.reset(new DFSClient(protocol_));

    tryConnect();
}

bool Host::operator<(const Host& rhs) const {
    return (id_.hostname == rhs.id_.hostname) ?
        id_.port < rhs.id_.port : id_.hostname < rhs.id_.hostname;
}

bool Host::tryConnect() {
    if (state_ == ME) {
        cerr << "Tried to open connection to myself. Skipping..." << endl;
        return true;
    }
    if (state_ == UNKNOWN) {
        cerr << "Tried to open connection to unknown host..." << endl;
        return false;
    }

    if(transport_->isOpen()) {
        state_ = ALIVE;
        return true;
    }

    cerr << "Trying to open connection to " << identifier();
    try {
        transport_->open();
    } catch (apache::thrift::TException& tx) {
        state_ = DEAD;
        cerr << "...failed" << endl;
        return false;
    }

    cerr << "...succeded" << endl;
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

#define TRY(ON_ME, ON_FAIL, ON_SUCCESS, ATTEMPT)    \
    if (state_ == ME)      { ON_ME;   }             \
    if (state_ == UNKNOWN) { ON_ME;   }             \
    if (me_ == NULL)       { ON_FAIL; }             \
    if (state_ != ALIVE)   { ON_FAIL; }             \
    int tryCount = 0;                               \
    bool trySuccess = false;                        \
    while (tryCount < 2 && tryConnect()) {          \
        pthread_mutex_lock(&connectLock_);              \
        try {                                       \
            { ATTEMPT; }                            \
            trySuccess = true;                      \
            pthread_mutex_unlock(&connectLock_);    \
            break;                                  \
        } catch (apache::thrift::TException &tx) {  \
            state_ = DEAD;                          \
        }                                           \
        pthread_mutex_unlock(&connectLock_);            \
        ++tryCount;                                 \
    }                                               \
    if (trySuccess) { ON_SUCCESS; }                 \
    else { ON_FAIL; }


bool Host::ping() {
    TRY(return true, return false, return true, client_->ping(*me_));
}

void Host::unlock(const string& file) {
    TRY(return, return, return, client_->unlock(*me_, file));
}

void Host::die() {
    cerr << "Telling " << identifier() << " to die." << endl;
    TRY(return, return, return, client_->die(*me_));
}

void Host::addServer(const DFS::HostID& newServer) {
    TRY(return, return, return, client_->addServer(*me_, newServer));
}

void Host::releaseJoinLock() {
    TRY(return, return, return, client_->releaseJoinLock(*me_));
}

bool Host::lock(const string& file, LockType::type type) {
    bool ret;
    TRY(return true, return true, return ret, ret = client_->lock(*me_, file, type));
}

void Host::join(set<DFS::HostID> & _return) {
    TRY(return, return, return, client_->join(_return, *me_));
}

bool Host::requestJoinLock(string& _return) {
    cerr << "Requesting JoinLock from " << identifier() << "...";
    TRY(cerr << "Failed (me)" << endl; return false, cerr << "Failed (ON_FAIL)" << endl; return true, if(_return == "") return false; else return true;, client_->requestJoinLock(_return, *me_));
    return true;
}

bool Host::getJoinLock(const DFS::HostID& newServer) {
    bool ret = false;
    TRY(return true, return true,, ret = client_->getJoinLock(*me_, newServer));
    return ret;
}

void Host::releasedir(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->releasedir(*me_, path, fi, counter));
}

void Host::mkdir(const string& path, const int32_t mode) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->mkdir(*me_, path, mode, counter));
}

void Host::unlink(const string& path) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->unlink(*me_, path, counter));
}

void Host::rmdir(const string& path) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->rmdir(*me_, path, counter));
}

void Host::symlink(const string& from, const string& to) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->symlink(*me_, from, to, counter));
}

void Host::rename(const string& from, const string& to) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->rename(*me_, from, to, counter));
}

void Host::link(const string& from, const string& to) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->link(*me_, from, to, counter));
}

void Host::chmod(const string& path, const int32_t mode) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->chmod(*me_, path, mode, counter));
}

void Host::chown(const string& path, const int32_t uid, const int32_t gid) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->chown(*me_, path, uid, gid, counter));
}

void Host::truncate(const string& path, const int64_t size) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->truncate(*me_, path, size, counter));
}

void Host::ftruncate(const string& path, const int64_t size, const DFS::FUSEFileInfoTransport& fi) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->ftruncate(*me_, path, size, fi, counter));
}

void Host::create(const string& path, const int32_t mode, const DFS::FUSEFileInfoTransport& fi) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->create(*me_, path, mode, fi, counter));
}

void Host::write(const string& path, const vector<int8_t> & buf, const int64_t size, const int64_t offset, const DFS::FUSEFileInfoTransport& fi) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->write(*me_, path, buf, size, offset, fi, counter));
}

void Host::flush(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->flush(*me_, path, fi, counter));
}

void Host::release(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->release(*me_, path, fi, counter));
}

void Host::flock(const string& path, const DFS::FUSEFileInfoTransport& fi, const int64_t op) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->flock(*me_, path, fi, op, counter));
}

void Host::fallocate(const string& path, const int64_t mode, const int64_t offset, const int64_t length, const DFS::FUSEFileInfoTransport& fi) {
    int64_t counter = ++counter_;
    TRY(return, return, return, client_->fallocate(*me_, path, mode, offset, length, fi, counter));
}

bool Host::fsync(const string& path, const int32_t isdatasync, const DFS::FUSEFileInfoTransport& fi) {
    int64_t counter = ++counter_;
    bool ret = false;
    TRY(return true, return false, , ret = client_->fsync(*me_, path, isdatasync, fi, counter));
    return ret;
}

bool Host::open(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    int64_t counter = ++counter_;
    bool ret = false;
    TRY(return true, return false, , ret = client_->open(*me_, path, fi, counter));
    return ret;
}

bool Host::opendir(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    int64_t counter = ++counter_;
    bool ret = false;
    TRY(return true, return false, , ret = client_->opendir(*me_, path, fi, counter));
    return ret;
}

void Host::utimens(const string& path, const TimeSpec& atime, const TimeSpec& mtime) {
    int64_t counter = ++counter_;
    TRY(return, return, , client_->utimens(*me_, path, atime, mtime, counter));
}


