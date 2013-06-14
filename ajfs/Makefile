THRIFT_DIR := /usr/local/include/thrift
BOOST_DIR := /usr/local/include
DFS_SRC := DFS.cpp rpc_constants.cpp rpc_types.cpp
DFS_OBJ := $(patsubst %.cpp,%.o, $(DFS_SRC))

BINS    := dfs_server dfs_client fusexmp_fh ajfs

#FUSE_DIR=/classes/cse223b/sp13/mdeblasi/fuse
#FUSE_PKGS=-D_FILE_OFFSET_BITS=64 -I$(FUSE_DIR)/include -pthread -L/lib -lfuse  
FUSE_PKGS=`pkg-config fuse --cflags --libs`
THRIFT_PKGS=`pkg-config thrift-nb libevent --cflags --libs`
#THRIFT_PKGS := -L/usr/local/lib -lthriftnb -lthrift

fusexmp_fh_DEPS=fusexmp_fh.cpp
AJFS_DEPS=ajfs.o $(DFS_OBJ) thrift_server.o local_operations.o fuse_operations.o \
			   Host.o HostManager.o Lock.o LockSet.o

CXXFLAGS := -g -Wall -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -std=c++0x -I$(THRIFT_DIR) -I$(BOOST_DIR)

.PHONY: all clean

default: ajfs
	
all: dfs_server dfs_client fusexmp_fh ajfs

#%.o: include/%.cpp
#	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) ${THRIFT_PKGS} ${FUSE_PKGS} -c $< -o $@

#DFS_client.o: DFS_client.cpp
#	$(CXX) $(CXXFLAGS) ${THRIFT_PKGS} $(INC) -c $< -o $@

dfs_server: DFS_server.skeleton.o $(DFS_OBJ)
	$(CXX) $^ -o $@ -L/usr/local/lib -lthrift

dfs_client: DFS_client.o $(DFS_OBJ)
	$(CXX) $^ -o $@ -L/usr/local/lib -lthrift

clean:
	$(RM) *.o $(BINS)

fusexmp_fh: ${fusexmp_fh_DEPS}
	$(CXX) -Wall $^ ${FUSE_PKGS} -o $@

ajfs: $(AJFS_DEPS)
	$(CXX) $(CXXFLAGS) $^ ${FUSE_PKGS} ${THRIFT_PKGS} -o $@

#depend:
#	perl -i~ -lpe 'if (m/^# -+(8<-+)+\s*\z/) { print; last; }' Makefile
#	g++ -MM $(ALL_OBJS:.o=.cpp) >> Makefile

