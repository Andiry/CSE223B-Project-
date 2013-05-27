#ifndef DFS_SERVER_HPP_INC
#define DFS_SERVER_HPP_INC 1

#include "DFS.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include <cstdint>

class DFSHandler : virtual public DFS::DFSIf {
    public:
        DFSHandler();
        bool lock(const std::string& path, const std::string& hostname);
        bool unlock(const std::string& path, const std::string& hostname);
        void commit(const int64_t id, const std::string& hostname);
        void Bla();
        void Ping();
        void Pong();
        void dfs_remote_opendir(const std::string& hostname);
        void dfs_remote_readdir(const std::string& hostname);
        void dfs_remote_releasedir(const std::string& hostname);
        void dfs_remote_mkdir(const std::string& hostname);
        void dfs_remote_symlink(const std::string& hostname);
        void dfs_remote_unlink(const std::string& hostname);
        void dfs_remote_rmdir(const std::string& hostname);
        void dfs_remote_rename(const std::string& hostname);
        void dfs_remote_link(const std::string& hostname);
        void dfs_remote_chmod(const std::string& hostname);
        void dfs_remote_chown(const std::string& hostname);
        void dfs_remote_truncate(const std::string& hostname);
        void dfs_remote_ftruncate(const std::string& hostname);
        void dfs_remote_create(const std::string& hostname);
        void dfs_remote_open(const std::string& hostname);
        void dfs_remote_write(const std::string& hostname);
        void dfs_remote_flush(const std::string& hostname);
        void dfs_remote_release(const std::string& hostname);
        void dfs_remote_fsync(const std::string& hostname);
        void dfs_remote_fallocate(const std::string& hostname);
        void dfs_remote_lock(const std::string& hostname);
        void dfs_remote_flock(const std::string& hostname);
};

void * startServer(void * port);

#endif
