/**
 * Autogenerated by Thrift Compiler (0.9.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef rpc_TYPES_H
#define rpc_TYPES_H

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>



namespace DFS {

struct LockType {
  enum type {
    READ = 1,
    WRITE = 2
  };
};

extern const std::map<int, const char*> _LockType_VALUES_TO_NAMES;

typedef struct _FUSEFileInfoTransport__isset {
  _FUSEFileInfoTransport__isset() : flags(false), fh_old(false), writepage(false), direct_io(false), keep_cache(false), flush(false), nonseekable(false), padding(false), fh(false), lock_owner(false) {}
  bool flags;
  bool fh_old;
  bool writepage;
  bool direct_io;
  bool keep_cache;
  bool flush;
  bool nonseekable;
  bool padding;
  bool fh;
  bool lock_owner;
} _FUSEFileInfoTransport__isset;

class FUSEFileInfoTransport {
 public:

  static const char* ascii_fingerprint; // = "BE5E44D98B2BCC2B88857EC323D2F13D";
  static const uint8_t binary_fingerprint[16]; // = {0xBE,0x5E,0x44,0xD9,0x8B,0x2B,0xCC,0x2B,0x88,0x85,0x7E,0xC3,0x23,0xD2,0xF1,0x3D};

  FUSEFileInfoTransport() : flags(0), fh_old(0), writepage(0), direct_io(0), keep_cache(0), flush(0), nonseekable(0), padding(0), fh(0), lock_owner(0) {
  }

  virtual ~FUSEFileInfoTransport() throw() {}

  int32_t flags;
  int64_t fh_old;
  int32_t writepage;
  int32_t direct_io;
  int32_t keep_cache;
  int32_t flush;
  int32_t nonseekable;
  int32_t padding;
  int64_t fh;
  int64_t lock_owner;

  _FUSEFileInfoTransport__isset __isset;

  void __set_flags(const int32_t val) {
    flags = val;
  }

  void __set_fh_old(const int64_t val) {
    fh_old = val;
  }

  void __set_writepage(const int32_t val) {
    writepage = val;
  }

  void __set_direct_io(const int32_t val) {
    direct_io = val;
  }

  void __set_keep_cache(const int32_t val) {
    keep_cache = val;
  }

  void __set_flush(const int32_t val) {
    flush = val;
  }

  void __set_nonseekable(const int32_t val) {
    nonseekable = val;
  }

  void __set_padding(const int32_t val) {
    padding = val;
  }

  void __set_fh(const int64_t val) {
    fh = val;
  }

  void __set_lock_owner(const int64_t val) {
    lock_owner = val;
  }

  bool operator == (const FUSEFileInfoTransport & rhs) const
  {
    if (!(flags == rhs.flags))
      return false;
    if (!(fh_old == rhs.fh_old))
      return false;
    if (!(writepage == rhs.writepage))
      return false;
    if (!(direct_io == rhs.direct_io))
      return false;
    if (!(keep_cache == rhs.keep_cache))
      return false;
    if (!(flush == rhs.flush))
      return false;
    if (!(nonseekable == rhs.nonseekable))
      return false;
    if (!(padding == rhs.padding))
      return false;
    if (!(fh == rhs.fh))
      return false;
    if (!(lock_owner == rhs.lock_owner))
      return false;
    return true;
  }
  bool operator != (const FUSEFileInfoTransport &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const FUSEFileInfoTransport & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

void swap(FUSEFileInfoTransport &a, FUSEFileInfoTransport &b);

typedef struct _TimeSpec__isset {
  _TimeSpec__isset() : sec(false), nsec(false) {}
  bool sec;
  bool nsec;
} _TimeSpec__isset;

class TimeSpec {
 public:

  static const char* ascii_fingerprint; // = "F33135321253DAEB67B0E79E416CA831";
  static const uint8_t binary_fingerprint[16]; // = {0xF3,0x31,0x35,0x32,0x12,0x53,0xDA,0xEB,0x67,0xB0,0xE7,0x9E,0x41,0x6C,0xA8,0x31};

  TimeSpec() : sec(0), nsec(0) {
  }

  virtual ~TimeSpec() throw() {}

  int64_t sec;
  int64_t nsec;

  _TimeSpec__isset __isset;

  void __set_sec(const int64_t val) {
    sec = val;
  }

  void __set_nsec(const int64_t val) {
    nsec = val;
  }

  bool operator == (const TimeSpec & rhs) const
  {
    if (!(sec == rhs.sec))
      return false;
    if (!(nsec == rhs.nsec))
      return false;
    return true;
  }
  bool operator != (const TimeSpec &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const TimeSpec & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

void swap(TimeSpec &a, TimeSpec &b);

typedef struct _HostID__isset {
  _HostID__isset() : hostname(false), port(false) {}
  bool hostname;
  bool port;
} _HostID__isset;

class HostID {
 public:

  static const char* ascii_fingerprint; // = "3628A1EB414F66736E1B2A082E79475F";
  static const uint8_t binary_fingerprint[16]; // = {0x36,0x28,0xA1,0xEB,0x41,0x4F,0x66,0x73,0x6E,0x1B,0x2A,0x08,0x2E,0x79,0x47,0x5F};

  HostID() : hostname(), port(0) {
  }

  virtual ~HostID() throw() {}

  std::string hostname;
  int16_t port;

  _HostID__isset __isset;

  void __set_hostname(const std::string& val) {
    hostname = val;
  }

  void __set_port(const int16_t val) {
    port = val;
  }

  bool operator == (const HostID & rhs) const
  {
    if (!(hostname == rhs.hostname))
      return false;
    if (!(port == rhs.port))
      return false;
    return true;
  }
  bool operator != (const HostID &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const HostID & ) const;

  void operator = (const HostID & rhs)
  {
    hostname = rhs.hostname;
    port = rhs.port;
  }

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

void swap(HostID &a, HostID &b);

} // namespace

#endif
