
namespace cpp DFS

service DFS {
    
    bool   lock(1:string path, 2:string hostname),
    bool unlock(1:string path, 2:string hostname),

    oneway void commit(1:i64 id, 2:string hostname)

    oneway void Bla(),

    oneway void Ping(),
    oneway void Pong(),

	oneway void dfs_remote_opendir(1:string hostname),
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
	oneway void dfs_remote_lock(1:string hostname),
	oneway void dfs_remote_flock(1:string hostname),
}
