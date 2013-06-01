
#include "Host.hpp"

using namespace std;
using namespace DFS;

Host::Host() : state_(UNKNOWN) {
    // Nothing else to do?
}

Host::Host(const HostID& id) : id_(id), state_(JOINING) {

    socket_.reset(new apache::thrift::transport::TSocket(id_.hostname, id_.port));
    transport_.reset(new apache::thrift::transport::TBufferedTransport(socket_));
    protocol_.reset(new apache::thrift::protocol::TBinaryProtocol(transport_));
    client_.reset(new DFSClient(protocol_));

    tryConnect();
}

Host::Host(const std::string& hostname, int16_t port, State state)
    : state_(state) 
{
    id_.hostname = hostname;
    id_.port = port;

    socket_.reset(new apache::thrift::transport::TSocket(id_.hostname, id_.port));
    transport_.reset(new apache::thrift::transport::TBufferedTransport(socket_));
    protocol_.reset(new apache::thrift::protocol::TBinaryProtocol(transport_));
    client_.reset(new DFSClient(protocol_));

    tryConnect();
}

Host::Host(const Host& rhs) : id_(rhs.id_), state_(rhs.state_) {
    // Nothing else to do
}

bool Host::operator<(const Host& rhs) const {
    return (id_.hostname == rhs.id_.hostname) ?
        id_.port < rhs.id_.port : id_.hostname < rhs.id_.hostname;
}

bool Host::tryConnect() {
    if(transport_->isOpen()) {
        state_ = ALIVE;
        return true;
    }

    try {
        transport_->open();
    } catch (apache::thrift::TException& tx) {
        state_ = DEAD;
        return false;
    }
    state_ = ALIVE;
    return true;
}

bool Host::ping() {
    if (state_ != ALIVE)
        return false;

    if (tryConnect()) {
        try {
            client_->ping(id_);
            return true;
        } catch (apache::thrift::TException &tx) {
            state_ = DEAD;
        }
    }
    return false;

}
