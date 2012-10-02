TARGET= i

SRCS= main.cc parserTree.cc import.cc parser.cc variable.cc scope.cc object.cc database.cc modification.cc error.cc network.cc print.cc
LIBS= -lboost_filesystem -lboost_system -lboost_thread -lssl -lpthread -lsnappy -ltorrent-rasterbar
#LIBS= /usr/lib/libboost_filesystem.a /usr/lib/libboost_system.a /usr/lib/libboost_thread.a -lsnappy -lpthread

MODULES= i/test.io net/curl.io

BUILDDIR=build
OBJS= $(BUILDDIR)/lex.yy.o $(BUILDDIR)/parser.tab.o $(addprefix $(BUILDDIR)/, $(patsubst %.cc, %.o, $(filter %.cc,$(SRCS))) $(patsubst %.c, %.o, $(filter %.c, $(SRCS))))

SRCDIR=src
SRCSD=$(addprefix $(SRCDIR)/, $(SRCS))

MODULESDIR=modules
MODULESD=$(addprefix $(BUILDDIR)/$(MODULESDIR)/, $(MODULES))

INCLUDEDIR=include

# turn off all warnings so I can more easily view the errors, these need to be turn back on latter
CXXFLAGS_BASE=-DILANG_VERSION=\"$(shell git describe --always --long --dirty --abbrev=12)\" -std=c++11 -Wall -w -I$(INCLUDEDIR)/ -I$(INCLUDEDIR)/ilang -I$(BUILDDIR)/ -Ideps/leveldb/include
CXXFLAGS= -ggdb -O0 -static -DILANG_STATIC_LIBRARY $(CXXFLAGS_BASE)
CXXFLAGS_MODULES= -ggdb -O0 -static -DILANG_STATIC_LIBRARY $(CXXFLAGS_BASE)
CXXFLAGS_MODULES_LINK=
LDFLAGS= -static-libgcc
# -Ideps/glog/src

CXX= g++
LD= g++ -L/usr/local/lib


# settings for building deps
glogLib=./deps/glog/.libs/libglog.a
#LIBS+= $(glogLib)
leveldb=./deps/leveldb/libleveldb.a
LIBS+=$(leveldb)

DEPS=$(leveldb)
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
	rm -rf DB/

depend: submodule
	makedepend -Y -- $(CXXFLAGS) -- $(SRCSD) 
	# fixes the problem with the build dir being different from the src
	sed -i 's/src\/\([^\.]*\).o/build\/\1.o/g' Makefile 

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

# DO NOT DELETE

build/main.o: include/ilang/parser.h include/debug.h include/ilang/import.h
build/main.o: include/ilang/variable.h include/ilang/database.h
build/main.o: deps/leveldb/include/leveldb/db.h
build/main.o: deps/leveldb/include/leveldb/iterator.h
build/main.o: deps/leveldb/include/leveldb/slice.h
build/main.o: deps/leveldb/include/leveldb/status.h
build/main.o: deps/leveldb/include/leveldb/options.h include/ilang/parserTree.h
build/main.o: include/ilang/scope.h include/ilang/error.h
build/parserTree.o: include/ilang/parserTree.h include/ilang/variable.h
build/parserTree.o: include/ilang/scope.h include/ilang/import.h
build/parserTree.o: include/debug.h include/ilang/parser.h
build/parserTree.o: include/ilang/object.h include/ilang/function.h
build/parserTree.o: include/ilang/error.h
build/import.o: include/ilang/import.h include/debug.h include/ilang/variable.h
build/import.o: include/ilang/scope.h include/ilang/object.h
build/import.o: include/ilang/parserTree.h include/ilang/parser.h
build/import.o: include/ilang/function.h include/ilang/ilang.h
build/import.o: include/ilang/import.h include/ilang/object.h
build/import.o: include/ilang/function.h
build/parser.o: include/ilang/parser.h
build/variable.o: include/ilang/variable.h include/debug.h
build/variable.o: include/ilang/error.h include/ilang/object.h
build/variable.o: include/ilang/parserTree.h include/ilang/scope.h
build/variable.o: include/ilang/import.h
build/scope.o: include/ilang/scope.h include/ilang/variable.h include/debug.h
build/scope.o: include/ilang/object.h include/ilang/parserTree.h
build/scope.o: include/ilang/import.h
build/object.o: include/ilang/object.h include/ilang/variable.h
build/object.o: include/ilang/parserTree.h include/ilang/scope.h
build/object.o: include/ilang/import.h include/debug.h include/ilang/error.h
build/object.o: include/ilang/ilang.h include/ilang/import.h
build/object.o: include/ilang/object.h include/ilang/function.h
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
build/modification.o: include/ilang/ilang.h include/ilang/import.h
build/modification.o: include/ilang/object.h include/ilang/function.h
build/modification.o: include/ilang/error.h
build/error.o: include/ilang/error.h
build/network.o: include/ilang/network.h
