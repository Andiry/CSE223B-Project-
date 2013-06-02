#ifndef FUSE_OPERATIONS_HPP_INC
#define FUSE_OPERATIONS_HPP_INC 1

#define FUSE_USE_VERSION 26

#include "LFS_operations.hpp"
#include "GlobalBucket.hpp"

extern "C" {
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
}

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>
#include <iostream>
#include <sstream>

namespace FUSEService {
    struct ArgStruct {
        int argc;
        char **argv;
        std::string backupPath;
        GlobalBucket * globals;
    };
    
    extern "C" {
        int fuse_getattr(const char *path, struct stat *stbuf);
        int fuse_fgetattr(const char *path, struct stat *stbuf,
                struct fuse_file_info *fi);
        int fuse_access(const char *path, int mask);
        int fuse_readlink(const char *path, char *buf, size_t size);
    
        int fuse_opendir(const char *path, fuse_file_info *fi);
        inline dirp *fuse_get_dirp(fuse_file_info *fi);
        int fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                off_t offset, struct fuse_file_info *fi);
        int fuse_releasedir(const char *path, fuse_file_info *fi);
        int fuse_mkdir(const char *path, mode_t mode);
        int fuse_unlink(const char *path);
        int fuse_rmdir(const char *path);
        int fuse_symlink(const char *from, const char *to);
        int fuse_rename(const char *from, const char *to);
        int fuse_link(const char *from, const char *to);
        int fuse_chmod(const char *path, mode_t mode);
        int fuse_chown(const char *path, uid_t uid, gid_t gid);
        int fuse_truncate(const char *path, off_t size);
        int fuse_ftruncate(const char *path, off_t size,
                fuse_file_info *fi);
        int fuse_create(const char *path, mode_t mode, fuse_file_info *fi);
        int fuse_open(const char *path, fuse_file_info *fi);
        int fuse_read(const char *path, char *buf, size_t size, off_t offset,
                fuse_file_info *fi);
        int fuse_read_buf(const char *path, fuse_bufvec **bufp,
                size_t size, off_t offset, fuse_file_info *fi);
        int fuse_write(const char *path, const char *buf, size_t size,
                off_t offset, fuse_file_info *fi);
        int fuse_flush(const char *path, fuse_file_info *fi);
        int fuse_release(const char *path, fuse_file_info *fi);
        int fuse_fsync(const char *path, int isdatasync,
                fuse_file_info *fi);
        int fuse_flock(const char *path, fuse_file_info *fi, int op);
        #ifdef HAVE_POSIX_FALLOCATE
        int fuse_fallocate(const char *path, int mode,
                off_t offset, off_t length, fuse_file_info *fi);
        #endif
    }
    
    void * start(void * arg);
    void stop();
}


#endif
