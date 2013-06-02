
#include "DFS_server.hpp"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace ::DFS;
using namespace std;

DFSHandler::DFSHandler() {
  // Your initialization goes here
}

void DFSHandler::ping(const HostID& sender) {
  // Your implementation goes here
  printf("ping\n");
}

void DFSHandler::unlock(const HostID& sender, const std::string& file) {
  // Your implementation goes here
  printf("unlock\n");
}

void DFSHandler::die(const HostID& sender) {
  // Your implementation goes here
  printf("die\n");
}

void DFSHandler::addServer(const HostID& sender, const HostID& newServer) {
  // Your implementation goes here
  printf("addServer\n");
}

void DFSHandler::releaseJoinLock(const HostID& sender) {
  // Your implementation goes here
  printf("releaseJoinLock\n");
}

void DFSHandler::lock(const HostID& sender, const std::string& file) {
  // Your implementation goes here
  printf("lock\n");
}

void DFSHandler::join(std::set<HostID> & _return, const HostID& sender) {
  // Your implementation goes here
  printf("join\n");
}

bool DFSHandler::requestJoinLock(const HostID& sender) {
  // Your implementation goes here
  printf("requestJoinLock\n");
  return false;
}

bool DFSHandler::getJoinLock(const HostID& sender) {
  // Your implementation goes here
  printf("getJoinLock\n");
  return false;
}

void DFSHandler::releasedir(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi) {
  // Your implementation goes here
  printf("releasedir\n");
}

void DFSHandler::mkdir(const HostID& sender, const std::string& path, const int32_t mode) {
  // Your implementation goes here
  printf("mkdir\n");
}

void DFSHandler::unlink(const HostID& sender, const std::string& path) {
  // Your implementation goes here
  printf("unlink\n");
}

void DFSHandler::rmdir(const HostID& sender, const std::string& path) {
  // Your implementation goes here
  printf("rmdir\n");
}

void DFSHandler::symlink(const HostID& sender, const std::string& from, const std::string& to) {
  // Your implementation goes here
  printf("symlink\n");
}

void DFSHandler::rename(const HostID& sender, const std::string& from, const std::string& to) {
  // Your implementation goes here
  printf("rename\n");
}

void DFSHandler::link(const HostID& sender, const std::string& from, const std::string& to) {
  // Your implementation goes here
  printf("link\n");
}

void DFSHandler::chmod(const HostID& sender, const std::string& path, const int32_t mode) {
  // Your implementation goes here
  printf("chmod\n");
}

void DFSHandler::chown(const HostID& sender, const std::string& path, const int32_t uid, const int32_t gid) {
  // Your implementation goes here
  printf("chown\n");
}

void DFSHandler::truncate(const HostID& sender, const std::string& path, const int64_t size) {
  // Your implementation goes here
  printf("truncate\n");
}

void DFSHandler::ftruncate(const HostID& sender, const std::string& path, const int64_t size, const FUSEFileInfoTransport& fi) {
  // Your implementation goes here
  printf("ftruncate\n");
}

void DFSHandler::create(const HostID& sender, const std::string& path, const int32_t mode, const FUSEFileInfoTransport& fi) {
  // Your implementation goes here
  printf("create\n");
}

void DFSHandler::write(const HostID& sender, const std::string& path, const std::vector<int8_t> & buf, const int64_t size, const int64_t offset, const FUSEFileInfoTransport& fi) {
  // Your implementation goes here
  printf("write\n");
}

void DFSHandler::flush(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi) {
  // Your implementation goes here
  printf("flush\n");
}

void DFSHandler::release(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi) {
  // Your implementation goes here
  printf("release\n");
}

void DFSHandler::flock(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi, const int64_t op) {
  // Your implementation goes here
  printf("flock\n");
}

void DFSHandler::fallocate(const HostID& sender, const std::string& path, const int64_t mode, const int64_t offset, const int64_t length, const FUSEFileInfoTransport& fi) {
  // Your implementation goes here
  printf("fallocate\n");
}

bool DFSHandler::fsync(const HostID& sender, const std::string& path, const int32_t isdatasync, const FUSEFileInfoTransport& fi) {
  // Your implementation goes here
  printf("fsync\n");
  return false;
}

bool DFSHandler::open(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi) {
  // Your implementation goes here
  printf("open\n");
  return false;
}

bool DFSHandler::opendir(const HostID& sender, const std::string& path, const FUSEFileInfoTransport& fi) {
  // Your implementation goes here
  printf("opendir\n");
  return false;
}

boost::shared_ptr<TSimpleServer> server_;

void DFSServer::stop() {
    server_->stop();
}

void * DFSServer::start(void * arg) {
    GlobalBucket * globals = (GlobalBucket *) arg;
    int port = globals->me_.port;

    cerr << "Starting Thrift server..." << endl;

    boost::shared_ptr<DFSHandler> handler(new DFSHandler());
    boost::shared_ptr<TProcessor> processor(new DFSProcessor(handler));
    boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    server_.reset(new TSimpleServer(processor, serverTransport, transportFactory, protocolFactory));
    server_->serve();

    cerr << "Thrift server done!" << endl;

    return NULL;
}

