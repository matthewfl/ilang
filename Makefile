TARGET= i

SRCS= main.cc parserTree.cc import.cc parser.cc variable.cc
LIBS=-lfl

BUILDDIR=build
OBJS= $(BUILDDIR)/lex.yy.o $(BUILDDIR)/parser.tab.o $(addprefix $(BUILDDIR)/, $(patsubst %.cc, %.o, $(filter %.cc,$(SRCS))) $(patsubst %.c, %.o, $(filter %.c, $(SRCS))))

SRCDIR=src
SRCSD=$(addprefix $(SRCDIR)/, $(SRCS))

INCLUDEDIR=include

CXXFLAGS= -ggdb -Wall -O0 -I$(INCLUDEDIR)/ -I$(INCLUDEDIR)/ilang -I$(BUILDDIR)/

CC= g++
LD= g++

all: $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cc
	$(CC) $(CXXFLAGS) -o $@ -c $<

$(BUILDDIR)/parser.tab.cc $(BUILDDIR)/parser.tab.hh: $(SRCDIR)/parser.y
	bison -v -t -o $(BUILDDIR)/parser.tab.cc $<

$(BUILDDIR)/lex.yy.cc: $(SRCDIR)/scanner.l $(BUILDDIR)/parser.tab.hh
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

src/main.o: include/parser.h /usr/include/stdio.h /usr/include/features.h
src/main.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
src/main.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
src/main.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
src/main.o: /usr/include/libio.h /usr/include/_G_config.h
src/main.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
src/main.o: /usr/include/bits/sys_errlist.h /usr/include/string.h
src/main.o: /usr/include/xlocale.h
src/parserTree.o: include/ilang/parserTree.h include/ilang/variable.h
src/import.o: include/ilang/import.h
src/parser.o: include/parser.h /usr/include/stdio.h /usr/include/features.h
src/parser.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
src/parser.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
src/parser.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
src/parser.o: /usr/include/libio.h /usr/include/_G_config.h
src/parser.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
src/parser.o: /usr/include/bits/sys_errlist.h
src/variable.o: include/ilang/variable.h
