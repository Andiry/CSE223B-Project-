#ifndef DFS_SERVER_HPP_INC
#define DFS_SERVER_HPP_INC 1

#include "DFS.h"
#include "rpc_types.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include <cstdint>
#include <vector>
#include <map>
#include <stdlib.h>

#ifndef _ARG_STRUCT_
#define _ARG_STRUCT_
struct ArgStruct {
    int argc;
    char **argv;
};
#endif

class DFSHandler : virtual public DFS::DFSIf {
    public:
        DFSHandler(int argc, char** argv);
        bool lock(const std::string& path, const std::string& hostname);
        bool unlock(const std::string& path, const std::string& hostname);
        void commit(const int64_t id, const std::string& hostname);
        void Bla();
        void Ping();
        void Pong();
        void dfs_doOperation(const std::string& operation, const std::string& hostname);
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

//	void GetInfo(GetInfoResponse& _return, const std::string& key);

	/* Request send to other servers */
	void RsyncWithOtherServers(void);
 
	int _id;
	std::vector < std::pair<std::string, int> > _backendServerVector;
	std::string _backup_path;
};

void * startServer(void * port);

#endif
