
#include "fuse_operations.hpp"

using namespace std;
using namespace FUSEService;

namespace FUSEService {
    void initOpers(fuse_operations& oper);
    const char * convert(const char *path);
    static int fuseMain(int argc, char *argv[], const fuse_operations *op, void *user_data);

    fuse * fuse_ = NULL;
    string backup_path_;
}


const char * FUSEService::convert(const char *path)
{
	return (backup_path_ + path).c_str();
}

int FUSEService::getattr(const char *path, struct stat *stbuf)
{
    return local_getattr(convert(path), stbuf);
}

int FUSEService::fgetattr(const char *path, struct stat *stbuf,
        struct fuse_file_info *fi)
{
    return local_fgetattr(convert(path), stbuf, fi);
}

int FUSEService::access(const char *path, int mask)
{
    return local_access(convert(path), mask);
}

int FUSEService::readlink(const char *path, char *buf, size_t size)
{
    return local_readlink(convert(path), buf, size);
}

int FUSEService::opendir(const char *path, struct fuse_file_info *fi)
{
    return local_opendir(convert(path), fi);
}

inline dirp * FUSEService::get_dirp(struct fuse_file_info *fi)
{
    return local_get_dirp(fi);
}

int FUSEService::readdir(const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi)
{
    return local_readdir(convert(path), buf, filler, offset, fi);
}

int FUSEService::releasedir(const char *path, struct fuse_file_info *fi)
{
    return local_releasedir(convert(path), fi);
}

int FUSEService::mkdir(const char *path, mode_t mode)
{
    return local_mkdir(convert(path), mode);
}

int FUSEService::unlink(const char *path)
{
    return local_unlink(convert(path));
}

int FUSEService::rmdir(const char *path)
{
    return local_rmdir(convert(path));
}

int FUSEService::symlink(const char *from, const char *to)
{
    return local_symlink(from, to);
}

int FUSEService::rename(const char *from, const char *to)
{
    return local_rename(from, to);
}

int FUSEService::link(const char *from, const char *to)
{
    return local_link(from, to);
}

int FUSEService::chmod(const char *path, mode_t mode)
{
    return local_chmod(convert(path), mode);
}

int FUSEService::chown(const char *path, uid_t uid, gid_t gid)
{
    return local_chown(convert(path), uid, gid);
}

int FUSEService::truncate(const char *path, off_t size)
{
    return local_truncate(convert(path), size);
}

int FUSEService::ftruncate(const char *path, off_t size,
        struct fuse_file_info *fi)
{
    return local_ftruncate(convert(path), size, fi);
}

int FUSEService::create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    return local_create(convert(path), mode, fi);
}

int FUSEService::open(const char *path, struct fuse_file_info *fi)
{
    return local_open(convert(path), fi);
}

int FUSEService::read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
    return local_read(convert(path), buf, size, offset, fi);
}

int FUSEService::read_buf(const char *path, struct fuse_bufvec **bufp,
        size_t size, off_t offset, struct fuse_file_info *fi)
{
    return local_read_buf(convert(path), bufp, size, offset, fi);
}

int FUSEService::write(const char *path, const char *buf, size_t size,
        off_t offset, struct fuse_file_info *fi)
{
    return local_write(convert(path), buf, size, offset, fi);
}

int FUSEService::flush(const char *path, struct fuse_file_info *fi)
{
    return local_flush(convert(path), fi);
}

int FUSEService::release(const char *path, struct fuse_file_info *fi)
{
    return local_release(convert(path), fi);
}

int FUSEService::fsync(const char *path, int isdatasync,
        struct fuse_file_info *fi)
{
    return local_fsync(convert(path), isdatasync, fi);
}

#ifdef HAVE_POSIX_FALLOCATE
int FUSEService::fallocate(const char *path, int mode,
        off_t offset, off_t length, struct fuse_file_info *fi)
{
    return local_fallocate(convert(path), mode, offset, length, fi);
}
#endif

int FUSEService::flock(const char *path, struct fuse_file_info *fi, int op)
{
    return local_flock(convert(path), fi, op);
}


void FUSEService::initOpers(fuse_operations& oper) {
    oper.getattr     = FUSEService::getattr;
    oper.fgetattr    = FUSEService::fgetattr;
    oper.access      = FUSEService::access;
    oper.readlink    = FUSEService::readlink;
    oper.read        = FUSEService::read;
    oper.read_buf    = FUSEService::read_buf;

    oper.opendir     = FUSEService::opendir;
    oper.readdir     = FUSEService::readdir;
    oper.releasedir  = FUSEService::releasedir;
    oper.mkdir       = FUSEService::mkdir;
    oper.symlink     = FUSEService::symlink;
    oper.unlink      = FUSEService::unlink;
    oper.rmdir       = FUSEService::rmdir;
    oper.rename      = FUSEService::rename;
    oper.link        = FUSEService::link;
    oper.chmod       = FUSEService::chmod;
    oper.chown       = FUSEService::chown;
    oper.truncate    = FUSEService::truncate;
    oper.ftruncate   = FUSEService::ftruncate;
    oper.create      = FUSEService::create;
    oper.open        = FUSEService::open;
    oper.write       = FUSEService::write;

    oper.flush       = FUSEService::flush;
    oper.release     = FUSEService::release;
    oper.fsync       = FUSEService::fsync;
#ifdef HAVE_POSIX_FALLOCATE
    oper.fallocate   = FUSEService::fallocate;
#endif
    //oper.lock            = FUSEService::lock;
    oper.flock       = FUSEService::flock;

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

    fuse_ = fuse_setup(argc, argv, op, sizeof(op), &mountpoint,
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

    backup_path_ = args->backupPath;

    if (backup_path_.back() == '/')
        backup_path_.erase(--backup_path_.end());

    fuse_operations oper;
    initOpers(oper);

    intptr_t ret = fuseMain(args->argc, args->argv, &oper, NULL);
    if (ret)
        return (void *) ret;

    delete [] args->argv;
    delete args;

    cerr << "FUSE Done..." << endl;

    return NULL;
}

