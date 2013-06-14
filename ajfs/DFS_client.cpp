#include "DFS.h"
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include <iostream>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace DFS;
using namespace std;

int main(int argc, char **argv) {
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " <ServerIP> <ServerPort> <MyServerIP> <MyServerPort>" << endl;
        exit(0);
    }
    string server = string(argv[1]);
    int port = atoi(argv[2]);
    boost::shared_ptr<TSocket> socket(new TSocket(server, port));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    DFSClient client(protocol);

    HostID hostId;
    hostId.hostname = string(argv[3]);
    hostId.port = atoi(argv[4]);

    try {
        transport->open();
        client.ping(hostId);
    } catch (TException &tx) {
        cout << "ERROR: " <<  tx.what() << endl;
    }
}

