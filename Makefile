TARGET= i

SRCS= main.cc parserTree.cc import.cc parser.cc variable.cc scope.cc
LIBS=-lfl

BUILDDIR=build
OBJS= $(BUILDDIR)/lex.yy.o $(BUILDDIR)/parser.tab.o $(addprefix $(BUILDDIR)/, $(patsubst %.cc, %.o, $(filter %.cc,$(SRCS))) $(patsubst %.c, %.o, $(filter %.c, $(SRCS))))

SRCDIR=src
SRCSD=$(addprefix $(SRCDIR)/, $(SRCS))

INCLUDEDIR=include

CXXFLAGS= -ggdb -Wall -O0 -std=c++0x -I$(INCLUDEDIR)/ -I$(INCLUDEDIR)/ilang -I$(BUILDDIR)/

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

test: $(TARGET)
	./i test.i

# DO NOT DELETE

src/main.o: include/ilang/parser.h /usr/include/stdio.h
src/main.o: /usr/include/features.h /usr/include/sys/cdefs.h
src/main.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
src/main.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
src/main.o: /usr/include/bits/typesizes.h /usr/include/libio.h
src/main.o: /usr/include/_G_config.h /usr/include/wchar.h
src/main.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/main.o: /usr/include/string.h /usr/include/xlocale.h
src/parserTree.o: include/ilang/parserTree.h include/ilang/variable.h
src/parserTree.o: include/ilang/parser.h /usr/include/stdio.h
src/parserTree.o: /usr/include/features.h /usr/include/sys/cdefs.h
src/parserTree.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
src/parserTree.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
src/parserTree.o: /usr/include/bits/typesizes.h /usr/include/libio.h
src/parserTree.o: /usr/include/_G_config.h /usr/include/wchar.h
src/parserTree.o: /usr/include/bits/stdio_lim.h
src/parserTree.o: /usr/include/bits/sys_errlist.h include/ilang/scope.h
src/import.o: include/ilang/import.h
src/parser.o: include/ilang/parser.h /usr/include/stdio.h
src/parser.o: /usr/include/features.h /usr/include/sys/cdefs.h
src/parser.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
src/parser.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
src/parser.o: /usr/include/bits/typesizes.h /usr/include/libio.h
src/parser.o: /usr/include/_G_config.h /usr/include/wchar.h
src/parser.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
src/variable.o: include/ilang/variable.h
src/scope.o: include/ilang/scope.h include/ilang/variable.h
