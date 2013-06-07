#ifndef DFS_SERVER_HPP_INC
#define DFS_SERVER_HPP_INC 1

#include "DFS.h"
#include "GlobalBucket.hpp"
#include "LFS_operations.hpp"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TNonblockingServer.h>
//#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include <cstdint>

#include <list>
#include <sstream>

class DFSHandler : virtual public DFS::DFSIf {
  private:
    GlobalBucket * globals_;
    bool checkForDead(const DFS::HostID& sender);
    void ffit2ffi(const DFS::FUSEFileInfoTransport& ffit, fuse_file_info& ffi);
    std::string convert(const std::string& path);

    inline void announceOperation(const std::string& oper, 
                                  const DFS::HostID& sender,
                                  const std::string& path) {
        globals_->debug_ << ">> REMOTE >>>>>>>>> "
                         << sender.hostname << ":" << sender.port
                         << ": " << oper << " on " << path << std::endl;
    }
    inline void announceOperation(const std::string& oper, 
                                  const DFS::HostID& sender,
                                  const std::string& from,
                                  const std::string& to) {
        globals_->debug_ << ">> REMOTE >>>>>>>>> "
                         << sender.hostname << ":" << sender.port
                         << ": " << oper
                         << " from " << from << " to " << to << std::endl;
    }

  public:
    DFSHandler(GlobalBucket* globals);
    void ping(const DFS::HostID& sender);    
    void unlock(const DFS::HostID& sender, const std::string& file);    
    void die(const DFS::HostID& sender);    
    void addServer(const DFS::HostID& sender, const DFS::HostID& newServer);    
    void releaseJoinLock(const DFS::HostID& sender);    
    bool lock(const DFS::HostID& sender, const std::string& file, const DFS::LockType::type lockType);
    void join(std::set<DFS::HostID> & _return, const DFS::HostID& sender);    
    void requestJoinLock(std::string & _return, const DFS::HostID& sender);    
    bool getJoinLock(const DFS::HostID& sender);    
    void releasedir(const DFS::HostID& sender, const std::string& path, const DFS::FUSEFileInfoTransport& fi);    
    void mkdir(const DFS::HostID& sender, const std::string& path, const int32_t mode);    
    void unlink(const DFS::HostID& sender, const std::string& path);    
    void rmdir(const DFS::HostID& sender, const std::string& path);    
    void symlink(const DFS::HostID& sender, const std::string& from, const std::string& to);    
    void rename(const DFS::HostID& sender, const std::string& from, const std::string& to);    
    void link(const DFS::HostID& sender, const std::string& from, const std::string& to);    
    void chmod(const DFS::HostID& sender, const std::string& path, const int32_t mode);    
    void chown(const DFS::HostID& sender, const std::string& path, const int32_t uid, const int32_t gid);    
    void truncate(const DFS::HostID& sender, const std::string& path, const int64_t size);    
    void ftruncate(const DFS::HostID& sender, const std::string& path, const int64_t size, const DFS::FUSEFileInfoTransport& fi);    
    void create(const DFS::HostID& sender, const std::string& path, const int32_t mode, const DFS::FUSEFileInfoTransport& fi);    
    void write(const DFS::HostID& sender, const std::string& path, const std::vector<int8_t> & buf, const int64_t size, const int64_t offset, const DFS::FUSEFileInfoTransport& fi);    
    void flush(const DFS::HostID& sender, const std::string& path, const DFS::FUSEFileInfoTransport& fi);    
    void release(const DFS::HostID& sender, const std::string& path, const DFS::FUSEFileInfoTransport& fi);    
    void flock(const DFS::HostID& sender, const std::string& path, const DFS::FUSEFileInfoTransport& fi, const int64_t op);    
    void fallocate(const DFS::HostID& sender, const std::string& path, const int64_t mode, const int64_t offset, const int64_t length, const DFS::FUSEFileInfoTransport& fi);    
    bool fsync(const DFS::HostID& sender, const std::string& path, const int32_t isdatasync, const DFS::FUSEFileInfoTransport& fi);    
    bool open(const DFS::HostID& sender, const std::string& path, const DFS::FUSEFileInfoTransport& fi);    
    bool opendir(const DFS::HostID& sender, const std::string& path, const DFS::FUSEFileInfoTransport& fi);
    void utimens(const DFS::HostID& sender, const std::string& path, const DFS::TimeSpec& atime, const DFS::TimeSpec& mtime);

};

namespace DFSServer {
    void stop();
    void * start(void * port);
}

#endif
