#ifndef HOST_HPP_INC
#define HOST_HPP_INC 1

#include "DFS.h"
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include <map>

class Host {
  public:

    enum State { ME, JOINING, ALIVE, DEAD, UNKNOWN };

    DFS::HostID id_;
    State state_;

    Host();
    Host(const DFS::HostID& id);
    Host(const std::string& hostname, int16_t port, State state);
    Host(const Host& rhs);

    bool operator<(const Host& rhs) const;

    bool ping();

  private:
    bool tryConnect();

    boost::shared_ptr<apache::thrift::transport::TSocket> socket_;
    boost::shared_ptr<apache::thrift::transport::TTransport> transport_;
    boost::shared_ptr<apache::thrift::protocol::TProtocol> protocol_;
    boost::shared_ptr<DFS::DFSClient> client_;
};

typedef std::map<DFS::HostID, Host> HostMap_t;

#endif
