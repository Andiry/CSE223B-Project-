
#include "Host.hpp"
    std::mt19937_64 randGen_;

using namespace std;
using namespace DFS;

Host::Host() : state_(UNKNOWN), me_(NULL) {
    // Nothing else to do?
}

Host::Host(const HostID& id, const HostID& me)
    : id_(id), state_(ALIVE), me_(&me) {
    randGen_.seed(chrono::system_clock::now().time_since_epoch().count());
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

    randGen_.seed(chrono::system_clock::now().time_since_epoch().count());

    socket_.reset(new apache::thrift::transport::TSocket(id_.hostname, id_.port));
    transport_.reset(new apache::thrift::transport::TFramedTransport(socket_));
    //transport_.reset(new apache::thrift::transport::TBufferedTransport(socket_));
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

#define TRY(ON_ME, ON_FAIL, ON_SUCCESS, ATTEMPT)    \
    if (state_ == ME)      { ON_ME;   }             \
    if (state_ == UNKNOWN) { ON_ME;   }             \
    if (me_ == NULL)       { ON_FAIL; }             \
    if (state_ != ALIVE)   { ON_FAIL; }             \
    int tryCount = 0; bool trySuccess = false;      \
    while (tryCount < 2 && tryConnect()) {          \
        try {                                       \
            { ATTEMPT; }                            \
            trySuccess = true; break;               \
        } catch (apache::thrift::TException &tx) {  \
            state_ = DEAD;                          \
        }                                           \
        ++tryCount;                                 \
    }                                               \
    if (trySuccess) { ON_SUCCESS; } else {ON_FAIL;} \


bool Host::ping() {
    //#define TRY(ON_ME, ON_FAIL, ON_SUCCESS, ATTEMPT)
    TRY(return true, return false, return true, client_->ping(*me_));
    /*
    PRECHECK(return true, return false);
    client_->ping(*me_);
    POSTCHECK(return false);
    return true;
    */
}

void Host::unlock(const string& file) {
    TRY(return, return, return, client_->unlock(*me_, file));
    //PRECHECK(return, return);
    //client_->unlock(*me_, file);
    //POSTCHECK(return);
}

void Host::die() {
    cerr << "Telling " << identifier() << " to die." << endl;
    TRY(return, return, return, client_->die(*me_));
    //PRECHECK(, return);
    //client_->die(*me_);
    //POSTCHECK(return);
}

void Host::addServer(const DFS::HostID& newServer) {
    //PRECHECK(return, return);
    //client_->addServer(*me_, newServer);
    //POSTCHECK(return);
    TRY(return, return, return, client_->addServer(*me_, newServer));
}

void Host::releaseJoinLock() {
    //PRECHECK(return, return);
    //client_->releaseJoinLock(*me_);
    //POSTCHECK(return);
    TRY(return, return, return, client_->releaseJoinLock(*me_));
}

bool Host::lock(const string& file, LockType::type type) {
    bool ret;
    TRY(return true, return true, return ret, ret = client_->lock(*me_, file, type));
    //PRECHECK(return true;, cerr << "First fail" << endl; return true);
    //ret = client_->lock(*me_, file, type);
    //cerr << "Succeeded to request lock on " << file << " from " << identifier() << endl;
    //POSTCHECK(cerr << "Second lock fail" << endl; return true;);
    //return ret;
}

void Host::join(set<DFS::HostID> & _return) {
    //TRY(ON_ME, ON_FAIL, ON_SUCCESS, ATTEMPT)
    //PRECHECK(return, return);
    //client_->join(_return, *me_);
    //POSTCHECK(return);
    TRY(return, return, return, client_->join(_return, *me_));
}

bool Host::requestJoinLock(string& _return) {
    cerr << "Requesting JoinLock from " << identifier() << "...";
    TRY(cerr << "Failed (me)" << endl; return false, cerr << "Failed (ON_FAIL)" << endl; return true, if(_return == "") return false; else return true;, client_->requestJoinLock(_return, *me_));
    //PRECHECK(cerr << "Failed (me)" << endl; return false, cerr << "Failed (ON_FAIL)" << endl; return true);
    //client_->requestJoinLock(_return, *me_);
    //if(_return == "")
    //    return false;
    //POSTCHECK(return true);
    return true;
}

bool Host::getJoinLock(const DFS::HostID& newServer) {
    TRY(return true, return true,, return client_->getJoinLock(*me_, newServer));
    //PRECHECK(return true, return true);
    //return client_->getJoinLock(*me_, newServer);
    //POSTCHECK(return true);
    return false;
}

void Host::releasedir(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    TRY(return, return, return, client_->releasedir(*me_, path, fi, randGen_()));
    //PRECHECK(return, return);
    //client_->releasedir(*me_, path, fi);
    //POSTCHECK(return);
}

void Host::mkdir(const string& path, const int32_t mode) {
    TRY(return, return, return, client_->mkdir(*me_, path, mode, randGen_()));
    //PRECHECK(return, return);
    //client_->mkdir(*me_, path, mode);
    //POSTCHECK(return);
}

void Host::unlink(const string& path) {
    TRY(return, return, return, client_->unlink(*me_, path, randGen_()));
    //PRECHECK(return, return);
    //client_->unlink(*me_, path);
    //POSTCHECK(return);
}

void Host::rmdir(const string& path) {
    TRY(return, return, return, client_->rmdir(*me_, path, randGen_()));
    //PRECHECK(return, return);
    //client_->rmdir(*me_, path);
    //POSTCHECK(return);
}

void Host::symlink(const string& from, const string& to) {
    //PRECHECK(return, return);
    //client_->symlink(*me_, from, to);
    //POSTCHECK(return);
    TRY(return, return, return, client_->symlink(*me_, from, to, randGen_()));
}

void Host::rename(const string& from, const string& to) {
    //PRECHECK(return, return);
    //client_->rename(*me_, from, to);
    //POSTCHECK(return);
    TRY(return, return, return, client_->rename(*me_, from, to, randGen_()));
}

void Host::link(const string& from, const string& to) {
    //PRECHECK(return, return);
    //client_->link(*me_, from, to);
    //POSTCHECK(return);
    TRY(return, return, return, client_->link(*me_, from, to, randGen_()));
}

void Host::chmod(const string& path, const int32_t mode) {
    //PRECHECK(return, return);
    //client_->chmod(*me_, path, mode);
    //POSTCHECK(return);
    TRY(return, return, return, client_->chmod(*me_, path, mode, randGen_()));
}

void Host::chown(const string& path, const int32_t uid, const int32_t gid) {
    //PRECHECK(return, return);
    //client_->chown(*me_, path, uid, gid);
    //POSTCHECK(return);
    TRY(return, return, return, client_->chown(*me_, path, uid, gid, randGen_()));
}

void Host::truncate(const string& path, const int64_t size) {
    //PRECHECK(return, return);
    //client_->truncate(*me_, path, size);
    //POSTCHECK(return);
    TRY(return, return, return, client_->truncate(*me_, path, size, randGen_()));
}

void Host::ftruncate(const string& path, const int64_t size, const DFS::FUSEFileInfoTransport& fi) {
    //PRECHECK(return, return);
    //client_->ftruncate(*me_, path, size, fi);
    //POSTCHECK(return);
    TRY(return, return, return, client_->ftruncate(*me_, path, size, fi, randGen_()));
}

void Host::create(const string& path, const int32_t mode, const DFS::FUSEFileInfoTransport& fi) {
    //PRECHECK(return, return);
    //client_->create(*me_, path, mode, fi);
    //POSTCHECK(return);
    TRY(return, return, return, client_->create(*me_, path, mode, fi, randGen_()));
}

void Host::write(const string& path, const vector<int8_t> & buf, const int64_t size, const int64_t offset, const DFS::FUSEFileInfoTransport& fi) {
    //PRECHECK(return, return);
    //client_->write(*me_, path, buf, size, offset, fi);
    //POSTCHECK(return);
    TRY(return, return, return, client_->write(*me_, path, buf, size, offset, fi, randGen_()));
}

void Host::flush(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    //PRECHECK(return, return);
    //client_->flush(*me_, path, fi);
    //POSTCHECK(return);
    TRY(return, return, return, client_->flush(*me_, path, fi, randGen_()));
}

void Host::release(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    //PRECHECK(return, return);
    //client_->release(*me_, path, fi);
    //POSTCHECK(return);
    TRY(return, return, return, client_->release(*me_, path, fi, randGen_()));
}

void Host::flock(const string& path, const DFS::FUSEFileInfoTransport& fi, const int64_t op) {
    //PRECHECK(return, return);
    //client_->flock(*me_, path, fi, op);
    //POSTCHECK(return);
    TRY(return, return, return, client_->flock(*me_, path, fi, op, randGen_()));
}

void Host::fallocate(const string& path, const int64_t mode, const int64_t offset, const int64_t length, const DFS::FUSEFileInfoTransport& fi) {
    //PRECHECK(return, return);
    //client_->fallocate(*me_, path, mode, offset, length, fi);
    //POSTCHECK(return);
    TRY(return, return, return, client_->fallocate(*me_, path, mode, offset, length, fi, randGen_()));
}

bool Host::fsync(const string& path, const int32_t isdatasync, const DFS::FUSEFileInfoTransport& fi) {
    //PRECHECK(return true, return false);
    //return client_->fsync(*me_, path, isdatasync, fi);
    //POSTCHECK(return true);
    TRY(return true, return false, , return client_->fsync(*me_, path, isdatasync, fi, randGen_()));
    return false;
}

bool Host::open(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    //PRECHECK(return true, return false);
    //return client_->open(*me_, path, fi);
    //POSTCHECK(return true);
    TRY(return true, return false, , return client_->open(*me_, path, fi, randGen_()));
    return false;
}

bool Host::opendir(const string& path, const DFS::FUSEFileInfoTransport& fi) {
    //PRECHECK(return true, return false);
    //return client_->opendir(*me_, path, fi);
    //POSTCHECK(return true);
    TRY(return true, return false, , return client_->opendir(*me_, path, fi, randGen_()));
    return false;
}

void Host::utimens(const string& path, const TimeSpec& atime, const TimeSpec& mtime) {
    //PRECHECK(return, return);
    //client_->utimens(*me_, path, atime, mtime);
    //POSTCHECK(return);
    TRY(return, return, , client_->utimens(*me_, path, atime, mtime, randGen_()));
}


