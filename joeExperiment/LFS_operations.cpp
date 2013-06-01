
#include "LFS_operations.hpp"

using namespace std;

int local_getattr(const char *path, struct stat *stbuf)
{
    int res;

    res = lstat(path, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

int local_fgetattr(const char *path, struct stat *stbuf,
        struct fuse_file_info *fi)
{
    int res;

    (void) path;

    res = fstat(fi->fh, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

int local_access(const char *path, int mask)
{
    int res;

    res = access(path, mask);
    if (res == -1)
        return -errno;

    return 0;
}

int local_readlink(const char *path, char *buf, size_t size)
{
    int res;

    res = readlink(path, buf, size - 1);
    if (res == -1)
        return -errno;

    buf[res] = '\0';
    return 0;
}

int local_opendir(const char *path, struct fuse_file_info *fi)
{
    int res;
    struct dirp *d = (dirp *) malloc(sizeof(struct dirp));
    if (d == NULL)
        return -ENOMEM;

    d->dp = opendir(path);
    if (d->dp == NULL) {
        res = -errno;
        free(d);
        return res;
    }
    d->offset = 0;
    d->entry = NULL;

    fi->fh = (unsigned long) d;
    return 0;
}

inline struct dirp *local_get_dirp(struct fuse_file_info *fi)
{
    return (struct dirp *) (uintptr_t) fi->fh;
}

int local_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi)
{
    struct dirp *d = local_get_dirp(fi);

    (void) path;
    if (offset != d->offset) {
        seekdir(d->dp, offset);
        d->entry = NULL;
        d->offset = offset;
    }
    while (1) {
        struct stat st;
        off_t nextoff;

        if (!d->entry) {
            d->entry = readdir(d->dp);
            if (!d->entry)
                break;
        }

        memset(&st, 0, sizeof(st));
        st.st_ino = d->entry->d_ino;
        st.st_mode = d->entry->d_type << 12;
        nextoff = telldir(d->dp);
        if (filler(buf, d->entry->d_name, &st, nextoff))
            break;

        d->entry = NULL;
        d->offset = nextoff;
    }

    return 0;
}

int local_releasedir(const char *path, struct fuse_file_info *fi)
{
    struct dirp *d = local_get_dirp(fi);
    (void) path;
    closedir(d->dp);
    free(d);
    return 0;
}

int local_mkdir(const char *path, mode_t mode)
{
    int res;

    res = mkdir(path, mode);
    if (res == -1)
        return -errno;

    return 0;
}

int local_unlink(const char *path)
{
    int res;

    res = unlink(path);
    if (res == -1)
        return -errno;

    return 0;
}

int local_rmdir(const char *path)
{
    int res;

    res = rmdir(path);
    if (res == -1)
        return -errno;

    return 0;
}

int local_symlink(const char *from, const char *to)
{
    int res;

    res = symlink(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

int local_rename(const char *from, const char *to)
{
    int res;

    res = rename(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

int local_link(const char *from, const char *to)
{
    int res;

    res = link(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

int local_chmod(const char *path, mode_t mode)
{
    int res;

    res = chmod(path, mode);
    if (res == -1)
        return -errno;

    return 0;
}

int local_chown(const char *path, uid_t uid, gid_t gid)
{
    int res;

    res = lchown(path, uid, gid);
    if (res == -1)
        return -errno;

    return 0;
}

int local_truncate(const char *path, off_t size)
{
    int res;

    res = truncate(path, size);
    if (res == -1)
        return -errno;

    return 0;
}

int local_ftruncate(const char *path, off_t size,
        struct fuse_file_info *fi)
{
    int res;

    (void) path;

    res = ftruncate(fi->fh, size);
    if (res == -1)
        return -errno;

    return 0;
}

int local_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    int fd;

    fd = open(path, fi->flags, mode);
    if (fd == -1)
        return -errno;

    fi->fh = fd;
    return 0;
}

int local_open(const char *path, struct fuse_file_info *fi)
{
    int fd;

    fd = open(path, fi->flags);
    if (fd == -1)
        return -errno;

    fi->fh = fd;
    return 0;
}

int local_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
    int res;

    (void) path;
    res = pread(fi->fh, buf, size, offset);
    if (res == -1)
        res = -errno;

    return res;
}

int local_read_buf(const char *path, struct fuse_bufvec **bufp,
        size_t size, off_t offset, struct fuse_file_info *fi)
{
    struct fuse_bufvec *src;

    (void) path;

    src = (fuse_bufvec *) malloc(sizeof(struct fuse_bufvec));
    if (src == NULL)
        return -ENOMEM;

    *src = FUSE_BUFVEC_INIT(size);

    src->buf[0].flags = (fuse_buf_flags) (FUSE_BUF_IS_FD | FUSE_BUF_FD_SEEK);
    src->buf[0].fd = fi->fh;
    src->buf[0].pos = offset;

    *bufp = src;

    return 0;
}

int local_write(const char *path, const char *buf, size_t size,
        off_t offset, struct fuse_file_info *fi)
{
    int res;

    (void) path;
    res = pwrite(fi->fh, buf, size, offset);
    if (res == -1)
        res = -errno;

    return res;
}

int local_flush(const char *path, struct fuse_file_info *fi)
{
    int res;

    (void) path;
    /* This is called from every close on an open file, so call the
       close on the underlying filesystem.	But since flush may be
       called multiple times for an open file, this must not really
       close the file.  This is important if used on a network
       filesystem like NFS which flush the data/metadata on close() */
    res = close(dup(fi->fh));
    if (res == -1)
        return -errno;

    return 0;
}

int local_release(const char *path, struct fuse_file_info *fi)
{
    (void) path;
    close(fi->fh);

    return 0;
}

int local_fsync(const char *path, int isdatasync,
        struct fuse_file_info *fi)
{
    int res;
    (void) path;

#ifndef HAVE_FDATASYNC
    (void) isdatasync;
#else
    if (isdatasync)
        res = fdatasync(fi->fh);
    else
#endif
        res = fsync(fi->fh);
    if (res == -1)
        return -errno;

    return 0;
}

#ifdef HAVE_POSIX_FALLOCATE
int local_fallocate(const char *path, int mode,
        off_t offset, off_t length, struct fuse_file_info *fi)
{
    (void) path;

    if (mode)
        return -EOPNOTSUPP;

    return -posix_fallocate(fi->fh, offset, length);
}
#endif

int local_flock(const char *path, struct fuse_file_info *fi, int op)
{
    int res;
    (void) path;

    res = flock(fi->fh, op);
    if (res == -1)
        return -errno;

    return 0;
}

/*
void initOpers(fuse_operations& oper) {
    oper.getattr	= local_getattr;
    oper.fgetattr	= local_fgetattr;
    oper.access		= local_access;
    oper.readlink	= local_readlink;
    oper.read		= local_read;
    oper.read_buf	= local_read_buf;

    oper.opendir	= local_opendir;
    oper.readdir	= local_readdir;
    oper.releasedir	= local_releasedir;
    oper.mkdir		= local_mkdir;
    oper.symlink	= local_symlink;
    oper.unlink		= local_unlink;
    oper.rmdir		= local_rmdir;
    oper.rename		= local_rename;
    oper.link		= local_link;
    oper.chmod		= local_chmod;
    oper.chown		= local_chown;
    oper.truncate	= local_truncate;
    oper.ftruncate	= local_ftruncate;
    oper.create		= local_create;
    oper.open		= local_open;
    oper.write		= local_write;

    oper.flush		= local_flush;
    oper.release	= local_release;
    oper.fsync		= local_fsync;
#ifdef HAVE_POSIX_FALLOCATE
    oper.fallocate	= local_fallocate;
#endif
    //oper.lock		    = local_lock;
    oper.flock		= local_flock;

    oper.flag_nullpath_ok   = 1;
#if HAVE_UTIMENSAT
    oper.flag_utime_omit_ok = 1;
#endif
}

void * startFuse(void * arg) {
    cerr << "Starting FUSE..." << endl;
    ArgStruct * args = (ArgStruct *) arg;

    fuse_operations local_oper;
    initOpers(local_oper);
    int ret = fuse_main(args->argc, args->argv, &local_oper, NULL);
    //if (int ret = fuse_main(argc, argv, &local_oper, NULL) != 0)
    //    return ret;

    delete args;

    return NULL;
}*/
