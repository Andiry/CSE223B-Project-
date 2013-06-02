#ifndef FUSE_OPERATIONS_HPP_INC
#define FUSE_OPERATIONS_HPP_INC 1

#define FUSE_USE_VERSION 26

#include "LFS_operations.hpp"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuse.h>
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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>

using namespace std;

int from_fuse_getattr(const char *path, struct stat *stbuf);
int from_fuse_fgetattr(const char *path, struct stat *stbuf,
        struct fuse_file_info *fi);
int from_fuse_access(const char *path, int mask);
int from_fuse_readlink(const char *path, char *buf, size_t size);
/*
struct dirp {
    DIR *dp;
    struct dirent *entry;
    off_t offset;
};*/

int from_fuse_opendir(const char *path, struct fuse_file_info *fi);
inline struct dirp *from_fuse_get_dirp(struct fuse_file_info *fi);
int from_fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi);
int from_fuse_releasedir(const char *path, struct fuse_file_info *fi);
int from_fuse_mkdir(const char *path, mode_t mode);
int from_fuse_unlink(const char *path);
int from_fuse_rmdir(const char *path);
int from_fuse_symlink(const char *from, const char *to);
int from_fuse_rename(const char *from, const char *to);
int from_fuse_link(const char *from, const char *to);
int from_fuse_chmod(const char *path, mode_t mode);
int from_fuse_chown(const char *path, uid_t uid, gid_t gid);
int from_fuse_truncate(const char *path, off_t size);
int from_fuse_ftruncate(const char *path, off_t size,
        struct fuse_file_info *fi);
int from_fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi);
int from_fuse_open(const char *path, struct fuse_file_info *fi);
int from_fuse_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi);
int from_fuse_read_buf(const char *path, struct fuse_bufvec **bufp,
        size_t size, off_t offset, struct fuse_file_info *fi);
int from_fuse_write(const char *path, const char *buf, size_t size,
        off_t offset, struct fuse_file_info *fi);
int from_fuse_flush(const char *path, struct fuse_file_info *fi);
int from_fuse_release(const char *path, struct fuse_file_info *fi);
int from_fuse_fsync(const char *path, int isdatasync,
        struct fuse_file_info *fi);
int from_fuse_flock(const char *path, struct fuse_file_info *fi, int op);
#ifdef HAVE_POSIX_FALLOCATE
int from_fuse_fallocate(const char *path, int mode,
        off_t offset, off_t length, struct fuse_file_info *fi);
#endif


void initOpers(fuse_operations& oper);

#ifndef _ARG_STRUCT_
#define _ARG_STRUCT_
struct ArgStruct {
    int argc;
    char **argv;
};
#endif

void * startFuse(void * arg);
void PropagateToOtherServers(const string op, const char *path, mode_t mode, struct fuse_file_info *fi);

#endif
