
namespace cpp DFS

struct FUSEFileInfoTransport {
    1:  i32 flags,
    2:  i64 fh_old,
    3:  i32 writepage,
    4:  i32 direct_io,
    5:  i32 keep_cache,
    6:  i32 flush,
    7:  i32 nonseekable,
    8:  i32 padding,
    9:  i64 fh,
    10: i64 lock_owner,
}

struct HostID {
    1:  string  hostname,
    2:  i16     port,
}


service DFS {

/*
    enum ASOperType {
        CHOWN = 1,
        CHMOD = 2,
        RENAME = 3,
        FLUSH = 4, 
        RELEASE = 5,
        FALLOCATE = 6,
        CREATE = 7,
        WRITE = 8,
        TRUNCATE = 9,
        FTRUNCATE = 10,
        OPEN_DIR = 11,
        RELEASE_DIR = 12,
        MKDIR = 13,
        UNLINK = 14
    }
    enum ASOperType {
        OPEN = 1,
        FSYNC = 2
    }
*/

    #struct ASOper {
    #    1:  ASOperType  type,
    #    # TODO
    #}
    #struct SOper {
    #    1:  SOperType  type,
    #    # TODO
    #}

    oneway void ping(1:HostID sender),
    oneway void unlock(1:HostID sender, 2:string file),
    #oneway void doASOperation(1:HostID sender, 2:ASOper operation),
    oneway void die(1:HostID sender),
    oneway void addServer(1:HostID sender, 2:HostID newServer),
    oneway void releaseJoinLock(1:HostID sender),

    void        lock(1:HostID sender, 2:string file),
    #void        doSOperation(1:HostID sender, 2:SOper operation),
    set<HostID> join(1:HostID sender),
    bool        requestJoinLock(1:HostID sender),
    bool        getJoinLock(1:HostID sender),

    oneway void remote_releasedir(1:HostID sender, 2:string path, 3:FUSEFileInfoTransport fi),
    oneway void remote_mkdir(1:HostID sender, 2:string path, 3:i32 mode),
    oneway void remote_unlink(1:HostID sender, 2:string path),
    oneway void remote_rmdir(1:HostID sender, 2:string path),
    oneway void remote_symlink(1:HostID sender, 2:string from, 3:string to),
    oneway void remote_rename(1:HostID sender, 2:string from, 3:string to),
    oneway void remote_link(1:HostID sender, 2:string from, 3:string to),
    oneway void remote_chmod(1:HostID sender, 2:string path, 3:i32 mode),
    oneway void remote_chown(1:HostID sender, 2:string path, 3:i32 uid, 4:i32 gid),
    oneway void remote_truncate(1:HostID sender, 2:string path, 3:i64 size),
    oneway void remote_ftruncate(1:HostID sender, 2:string path, 3:i64 size, 4:FUSEFileInfoTransport fi),
    oneway void remote_create(1:HostID sender, 2:string path, 3:i32 mode, 4:FUSEFileInfoTransport fi),
    oneway void remote_write(1:HostID sender, 2:string path, 3:list<byte> buf, 4:i64 size, 5:i64 offset, 6:FUSEFileInfoTransport fi),
    oneway void remote_flush(1:HostID sender, 2:string path, 3:FUSEFileInfoTransport fi),
    oneway void remote_release(1:HostID sender, 2:string path, 3:FUSEFileInfoTransport fi),
    oneway void remote_flock(1:HostID sender, 2:string path, 3:FUSEFileInfoTransport fi, 4:i64 op),
    oneway void remote_fallocate(1:HostID sender, 2:string path, 3:i64 mode, 4:i64 offset, 5:i64 length, 6:FUSEFileInfoTransport fi),
    
    bool remote_fsync(1:HostID sender, 2:string path, 3:i32 isdatasync, 4:FUSEFileInfoTransport fi),
    bool remote_open(1:HostID sender, 2:string path, 3:FUSEFileInfoTransport fi),
    bool remote_opendir(1:HostID sender, 2:string path, 3:FUSEFileInfoTransport fi),
}

