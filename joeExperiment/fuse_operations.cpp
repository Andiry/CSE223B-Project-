
#include "fuse_operations.hpp"

using namespace std;

string backup_path;

const char *convert(const char *path)
{
	return (backup_path + path).c_str();
}

int from_fuse_getattr(const char *path, struct stat *stbuf)
{
    return local_getattr(convert(path), stbuf);
}

int from_fuse_fgetattr(const char *path, struct stat *stbuf,
        struct fuse_file_info *fi)
{
    return local_fgetattr(convert(path), stbuf, fi);
}

int from_fuse_access(const char *path, int mask)
{
    return local_access(convert(path), mask);
}

int from_fuse_readlink(const char *path, char *buf, size_t size)
{
    return local_readlink(convert(path), buf, size);
}

int from_fuse_opendir(const char *path, struct fuse_file_info *fi)
{
    return local_opendir(convert(path), fi);
}

inline struct dirp *from_fuse_get_dirp(struct fuse_file_info *fi)
{
    return local_get_dirp(fi);
}

int from_fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi)
{
    return local_readdir(convert(path), buf, filler, offset, fi);
}

int from_fuse_releasedir(const char *path, struct fuse_file_info *fi)
{
    return local_releasedir(convert(path), fi);
}

int from_fuse_mkdir(const char *path, mode_t mode)
{
    return local_mkdir(convert(path), mode);
}

int from_fuse_unlink(const char *path)
{
    return local_unlink(convert(path));
}

int from_fuse_utime(const char *path, struct utimbuf *times)
{
    return local_utime(convert(path), times);
}

int from_fuse_rmdir(const char *path)
{
    return local_rmdir(convert(path));
}

int from_fuse_symlink(const char *from, const char *to)
{
    return local_symlink(from, to);
}

int from_fuse_rename(const char *from, const char *to)
{
    return local_rename(from, to);
}

int from_fuse_link(const char *from, const char *to)
{
    return local_link(from, to);
}

int from_fuse_chmod(const char *path, mode_t mode)
{
    return local_chmod(convert(path), mode);
}

int from_fuse_chown(const char *path, uid_t uid, gid_t gid)
{
    return local_chown(convert(path), uid, gid);
}

int from_fuse_truncate(const char *path, off_t size)
{
    return local_truncate(convert(path), size);
}

int from_fuse_ftruncate(const char *path, off_t size,
        struct fuse_file_info *fi)
{
    return local_ftruncate(convert(path), size, fi);
}

int from_fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    int ret;
    ret = local_create(convert(path), mode, fi);
    PropagateToOtherServers("create", path, mode, fi);
    return ret;
}

int from_fuse_open(const char *path, struct fuse_file_info *fi)
{
    int ret;
    ret = local_open(convert(path), fi);
    PropagateToOtherServers("open", path, 0, fi);
    return ret;
}

int from_fuse_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
    return local_read(convert(path), buf, size, offset, fi);
}

int from_fuse_read_buf(const char *path, struct fuse_bufvec **bufp,
        size_t size, off_t offset, struct fuse_file_info *fi)
{
    return local_read_buf(convert(path), bufp, size, offset, fi);
}

int from_fuse_write(const char *path, const char *buf, size_t size,
        off_t offset, struct fuse_file_info *fi)
{
    int ret;
    ret = local_write(convert(path), buf, size, offset, fi);
    PropagateToOtherServers("write", path, 0, fi);
    return ret;
}

int from_fuse_flush(const char *path, struct fuse_file_info *fi)
{
    return local_flush(convert(path), fi);
}

int from_fuse_release(const char *path, struct fuse_file_info *fi)
{
    return local_release(convert(path), fi);
}

int from_fuse_fsync(const char *path, int isdatasync,
        struct fuse_file_info *fi)
{
    return local_fsync(convert(path), isdatasync, fi);
}

#ifdef HAVE_POSIX_FALLOCATE
int from_fuse_fallocate(const char *path, int mode,
        off_t offset, off_t length, struct fuse_file_info *fi)
{
    return local_fallocate(convert(path), mode, offset, length, fi);
}
#endif

int from_fuse_flock(const char *path, struct fuse_file_info *fi, int op)
{
    return local_flock(convert(path), fi, op);
}

void initOpers(fuse_operations& oper) {
    oper.getattr    = from_fuse_getattr;
    oper.fgetattr    = from_fuse_fgetattr;
    oper.access        = from_fuse_access;
    oper.readlink    = from_fuse_readlink;
    oper.read        = from_fuse_read;
    oper.read_buf    = from_fuse_read_buf;

    oper.opendir    = from_fuse_opendir;
    oper.readdir    = from_fuse_readdir;
    oper.releasedir    = from_fuse_releasedir;
    oper.mkdir        = from_fuse_mkdir;
    oper.symlink    = from_fuse_symlink;
    oper.unlink        = from_fuse_unlink;
    oper.utime        = from_fuse_utime;
    oper.rmdir        = from_fuse_rmdir;
    oper.rename        = from_fuse_rename;
    oper.link        = from_fuse_link;
    oper.chmod        = from_fuse_chmod;
    oper.chown        = from_fuse_chown;
    oper.truncate    = from_fuse_truncate;
    oper.ftruncate    = from_fuse_ftruncate;
    oper.create        = from_fuse_create;
    oper.open        = from_fuse_open;
    oper.write        = from_fuse_write;

    oper.flush        = from_fuse_flush;
    oper.release    = from_fuse_release;
    oper.fsync        = from_fuse_fsync;
#ifdef HAVE_POSIX_FALLOCATE
    oper.fallocate    = from_fuse_fallocate;
#endif
    //oper.lock            = from_fuse_lock;
    oper.flock        = from_fuse_flock;

    oper.flag_nullpath_ok   = 1;
#if HAVE_UTIMENSAT
    oper.flag_utime_omit_ok = 1;
#endif
}

/*
static int fuseMain(int argc, char *argv[],
                const struct fuse_operations *op, size_t op_size,
                void *user_data, int compat)
{
    struct fuse *fuse;
    char *mountpoint;
    int multithreaded;
    int res;

    fuse = fuse_setup_common(argc, argv, op, op_size, &mountpoint,
                 &multithreaded, NULL, user_data, 0);

    if (fuse == NULL)
        return 1;

    //if (multithreaded)
    //    res = fuse_loop_mt(fuse);
    //else
    //  res = fuse_loop(fuse);
    res = fuse_loop(fuse);

    fuse_teardown_common(fuse, mountpoint);
    if (res == -1)
        return 1;

    return 0;
}
*/

void * startFuse(void * arg) {
    cerr << "Starting FUSE..." << endl;
    ArgStruct * args = (ArgStruct *) arg;
    int argc = 3;
    backup_path = args->argv[3];

    fuse_operations from_fuse_oper;
    initOpers(from_fuse_oper);

    intptr_t ret = fuse_main(argc, args->argv, &from_fuse_oper, NULL);
    //intptr_t ret = fuseMain(args->argc, args->argv, &from_fuse_oper, sizeof(from_fuse_oper), NULL);
    if (ret)
        return (void *) ret;

    delete args;

    pthread_exit(NULL);

    return NULL;
}

