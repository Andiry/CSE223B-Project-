
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

struct TimeSpec {
    1:  i64 sec;
    2:  i64 nsec;
}

struct HostID {
    1:  string  hostname,
    2:  i16     port,
}

enum LockType {
    READ    = 1,
    WRITE   = 2,
}

service DFS {

    oneway void ping(1:HostID sender),
    oneway void unlock(1:HostID sender, 2:string file),
    oneway void die(1:HostID sender),
    oneway void addServer(1:HostID sender, 2:HostID newServer),
    oneway void releaseJoinLock(1:HostID sender),

    bool        lock(1:HostID sender, 2:string file, 3:LockType lockType),
    set<HostID> join(1:HostID sender),
    string      requestJoinLock(1:HostID sender),
    bool        getJoinLock(1:HostID sender, 2:HostID newServer),

    oneway void releasedir(1:HostID sender, 2:string path, 3:FUSEFileInfoTransport fi, 4:i64 rand),
    oneway void mkdir(1:HostID sender, 2:string path, 3:i32 mode, 4:i64 rand),
    oneway void unlink(1:HostID sender, 2:string path, 3:i64 rand),
    oneway void rmdir(1:HostID sender, 2:string path, 3:i64 rand),
    oneway void symlink(1:HostID sender, 2:string from, 3:string to, 4:i64 rand),
    oneway void rename(1:HostID sender, 2:string from, 3:string to, 4:i64 rand),
    oneway void link(1:HostID sender, 2:string from, 3:string to, 4:i64 rand),
    oneway void chmod(1:HostID sender, 2:string path, 3:i32 mode, 4:i64 rand),
    oneway void chown(1:HostID sender, 2:string path, 3:i32 uid, 4:i32 gid, 5:i64 rand),
    oneway void truncate(1:HostID sender, 2:string path, 3:i64 size, 4:i64 rand),
    oneway void ftruncate(1:HostID sender, 2:string path, 3:i64 size, 4:FUSEFileInfoTransport fi, 5:i64 rand),
    oneway void create(1:HostID sender, 2:string path, 3:i32 mode, 4:FUSEFileInfoTransport fi, 5:i64 rand),
    oneway void write(1:HostID sender, 2:string path, 3:list<byte> buf, 4:i64 size, 5:i64 offset, 6:FUSEFileInfoTransport fi, 7:i64 rand),
    oneway void flush(1:HostID sender, 2:string path, 3:FUSEFileInfoTransport fi, 4:i64 rand),
    oneway void release(1:HostID sender, 2:string path, 3:FUSEFileInfoTransport fi, 4:i64 rand),
    oneway void flock(1:HostID sender, 2:string path, 3:FUSEFileInfoTransport fi, 4:i64 op, 5:i64 rand),
    oneway void fallocate(1:HostID sender, 2:string path, 3:i64 mode, 4:i64 offset, 5:i64 length, 6:FUSEFileInfoTransport fi, 7:i64 rand),
    oneway void utimens(1:HostID sender, 2:string path, 3:TimeSpec atime, 4:TimeSpec mtime, 5:i64 rand),
    #utimensat
    
    bool fsync(1:HostID sender, 2:string path, 3:i32 isdatasync, 4:FUSEFileInfoTransport fi, 5:i64 rand),
    bool open(1:HostID sender, 2:string path, 3:FUSEFileInfoTransport fi, 4:i64 rand),
    bool opendir(1:HostID sender, 2:string path, 3:FUSEFileInfoTransport fi, 4:i64 rand),
}

#bool HostID::operator<(DFS::HostID const& rhs) const {
#    return (hostname == rhs.hostname) ?
#        port < rhs.port : hostname < rhs.hostname;
#}
#
