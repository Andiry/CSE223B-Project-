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

#include <string>
#include <iostream>

namespace FUSEService {
    struct ArgStruct {
        int argc;
        char **argv;
        std::string backupPath;
    };
    
    int getattr(const char *path, struct stat *stbuf);
    int fgetattr(const char *path, struct stat *stbuf,
            struct fuse_file_info *fi);
    int access(const char *path, int mask);
    int readlink(const char *path, char *buf, size_t size);

    int opendir(const char *path, fuse_file_info *fi);
    inline dirp *get_dirp(fuse_file_info *fi);
    int readdir(const char *path, void *buf, fuse_fill_dir_t filler,
            off_t offset, fuse_file_info *fi);
    int releasedir(const char *path, fuse_file_info *fi);
    int mkdir(const char *path, mode_t mode);
    int unlink(const char *path);
    int rmdir(const char *path);
    int symlink(const char *from, const char *to);
    int rename(const char *from, const char *to);
    int link(const char *from, const char *to);
    int chmod(const char *path, mode_t mode);
    int chown(const char *path, uid_t uid, gid_t gid);
    int truncate(const char *path, off_t size);
    int ftruncate(const char *path, off_t size,
            fuse_file_info *fi);
    int create(const char *path, mode_t mode, fuse_file_info *fi);
    int open(const char *path, fuse_file_info *fi);
    int read(const char *path, char *buf, size_t size, off_t offset,
            fuse_file_info *fi);
    int read_buf(const char *path, fuse_bufvec **bufp,
            size_t size, off_t offset, fuse_file_info *fi);
    int write(const char *path, const char *buf, size_t size,
            off_t offset, fuse_file_info *fi);
    int flush(const char *path, fuse_file_info *fi);
    int release(const char *path, fuse_file_info *fi);
    int fsync(const char *path, int isdatasync,
            fuse_file_info *fi);
    int flock(const char *path, fuse_file_info *fi, int op);
    #ifdef HAVE_POSIX_FALLOCATE
    int fallocate(const char *path, int mode,
            off_t offset, off_t length, fuse_file_info *fi);
    #endif
    
    void * start(void * arg);
    void stop();
}


#endif
