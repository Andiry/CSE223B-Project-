
namespace cpp DFS

enum DFS_status {
	OK = 1,
	EKEYNOTFOUND = 2,
	EITEMNOTFOUND = 3, // item not found in lists
	EPUTFAILED = 4,
	EITEMEXISTS = 5, // duplicate in lists
	INTERNAL_FAILURE = 6,
	NOT_IMPLEMENTED = 7
}


struct GetInfoResponse {
	1: DFS_status status,
	2: list<string> values
}


service DFS {
    
    bool   lock(1:string path, 2:string hostname),
    bool unlock(1:string path, 2:string hostname),

    oneway void commit(1:i64 id, 2:string hostname)

    oneway void Bla(),

    oneway void Ping(),
    oneway void Pong(),

	oneway void dfs_doOperation(1:string operation, 2:string path, 3:i32 mode, 4:i32 flags),
	oneway void dfs_remote_readdir(1:string hostname),
	oneway void dfs_remote_releasedir(1:string hostname),
	oneway void dfs_remote_mkdir(1:string hostname),
	oneway void dfs_remote_symlink(1:string hostname),
	oneway void dfs_remote_unlink(1:string hostname),
	oneway void dfs_remote_rmdir(1:string hostname),
	oneway void dfs_remote_rename(1:string hostname),
	oneway void dfs_remote_link(1:string hostname),
	oneway void dfs_remote_chmod(1:string hostname),
	oneway void dfs_remote_chown(1:string hostname),
	oneway void dfs_remote_truncate(1:string hostname),
	oneway void dfs_remote_ftruncate(1:string hostname),
	oneway void dfs_remote_create(1:string hostname),
	oneway void dfs_remote_open(1:string hostname),
	oneway void dfs_remote_write(1:string hostname),

	oneway void dfs_remote_flush(1:string hostname),
	oneway void dfs_remote_release(1:string hostname),
	oneway void dfs_remote_fsync(1:string hostname),
#ifdef HAVE_POSIX_FALLOCATE
	oneway void dfs_remote_fallocate(1:string hostname),
#endif
	GetInfoResponse GetInfo(1:string key)
}
