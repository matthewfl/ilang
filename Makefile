TARGET= i

# currently not in system: netowrk.cc
# main.cc added in manually so it isn't included when unit testing
SRCS= parserTree.cc import.cc parser.cc variable.cc scope.cc object.cc database.cc modification.cc error.cc print.cc init.cc thread.cc
LIBS= -lboost_filesystem -lboost_system -lboost_thread -lssl -lpthread -lsnappy -ltbb -ltorrent-rasterbar -lprotobuf
UNIT_TESTS=$(wildcard unit_tests/*.cc)
#LIBS= /usr/lib/libboost_filesystem.a /usr/lib/libboost_system.a /usr/lib/libboost_thread.a -lsnappy -lpthread

MODULES= i/channel.io i/test.io net/curl.io net/httpd.io i/timer.io i/map.io i/eval.io

BUILDDIR=build
OBJS= $(BUILDDIR)/lex.yy.o $(BUILDDIR)/parser.tab.o $(BUILDDIR)/database.pb.o $(addprefix $(BUILDDIR)/, $(patsubst %.cc, %.o, $(filter %.cc,$(SRCS))) $(patsubst %.c, %.o, $(filter %.c, $(SRCS))))

SRCDIR=src
SRCSD=$(addprefix $(SRCDIR)/, $(SRCS))

MODULESDIR=modules
MODULESD=$(addprefix $(BUILDDIR)/$(MODULESDIR)/, $(MODULES))

INCLUDEDIR=include

# turn off all warnings so I can more easily view the errors, these need to be turn back on latter
CXXFLAGS_BASE=-DILANG_VERSION=\"$(shell git describe --always --long --dirty --abbrev=12)\" -std=c++1y -Wall -w -I$(SRCDIR)/ -I$(BUILDDIR)/ -Ideps/leveldb/include -Ideps/catch
CXXFLAGS= -ggdb -O0 -DILANG_STATIC_LIBRARY $(CXXFLAGS_BASE)
CXXFLAGS_MODULES= -ggdb -O0 -DILANG_STATIC_LIBRARY $(CXXFLAGS_BASE)
CXXFLAGS_MODULES_LINK=
########### -rdynamic might enable the Linking to work with exporting symbols to be dynamically loaded by the dynamic modules
LDFLAGS= #-Wl,-export-dynamic
# -Ideps/glog/src

CXX= g++
LD= g++ -L/usr/local/lib
PROTOC= protoc


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

$(BUILDDIR)/$(MODULESDIR)/%.io: $(MODULESDIR)/%.cc src/ilang.h
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS_MODULES) $(MINCLUDE) -o $@ -c $<

$(BUILDDIR)/parser.tab.cc $(BUILDDIR)/parser.tab.hh: $(SRCDIR)/parser.y
	bison -v -t -o $(BUILDDIR)/parser.tab.cc $<

$(BUILDDIR)/lex.yy.cc: $(SRCDIR)/scanner.l $(BUILDDIR)/parser.tab.cc $(BUILDDIR)/parser.tab.hh
	flex -o $@ $<

$(BUILDDIR)/lex.yy.o: $(BUILDDIR)/lex.yy.cc $(BUILDDIR)/parser.tab.hh
$(BUILDDIR)/parser.tab.o: $(BUILDDIR)/parser.tab.cc

$(BUILDDIR)/%.pb.cc: $(SRCDIR)/%.proto
	protoc $< --cpp_out=$(dir $@)
	cp $(BUILDDIR)/src/* $(BUILDDIR)

$(BUILDDIR)/database.pb.o: $(BUILDDIR)/database.pb.cc

$(TARGET): $(DEPS) $(OBJS) $(BUILDDIR)/main.o $(MODULESD)
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(BUILDDIR)/main.o $(MODULESD) $(LIBS)

modules: $(MODULESD)

clean:
	@rm -rf $(OBJS) $(TARGET) $(BUILDDIR)/parser.* $(BUILDDIR)/lex.yy.cc $(BUILDDIR)/src $(BUILDDIR)/database.pb* $(MODULESD) $(BUILDDIR)/$(MODULESDIR) **/*.gcov **/*.gcno **/*.gcda *.gcov *.gcno *.gcda
clean-all: clean
	cd deps/leveldb && make clean
	cd deps/libuv && make distclean
	rm -rf DB/

depend:
	makedepend -Y -- $(CXXFLAGS) -- $(SRCSD) src/main.cc
	# fixes the problem with the build dir being different from the src
	sed -i 's/src\/\([^\.]*\).o:/build\/\1.o:/g' Makefile
#eventually change this to use "g++ -MM" to generate the source files

test: $(TARGET)
	./$(TARGET) test.i -v 10
debug: $(TARGET)
	gdb i "--eval-command=run test.i -v 10" --eval-command=bt

check: $(TARGET)
	cd checks && bash ./run

unit: $(BUILDDIR)/unit
	./$(BUILDDIR)/unit

$(BUILDDIR)/unit: $(UNIT_TESTS) $(TARGET) $(MODULESD)
	$(CXX) $(CXXFLAGS) -o $(BUILDDIR)/unit $(UNIT_TESTS) $(LDFLAGS) $(OBJS) $(MODULESD) $(LIBS)

.PHONY: coverage _coverage-core
coverage:
	make clean-all
	make _coverage-core

_coverage-core: CXXFLAGS+=--coverage -fprofile-arcs -ftest-coverage
_coverage-core: LDFLAGS+= -fprofile-arcs
_coverage-core: all unit



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

build/parserTree.o: src/parserTree.h src/variable.h src/scope.h src/import.h
build/parserTree.o: src/debug.h src/print.h src/parser.h src/object.h
build/parserTree.o: src/function.h src/thread.h src/error.h
build/import.o: src/import.h src/debug.h src/variable.h src/scope.h
build/import.o: src/object.h src/parserTree.h src/print.h src/parser.h
build/import.o: src/function.h src/error.h src/ilang.h
build/parser.o: src/parser.h
build/variable.o: src/variable.h src/debug.h src/error.h src/function.h
build/variable.o: src/scope.h src/parserTree.h src/import.h src/print.h
build/variable.o: src/object.h
build/scope.o: src/scope.h src/variable.h src/debug.h src/object.h
build/scope.o: src/parserTree.h src/import.h src/print.h
build/object.o: src/object.h src/variable.h src/parserTree.h src/scope.h
build/object.o: src/import.h src/debug.h src/print.h src/error.h src/ilang.h
build/object.o: src/function.h
build/database.o: src/database.h src/debug.h src/variable.h
build/database.o: deps/leveldb/include/leveldb/db.h
build/database.o: deps/leveldb/include/leveldb/iterator.h
build/database.o: deps/leveldb/include/leveldb/slice.h
build/database.o: deps/leveldb/include/leveldb/status.h
build/database.o: deps/leveldb/include/leveldb/options.h src/ilang.h
build/database.o: src/import.h src/object.h src/parserTree.h src/scope.h
build/database.o: src/print.h src/function.h src/error.h build/database.pb.h
build/modification.o: src/modification.h src/parserTree.h src/variable.h
build/modification.o: src/scope.h src/import.h src/debug.h src/print.h
build/modification.o: src/ilang.h src/object.h src/function.h src/error.h
build/error.o: src/error.h
build/print.o: src/print.h src/debug.h
build/init.o: src/ilang.h src/import.h src/debug.h src/variable.h src/object.h
build/init.o: src/parserTree.h src/scope.h src/print.h src/function.h
build/init.o: src/database.h deps/leveldb/include/leveldb/db.h
build/init.o: deps/leveldb/include/leveldb/iterator.h
build/init.o: deps/leveldb/include/leveldb/slice.h
build/init.o: deps/leveldb/include/leveldb/status.h
build/init.o: deps/leveldb/include/leveldb/options.h
build/thread.o: src/thread.h src/debug.h deps/libuv/include/uv.h
build/thread.o: deps/libuv/include/uv-private/uv-unix.h
build/thread.o: deps/libuv/include/uv-private/ngx-queue.h
build/main.o: src/parser.h src/debug.h src/import.h src/variable.h
build/main.o: src/database.h deps/leveldb/include/leveldb/db.h
build/main.o: deps/leveldb/include/leveldb/iterator.h
build/main.o: deps/leveldb/include/leveldb/slice.h
build/main.o: deps/leveldb/include/leveldb/status.h
build/main.o: deps/leveldb/include/leveldb/options.h src/parserTree.h
build/main.o: src/scope.h src/print.h src/error.h src/thread.h
