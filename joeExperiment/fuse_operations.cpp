
#include "fuse_operations.hpp"

using namespace std;
using namespace FUSEService;

namespace FUSEService {
    void initOpers(fuse_operations& oper);
    static int fuseMain(int argc, char *argv[], const fuse_operations *op, void *user_data);

    extern "C" {
        fuse * fuse_ = NULL;
        string convert(const char *path);
    }
    GlobalBucket * globals_;
}

string FUSEService::convert(const char *path)
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

int FUSEService::fuse_opendir(const char *path, struct fuse_file_info *fi)
{
    fi->fh = globals_->randGen_();
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_opendir(convert(path).c_str(), fi, fh);
}

inline dirp * FUSEService::fuse_get_dirp(struct fuse_file_info *fi)
{
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_get_dirp(fi, fh);
}

int FUSEService::fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi)
{
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_readdir(convert(path).c_str(), buf, filler, offset, fi, fh);
}

int FUSEService::fuse_releasedir(const char *path, struct fuse_file_info *fi)
{
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_releasedir(convert(path).c_str(), fi, fh);
}

int FUSEService::fuse_mkdir(const char *path, mode_t mode)
{
    return local_mkdir(convert(path).c_str(), mode);
}

int FUSEService::fuse_unlink(const char *path)
{
    return local_unlink(convert(path).c_str());
}

int FUSEService::fuse_rmdir(const char *path)
{
    return local_rmdir(convert(path).c_str());
}

int FUSEService::fuse_symlink(const char *from, const char *to)
{
    return local_symlink(from, to);
}

int FUSEService::fuse_rename(const char *from, const char *to)
{
    return local_rename(from, to);
}

int FUSEService::fuse_link(const char *from, const char *to)
{
    return local_link(from, to);
}

int FUSEService::fuse_chmod(const char *path, mode_t mode)
{
    return local_chmod(convert(path).c_str(), mode);
}

int FUSEService::fuse_chown(const char *path, uid_t uid, gid_t gid)
{
    return local_chown(convert(path).c_str(), uid, gid);
}

int FUSEService::fuse_truncate(const char *path, off_t size)
{
    return local_truncate(convert(path).c_str(), size);
}

int FUSEService::fuse_ftruncate(const char *path, off_t size,
        struct fuse_file_info *fi)
{
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_ftruncate(convert(path).c_str(), size, fi, fh);
}

int FUSEService::fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    fi->fh = globals_->randGen_();
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_create(convert(path).c_str(), mode, fi, fh);
}

int FUSEService::fuse_open(const char *path, struct fuse_file_info *fi)
{
    fi->fh = globals_->randGen_();
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_open(convert(path).c_str(), fi, fh);
}

int FUSEService::fuse_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_read(convert(path).c_str(), buf, size, offset, fi, fh);
}

int FUSEService::fuse_read_buf(const char *path, struct fuse_bufvec **bufp,
        size_t size, off_t offset, struct fuse_file_info *fi)
{
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_read_buf(convert(path).c_str(), bufp, size, offset, fi, fh);
}

int FUSEService::fuse_write(const char *path, const char *buf, size_t size,
        off_t offset, struct fuse_file_info *fi)
{
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_write(convert(path).c_str(), buf, size, offset, fi, fh);
}

int FUSEService::fuse_flush(const char *path, struct fuse_file_info *fi)
{
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_flush(convert(path).c_str(), fi, fh);
}

int FUSEService::fuse_release(const char *path, struct fuse_file_info *fi)
{
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_release(convert(path).c_str(), fi, fh);
}

int FUSEService::fuse_fsync(const char *path, int isdatasync,
        struct fuse_file_info *fi)
{
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_fsync(convert(path).c_str(), isdatasync, fi, fh);
}

#ifdef HAVE_POSIX_FALLOCATE
int FUSEService::fuse_fallocate(const char *path, int mode,
        off_t offset, off_t length, struct fuse_file_info *fi)
{
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_fallocate(convert(path).c_str(), mode, offset, length, fi, fh);
}
#endif

int FUSEService::fuse_flock(const char *path, struct fuse_file_info *fi, int op)
{
    uint64_t& fh(globals_->fhMap_[fi->fh]);
    return local_flock(convert(path).c_str(), fi, op, fh);
}

void FUSEService::initOpers(fuse_operations& oper) {
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
}

void * FUSEService::start(void * arg) {
    cerr << "Starting FUSE..." << endl;
    ArgStruct * args = (ArgStruct *) arg;

    fuse_operations oper;
    initOpers(oper);

    globals_ = args->globals;

    intptr_t ret = fuseMain(args->argc, args->argv, &oper, NULL);
    if (ret)
        return (void *) ret;

    delete [] args->argv;
    delete args;

    cerr << "FUSE done!" << endl;

    return NULL;
}

