// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "DFS.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::DFS;

class DFSHandler : virtual public DFSIf {
 public:
  DFSHandler() {
    // Your initialization goes here
  }

  bool lock(const std::string& path, const std::string& hostname) {
    // Your implementation goes here
    printf("lock\n");
  }

  bool unlock(const std::string& path, const std::string& hostname) {
    // Your implementation goes here
    printf("unlock\n");
  }

  void commit(const int64_t id, const std::string& hostname) {
    // Your implementation goes here
    printf("commit\n");
  }

  void Bla() {
    // Your implementation goes here
    printf("Bla\n");
  }

  void Ping() {
    // Your implementation goes here
    printf("Ping\n");
  }

  void Pong() {
    // Your implementation goes here
    printf("Pong\n");
  }

  void dfs_doOperation(const std::string& operation, const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_doOperation\n");
  }

  void dfs_remote_readdir(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_readdir\n");
  }

  void dfs_remote_releasedir(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_releasedir\n");
  }

  void dfs_remote_mkdir(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_mkdir\n");
  }

  void dfs_remote_symlink(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_symlink\n");
  }

  void dfs_remote_unlink(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_unlink\n");
  }

  void dfs_remote_rmdir(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_rmdir\n");
  }

  void dfs_remote_rename(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_rename\n");
  }

  void dfs_remote_link(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_link\n");
  }

  void dfs_remote_chmod(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_chmod\n");
  }

  void dfs_remote_chown(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_chown\n");
  }

  void dfs_remote_truncate(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_truncate\n");
  }

  void dfs_remote_ftruncate(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_ftruncate\n");
  }

  void dfs_remote_create(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_create\n");
  }

  void dfs_remote_open(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_open\n");
  }

  void dfs_remote_write(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_write\n");
  }

  void dfs_remote_flush(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_flush\n");
  }

  void dfs_remote_release(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_release\n");
  }

  void dfs_remote_fsync(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_fsync\n");
  }

  void dfs_remote_fallocate(const std::string& hostname) {
    // Your implementation goes here
    printf("dfs_remote_fallocate\n");
  }

  void GetInfo(GetInfoResponse& _return, const std::string& key) {
    // Your implementation goes here
    printf("GetInfo\n");
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  shared_ptr<DFSHandler> handler(new DFSHandler());
  shared_ptr<TProcessor> processor(new DFSProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

