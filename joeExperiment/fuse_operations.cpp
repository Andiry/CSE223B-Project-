
#include "fuse_operations.hpp"

using namespace std;
using namespace FUSEService;
using namespace DFS;

namespace FUSEService {
    void initOpers(fuse_operations& oper);
    static int fuseMain(int argc, char *argv[], const fuse_operations *op, void *user_data);
    void ffi2ffit(const fuse_file_info& ffi, FUSEFileInfoTransport& ffit);

    void unlockAll(const string& path);
    static bool lockAll(const string& path, const DFS::LockType::type lockType);

    GlobalBucket * globals_;

    inline void announceOperation(const string& oper, const char *path) {
        if (path == NULL) {
            globals_->debug_ << "== LOCAL ========== "
                         << oper << " ERROR: UNKNOWN PATH" << endl;
            return;
        }
        globals_->debug_ << "== LOCAL ========== "
                         << oper << " on " << path << endl;
    }
    inline void announceOperation(const string& oper, const char *from, const char* to) {
        if (from == NULL || to == NULL) {
            globals_->debug_ << "== LOCAL ========== "
                         << oper << " ERROR: UNKNOWN FROM/TO" << endl;
            return;
        }

        globals_->debug_ << "== LOCAL ========== "
                         << oper << " from " << from << " to " << to << endl;
    }

    extern "C" {
        fuse * fuse_ = NULL;
        string convert(const string& path);
    }
}

string FUSEService::convert(const string& path)
{
    stringstream ss;
    ss << globals_->backupPath_ << path;
	string newPath = ss.str();
    return newPath;
}

int FUSEService::fuse_getattr(const char *path, struct stat *stbuf)
{
    return local_getattr(convert(path).c_str(), stbuf);
}

int FUSEService::fuse_fgetattr(const char *path, struct stat *stbuf,
        struct fuse_file_info *fi)
{
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_fgetattr(convert(path).c_str(), stbuf, fi, fh);
}

int FUSEService::fuse_access(const char *path, int mask)
{
    return local_access(convert(path).c_str(), mask);
}

int FUSEService::fuse_readlink(const char *path, char *buf, size_t size)
{
    return local_readlink(convert(path).c_str(), buf, size);
}

static bool FUSEService::lockAll(const string& path, const DFS::LockType::type lockType) {
    vector<Host*> backoutHosts;
    bool backout = false;
    for (auto& pair : globals_->hostMap_) {
        bool tryLock = pair.second.lock(path, lockType);
        if (!tryLock) {
            globals_->debug_ << "Failed to acquire lock for " << pair.second.identifier() << endl;
            backout = true;
            break;
        }
        backoutHosts.push_back(&pair.second);
    }

    if (backout)
        for (auto& host : backoutHosts)
            host->unlock(path);

    return !backout;
}

inline void FUSEService::unlockAll(const string& path) {
    for (auto& pair : globals_->hostMap_)
        pair.second.unlock(path);
}

void FUSEService::ffi2ffit(const fuse_file_info& ffi, FUSEFileInfoTransport& ffit) {
    ffit.flags       = ffi.flags;
    ffit.fh_old      = ffi.fh_old;
    ffit.writepage   = ffi.writepage;
    ffit.direct_io   = ffi.direct_io;
    ffit.keep_cache  = ffi.keep_cache;
    ffit.flush       = ffi.flush;
    ffit.nonseekable = ffi.nonseekable;
    ffit.padding     = ffi.padding;
    ffit.fh          = ffi.fh;
    ffit.lock_owner  = ffi.lock_owner;
}

int FUSEService::fuse_opendir(const char *path, struct fuse_file_info *fi)
{
    announceOperation("opendir", path);

    if (!lockAll(path, DFS::LockType::type::READ)) {
        globals_->debug_ << "Unable to lock directory...?" << endl;
        return -ENOLCK;
    }

    fi->fh = globals_->randGen_();
    uint64_t& fh(globals_->fhMap_[fi->fh]);

    string cpath = convert(path);
    int ret = local_opendir(cpath.c_str(), fi, fh);

    FUSEFileInfoTransport ffit;
    ffi2ffit(*fi, ffit);
    for (auto& pair : globals_->hostMap_)
        pair.second.opendir(path, ffit);

    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}

inline dirp * FUSEService::fuse_get_dirp(struct fuse_file_info *fi)
{
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_get_dirp(fi, fh);
}

int FUSEService::fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi)
{
    announceOperation("readdir", path);
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_readdir(convert(path).c_str(), buf, filler, offset, fi, fh);
}

int FUSEService::fuse_releasedir(const char *path, struct fuse_file_info *fi)
{
    string spath(path == NULL ? "" : path);
    // TODO - called with empty path?
    announceOperation("releasedir", path);

    uint64_t& fh(globals_->fhMap_[fi->fh]);
    string cpath = convert(spath);
    int ret = local_releasedir(cpath.c_str(), fi, fh);

    FUSEFileInfoTransport ffit;
    ffi2ffit(*fi, ffit);
    for (auto& pair : globals_->hostMap_)
        pair.second.releasedir(spath, ffit);

    unlockAll(spath);

    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}

int FUSEService::fuse_mkdir(const char *path, mode_t mode)
{
    announceOperation("mkdir", path);

    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    string cpath = convert(path);
    int ret = local_mkdir(cpath.c_str(), mode);
    for (auto& pair : globals_->hostMap_)
        pair.second.mkdir(path, mode);
    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}

int FUSEService::fuse_unlink(const char *path)
{
    announceOperation("unlink", path);

    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    string cpath = convert(path);
    int ret = local_unlink(cpath.c_str());
    for (auto& pair : globals_->hostMap_)
        pair.second.unlink(path);
    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}

int FUSEService::fuse_rmdir(const char *path)
{
    // TODO - do we want locking here?
    announceOperation("rmdir", path);

    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    string cpath = convert(path);

    int ret = local_rmdir(cpath.c_str());
    for (auto& pair : globals_->hostMap_)
        pair.second.rmdir(path);
    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}

int FUSEService::fuse_symlink(const char *from, const char *to)
{
    announceOperation("symlink", from, to);
    return -1;
    //return local_symlink(from, to);
}

int FUSEService::fuse_rename(const char *from, const char *to)
{
    announceOperation("rename", from, to);

    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    if (!lockAll(from, DFS::LockType::type::WRITE)) {
        globals_->debug_ << "Unable to lock for rename" << endl;
        return -ENOLCK;
    }

    string cfrom = convert(from);
    string cto   = convert(to);
    int ret = local_rename(cfrom.c_str(), cto.c_str());
    for (auto& pair : globals_->hostMap_)
        pair.second.rename(from, cto);

    unlockAll(from);

    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}

int FUSEService::fuse_link(const char *from, const char *to)
{
    announceOperation("link", from, to);
    //return local_link(from, to);
    return -1;
}

int FUSEService::fuse_chmod(const char *path, mode_t mode)
{
    announceOperation("chmod", path);

    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    string cpath = convert(path);
    int ret = local_chmod(cpath.c_str(), mode);
    for (auto& pair : globals_->hostMap_)
        pair.second.chmod(path, mode);

    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}

int FUSEService::fuse_chown(const char *path, uid_t uid, gid_t gid)
{
    announceOperation("chown", path);

    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    string cpath = convert(path);
    int ret = local_chown(cpath.c_str(), uid, gid);
    for (auto& pair : globals_->hostMap_)
        pair.second.chown(path, uid, gid);
    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}

int FUSEService::fuse_truncate(const char *path, off_t size)
{
    announceOperation("truncate", path);

    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    string cpath = convert(path);
    int ret = local_truncate(cpath.c_str(), size);
    for (auto& pair : globals_->hostMap_)
        pair.second.truncate(path, size);
    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}

int FUSEService::fuse_ftruncate(const char *path, off_t size,
        struct fuse_file_info *fi)
{
    announceOperation("ftruncate", path);

    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    uint64_t& fh(globals_->fhMap_[fi->fh]);
    string cpath = convert(path);
    int ret = local_ftruncate(cpath.c_str(), size, fi, fh);
    FUSEFileInfoTransport ffit;
    ffi2ffit(*fi, ffit);
    for (auto& pair : globals_->hostMap_)
        pair.second.ftruncate(path, size, ffit);
    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}

int FUSEService::fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    announceOperation("create", path);

    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    if (!lockAll(path, DFS::LockType::type::WRITE))
        return -ENOLCK;

    globals_->debug_ << "Got lock" << endl;

    fi->fh = globals_->randGen_();
    uint64_t& fh(globals_->fhMap_[fi->fh]);

    string cpath = convert(path);
    int ret = local_create(cpath.c_str(), mode, fi, fh);

    FUSEFileInfoTransport ffit;
    ffi2ffit(*fi, ffit);
    for (auto& pair : globals_->hostMap_)
        pair.second.create(path, mode, ffit);

    globals_->debug_ << "Sent create commands" << endl;

    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}

int FUSEService::fuse_open(const char *path, struct fuse_file_info *fi)
{
    announceOperation("open", path);

    // TODO - make this block instead
    if (fi->flags & (O_WRONLY | O_RDWR) && globals_->joinLock_)
        return -EAGAIN;

    if (!lockAll(path,
                ((fi->flags & (O_WRONLY | O_RDWR)) ?
                    DFS::LockType::type::WRITE :
                    DFS::LockType::type::READ)))
        return -ENOLCK;

    fi->fh = globals_->randGen_();
    uint64_t& fh(globals_->fhMap_[fi->fh]);

    string cpath = convert(path);
    int ret = local_open(cpath.c_str(), fi, fh);

    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;

    // We only have to forward on writing opens.
    if (!(fi->flags & (O_WRONLY | O_RDWR)))
        return ret;

    FUSEFileInfoTransport ffit;
    ffi2ffit(*fi, ffit);
    for (auto& pair : globals_->hostMap_)
        pair.second.open(path, ffit);

    return ret;
}

int FUSEService::fuse_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
    announceOperation("read", path);
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_read(convert(path).c_str(), buf, size, offset, fi, fh);
}

int FUSEService::fuse_read_buf(const char *path, struct fuse_bufvec **bufp,
        size_t size, off_t offset, struct fuse_file_info *fi)
{
    announceOperation("read_buf", path);
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_read_buf(convert(path).c_str(), bufp, size, offset, fi, fh);
}

int FUSEService::fuse_write(const char *path, const char *buf, size_t size,
        off_t offset, struct fuse_file_info *fi)
{
    announceOperation("write", path);

    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    uint64_t& fh(globals_->fhMap_[fi->fh]);
    string cpath = convert(path);
    int ret = local_write(cpath.c_str(), buf, size, offset, fi, fh);
    FUSEFileInfoTransport ffit;
    ffi2ffit(*fi, ffit);

    vector<int8_t> vbuf;
    for (unsigned i = 0; i < size; ++i)
        vbuf.push_back(buf[i]);

    for (auto& pair : globals_->hostMap_)
        pair.second.write(path, vbuf, size, offset, ffit);

    return ret;
}

int FUSEService::fuse_flush(const char *path, struct fuse_file_info *fi)
{
    announceOperation("flush", path);

    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    uint64_t& fh(globals_->fhMap_[fi->fh]);
    string cpath = convert(path);
    int ret = local_flush(cpath.c_str(), fi, fh);
    FUSEFileInfoTransport ffit;
    ffi2ffit(*fi, ffit);
    for (auto& pair : globals_->hostMap_)
        pair.second.flush(path, ffit);

    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}

int FUSEService::fuse_release(const char *path, struct fuse_file_info *fi)
{
    announceOperation("release", path);

    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    uint64_t& fh(globals_->fhMap_[fi->fh]);
    string cpath = convert(path);
    int ret = local_release(cpath.c_str(), fi, fh);

    FUSEFileInfoTransport ffit;
    ffi2ffit(*fi, ffit);
    for (auto& pair : globals_->hostMap_)
        pair.second.release(path, ffit);

    unlockAll(path);

    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}

int FUSEService::fuse_fsync(const char *path, int isdatasync,
        struct fuse_file_info *fi)
{
    announceOperation("fsync", path);

    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    uint64_t& fh(globals_->fhMap_[fi->fh]);
    string cpath = convert(path);
    int ret = local_fsync(cpath.c_str(), isdatasync, fi, fh);
    FUSEFileInfoTransport ffit;
    ffi2ffit(*fi, ffit);
    for (auto& pair : globals_->hostMap_)
        pair.second.fsync(path, isdatasync, ffit);

    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}

#ifdef HAVE_POSIX_FALLOCATE
int FUSEService::fuse_fallocate(const char *path, int mode,
        off_t offset, off_t length, struct fuse_file_info *fi)
{
    announceOperation("fallocate", path);

    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    uint64_t& fh(globals_->fhMap_[fi->fh]);
    string cpath = convert(path);
    int ret = local_fallocate(cpath.c_str(), mode, offset, length, fi, fh);
    FUSEFileInfoTransport ffit;
    ffi2ffit(*fi, ffit);
    for (auto& pair : globals_->hostMap_)
        pair.second.fallocate(path, mode, offset, length, ffit);
    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}
#endif

int FUSEService::fuse_flock(const char *path, struct fuse_file_info *fi, int op)
{
    announceOperation("flock", path);
    
    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    uint64_t& fh(globals_->fhMap_[fi->fh]);
    string cpath = convert(path);
    int ret = local_flock(cpath.c_str(), fi, op, fh);
    FUSEFileInfoTransport ffit;
    ffi2ffit(*fi, ffit);
    for (auto& pair : globals_->hostMap_)
        pair.second.flock(path, ffit, op);
    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;
}

int FUSEService::fuse_utimens(const char * path, const struct timespec ts[2])
{
    announceOperation("utimens", path);

    if (globals_->joinLock_) // TODO - make this block instead
        return -EAGAIN;

    string cpath = convert(path);

    int ret = local_utimens(cpath.c_str(), ts);

    TimeSpec atime;
    TimeSpec mtime;
    atime.sec  = ts[0].tv_sec;
    atime.nsec = ts[0].tv_nsec;
    mtime.sec  = ts[1].tv_sec;
    mtime.nsec = ts[1].tv_nsec;

    for (auto& pair : globals_->hostMap_)
        pair.second.utimens(path, atime, mtime);

    if (ret) globals_->debug_ << "Error'd out: " << strerror(ret) << endl;
    return ret;

    //utimensat
}

void FUSEService::initOpers(fuse_operations& oper) {
    oper.utimens    = FUSEService::fuse_utimens;
    oper.getattr    = FUSEService::fuse_getattr;
    oper.fgetattr   = FUSEService::fuse_fgetattr;
    oper.access     = FUSEService::fuse_access;
    oper.readlink   = FUSEService::fuse_readlink;
    oper.read       = FUSEService::fuse_read;
    oper.read_buf   = FUSEService::fuse_read_buf;

    oper.opendir    = FUSEService::fuse_opendir;
    oper.readdir    = FUSEService::fuse_readdir;
    oper.releasedir = FUSEService::fuse_releasedir;
    oper.mkdir      = FUSEService::fuse_mkdir;
    oper.symlink    = FUSEService::fuse_symlink;
    oper.unlink     = FUSEService::fuse_unlink;
    oper.rmdir      = FUSEService::fuse_rmdir;
    oper.rename     = FUSEService::fuse_rename;
    oper.link       = FUSEService::fuse_link;
    oper.chmod      = FUSEService::fuse_chmod;
    oper.chown      = FUSEService::fuse_chown;
    oper.truncate   = FUSEService::fuse_truncate;
    oper.ftruncate  = FUSEService::fuse_ftruncate;
    oper.create     = FUSEService::fuse_create;
    oper.open       = FUSEService::fuse_open;
    oper.write      = FUSEService::fuse_write;

    oper.flush      = FUSEService::fuse_flush;
    oper.release    = FUSEService::fuse_release;
    oper.fsync      = FUSEService::fuse_fsync;
#ifdef HAVE_POSIX_FALLOCATE
    oper.fallocate  = FUSEService::fuse_fallocate;
#endif
    /*
#ifdef HAVE_SETXATTR
	.setxattr	    = FUSEService::fuse_setxattr,
	.getxattr	    = FUSEService::fuse_getxattr,
	.listxattr	    = FUSEService::fuse_listxattr,
	.removexattr	= FUSEService::fuse_removexattr,
#endif
    */
    //oper.lock            = FUSEService::lock;
    oper.flock       = FUSEService::fuse_flock;

    oper.flag_nullpath_ok   = 1;
#if HAVE_UTIMENSAT
    oper.flag_utime_omit_ok = 1;
#endif
}

static int FUSEService::fuseMain(int argc, char *argv[],
                const fuse_operations *op,
                void *user_data)
{
    char *mountpoint;
    int multithreaded;
    int res;

    fuse_ = fuse_setup(argc, argv, op, sizeof(fuse_operations), &mountpoint,
                       &multithreaded, user_data);

    if (fuse_ == NULL)
        return 1;

    if (multithreaded)
        res = fuse_loop_mt(fuse_);
    else
        res = fuse_loop(fuse_);

    fuse_teardown(fuse_, mountpoint);
    if (res == -1)
        return 1;

    return 0;
}

void FUSEService::stop() {
    fuse_exit(fuse_);
    cerr << endl
         << "\t == "
         << "HINT: Do an \"ls\" in the mounted directory to exit."
         << " ==" << endl;
}

void * FUSEService::start(void * arg) {
    cerr << "Starting FUSE..." << endl;
    ArgStruct * args = (ArgStruct *) arg;

    fuse_operations oper;
    initOpers(oper);

    globals_ = args->globals;

    intptr_t ret = fuseMain(args->argc, args->argv, &oper, NULL);

    cerr << "FUSE done!" << endl;

    globals_->killall_();

    delete [] args->argv;
    delete args;

    return (void *) ret;
}

