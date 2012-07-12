TARGET= i

SRCS= main.cc parserTree.cc import.cc parser.cc variable.cc scope.cc object.cc
LIBS=-lfl -lpthread

BUILDDIR=build
OBJS= $(BUILDDIR)/lex.yy.o $(BUILDDIR)/parser.tab.o $(addprefix $(BUILDDIR)/, $(patsubst %.cc, %.o, $(filter %.cc,$(SRCS))) $(patsubst %.c, %.o, $(filter %.c, $(SRCS))))

SRCDIR=src
SRCSD=$(addprefix $(SRCDIR)/, $(SRCS))

INCLUDEDIR=include

# turn off all warnings so I can more easily view the errors, these need to be turn back on latter
CXXFLAGS_BASE=-Wall -std=c++11 -w -I$(INCLUDEDIR)/ -I$(INCLUDEDIR)/ilang -I$(BUILDDIR)/
CXXFLAGS= -ggdb -O0 $(CXXFLAGS_BASE)
LDFLAGS=
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


release: CXXFLAGS= -O3 -s $(CXXFLAGS_BASE)
release: LDFLAGS= -s -static
release: clean $(TARGET)

submodule:
	git submodule update --init --recursive 

$(BUILDDIR)/%.o: $(SRCDIR)/%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILDDIR)/parser.tab.cc $(BUILDDIR)/parser.tab.hh: $(SRCDIR)/parser.y
	bison -v -t -o $(BUILDDIR)/parser.tab.cc $<

$(BUILDDIR)/lex.yy.cc: $(SRCDIR)/scanner.l $(BUILDDIR)/parser.tab.cc
	flex -o $@ $<

$(BUILDDIR)/lex.yy.o: $(BUILDDIR)/lex.yy.cc $(BUILDDIR)/parser.tab.hh
$(BUILDDIR)/parser.tab.o: $(BUILDDIR)/parser.tab.cc

$(TARGET): $(DEPS) $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

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
	gdb i "--eval-command=run test.i -v 10" --eval-command=bt


# all the commands to build any deps
$(glogLib): ./deps/glog/build/base/g.ogleinit.h
	cd deps/glog && ./configure
	cd deps/glog && make

# DO NOT DELETE

build/main.o: include/ilang/parser.h /usr/include/stdio.h
build/main.o: /usr/include/features.h /usr/include/stdc-predef.h
build/main.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
build/main.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
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
build/parserTree.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
build/parserTree.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
build/parserTree.o: /usr/include/bits/posix_opt.h
build/parserTree.o: /usr/include/bits/environments.h /usr/include/bits/types.h
build/parserTree.o: /usr/include/bits/typesizes.h /usr/include/bits/confname.h
build/parserTree.o: /usr/include/getopt.h /usr/include/boost/config/suffix.hpp
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
build/parserTree.o: /usr/include/pthread.h /usr/include/endian.h
build/parserTree.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
build/parserTree.o: /usr/include/bits/byteswap-16.h /usr/include/sched.h
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
build/parserTree.o: /usr/include/bits/wchar.h /usr/include/xlocale.h
build/parserTree.o: /usr/include/bits/stdio_lim.h
build/parserTree.o: /usr/include/bits/sys_errlist.h include/ilang/scope.h
build/parserTree.o: /usr/include/boost/utility.hpp
build/parserTree.o: /usr/include/boost/utility/addressof.hpp
build/parserTree.o: /usr/include/boost/utility/base_from_member.hpp
build/parserTree.o: /usr/include/boost/preprocessor/repetition/enum_binary_params.hpp
build/parserTree.o: /usr/include/boost/preprocessor/repetition/enum_params.hpp
build/parserTree.o: /usr/include/boost/preprocessor/repetition/repeat_from_to.hpp
build/parserTree.o: /usr/include/boost/preprocessor/arithmetic/add.hpp
build/parserTree.o: /usr/include/boost/preprocessor/arithmetic/dec.hpp
build/parserTree.o: /usr/include/boost/preprocessor/control/while.hpp
build/parserTree.o: /usr/include/boost/preprocessor/list/fold_left.hpp
build/parserTree.o: /usr/include/boost/preprocessor/list/detail/fold_left.hpp
build/parserTree.o: /usr/include/boost/preprocessor/list/fold_right.hpp
build/parserTree.o: /usr/include/boost/preprocessor/list/detail/fold_right.hpp
build/parserTree.o: /usr/include/boost/preprocessor/list/reverse.hpp
build/parserTree.o: /usr/include/boost/preprocessor/logical/bitand.hpp
build/parserTree.o: /usr/include/boost/preprocessor/control/detail/while.hpp
build/parserTree.o: /usr/include/boost/preprocessor/arithmetic/sub.hpp
build/parserTree.o: /usr/include/boost/utility/binary.hpp
build/parserTree.o: /usr/include/boost/preprocessor/control/deduce_d.hpp
build/parserTree.o: /usr/include/boost/preprocessor/facilities/identity.hpp
build/parserTree.o: /usr/include/boost/preprocessor/seq/cat.hpp
build/parserTree.o: /usr/include/boost/preprocessor/seq/fold_left.hpp
build/parserTree.o: /usr/include/boost/preprocessor/seq/seq.hpp
build/parserTree.o: /usr/include/boost/preprocessor/seq/elem.hpp
build/parserTree.o: /usr/include/boost/preprocessor/seq/size.hpp
build/parserTree.o: /usr/include/boost/preprocessor/seq/transform.hpp
build/parserTree.o: /usr/include/boost/preprocessor/arithmetic/mod.hpp
build/parserTree.o: /usr/include/boost/preprocessor/arithmetic/detail/div_base.hpp
build/parserTree.o: /usr/include/boost/preprocessor/comparison/less_equal.hpp
build/parserTree.o: /usr/include/boost/preprocessor/logical/not.hpp
build/parserTree.o: /usr/include/boost/utility/enable_if.hpp
build/parserTree.o: /usr/include/boost/next_prior.hpp
build/parserTree.o: /usr/include/boost/noncopyable.hpp include/ilang/object.h
build/parserTree.o: include/ilang/function.h /usr/include/boost/bind.hpp
build/parserTree.o: /usr/include/boost/bind/bind.hpp /usr/include/boost/ref.hpp
build/parserTree.o: /usr/include/boost/mem_fn.hpp
build/parserTree.o: /usr/include/boost/bind/mem_fn.hpp
build/parserTree.o: /usr/include/boost/get_pointer.hpp
build/parserTree.o: /usr/include/boost/bind/mem_fn_template.hpp
build/parserTree.o: /usr/include/boost/bind/mem_fn_cc.hpp
build/parserTree.o: /usr/include/boost/type.hpp
build/parserTree.o: /usr/include/boost/is_placeholder.hpp
build/parserTree.o: /usr/include/boost/bind/arg.hpp
build/parserTree.o: /usr/include/boost/visit_each.hpp
build/parserTree.o: /usr/include/boost/bind/storage.hpp
build/parserTree.o: /usr/include/boost/bind/bind_template.hpp
build/parserTree.o: /usr/include/boost/bind/bind_cc.hpp
build/parserTree.o: /usr/include/boost/bind/bind_mf_cc.hpp
build/parserTree.o: /usr/include/boost/bind/bind_mf2_cc.hpp
build/parserTree.o: /usr/include/boost/bind/placeholders.hpp
build/parserTree.o: /usr/include/boost/function.hpp
build/parserTree.o: /usr/include/boost/preprocessor/iterate.hpp
build/parserTree.o: /usr/include/boost/preprocessor/iteration/iterate.hpp
build/parserTree.o: /usr/include/boost/preprocessor/array/elem.hpp
build/parserTree.o: /usr/include/boost/preprocessor/array/data.hpp
build/parserTree.o: /usr/include/boost/preprocessor/array/size.hpp
build/parserTree.o: /usr/include/boost/preprocessor/slot/slot.hpp
build/parserTree.o: /usr/include/boost/preprocessor/slot/detail/def.hpp
build/parserTree.o: /usr/include/boost/function/detail/prologue.hpp
build/parserTree.o: /usr/include/boost/config/no_tr1/functional.hpp
build/parserTree.o: /usr/include/boost/function/function_base.hpp
build/parserTree.o: /usr/include/boost/integer.hpp
build/parserTree.o: /usr/include/boost/integer_fwd.hpp
build/parserTree.o: /usr/include/boost/limits.hpp
build/parserTree.o: /usr/include/boost/cstdint.hpp /usr/include/stdint.h
build/parserTree.o: /usr/include/boost/integer_traits.hpp /usr/include/limits.h
build/parserTree.o: /usr/include/bits/posix1_lim.h
build/parserTree.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
build/parserTree.o: /usr/include/bits/posix2_lim.h
build/parserTree.o: /usr/include/boost/type_traits/has_trivial_copy.hpp
build/parserTree.o: /usr/include/boost/type_traits/intrinsics.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_volatile.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/cv_traits_impl.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_pod.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_void.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_scalar.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_arithmetic.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_integral.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_float.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_enum.hpp
build/parserTree.o: /usr/include/boost/type_traits/add_reference.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_convertible.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_array.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_abstract.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_class.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_union.hpp
build/parserTree.o: /usr/include/boost/type_traits/remove_cv.hpp
build/parserTree.o: /usr/include/boost/type_traits/add_rvalue_reference.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_function.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/false_result.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/is_function_ptr_helper.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_pointer.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_member_pointer.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_member_function_pointer.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/is_mem_fun_pointer_impl.hpp
build/parserTree.o: /usr/include/boost/type_traits/has_trivial_destructor.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_const.hpp
build/parserTree.o: /usr/include/boost/type_traits/composite_traits.hpp
build/parserTree.o: /usr/include/boost/mpl/if.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/value_wknd.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/integral.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/eti.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/na_spec.hpp
build/parserTree.o: /usr/include/boost/mpl/lambda_fwd.hpp
build/parserTree.o: /usr/include/boost/mpl/void_fwd.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/na.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/arity.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/dtp.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/preprocessor/def_params_tail.hpp
build/parserTree.o: /usr/include/boost/mpl/limits/arity.hpp
build/parserTree.o: /usr/include/boost/preprocessor/logical/and.hpp
build/parserTree.o: /usr/include/boost/preprocessor/identity.hpp
build/parserTree.o: /usr/include/boost/preprocessor/empty.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/lambda_arity_param.hpp
build/parserTree.o: /usr/include/boost/type_traits/alignment_of.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/size_t_trait_def.hpp
build/parserTree.o: /usr/include/boost/mpl/size_t.hpp
build/parserTree.o: /usr/include/boost/mpl/size_t_fwd.hpp
build/parserTree.o: /usr/include/boost/type_traits/detail/size_t_trait_undef.hpp
build/parserTree.o: /usr/include/boost/function_equal.hpp
build/parserTree.o: /usr/include/boost/function/function_fwd.hpp
build/parserTree.o: /usr/include/boost/preprocessor/enum.hpp
build/parserTree.o: /usr/include/boost/preprocessor/repetition/enum.hpp
build/parserTree.o: /usr/include/boost/preprocessor/enum_params.hpp
build/parserTree.o: include/debug.h /usr/include/boost/algorithm/string.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/std_containers_traits.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/std/string_traits.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/yes_no_type.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/sequence_traits.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/std/list_traits.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/trim.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/config.hpp
build/parserTree.o: /usr/include/boost/range/begin.hpp
build/parserTree.o: /usr/include/boost/range/config.hpp
build/parserTree.o: /usr/include/boost/range/iterator.hpp
build/parserTree.o: /usr/include/boost/range/mutable_iterator.hpp
build/parserTree.o: /usr/include/boost/range/detail/extract_optional_type.hpp
build/parserTree.o: /usr/include/boost/iterator/iterator_traits.hpp
build/parserTree.o: /usr/include/boost/detail/iterator.hpp
build/parserTree.o: /usr/include/boost/range/const_iterator.hpp
build/parserTree.o: /usr/include/boost/type_traits/remove_const.hpp
build/parserTree.o: /usr/include/boost/mpl/eval_if.hpp
build/parserTree.o: /usr/include/boost/range/end.hpp
build/parserTree.o: /usr/include/boost/range/detail/implementation_help.hpp
build/parserTree.o: /usr/include/boost/range/detail/common.hpp
build/parserTree.o: /usr/include/boost/range/detail/sfinae.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_same.hpp
build/parserTree.o: /usr/include/string.h
build/parserTree.o: /usr/include/boost/range/as_literal.hpp
build/parserTree.o: /usr/include/boost/range/iterator_range.hpp
build/parserTree.o: /usr/include/boost/range/iterator_range_core.hpp
build/parserTree.o: /usr/include/boost/iterator/iterator_facade.hpp
build/parserTree.o: /usr/include/boost/iterator.hpp
build/parserTree.o: /usr/include/boost/iterator/interoperable.hpp
build/parserTree.o: /usr/include/boost/mpl/or.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/use_preprocessed.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/nested_type_wknd.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/include_preprocessed.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/compiler.hpp
build/parserTree.o: /usr/include/boost/preprocessor/stringize.hpp
build/parserTree.o: /usr/include/boost/iterator/detail/config_def.hpp
build/parserTree.o: /usr/include/boost/iterator/detail/config_undef.hpp
build/parserTree.o: /usr/include/boost/iterator/detail/facade_iterator_category.hpp
build/parserTree.o: /usr/include/boost/iterator/iterator_categories.hpp
build/parserTree.o: /usr/include/boost/mpl/identity.hpp
build/parserTree.o: /usr/include/boost/mpl/placeholders.hpp
build/parserTree.o: /usr/include/boost/mpl/arg.hpp
build/parserTree.o: /usr/include/boost/mpl/arg_fwd.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/na_assert.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/arity_spec.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/arg_typedef.hpp
build/parserTree.o: /usr/include/boost/mpl/and.hpp
build/parserTree.o: /usr/include/boost/mpl/assert.hpp
build/parserTree.o: /usr/include/boost/mpl/not.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/pp_counter.hpp
build/parserTree.o: /usr/include/boost/detail/indirect_traits.hpp
build/parserTree.o: /usr/include/boost/type_traits/remove_pointer.hpp
build/parserTree.o: /usr/include/boost/iterator/detail/enable_if.hpp
build/parserTree.o: /usr/include/boost/implicit_cast.hpp
build/parserTree.o: /usr/include/boost/type_traits/add_const.hpp
build/parserTree.o: /usr/include/boost/type_traits/add_pointer.hpp
build/parserTree.o: /usr/include/boost/mpl/always.hpp
build/parserTree.o: /usr/include/boost/mpl/apply.hpp
build/parserTree.o: /usr/include/boost/mpl/apply_fwd.hpp
build/parserTree.o: /usr/include/boost/mpl/apply_wrap.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/has_apply.hpp
build/parserTree.o: /usr/include/boost/mpl/has_xxx.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/type_wrapper.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/has_xxx.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/msvc_typename.hpp
build/parserTree.o: /usr/include/boost/preprocessor/repetition/enum_trailing_params.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/has_apply.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/msvc_never_true.hpp
build/parserTree.o: /usr/include/boost/mpl/lambda.hpp
build/parserTree.o: /usr/include/boost/mpl/bind.hpp
build/parserTree.o: /usr/include/boost/mpl/bind_fwd.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/config/bind.hpp
build/parserTree.o: /usr/include/boost/mpl/next.hpp
build/parserTree.o: /usr/include/boost/mpl/next_prior.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/common_name_wknd.hpp
build/parserTree.o: /usr/include/boost/mpl/protect.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/lambda_no_ctps.hpp
build/parserTree.o: /usr/include/boost/mpl/is_placeholder.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/template_arity.hpp
build/parserTree.o: /usr/include/boost/mpl/aux_/has_rebind.hpp
build/parserTree.o: /usr/include/boost/range/functions.hpp
build/parserTree.o: /usr/include/boost/range/size.hpp
build/parserTree.o: /usr/include/boost/range/difference_type.hpp
build/parserTree.o: /usr/include/boost/range/distance.hpp
build/parserTree.o: /usr/include/boost/range/empty.hpp
build/parserTree.o: /usr/include/boost/range/rbegin.hpp
build/parserTree.o: /usr/include/boost/range/reverse_iterator.hpp
build/parserTree.o: /usr/include/boost/iterator/reverse_iterator.hpp
build/parserTree.o: /usr/include/boost/iterator/iterator_adaptor.hpp
build/parserTree.o: /usr/include/boost/range/rend.hpp
build/parserTree.o: /usr/include/boost/range/algorithm/equal.hpp
build/parserTree.o: /usr/include/boost/range/concepts.hpp
build/parserTree.o: /usr/include/boost/concept_check.hpp
build/parserTree.o: /usr/include/boost/concept/assert.hpp
build/parserTree.o: /usr/include/boost/concept/detail/borland.hpp
build/parserTree.o: /usr/include/boost/concept/detail/backward_compatibility.hpp
build/parserTree.o: /usr/include/boost/type_traits/conversion_traits.hpp
build/parserTree.o: /usr/include/boost/concept/usage.hpp
build/parserTree.o: /usr/include/boost/concept/detail/concept_def.hpp
build/parserTree.o: /usr/include/boost/preprocessor/seq/for_each_i.hpp
build/parserTree.o: /usr/include/boost/preprocessor/seq/enum.hpp
build/parserTree.o: /usr/include/boost/concept/detail/concept_undef.hpp
build/parserTree.o: /usr/include/boost/iterator/iterator_concepts.hpp
build/parserTree.o: /usr/include/boost/range/value_type.hpp
build/parserTree.o: /usr/include/boost/range/detail/misc_concept.hpp
build/parserTree.o: /usr/include/boost/range/detail/safe_bool.hpp
build/parserTree.o: /usr/include/boost/range/iterator_range_io.hpp
build/parserTree.o: /usr/include/boost/range/detail/str_types.hpp
build/parserTree.o: /usr/include/boost/range/size_type.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/detail/trim.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/classification.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/detail/classification.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/predicate_facade.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/case_conv.hpp
build/parserTree.o: /usr/include/boost/iterator/transform_iterator.hpp
build/parserTree.o: /usr/include/boost/type_traits/function_traits.hpp
build/parserTree.o: /usr/include/boost/utility/result_of.hpp
build/parserTree.o: /usr/include/boost/preprocessor/repetition/enum_shifted_params.hpp
build/parserTree.o: /usr/include/boost/preprocessor/facilities/intercept.hpp
build/parserTree.o: /usr/include/boost/utility/declval.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_base_and_derived.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/detail/case_conv.hpp
build/parserTree.o: /usr/include/boost/type_traits/make_unsigned.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_signed.hpp
build/parserTree.o: /usr/include/boost/type_traits/is_unsigned.hpp
build/parserTree.o: /usr/include/boost/type_traits/add_volatile.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/predicate.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/compare.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/find.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/finder.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/constants.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/detail/finder.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/detail/predicate.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/split.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/iter_find.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/concept.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/find_iterator.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/detail/find_iterator.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/detail/util.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/join.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/detail/sequence.hpp
build/parserTree.o: /usr/include/boost/mpl/logical.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/replace.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/find_format.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/detail/find_format.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/detail/find_format_store.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/detail/replace_storage.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/detail/find_format_all.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/formatter.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/detail/formatter.hpp
build/parserTree.o: /usr/include/boost/algorithm/string/erase.hpp
build/import.o: include/ilang/import.h
build/parser.o: include/ilang/parser.h /usr/include/stdio.h
build/parser.o: /usr/include/features.h /usr/include/stdc-predef.h
build/parser.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
build/parser.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
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
build/variable.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
build/variable.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
build/variable.o: /usr/include/bits/posix_opt.h
build/variable.o: /usr/include/bits/environments.h /usr/include/bits/types.h
build/variable.o: /usr/include/bits/typesizes.h /usr/include/bits/confname.h
build/variable.o: /usr/include/getopt.h /usr/include/boost/config/suffix.hpp
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
build/variable.o: /usr/include/pthread.h /usr/include/endian.h
build/variable.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
build/variable.o: /usr/include/bits/byteswap-16.h /usr/include/sched.h
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
build/scope.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
build/scope.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
build/scope.o: /usr/include/bits/posix_opt.h /usr/include/bits/environments.h
build/scope.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
build/scope.o: /usr/include/bits/confname.h /usr/include/getopt.h
build/scope.o: /usr/include/boost/config/suffix.hpp
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
build/scope.o: /usr/include/pthread.h /usr/include/endian.h
build/scope.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
build/scope.o: /usr/include/bits/byteswap-16.h /usr/include/sched.h
build/scope.o: /usr/include/time.h /usr/include/bits/sched.h
build/scope.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/setjmp.h
build/scope.o: /usr/include/boost/memory_order.hpp
build/scope.o: /usr/include/boost/smart_ptr/detail/operator_bool.hpp
build/scope.o: /usr/include/boost/utility.hpp
build/scope.o: /usr/include/boost/utility/addressof.hpp
build/scope.o: /usr/include/boost/utility/base_from_member.hpp
build/scope.o: /usr/include/boost/preprocessor/repetition/enum_binary_params.hpp
build/scope.o: /usr/include/boost/preprocessor/repetition/enum_params.hpp
build/scope.o: /usr/include/boost/preprocessor/repetition/repeat_from_to.hpp
build/scope.o: /usr/include/boost/preprocessor/arithmetic/add.hpp
build/scope.o: /usr/include/boost/preprocessor/arithmetic/dec.hpp
build/scope.o: /usr/include/boost/preprocessor/control/while.hpp
build/scope.o: /usr/include/boost/preprocessor/list/fold_left.hpp
build/scope.o: /usr/include/boost/preprocessor/list/detail/fold_left.hpp
build/scope.o: /usr/include/boost/preprocessor/list/fold_right.hpp
build/scope.o: /usr/include/boost/preprocessor/list/detail/fold_right.hpp
build/scope.o: /usr/include/boost/preprocessor/list/reverse.hpp
build/scope.o: /usr/include/boost/preprocessor/logical/bitand.hpp
build/scope.o: /usr/include/boost/preprocessor/control/detail/while.hpp
build/scope.o: /usr/include/boost/preprocessor/arithmetic/sub.hpp
build/scope.o: /usr/include/boost/utility/binary.hpp
build/scope.o: /usr/include/boost/preprocessor/control/deduce_d.hpp
build/scope.o: /usr/include/boost/preprocessor/facilities/identity.hpp
build/scope.o: /usr/include/boost/preprocessor/seq/cat.hpp
build/scope.o: /usr/include/boost/preprocessor/seq/fold_left.hpp
build/scope.o: /usr/include/boost/preprocessor/seq/seq.hpp
build/scope.o: /usr/include/boost/preprocessor/seq/elem.hpp
build/scope.o: /usr/include/boost/preprocessor/seq/size.hpp
build/scope.o: /usr/include/boost/preprocessor/seq/transform.hpp
build/scope.o: /usr/include/boost/preprocessor/arithmetic/mod.hpp
build/scope.o: /usr/include/boost/preprocessor/arithmetic/detail/div_base.hpp
build/scope.o: /usr/include/boost/preprocessor/comparison/less_equal.hpp
build/scope.o: /usr/include/boost/preprocessor/logical/not.hpp
build/scope.o: /usr/include/boost/utility/enable_if.hpp
build/scope.o: /usr/include/boost/next_prior.hpp
build/scope.o: /usr/include/boost/noncopyable.hpp include/debug.h
build/scope.o: include/ilang/object.h include/ilang/parserTree.h
build/object.o: include/ilang/object.h include/ilang/variable.h
build/object.o: /usr/include/boost/any.hpp /usr/include/boost/config.hpp
build/object.o: /usr/include/boost/type_traits/remove_reference.hpp
build/object.o: /usr/include/boost/type_traits/broken_compiler_spec.hpp
build/object.o: /usr/include/boost/mpl/aux_/lambda_support.hpp
build/object.o: /usr/include/boost/mpl/aux_/config/lambda.hpp
build/object.o: /usr/include/boost/mpl/aux_/config/ttp.hpp
build/object.o: /usr/include/boost/mpl/aux_/config/msvc.hpp
build/object.o: /usr/include/boost/mpl/aux_/config/gcc.hpp
build/object.o: /usr/include/boost/mpl/aux_/config/workaround.hpp
build/object.o: /usr/include/boost/detail/workaround.hpp
build/object.o: /usr/include/boost/mpl/aux_/config/ctps.hpp
build/object.o: /usr/include/boost/mpl/int_fwd.hpp
build/object.o: /usr/include/boost/mpl/aux_/adl_barrier.hpp
build/object.o: /usr/include/boost/mpl/aux_/config/adl.hpp
build/object.o: /usr/include/boost/mpl/aux_/config/intel.hpp
build/object.o: /usr/include/boost/mpl/aux_/nttp_decl.hpp
build/object.o: /usr/include/boost/mpl/aux_/config/nttp.hpp
build/object.o: /usr/include/boost/preprocessor/cat.hpp
build/object.o: /usr/include/boost/preprocessor/config/config.hpp
build/object.o: /usr/include/boost/mpl/aux_/yes_no.hpp
build/object.o: /usr/include/boost/mpl/aux_/config/arrays.hpp
build/object.o: /usr/include/boost/mpl/aux_/na_fwd.hpp
build/object.o: /usr/include/boost/mpl/aux_/preprocessor/params.hpp
build/object.o: /usr/include/boost/mpl/aux_/config/preprocessor.hpp
build/object.o: /usr/include/boost/preprocessor/comma_if.hpp
build/object.o: /usr/include/boost/preprocessor/punctuation/comma_if.hpp
build/object.o: /usr/include/boost/preprocessor/control/if.hpp
build/object.o: /usr/include/boost/preprocessor/control/iif.hpp
build/object.o: /usr/include/boost/preprocessor/logical/bool.hpp
build/object.o: /usr/include/boost/preprocessor/facilities/empty.hpp
build/object.o: /usr/include/boost/preprocessor/punctuation/comma.hpp
build/object.o: /usr/include/boost/preprocessor/repeat.hpp
build/object.o: /usr/include/boost/preprocessor/repetition/repeat.hpp
build/object.o: /usr/include/boost/preprocessor/debug/error.hpp
build/object.o: /usr/include/boost/preprocessor/detail/auto_rec.hpp
build/object.o: /usr/include/boost/preprocessor/tuple/eat.hpp
build/object.o: /usr/include/boost/preprocessor/inc.hpp
build/object.o: /usr/include/boost/preprocessor/arithmetic/inc.hpp
build/object.o: /usr/include/boost/mpl/aux_/preprocessor/enum.hpp
build/object.o: /usr/include/boost/preprocessor/tuple/to_list.hpp
build/object.o: /usr/include/boost/preprocessor/facilities/overload.hpp
build/object.o: /usr/include/boost/preprocessor/variadic/size.hpp
build/object.o: /usr/include/boost/preprocessor/list/for_each_i.hpp
build/object.o: /usr/include/boost/preprocessor/list/adt.hpp
build/object.o: /usr/include/boost/preprocessor/detail/is_binary.hpp
build/object.o: /usr/include/boost/preprocessor/detail/check.hpp
build/object.o: /usr/include/boost/preprocessor/logical/compl.hpp
build/object.o: /usr/include/boost/preprocessor/repetition/for.hpp
build/object.o: /usr/include/boost/preprocessor/repetition/detail/for.hpp
build/object.o: /usr/include/boost/preprocessor/control/expr_iif.hpp
build/object.o: /usr/include/boost/preprocessor/tuple/elem.hpp
build/object.o: /usr/include/boost/preprocessor/tuple/rem.hpp
build/object.o: /usr/include/boost/preprocessor/variadic/elem.hpp
build/object.o: /usr/include/boost/type_traits/detail/type_trait_def.hpp
build/object.o: /usr/include/boost/type_traits/detail/template_arity_spec.hpp
build/object.o: /usr/include/boost/mpl/int.hpp
build/object.o: /usr/include/boost/mpl/aux_/integral_wrapper.hpp
build/object.o: /usr/include/boost/mpl/integral_c_tag.hpp
build/object.o: /usr/include/boost/mpl/aux_/config/static_constant.hpp
build/object.o: /usr/include/boost/mpl/aux_/static_cast.hpp
build/object.o: /usr/include/boost/mpl/aux_/template_arity_fwd.hpp
build/object.o: /usr/include/boost/mpl/aux_/config/overload_resolution.hpp
build/object.o: /usr/include/boost/type_traits/detail/type_trait_undef.hpp
build/object.o: /usr/include/boost/type_traits/is_reference.hpp
build/object.o: /usr/include/boost/type_traits/config.hpp
build/object.o: /usr/include/boost/type_traits/is_lvalue_reference.hpp
build/object.o: /usr/include/boost/type_traits/detail/bool_trait_def.hpp
build/object.o: /usr/include/boost/type_traits/integral_constant.hpp
build/object.o: /usr/include/boost/mpl/bool.hpp
build/object.o: /usr/include/boost/mpl/bool_fwd.hpp
build/object.o: /usr/include/boost/mpl/integral_c.hpp
build/object.o: /usr/include/boost/mpl/integral_c_fwd.hpp
build/object.o: /usr/include/boost/type_traits/detail/bool_trait_undef.hpp
build/object.o: /usr/include/boost/type_traits/is_rvalue_reference.hpp
build/object.o: /usr/include/boost/type_traits/ice.hpp
build/object.o: /usr/include/boost/type_traits/detail/yes_no_type.hpp
build/object.o: /usr/include/boost/type_traits/detail/ice_or.hpp
build/object.o: /usr/include/boost/type_traits/detail/ice_and.hpp
build/object.o: /usr/include/boost/type_traits/detail/ice_not.hpp
build/object.o: /usr/include/boost/type_traits/detail/ice_eq.hpp
build/object.o: /usr/include/boost/throw_exception.hpp
build/object.o: /usr/include/boost/exception/detail/attribute_noreturn.hpp
build/object.o: /usr/include/boost/static_assert.hpp
build/object.o: /usr/include/boost/shared_ptr.hpp
build/object.o: /usr/include/boost/smart_ptr/shared_ptr.hpp
build/object.o: /usr/include/boost/config.hpp
build/object.o: /usr/include/boost/config/select_compiler_config.hpp
build/object.o: /usr/include/boost/config/compiler/gcc.hpp
build/object.o: /usr/include/boost/config/select_platform_config.hpp
build/object.o: /usr/include/boost/config/posix_features.hpp
build/object.o: /usr/include/unistd.h /usr/include/features.h
build/object.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
build/object.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
build/object.o: /usr/include/bits/posix_opt.h /usr/include/bits/environments.h
build/object.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
build/object.o: /usr/include/bits/confname.h /usr/include/getopt.h
build/object.o: /usr/include/boost/config/suffix.hpp
build/object.o: /usr/include/boost/config/no_tr1/memory.hpp
build/object.o: /usr/include/boost/assert.hpp /usr/include/assert.h
build/object.o: /usr/include/boost/current_function.hpp
build/object.o: /usr/include/boost/checked_delete.hpp
build/object.o: /usr/include/boost/smart_ptr/detail/shared_count.hpp
build/object.o: /usr/include/boost/smart_ptr/bad_weak_ptr.hpp
build/object.o: /usr/include/boost/smart_ptr/detail/sp_counted_base.hpp
build/object.o: /usr/include/boost/smart_ptr/detail/sp_has_sync.hpp
build/object.o: /usr/include/boost/smart_ptr/detail/sp_counted_base_gcc_x86.hpp
build/object.o: /usr/include/boost/detail/sp_typeinfo.hpp
build/object.o: /usr/include/boost/smart_ptr/detail/sp_counted_impl.hpp
build/object.o: /usr/include/boost/smart_ptr/detail/sp_convertible.hpp
build/object.o: /usr/include/boost/smart_ptr/detail/spinlock_pool.hpp
build/object.o: /usr/include/boost/smart_ptr/detail/spinlock.hpp
build/object.o: /usr/include/boost/smart_ptr/detail/spinlock_pt.hpp
build/object.o: /usr/include/pthread.h /usr/include/endian.h
build/object.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
build/object.o: /usr/include/bits/byteswap-16.h /usr/include/sched.h
build/object.o: /usr/include/time.h /usr/include/bits/sched.h
build/object.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/setjmp.h
build/object.o: /usr/include/boost/memory_order.hpp
build/object.o: /usr/include/boost/smart_ptr/detail/operator_bool.hpp
build/object.o: include/ilang/parserTree.h include/ilang/scope.h
build/object.o: /usr/include/boost/utility.hpp
build/object.o: /usr/include/boost/utility/addressof.hpp
build/object.o: /usr/include/boost/utility/base_from_member.hpp
build/object.o: /usr/include/boost/preprocessor/repetition/enum_binary_params.hpp
build/object.o: /usr/include/boost/preprocessor/repetition/enum_params.hpp
build/object.o: /usr/include/boost/preprocessor/repetition/repeat_from_to.hpp
build/object.o: /usr/include/boost/preprocessor/arithmetic/add.hpp
build/object.o: /usr/include/boost/preprocessor/arithmetic/dec.hpp
build/object.o: /usr/include/boost/preprocessor/control/while.hpp
build/object.o: /usr/include/boost/preprocessor/list/fold_left.hpp
build/object.o: /usr/include/boost/preprocessor/list/detail/fold_left.hpp
build/object.o: /usr/include/boost/preprocessor/list/fold_right.hpp
build/object.o: /usr/include/boost/preprocessor/list/detail/fold_right.hpp
build/object.o: /usr/include/boost/preprocessor/list/reverse.hpp
build/object.o: /usr/include/boost/preprocessor/logical/bitand.hpp
build/object.o: /usr/include/boost/preprocessor/control/detail/while.hpp
build/object.o: /usr/include/boost/preprocessor/arithmetic/sub.hpp
build/object.o: /usr/include/boost/utility/binary.hpp
build/object.o: /usr/include/boost/preprocessor/control/deduce_d.hpp
build/object.o: /usr/include/boost/preprocessor/facilities/identity.hpp
build/object.o: /usr/include/boost/preprocessor/seq/cat.hpp
build/object.o: /usr/include/boost/preprocessor/seq/fold_left.hpp
build/object.o: /usr/include/boost/preprocessor/seq/seq.hpp
build/object.o: /usr/include/boost/preprocessor/seq/elem.hpp
build/object.o: /usr/include/boost/preprocessor/seq/size.hpp
build/object.o: /usr/include/boost/preprocessor/seq/transform.hpp
build/object.o: /usr/include/boost/preprocessor/arithmetic/mod.hpp
build/object.o: /usr/include/boost/preprocessor/arithmetic/detail/div_base.hpp
build/object.o: /usr/include/boost/preprocessor/comparison/less_equal.hpp
build/object.o: /usr/include/boost/preprocessor/logical/not.hpp
build/object.o: /usr/include/boost/utility/enable_if.hpp
build/object.o: /usr/include/boost/next_prior.hpp
build/object.o: /usr/include/boost/noncopyable.hpp include/debug.h
