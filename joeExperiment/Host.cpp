
#include "Host.hpp"

using namespace std;
using namespace DFS;

Host::Host() : state_(UNKNOWN) {
    // Nothing else to do?
}

Host::Host(const HostID& id) : id_(id), state_(JOINING) {
    setup();
}

Host::Host(const std::string& hostname, int16_t port, State state)
    : state_(state) 
{
    id_.hostname = hostname;
    id_.port = port;

    setup();
}

Host::Host(const Host& rhs) : id_(rhs.id_), state_(rhs.state_) {
    setup();
}

string Host::identifier() const {
    return id_.hostname + ":" + to_string(id_.port);
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

#define PRECHECK(ON_ME, ON_FAIL)        \
    if (state_ == ME) { ON_ME; }        \
    if (state_ != ALIVE) { ON_FAIL; }   \
    if (tryConnect()) {                 \
        try {           
#define POSTCHECK(ON_FAIL)                          \
        } catch (apache::thrift::TException &tx) {  \
            state_ = DEAD;                          \
        }                                           \
    }                                               \
    { ON_FAIL; }

bool Host::ping() {
    cerr << "Pinging " << id_.hostname << ":" << id_.port << endl;
    PRECHECK(return true, return false);
    client_->ping(id_);
    POSTCHECK(return false);
    return true;
}
