/* AJFS - a simple distributed file system.
 *
 * By Joe DeBlasio and Andiry Xu
 * For UCSD CSE223B, SP13
 * */
#ifndef HOST_HPP_INC
#define HOST_HPP_INC 1

#include "DFS.h"
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include <map>

class Host {
  public:

    enum State { ME, /*JOINING,*/ ALIVE, DEAD, UNKNOWN };

    DFS::HostID id_;
    State state_;

    const DFS::HostID * me_;

    Host();
    Host(const DFS::HostID& id, const DFS::HostID& me);
    Host(const std::string& hostname, int16_t port, State state, const DFS::HostID& me);
    Host(const Host& rhs);

    bool operator<(const Host& rhs) const;
    inline std::string identifier() const {
        return id_.hostname + ":" + std::to_string(id_.port);
    }

    int64_t farCounter_;

    bool ping();

    void unlock(const std::string& file);
    void die();
    void addServer(const DFS::HostID& newServer);
    void releaseJoinLock();
    bool lock(const std::string& file, DFS::LockType::type type);
    void join(std::set<DFS::HostID> & _return);
    bool requestJoinLock(std::string& path);
    bool getJoinLock(const DFS::HostID& newServer);
    void releasedir(const std::string& path, const DFS::FUSEFileInfoTransport& fi);
    void mkdir(const std::string& path, const int32_t mode);
    void unlink(const std::string& path);
    void rmdir(const std::string& path);
    void symlink(const std::string& from, const std::string& to);
    void rename(const std::string& from, const std::string& to);
    void link(const std::string& from, const std::string& to);
    void chmod(const std::string& path, const int32_t mode);
    void chown(const std::string& path, const int32_t uid, const int32_t gid);
    void truncate(const std::string& path, const int64_t size);
    void ftruncate(const std::string& path, const int64_t size, const DFS::FUSEFileInfoTransport& fi);
    void create(const std::string& path, const int32_t mode, const DFS::FUSEFileInfoTransport& fi);
    void write(const std::string& path, const std::vector<int8_t> & buf, const int64_t size, const int64_t offset, const DFS::FUSEFileInfoTransport& fi);
    void flush(const std::string& path, const DFS::FUSEFileInfoTransport& fi);
    void release(const std::string& path, const DFS::FUSEFileInfoTransport& fi);
    void flock(const std::string& path, const DFS::FUSEFileInfoTransport& fi, const int64_t op);
    void fallocate(const std::string& path, const int64_t mode, const int64_t offset, const int64_t length, const DFS::FUSEFileInfoTransport& fi);
    bool fsync(const std::string& path, const int32_t isdatasync, const DFS::FUSEFileInfoTransport& fi);
    bool open(const std::string& path, const DFS::FUSEFileInfoTransport& fi);
    bool opendir(const std::string& path, const DFS::FUSEFileInfoTransport& fi);
    void utimens(const std::string& path, const DFS::TimeSpec& atime, const DFS::TimeSpec& mtime);

    void kill();

  private:
    void setup();
    bool tryConnect();

    boost::shared_ptr<apache::thrift::transport::TSocket> socket_;
    boost::shared_ptr<apache::thrift::transport::TTransport> transport_;
    boost::shared_ptr<apache::thrift::protocol::TProtocol> protocol_;
    boost::shared_ptr<DFS::DFSClient> client_;
    
    int64_t counter_;
    pthread_mutex_t connectLock_;
};

#endif
