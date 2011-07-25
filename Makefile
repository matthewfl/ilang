TARGET= i

SRCS= main.cc parserTree.cc import.cc parser.cc variable.cc scope.cc
LIBS=-lfl

BUILDDIR=build
OBJS= $(BUILDDIR)/lex.yy.o $(BUILDDIR)/parser.tab.o $(addprefix $(BUILDDIR)/, $(patsubst %.cc, %.o, $(filter %.cc,$(SRCS))) $(patsubst %.c, %.o, $(filter %.c, $(SRCS))))

SRCDIR=src
SRCSD=$(addprefix $(SRCDIR)/, $(SRCS))

INCLUDEDIR=include

# turn off all warnings so I can more easily view the errors, these need to be turn back on latter
CXXFLAGS= -ggdb -Wall -O0 -std=c++0x -w -I$(INCLUDEDIR)/ -I$(INCLUDEDIR)/ilang -I$(BUILDDIR)/

CXX= g++
LD= g++

all: $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILDDIR)/parser.tab.cc $(BUILDDIR)/parser.tab.hh: $(SRCDIR)/parser.y
	bison -v -t -o $(BUILDDIR)/parser.tab.cc $<

$(BUILDDIR)/lex.yy.cc: $(SRCDIR)/scanner.l $(BUILDDIR)/parser.tab.cc
	flex -o $@ $<

$(BUILDDIR)/lex.yy.o: $(BUILDDIR)/lex.yy.cc $(BUILDDIR)/parser.tab.hh
$(BUILDDIR)/parser.tab.o: $(BUILDDIR)/parser.tab.cc

$(TARGET): $(OBJS)
	$(LD) -o $@ $(OBJS) $(LIBS)

clean:
	rm -rf $(OBJS) $(TARGET) $(BUILDDIR)/parser.* $(BUILDDIR)/lex.yy.cc

depend:
	makedepend -- $(CXXFLAGS) -- $(SRCSD) 
	# fixes the problem with the build dir being different from the src
	sed -i 's/src\/\([^\.]*\).o/build\/\1.o/g' Makefile 

test: $(TARGET)
	$(TARGET) test.i
debug: $(TARGET)
	gdb i "--eval-command=run test.i" --eval-command=bt

# DO NOT DELETE

build/main.o: include/ilang/parser.h /usr/include/stdio.h
build/main.o: /usr/include/features.h /usr/include/sys/cdefs.h
build/main.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
build/main.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
build/main.o: /usr/include/bits/typesizes.h /usr/include/libio.h
build/main.o: /usr/include/_G_config.h /usr/include/wchar.h
build/main.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
build/main.o: /usr/include/string.h /usr/include/xlocale.h
build/parserTree.o: include/ilang/parserTree.h include/ilang/variable.h
build/parserTree.o: include/ilang/parser.h /usr/include/stdio.h
build/parserTree.o: /usr/include/features.h /usr/include/sys/cdefs.h
build/parserTree.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
build/parserTree.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
build/parserTree.o: /usr/include/bits/typesizes.h /usr/include/libio.h
build/parserTree.o: /usr/include/_G_config.h /usr/include/wchar.h
build/parserTree.o: /usr/include/bits/stdio_lim.h
build/parserTree.o: /usr/include/bits/sys_errlist.h include/ilang/scope.h
build/parserTree.o: include/debug.h /usr/include/assert.h
build/import.o: include/ilang/import.h
build/parser.o: include/ilang/parser.h /usr/include/stdio.h
build/parser.o: /usr/include/features.h /usr/include/sys/cdefs.h
build/parser.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
build/parser.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
build/parser.o: /usr/include/bits/typesizes.h /usr/include/libio.h
build/parser.o: /usr/include/_G_config.h /usr/include/wchar.h
build/parser.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
build/variable.o: include/ilang/variable.h include/debug.h
build/variable.o: /usr/include/assert.h /usr/include/features.h
build/variable.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
build/variable.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
build/scope.o: include/ilang/scope.h include/ilang/variable.h include/debug.h
build/scope.o: /usr/include/assert.h /usr/include/features.h
build/scope.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
build/scope.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
