#ifndef LFS_OPERATIONS_HPP_INC
#define LFS_OPERATIONS_HPP_INC 1

#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif
#include <sys/file.h> /* flock(2) */

#include <iostream>

int local_getattr(const char *path, struct stat *stbuf);
int local_fgetattr(const char *path, struct stat *stbuf,
        struct fuse_file_info *fi);
int local_access(const char *path, int mask);
int local_readlink(const char *path, char *buf, size_t size);
struct dirp {
    DIR *dp;
    struct dirent *entry;
    off_t offset;
};

int local_opendir(const char *path, struct fuse_file_info *fi);
struct dirp *local_get_dirp(struct fuse_file_info *fi);
int local_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi);
int local_releasedir(const char *path, struct fuse_file_info *fi);
int local_mkdir(const char *path, mode_t mode);
int local_unlink(const char *path);
int local_rmdir(const char *path);
int local_symlink(const char *from, const char *to);
int local_rename(const char *from, const char *to);
int local_link(const char *from, const char *to);
int local_chmod(const char *path, mode_t mode);
int local_chown(const char *path, uid_t uid, gid_t gid);
int local_truncate(const char *path, off_t size);
int local_ftruncate(const char *path, off_t size,
        struct fuse_file_info *fi);
int local_create(const char *path, mode_t mode, struct fuse_file_info *fi);
int local_open(const char *path, struct fuse_file_info *fi);
int local_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi);
int local_read_buf(const char *path, struct fuse_bufvec **bufp,
        size_t size, off_t offset, struct fuse_file_info *fi);
int local_write(const char *path, const char *buf, size_t size,
        off_t offset, struct fuse_file_info *fi);
int local_flush(const char *path, struct fuse_file_info *fi);
int local_release(const char *path, struct fuse_file_info *fi);
int local_fsync(const char *path, int isdatasync,
        struct fuse_file_info *fi);
int local_flock(const char *path, struct fuse_file_info *fi, int op);
#ifdef HAVE_POSIX_FALLOCATE
int local_fallocate(const char *path, int mode,
        off_t offset, off_t length, struct fuse_file_info *fi);
#endif

#endif

