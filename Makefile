TARGET= i

SRCS= main.cc parserTree.cc import.cc parser.cc variable.cc scope.cc object.cc
LIBS=-lfl -lpthread

BUILDDIR=build
OBJS= $(BUILDDIR)/lex.yy.o $(BUILDDIR)/parser.tab.o $(addprefix $(BUILDDIR)/, $(patsubst %.cc, %.o, $(filter %.cc,$(SRCS))) $(patsubst %.c, %.o, $(filter %.c, $(SRCS))))

SRCDIR=src
SRCSD=$(addprefix $(SRCDIR)/, $(SRCS))

INCLUDEDIR=include

# turn off all warnings so I can more easily view the errors, these need to be turn back on latter
CXXFLAGS= -ggdb -Wall -O0 -std=c++0x -w -I$(INCLUDEDIR)/ -I$(INCLUDEDIR)/ilang -I$(BUILDDIR)/
# -Ideps/glog/src

CXX= g++
LD= g++


# settings for building deps
glogLib=./deps/glog/.libs/libglog.a
#LIBS+= $(glogLib)

DEPS=
#$(glogLib)



# start of commands
all: $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILDDIR)/parser.tab.cc $(BUILDDIR)/parser.tab.hh: $(SRCDIR)/parser.y
	bison -v -t -o $(BUILDDIR)/parser.tab.cc $<

$(BUILDDIR)/lex.yy.cc: $(SRCDIR)/scanner.l $(BUILDDIR)/parser.tab.cc
	flex -o $@ $<

$(BUILDDIR)/lex.yy.o: $(BUILDDIR)/lex.yy.cc $(BUILDDIR)/parser.tab.hh
$(BUILDDIR)/parser.tab.o: $(BUILDDIR)/parser.tab.cc

$(TARGET): $(DEPS) $(OBJS)
	$(LD) -o $@ $(OBJS) $(LIBS)

clean:
	rm -rf $(OBJS) $(TARGET) $(BUILDDIR)/parser.* $(BUILDDIR)/lex.yy.cc
clean-all: clean
	rm -rf deps/glog/Makefile deps/glog/.libs

depend:
	makedepend -- $(CXXFLAGS) -- $(SRCSD) 
	# fixes the problem with the build dir being different from the src
	sed -i 's/src\/\([^\.]*\).o/build\/\1.o/g' Makefile 

test: $(TARGET)
	./$(TARGET) test.i -v 10
debug: $(TARGET)
	gdb i "--eval-command=run test.i" --eval-command=bt


# all the commands to build any deps
$(glogLib): ./deps/glog/build/base/g.ogleinit.h
	cd deps/glog && ./configure
	cd deps/glog && make

# DO NOT DELETE

build/main.o: include/ilang/parser.h /usr/include/stdio.h
build/main.o: /usr/include/features.h /usr/include/sys/cdefs.h
build/main.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
build/main.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
build/main.o: /usr/include/bits/typesizes.h /usr/include/libio.h
build/main.o: /usr/include/_G_config.h /usr/include/wchar.h
build/main.o: /usr/include/bits/wchar.h /usr/include/xlocale.h
build/main.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
build/main.o: /usr/include/string.h include/debug.h /usr/include/assert.h
build/parserTree.o: include/ilang/parserTree.h
build/parserTree.o: /usr/include/boost/shared_ptr.hpp
build/parserTree.o: /usr/include/boost/smart_ptr/shared_ptr.hpp
build/parserTree.o: /usr/include/boost/config.hpp
build/parserTree.o: /usr/include/boost/config/select_compiler_config.hpp
build/parserTree.o: /usr/include/boost/config/compiler/gcc.hpp
build/parserTree.o: /usr/include/boost/config/select_platform_config.hpp
build/parserTree.o: /usr/include/boost/config/posix_features.hpp
build/parserTree.o: /usr/include/unistd.h /usr/include/features.h
build/parserTree.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
build/parserTree.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
build/parserTree.o: /usr/include/bits/posix_opt.h /usr/include/bits/types.h
build/parserTree.o: /usr/include/bits/typesizes.h /usr/include/bits/confname.h
build/parserTree.o: /usr/include/getopt.h /usr/include/ctype.h
build/parserTree.o: /usr/include/endian.h /usr/include/bits/endian.h
build/parserTree.o: /usr/include/bits/byteswap.h /usr/include/xlocale.h
build/parserTree.o: /usr/include/boost/config/suffix.hpp
build/parserTree.o: /usr/include/boost/config/no_tr1/memory.hpp
build/parserTree.o: /usr/include/boost/assert.hpp /usr/include/assert.h
build/parserTree.o: /usr/include/boost/current_function.hpp
build/parserTree.o: /usr/include/boost/checked_delete.hpp
build/parserTree.o: /usr/include/boost/throw_exception.hpp
build/parserTree.o: /usr/include/boost/exception/detail/attribute_noreturn.hpp
build/parserTree.o: /usr/include/boost/detail/workaround.hpp
build/parserTree.o: /usr/include/boost/config.hpp
build/parserTree.o: /usr/include/boost/smart_ptr/detail/shared_count.hpp
build/parserTree.o: /usr/include/boost/smart_ptr/bad_weak_ptr.hpp
build/parserTree.o: /usr/include/boost/smart_ptr/detail/sp_counted_base.hpp
build/parserTree.o: /usr/include/boost/smart_ptr/detail/sp_has_sync.hpp
build/parserTree.o: /usr/include/boost/smart_ptr/detail/sp_counted_base_gcc_x86.hpp
build/parserTree.o: /usr/include/boost/detail/sp_typeinfo.hpp
build/parserTree.o: /usr/include/boost/smart_ptr/detail/sp_counted_impl.hpp
build/parserTree.o: /usr/include/boost/smart_ptr/detail/sp_convertible.hpp
build/parserTree.o: /usr/include/boost/smart_ptr/detail/spinlock_pool.hpp
build/parserTree.o: /usr/include/boost/smart_ptr/detail/spinlock.hpp
build/parserTree.o: /usr/include/boost/smart_ptr/detail/spinlock_pt.hpp
build/parserTree.o: /usr/include/pthread.h /usr/include/sched.h
build/parserTree.o: /usr/include/time.h /usr/include/bits/sched.h
build/parserTree.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/setjmp.h
build/parserTree.o: /usr/include/boost/memory_order.hpp
build/parserTree.o: /usr/include/boost/smart_ptr/detail/operator_bool.hpp
build/parserTree.o: include/ilang/variable.h /usr/include/boost/any.hpp
build/parserTree.o: /usr/include/boost/type_traits/remove_reference.hpp
build/parserTree.o: /usr/include/boost/type_traits/broken_compiler_spec.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/lambda_support.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/lambda.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/ttp.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/msvc.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/gcc.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/workaround.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/ctps.hpp
build/parserTree.o: /usr/include/boost/mpl/int_fwd.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/adl_barrier.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/adl.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/intel.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/nttp_decl.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/nttp.hpp
build/parserTree.o: /usr/include/boost/preprocessor/cat.hpp
build/parserTree.o: /usr/include/boost/preprocessor/config/config.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/yes_no.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/arrays.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/na_fwd.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/preprocessor/params.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/preprocessor.hpp
build/parserTree.o: /usr/include/boost/preprocessor/comma_if.hpp
build/parserTree.o: /usr/include/boost/preprocessor/punctuation/comma_if.hpp
build/parserTree.o: /usr/include/boost/preprocessor/control/if.hpp
build/parserTree.o: /usr/include/boost/preprocessor/control/iif.hpp
build/parserTree.o: /usr/include/boost/preprocessor/logical/bool.hpp
build/parserTree.o: /usr/include/boost/preprocessor/facilities/empty.hpp
build/parserTree.o: /usr/include/boost/preprocessor/punctuation/comma.hpp
build/parserTree.o: /usr/include/boost/preprocessor/repeat.hpp
build/parserTree.o: /usr/include/boost/preprocessor/repetition/repeat.hpp
build/parserTree.o: /usr/include/boost/preprocessor/debug/error.hpp
build/parserTree.o: /usr/include/boost/preprocessor/detail/auto_rec.hpp
build/parserTree.o: /usr/include/boost/preprocessor/tuple/eat.hpp
build/parserTree.o: /usr/include/boost/preprocessor/inc.hpp
build/parserTree.o: /usr/include/boost/preprocessor/arithmetic/inc.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/preprocessor/enum.hpp
build/parserTree.o: /usr/include/boost/preprocessor/tuple/to_list.hpp
build/parserTree.o: /usr/include/boost/preprocessor/facilities/overload.hpp
build/parserTree.o: /usr/include/boost/preprocessor/variadic/size.hpp
build/parserTree.o: /usr/include/boost/preprocessor/list/for_each_i.hpp
build/parserTree.o: /usr/include/boost/preprocessor/list/adt.hpp
build/parserTree.o: /usr/include/boost/preprocessor/detail/is_binary.hpp
build/parserTree.o: /usr/include/boost/preprocessor/detail/check.hpp
build/parserTree.o: /usr/include/boost/preprocessor/logical/compl.hpp
build/parserTree.o: /usr/include/boost/preprocessor/repetition/for.hpp
build/parserTree.o: /usr/include/boost/preprocessor/repetition/detail/for.hpp
build/parserTree.o: /usr/include/boost/preprocessor/control/expr_iif.hpp
build/parserTree.o: /usr/include/boost/preprocessor/tuple/elem.hpp
build/parserTree.o: /usr/include/boost/preprocessor/tuple/rem.hpp
build/parserTree.o: /usr/include/boost/preprocessor/variadic/elem.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/type_trait_def.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/template_arity_spec.hpp
build/parserTree.o: /usr/include/boost/mpl/int.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/integral_wrapper.hpp
build/parserTree.o: /usr/include/boost/mpl/integral_c_tag.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/static_constant.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/static_cast.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/template_arity_fwd.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/overload_resolution.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/type_trait_undef.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_reference.hpp
build/parserTree.o: /usr/include/boost/type_traits/config.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_lvalue_reference.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/bool_trait_def.hpp
build/parserTree.o: /usr/include/boost/type_traits/integral_constant.hpp
build/parserTree.o: /usr/include/boost/mpl/bool.hpp
build/parserTree.o: /usr/include/boost/mpl/bool_fwd.hpp
build/parserTree.o: /usr/include/boost/mpl/integral_c.hpp
build/parserTree.o: /usr/include/boost/mpl/integral_c_fwd.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/bool_trait_undef.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_rvalue_reference.hpp
build/parserTree.o: /usr/include/boost/type_traits/ice.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/yes_no_type.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/ice_or.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/ice_and.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/ice_not.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/ice_eq.hpp
build/parserTree.o: /usr/include/boost/static_assert.hpp include/ilang/parser.h
build/parserTree.o: /usr/include/stdio.h /usr/include/libio.h
build/parserTree.o: /usr/include/_G_config.h /usr/include/wchar.h
build/parserTree.o: /usr/include/bits/wchar.h /usr/include/bits/stdio_lim.h
build/parserTree.o: /usr/include/bits/sys_errlist.h include/ilang/scope.h
build/parserTree.o: include/debug.h
build/import.o: include/ilang/import.h
build/parser.o: include/ilang/parser.h /usr/include/stdio.h
build/parser.o: /usr/include/features.h /usr/include/sys/cdefs.h
build/parser.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
build/parser.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
build/parser.o: /usr/include/bits/typesizes.h /usr/include/libio.h
build/parser.o: /usr/include/_G_config.h /usr/include/wchar.h
build/parser.o: /usr/include/bits/wchar.h /usr/include/xlocale.h
build/parser.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
build/variable.o: include/ilang/variable.h /usr/include/boost/any.hpp
build/variable.o: /usr/include/boost/config.hpp
build/variable.o: /usr/include/boost/type_traits/remove_reference.hpp
build/variable.o: /usr/include/boost/type_traits/broken_compiler_spec.hpp
build/variable.o: /usr/include/boost/mpl/aux_/lambda_support.hpp
build/variable.o: /usr/include/boost/mpl/aux_/config/lambda.hpp
build/variable.o: /usr/include/boost/mpl/aux_/config/ttp.hpp
build/variable.o: /usr/include/boost/mpl/aux_/config/msvc.hpp
build/variable.o: /usr/include/boost/mpl/aux_/config/gcc.hpp
build/variable.o: /usr/include/boost/mpl/aux_/config/workaround.hpp
build/variable.o: /usr/include/boost/detail/workaround.hpp
build/variable.o: /usr/include/boost/mpl/aux_/config/ctps.hpp
build/variable.o: /usr/include/boost/mpl/int_fwd.hpp
build/variable.o: /usr/include/boost/mpl/aux_/adl_barrier.hpp
build/variable.o: /usr/include/boost/mpl/aux_/config/adl.hpp
build/variable.o: /usr/include/boost/mpl/aux_/config/intel.hpp
build/variable.o: /usr/include/boost/mpl/aux_/nttp_decl.hpp
build/variable.o: /usr/include/boost/mpl/aux_/config/nttp.hpp
build/variable.o: /usr/include/boost/preprocessor/cat.hpp
build/variable.o: /usr/include/boost/preprocessor/config/config.hpp
build/variable.o: /usr/include/boost/mpl/aux_/yes_no.hpp
build/variable.o: /usr/include/boost/mpl/aux_/config/arrays.hpp
build/variable.o: /usr/include/boost/mpl/aux_/na_fwd.hpp
build/variable.o: /usr/include/boost/mpl/aux_/preprocessor/params.hpp
build/variable.o: /usr/include/boost/mpl/aux_/config/preprocessor.hpp
build/variable.o: /usr/include/boost/preprocessor/comma_if.hpp
build/variable.o: /usr/include/boost/preprocessor/punctuation/comma_if.hpp
build/variable.o: /usr/include/boost/preprocessor/control/if.hpp
build/variable.o: /usr/include/boost/preprocessor/control/iif.hpp
build/variable.o: /usr/include/boost/preprocessor/logical/bool.hpp
build/variable.o: /usr/include/boost/preprocessor/facilities/empty.hpp
build/variable.o: /usr/include/boost/preprocessor/punctuation/comma.hpp
build/variable.o: /usr/include/boost/preprocessor/repeat.hpp
build/variable.o: /usr/include/boost/preprocessor/repetition/repeat.hpp
build/variable.o: /usr/include/boost/preprocessor/debug/error.hpp
build/variable.o: /usr/include/boost/preprocessor/detail/auto_rec.hpp
build/variable.o: /usr/include/boost/preprocessor/tuple/eat.hpp
build/variable.o: /usr/include/boost/preprocessor/inc.hpp
build/variable.o: /usr/include/boost/preprocessor/arithmetic/inc.hpp
build/variable.o: /usr/include/boost/mpl/aux_/preprocessor/enum.hpp
build/variable.o: /usr/include/boost/preprocessor/tuple/to_list.hpp
build/variable.o: /usr/include/boost/preprocessor/facilities/overload.hpp
build/variable.o: /usr/include/boost/preprocessor/variadic/size.hpp
build/variable.o: /usr/include/boost/preprocessor/list/for_each_i.hpp
build/variable.o: /usr/include/boost/preprocessor/list/adt.hpp
build/variable.o: /usr/include/boost/preprocessor/detail/is_binary.hpp
build/variable.o: /usr/include/boost/preprocessor/detail/check.hpp
build/variable.o: /usr/include/boost/preprocessor/logical/compl.hpp
build/variable.o: /usr/include/boost/preprocessor/repetition/for.hpp
build/variable.o: /usr/include/boost/preprocessor/repetition/detail/for.hpp
build/variable.o: /usr/include/boost/preprocessor/control/expr_iif.hpp
build/variable.o: /usr/include/boost/preprocessor/tuple/elem.hpp
build/variable.o: /usr/include/boost/preprocessor/tuple/rem.hpp
build/variable.o: /usr/include/boost/preprocessor/variadic/elem.hpp
build/variable.o: /usr/include/boost/type_traits/detail/type_trait_def.hpp
build/variable.o: /usr/include/boost/type_traits/detail/template_arity_spec.hpp
build/variable.o: /usr/include/boost/mpl/int.hpp
build/variable.o: /usr/include/boost/mpl/aux_/integral_wrapper.hpp
build/variable.o: /usr/include/boost/mpl/integral_c_tag.hpp
build/variable.o: /usr/include/boost/mpl/aux_/config/static_constant.hpp
build/variable.o: /usr/include/boost/mpl/aux_/static_cast.hpp
build/variable.o: /usr/include/boost/mpl/aux_/template_arity_fwd.hpp
build/variable.o: /usr/include/boost/mpl/aux_/config/overload_resolution.hpp
build/variable.o: /usr/include/boost/type_traits/detail/type_trait_undef.hpp
build/variable.o: /usr/include/boost/type_traits/is_reference.hpp
build/variable.o: /usr/include/boost/type_traits/config.hpp
build/variable.o: /usr/include/boost/type_traits/is_lvalue_reference.hpp
build/variable.o: /usr/include/boost/type_traits/detail/bool_trait_def.hpp
build/variable.o: /usr/include/boost/type_traits/integral_constant.hpp
build/variable.o: /usr/include/boost/mpl/bool.hpp
build/variable.o: /usr/include/boost/mpl/bool_fwd.hpp
build/variable.o: /usr/include/boost/mpl/integral_c.hpp
build/variable.o: /usr/include/boost/mpl/integral_c_fwd.hpp
build/variable.o: /usr/include/boost/type_traits/detail/bool_trait_undef.hpp
build/variable.o: /usr/include/boost/type_traits/is_rvalue_reference.hpp
build/variable.o: /usr/include/boost/type_traits/ice.hpp
build/variable.o: /usr/include/boost/type_traits/detail/yes_no_type.hpp
build/variable.o: /usr/include/boost/type_traits/detail/ice_or.hpp
build/variable.o: /usr/include/boost/type_traits/detail/ice_and.hpp
build/variable.o: /usr/include/boost/type_traits/detail/ice_not.hpp
build/variable.o: /usr/include/boost/type_traits/detail/ice_eq.hpp
build/variable.o: /usr/include/boost/throw_exception.hpp
build/variable.o: /usr/include/boost/exception/detail/attribute_noreturn.hpp
build/variable.o: /usr/include/boost/static_assert.hpp
build/variable.o: /usr/include/boost/shared_ptr.hpp
build/variable.o: /usr/include/boost/smart_ptr/shared_ptr.hpp
build/variable.o: /usr/include/boost/config.hpp
build/variable.o: /usr/include/boost/config/select_compiler_config.hpp
build/variable.o: /usr/include/boost/config/compiler/gcc.hpp
build/variable.o: /usr/include/boost/config/select_platform_config.hpp
build/variable.o: /usr/include/boost/config/posix_features.hpp
build/variable.o: /usr/include/unistd.h /usr/include/features.h
build/variable.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
build/variable.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
build/variable.o: /usr/include/bits/posix_opt.h /usr/include/bits/types.h
build/variable.o: /usr/include/bits/typesizes.h /usr/include/bits/confname.h
build/variable.o: /usr/include/getopt.h /usr/include/ctype.h
build/variable.o: /usr/include/endian.h /usr/include/bits/endian.h
build/variable.o: /usr/include/bits/byteswap.h /usr/include/xlocale.h
build/variable.o: /usr/include/boost/config/suffix.hpp
build/variable.o: /usr/include/boost/config/no_tr1/memory.hpp
build/variable.o: /usr/include/boost/assert.hpp /usr/include/assert.h
build/variable.o: /usr/include/boost/current_function.hpp
build/variable.o: /usr/include/boost/checked_delete.hpp
build/variable.o: /usr/include/boost/smart_ptr/detail/shared_count.hpp
build/variable.o: /usr/include/boost/smart_ptr/bad_weak_ptr.hpp
build/variable.o: /usr/include/boost/smart_ptr/detail/sp_counted_base.hpp
build/variable.o: /usr/include/boost/smart_ptr/detail/sp_has_sync.hpp
build/variable.o: /usr/include/boost/smart_ptr/detail/sp_counted_base_gcc_x86.hpp
build/variable.o: /usr/include/boost/detail/sp_typeinfo.hpp
build/variable.o: /usr/include/boost/smart_ptr/detail/sp_counted_impl.hpp
build/variable.o: /usr/include/boost/smart_ptr/detail/sp_convertible.hpp
build/variable.o: /usr/include/boost/smart_ptr/detail/spinlock_pool.hpp
build/variable.o: /usr/include/boost/smart_ptr/detail/spinlock.hpp
build/variable.o: /usr/include/boost/smart_ptr/detail/spinlock_pt.hpp
build/variable.o: /usr/include/pthread.h /usr/include/sched.h
build/variable.o: /usr/include/time.h /usr/include/bits/sched.h
build/variable.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/setjmp.h
build/variable.o: /usr/include/boost/memory_order.hpp
build/variable.o: /usr/include/boost/smart_ptr/detail/operator_bool.hpp
build/variable.o: include/debug.h
build/scope.o: include/ilang/scope.h include/ilang/variable.h
build/scope.o: /usr/include/boost/any.hpp /usr/include/boost/config.hpp
build/scope.o: /usr/include/boost/type_traits/remove_reference.hpp
build/scope.o: /usr/include/boost/type_traits/broken_compiler_spec.hpp
build/scope.o: /usr/include/boost/mpl/aux_/lambda_support.hpp
build/scope.o: /usr/include/boost/mpl/aux_/config/lambda.hpp
build/scope.o: /usr/include/boost/mpl/aux_/config/ttp.hpp
build/scope.o: /usr/include/boost/mpl/aux_/config/msvc.hpp
build/scope.o: /usr/include/boost/mpl/aux_/config/gcc.hpp
build/scope.o: /usr/include/boost/mpl/aux_/config/workaround.hpp
build/scope.o: /usr/include/boost/detail/workaround.hpp
build/scope.o: /usr/include/boost/mpl/aux_/config/ctps.hpp
build/scope.o: /usr/include/boost/mpl/int_fwd.hpp
build/scope.o: /usr/include/boost/mpl/aux_/adl_barrier.hpp
build/scope.o: /usr/include/boost/mpl/aux_/config/adl.hpp
build/scope.o: /usr/include/boost/mpl/aux_/config/intel.hpp
build/scope.o: /usr/include/boost/mpl/aux_/nttp_decl.hpp
build/scope.o: /usr/include/boost/mpl/aux_/config/nttp.hpp
build/scope.o: /usr/include/boost/preprocessor/cat.hpp
build/scope.o: /usr/include/boost/preprocessor/config/config.hpp
build/scope.o: /usr/include/boost/mpl/aux_/yes_no.hpp
build/scope.o: /usr/include/boost/mpl/aux_/config/arrays.hpp
build/scope.o: /usr/include/boost/mpl/aux_/na_fwd.hpp
build/scope.o: /usr/include/boost/mpl/aux_/preprocessor/params.hpp
build/scope.o: /usr/include/boost/mpl/aux_/config/preprocessor.hpp
build/scope.o: /usr/include/boost/preprocessor/comma_if.hpp
build/scope.o: /usr/include/boost/preprocessor/punctuation/comma_if.hpp
build/scope.o: /usr/include/boost/preprocessor/control/if.hpp
build/scope.o: /usr/include/boost/preprocessor/control/iif.hpp
build/scope.o: /usr/include/boost/preprocessor/logical/bool.hpp
build/scope.o: /usr/include/boost/preprocessor/facilities/empty.hpp
build/scope.o: /usr/include/boost/preprocessor/punctuation/comma.hpp
build/scope.o: /usr/include/boost/preprocessor/repeat.hpp
build/scope.o: /usr/include/boost/preprocessor/repetition/repeat.hpp
build/scope.o: /usr/include/boost/preprocessor/debug/error.hpp
build/scope.o: /usr/include/boost/preprocessor/detail/auto_rec.hpp
build/scope.o: /usr/include/boost/preprocessor/tuple/eat.hpp
build/scope.o: /usr/include/boost/preprocessor/inc.hpp
build/scope.o: /usr/include/boost/preprocessor/arithmetic/inc.hpp
build/scope.o: /usr/include/boost/mpl/aux_/preprocessor/enum.hpp
build/scope.o: /usr/include/boost/preprocessor/tuple/to_list.hpp
build/scope.o: /usr/include/boost/preprocessor/facilities/overload.hpp
build/scope.o: /usr/include/boost/preprocessor/variadic/size.hpp
build/scope.o: /usr/include/boost/preprocessor/list/for_each_i.hpp
build/scope.o: /usr/include/boost/preprocessor/list/adt.hpp
build/scope.o: /usr/include/boost/preprocessor/detail/is_binary.hpp
build/scope.o: /usr/include/boost/preprocessor/detail/check.hpp
build/scope.o: /usr/include/boost/preprocessor/logical/compl.hpp
build/scope.o: /usr/include/boost/preprocessor/repetition/for.hpp
build/scope.o: /usr/include/boost/preprocessor/repetition/detail/for.hpp
build/scope.o: /usr/include/boost/preprocessor/control/expr_iif.hpp
build/scope.o: /usr/include/boost/preprocessor/tuple/elem.hpp
build/scope.o: /usr/include/boost/preprocessor/tuple/rem.hpp
build/scope.o: /usr/include/boost/preprocessor/variadic/elem.hpp
build/scope.o: /usr/include/boost/type_traits/detail/type_trait_def.hpp
build/scope.o: /usr/include/boost/type_traits/detail/template_arity_spec.hpp
build/scope.o: /usr/include/boost/mpl/int.hpp
build/scope.o: /usr/include/boost/mpl/aux_/integral_wrapper.hpp
build/scope.o: /usr/include/boost/mpl/integral_c_tag.hpp
build/scope.o: /usr/include/boost/mpl/aux_/config/static_constant.hpp
build/scope.o: /usr/include/boost/mpl/aux_/static_cast.hpp
build/scope.o: /usr/include/boost/mpl/aux_/template_arity_fwd.hpp
build/scope.o: /usr/include/boost/mpl/aux_/config/overload_resolution.hpp
build/scope.o: /usr/include/boost/type_traits/detail/type_trait_undef.hpp
build/scope.o: /usr/include/boost/type_traits/is_reference.hpp
build/scope.o: /usr/include/boost/type_traits/config.hpp
build/scope.o: /usr/include/boost/type_traits/is_lvalue_reference.hpp
build/scope.o: /usr/include/boost/type_traits/detail/bool_trait_def.hpp
build/scope.o: /usr/include/boost/type_traits/integral_constant.hpp
build/scope.o: /usr/include/boost/mpl/bool.hpp
build/scope.o: /usr/include/boost/mpl/bool_fwd.hpp
build/scope.o: /usr/include/boost/mpl/integral_c.hpp
build/scope.o: /usr/include/boost/mpl/integral_c_fwd.hpp
build/scope.o: /usr/include/boost/type_traits/detail/bool_trait_undef.hpp
build/scope.o: /usr/include/boost/type_traits/is_rvalue_reference.hpp
build/scope.o: /usr/include/boost/type_traits/ice.hpp
build/scope.o: /usr/include/boost/type_traits/detail/yes_no_type.hpp
build/scope.o: /usr/include/boost/type_traits/detail/ice_or.hpp
build/scope.o: /usr/include/boost/type_traits/detail/ice_and.hpp
build/scope.o: /usr/include/boost/type_traits/detail/ice_not.hpp
build/scope.o: /usr/include/boost/type_traits/detail/ice_eq.hpp
build/scope.o: /usr/include/boost/throw_exception.hpp
build/scope.o: /usr/include/boost/exception/detail/attribute_noreturn.hpp
build/scope.o: /usr/include/boost/static_assert.hpp
build/scope.o: /usr/include/boost/shared_ptr.hpp
build/scope.o: /usr/include/boost/smart_ptr/shared_ptr.hpp
build/scope.o: /usr/include/boost/config.hpp
build/scope.o: /usr/include/boost/config/select_compiler_config.hpp
build/scope.o: /usr/include/boost/config/compiler/gcc.hpp
build/scope.o: /usr/include/boost/config/select_platform_config.hpp
build/scope.o: /usr/include/boost/config/posix_features.hpp
build/scope.o: /usr/include/unistd.h /usr/include/features.h
build/scope.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
build/scope.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
build/scope.o: /usr/include/bits/posix_opt.h /usr/include/bits/types.h
build/scope.o: /usr/include/bits/typesizes.h /usr/include/bits/confname.h
build/scope.o: /usr/include/getopt.h /usr/include/ctype.h /usr/include/endian.h
build/scope.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
build/scope.o: /usr/include/xlocale.h /usr/include/boost/config/suffix.hpp
build/scope.o: /usr/include/boost/config/no_tr1/memory.hpp
build/scope.o: /usr/include/boost/assert.hpp /usr/include/assert.h
build/scope.o: /usr/include/boost/current_function.hpp
build/scope.o: /usr/include/boost/checked_delete.hpp
build/scope.o: /usr/include/boost/smart_ptr/detail/shared_count.hpp
build/scope.o: /usr/include/boost/smart_ptr/bad_weak_ptr.hpp
build/scope.o: /usr/include/boost/smart_ptr/detail/sp_counted_base.hpp
build/scope.o: /usr/include/boost/smart_ptr/detail/sp_has_sync.hpp
build/scope.o: /usr/include/boost/smart_ptr/detail/sp_counted_base_gcc_x86.hpp
build/scope.o: /usr/include/boost/detail/sp_typeinfo.hpp
build/scope.o: /usr/include/boost/smart_ptr/detail/sp_counted_impl.hpp
build/scope.o: /usr/include/boost/smart_ptr/detail/sp_convertible.hpp
build/scope.o: /usr/include/boost/smart_ptr/detail/spinlock_pool.hpp
build/scope.o: /usr/include/boost/smart_ptr/detail/spinlock.hpp
build/scope.o: /usr/include/boost/smart_ptr/detail/spinlock_pt.hpp
build/scope.o: /usr/include/pthread.h /usr/include/sched.h /usr/include/time.h
build/scope.o: /usr/include/bits/sched.h /usr/include/bits/pthreadtypes.h
build/scope.o: /usr/include/bits/setjmp.h /usr/include/boost/memory_order.hpp
build/scope.o: /usr/include/boost/smart_ptr/detail/operator_bool.hpp
build/scope.o: include/debug.h
