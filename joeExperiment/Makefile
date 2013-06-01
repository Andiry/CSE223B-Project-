THRIFT_DIR := /usr/local/include/thrift
BOOST_DIR := /usr/local/include
INCLUDE := include
DFS_SRC := DFS.cpp rpc_constants.cpp rpc_types.cpp
DFS_OBJ := $(patsubst %.cpp,%.o, $(DFS_SRC))

BINS    := dfs_server dfs_client fusexmp_fh connected

FUSE_PKGS=`pkg-config fuse --cflags --libs`
#FUSE_LIBS=-L/usr/local/lib -lthrift -lulockmgr
THRIFT_PKGS=-L/usr/local/lib -lthrift

fusexmp_fh_DEPS=fusexmp_fh.cpp
connected_DEPS=connected.o $(DFS_OBJ) DFS_server.o LFS_operations.o fuse_operations.o \
			   Host.o lockmanager.o

CXXFLAGS := -g -Wall -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -std=c++0x

INC := -I$(THRIFT_DIR) -I$(BOOST_DIR) -I$(INCLUDE)

.PHONY: all clean
	
all: dfs_server dfs_client fusexmp_fh connected

%.o: include/%.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) ${FUSE_PKGS} $(INC) -c $< -o $@

dfs_server: DFS_server.skeleton.o $(DFS_OBJ)
	$(CXX) $^ -o $@ -L/usr/local/lib -lthrift

dfs_client: DFS_client.o $(DFS_OBJ)
	$(CXX) $^ -o $@ -L/usr/local/lib -lthrift

clean:
	$(RM) *.o $(BINS)

fusexmp_fh: ${fusexmp_fh_DEPS}
	$(CXX) -Wall $^ ${FUSE_PKGS} -o $@

connected: $(connected_DEPS)
	$(CXX) $(CXXFLAGS) $^ ${FUSE_PKGS} ${THRIFT_PKGS} -o $@

