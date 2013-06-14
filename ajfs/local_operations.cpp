/* AJFS - a simple distributed file system.
 *
 * By Joe DeBlasio and Andiry Xu
 * For UCSD CSE223B, SP13
 * */

#include "local_operations.hpp"

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
        fuse_file_info *fi, uint64_t& fh)
{
    int res;

    (void) path;

    res = fstat(fh, stbuf);
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

int local_opendir(const char *path, fuse_file_info *fi, uint64_t& fh)
{
    static int count = 0;
    int res;
    dirp *d = (dirp *) malloc(sizeof(dirp));
    if (d == NULL)
        return -ENOMEM;

    ++count;
    d->dp = opendir(path);
    --count;
    if (d->dp == NULL) {
        res = -errno;
        free(d);
        return res;
    }
    d->offset = 0;
    d->entry = NULL;

    fh = (unsigned long) d;
    return 0;
}

inline dirp *local_get_dirp(uint64_t& fh)
{
    return (dirp *) (uintptr_t) fh;
}

int local_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, fuse_file_info *fi, uint64_t& fh)
{
    dirp *d = local_get_dirp(fh);

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

int local_releasedir(const char *path, fuse_file_info *fi, uint64_t& fh)
{
    dirp *d = local_get_dirp(fh);
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
        fuse_file_info *fi, uint64_t& fh)
{
    int res;

    (void) path;

    res = ftruncate(fh, size);
    if (res == -1)
        return -errno;

    return 0;
}

int local_create(const char *path, mode_t mode, fuse_file_info *fi, uint64_t& fh)
{
    int fd;

    fd = open(path, fi->flags, mode);
    if (fd == -1)
        return -errno;

    fh = fd;
    return 0;
}

int local_open(const char *path, fuse_file_info *fi, uint64_t& fh)
{
    int fd;

    fd = open(path, fi->flags);
    if (fd == -1)
        return -errno;

    fh = fd;
    return 0;
}

int local_read(const char *path, char *buf, size_t size, off_t offset,
        fuse_file_info *fi, uint64_t& fh)
{
    int res;

    (void) path;
    res = pread(fh, buf, size, offset);
    if (res == -1)
        res = -errno;

    return res;
}

int local_read_buf(const char *path, fuse_bufvec **bufp,
        size_t size, off_t offset, fuse_file_info *fi, uint64_t& fh)
{
    fuse_bufvec *src;

    (void) path;

    src = (fuse_bufvec *) malloc(sizeof(fuse_bufvec));
    if (src == NULL)
        return -ENOMEM;

    *src = FUSE_BUFVEC_INIT(size);

    src->buf[0].flags = (fuse_buf_flags) (FUSE_BUF_IS_FD | FUSE_BUF_FD_SEEK);
    src->buf[0].fd = fh;
    src->buf[0].pos = offset;

    *bufp = src;

    return 0;
}

int local_write(const char *path, const char *buf, size_t size,
        off_t offset, fuse_file_info *fi, uint64_t& fh)
{
    int res;

    (void) path;
    res = pwrite(fh, buf, size, offset);
    if (res == -1)
        res = -errno;

    return res;
}

int local_flush(const char *path, fuse_file_info *fi, uint64_t& fh)
{
    int res;

    (void) path;
    /* This is called from every close on an open file, so call the
       close on the underlying filesystem.	But since flush may be
       called multiple times for an open file, this must not really
       close the file.  This is important if used on a network
       filesystem like NFS which flush the data/metadata on close() */
    res = close(dup(fh));
    if (res == -1)
        return -errno;

    return 0;
}

int local_release(const char *path, fuse_file_info *fi, uint64_t& fh)
{
    (void) path;
    close(fh);

    return 0;
}

int local_fsync(const char *path, int isdatasync,
        fuse_file_info *fi, uint64_t& fh)
{
    int res;
    (void) path;

#ifndef HAVE_FDATASYNC
    (void) isdatasync;
#else
    if (isdatasync)
        res = fdatasync(fh);
    else
#endif
        res = fsync(fh);
    if (res == -1)
        return -errno;

    return 0;
}

#ifdef HAVE_POSIX_FALLOCATE
int local_fallocate(const char *path, int mode,
        off_t offset, off_t length, fuse_file_info *fi, uint64_t& fh)
{
    (void) path;

    if (mode)
        return -EOPNOTSUPP;

    return -posix_fallocate(fh, offset, length);
}
#endif

int local_flock(const char *path, fuse_file_info *fi, int op, uint64_t& fh)
{
    int res;
    (void) path;

    res = flock(fh, op);
    if (res == -1)
        return -errno;

    return 0;
}

int local_utimens(const char *path, const struct timespec times[2])
{
    int res = utimensat(AT_FDCWD, path, times, 0);

    if (res == -1)
        return -errno;

    return 0;
}

