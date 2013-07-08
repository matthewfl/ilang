TARGET= i

# currently not in system: netowrk.cc
SRCS= main.cc parserTree.cc import.cc parser.cc variable.cc scope.cc object.cc database.cc modification.cc error.cc print.cc init.cc thread.cc
LIBS= -lboost_filesystem -lboost_system -lboost_thread -lssl -lpthread -lsnappy -ltbb -ltorrent-rasterbar
#LIBS= /usr/lib/libboost_filesystem.a /usr/lib/libboost_system.a /usr/lib/libboost_thread.a -lsnappy -lpthread

MODULES= i/channel.io i/test.io net/curl.io net/httpd.io i/timer.io i/map.io i/eval.io

BUILDDIR=build
OBJS= $(BUILDDIR)/lex.yy.o $(BUILDDIR)/parser.tab.o $(addprefix $(BUILDDIR)/, $(patsubst %.cc, %.o, $(filter %.cc,$(SRCS))) $(patsubst %.c, %.o, $(filter %.c, $(SRCS))))

SRCDIR=src
SRCSD=$(addprefix $(SRCDIR)/, $(SRCS))

MODULESDIR=modules
MODULESD=$(addprefix $(BUILDDIR)/$(MODULESDIR)/, $(MODULES))

INCLUDEDIR=include

# turn off all warnings so I can more easily view the errors, these need to be turn back on latter
CXXFLAGS_BASE=-DILANG_VERSION=\"$(shell git describe --always --long --dirty --abbrev=12)\" -std=c++11 -Wall -w -I$(INCLUDEDIR)/ -I$(INCLUDEDIR)/ilang -I$(BUILDDIR)/ -Ideps/leveldb/include
CXXFLAGS= -ggdb -O0 -DILANG_STATIC_LIBRARY $(CXXFLAGS_BASE)
CXXFLAGS_MODULES= -ggdb -O0 -DILANG_STATIC_LIBRARY $(CXXFLAGS_BASE)
CXXFLAGS_MODULES_LINK=
########### -rdynamic might enable the Linking to work with exporting symbols to be dynamically loaded by the dynamic modules
LDFLAGS= -static-libgcc #-Wl,-export-dynamic
# -Ideps/glog/src

CXX= g++
LD= g++ -L/usr/local/lib


# settings for building deps
glogLib=./deps/glog/.libs/libglog.a
#LIBS+= $(glogLib)
leveldb=./deps/leveldb/libleveldb.a
LIBS+=$(leveldb)
libuv=./deps/libuv/libuv.a
LIBS+=$(libuv) -lrt

DEPS=$(leveldb) $(libuv)
#$(glogLib)

#libs for modules
#LIBS+= -Wl,-Bstatic $(shell pkg-config libcurl --libs --static)
#LIBS+= $(shell curl-config --static-libs)
LIBS+=-lcurl



.PHONY: all release test debug clean clean-all depend submodule check
# start of commands
all: $(TARGET) $(MODULESD)

#add back in to everything -s to remove symbols table
#would like to make the system build it static, but curl pervents this atm
# -O3 seems to cause a segfault with simple programs now, -O2 works
release: CXXFLAGS= -ggdb -O2 -DILANG_STATIC_LIBRARY $(CXXFLAGS_BASE)
release: CXXFLAGS_MODULES= -O2 -DILANG_STATIC_LIBRARY $(CXXFLAGS_BASE)
release: LDFLAGS+= $(MODULESD)
release: submodule clean $(MODULESD) $(TARGET)

submodule:
	git submodule update --init --recursive 

$(BUILDDIR)/%.o: $(SRCDIR)/%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILDDIR)/$(MODULESDIR)/%.io: $(MODULESDIR)/%.cc include/ilang/ilang.h
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS_MODULES) $(MINCLUDE) -o $@ -c $<

$(BUILDDIR)/parser.tab.cc $(BUILDDIR)/parser.tab.hh: $(SRCDIR)/parser.y
	bison -v -t -o $(BUILDDIR)/parser.tab.cc $<

$(BUILDDIR)/lex.yy.cc: $(SRCDIR)/scanner.l $(BUILDDIR)/parser.tab.cc $(BUILDDIR)/parser.tab.hh
	flex -o $@ $<

$(BUILDDIR)/lex.yy.o: $(BUILDDIR)/lex.yy.cc $(BUILDDIR)/parser.tab.hh
$(BUILDDIR)/parser.tab.o: $(BUILDDIR)/parser.tab.cc

$(TARGET): $(DEPS) $(OBJS) $(MODULESD) 
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(MODULESD) $(LIBS)

modules: $(MODULESD)

clean:
	rm -rf $(OBJS) $(TARGET) $(BUILDDIR)/parser.* $(BUILDDIR)/lex.yy.cc $(MODULESD) $(BUILDDIR)/$(MODULESDIR)
clean-all: clean
	cd deps/leveldb && make clean
	cd deps/libuv && make distclean
	rm -rf DB/

depend:
	makedepend -Y -- $(CXXFLAGS) -- $(SRCSD) 
	# fixes the problem with the build dir being different from the src
	sed -i 's/src\/\([^\.]*\).o/build\/\1.o/g' Makefile 
#eventually change this to use "g++ -MM" to generate the source files

test: $(TARGET)
	./$(TARGET) test.i -v 10
debug: $(TARGET)
	gdb i "--eval-command=run test.i -v 10" --eval-command=bt

check: $(TARGET)
	cd checks && bash ./run

# all the settings to build modules

#$(MODULESDIR)/test.io: LIBS= -lssl
#$(BUILDDIR)/$(MODULESDIR)/net/curl.io: MLIBS= -lcurl

# all the commands to build any deps
$(glogLib): ./deps/glog/build/base/g.ogleinit.h
	cd deps/glog && ./configure
	cd deps/glog && make

$(leveldb): ./deps/leveldb/include/leveldb/db.h
	cd deps/leveldb && make

$(libuv): ./deps/libuv/include/uv.h
	cd deps/libuv && make

# DO NOT DELETE

build/main.o: include/ilang/parser.h include/debug.h include/ilang/import.h
build/main.o: include/ilang/variable.h include/ilang/database.h
build/main.o: deps/leveldb/include/leveldb/db.h
build/main.o: deps/leveldb/include/leveldb/iterator.h
build/main.o: deps/leveldb/include/leveldb/slice.h
build/main.o: deps/leveldb/include/leveldb/status.h
build/main.o: deps/leveldb/include/leveldb/options.h include/ilang/parserTree.h
build/main.o: include/ilang/scope.h include/ilang/print.h include/ilang/error.h
build/main.o: include/ilang/thread.h
build/parserTree.o: include/ilang/parserTree.h include/ilang/variable.h
build/parserTree.o: include/ilang/scope.h include/ilang/import.h
build/parserTree.o: include/debug.h include/ilang/print.h
build/parserTree.o: include/ilang/parser.h include/ilang/object.h
build/parserTree.o: include/ilang/function.h include/ilang/thread.h
build/parserTree.o: include/ilang/error.h
build/import.o: include/ilang/import.h include/debug.h include/ilang/variable.h
build/import.o: include/ilang/scope.h include/ilang/object.h
build/import.o: include/ilang/parserTree.h include/ilang/print.h
build/import.o: include/ilang/parser.h include/ilang/function.h
build/import.o: include/ilang/error.h include/ilang/ilang.h
build/import.o: include/ilang/import.h include/ilang/object.h
build/import.o: include/ilang/function.h
build/parser.o: include/ilang/parser.h
build/variable.o: include/ilang/variable.h include/debug.h
build/variable.o: include/ilang/error.h include/ilang/function.h
build/variable.o: include/ilang/scope.h include/ilang/parserTree.h
build/variable.o: include/ilang/import.h include/ilang/print.h
build/variable.o: include/ilang/object.h
build/scope.o: include/ilang/scope.h include/ilang/variable.h include/debug.h
build/scope.o: include/ilang/object.h include/ilang/parserTree.h
build/scope.o: include/ilang/import.h include/ilang/print.h
build/object.o: include/ilang/object.h include/ilang/variable.h
build/object.o: include/ilang/parserTree.h include/ilang/scope.h
build/object.o: include/ilang/import.h include/debug.h include/ilang/print.h
build/object.o: include/ilang/error.h include/ilang/ilang.h
build/object.o: include/ilang/import.h include/ilang/object.h
build/object.o: include/ilang/function.h
build/database.o: include/ilang/database.h include/debug.h
build/database.o: include/ilang/variable.h deps/leveldb/include/leveldb/db.h
build/database.o: deps/leveldb/include/leveldb/iterator.h
build/database.o: deps/leveldb/include/leveldb/slice.h
build/database.o: deps/leveldb/include/leveldb/status.h
build/database.o: deps/leveldb/include/leveldb/options.h include/ilang/ilang.h
build/database.o: include/ilang/import.h include/ilang/object.h
build/database.o: include/ilang/function.h include/ilang/error.h
build/modification.o: include/ilang/modification.h include/ilang/parserTree.h
build/modification.o: include/ilang/variable.h include/ilang/scope.h
build/modification.o: include/ilang/import.h include/debug.h
build/modification.o: include/ilang/print.h include/ilang/ilang.h
build/modification.o: include/ilang/import.h include/ilang/object.h
build/modification.o: include/ilang/function.h include/ilang/error.h
build/error.o: include/ilang/error.h
build/print.o: include/ilang/print.h include/debug.h
build/init.o: include/ilang/ilang.h include/ilang/import.h
build/init.o: include/ilang/object.h include/ilang/function.h
build/init.o: include/ilang/import.h include/debug.h include/ilang/variable.h
build/init.o: include/ilang/database.h deps/leveldb/include/leveldb/db.h
build/init.o: deps/leveldb/include/leveldb/iterator.h
build/init.o: deps/leveldb/include/leveldb/slice.h
build/init.o: deps/leveldb/include/leveldb/status.h
build/init.o: deps/leveldb/include/leveldb/options.h
build/thread.o: include/ilang/thread.h include/debug.h deps/libuv/include/uv.h
build/thread.o: deps/libuv/include/uv-private/uv-unix.h
build/thread.o: deps/libuv/include/uv-private/ngx-queue.h
