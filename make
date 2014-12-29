#!/usr/bin/env python2

import sys
sys.path.insert(0, 'deps/fabricate/')

import glob
import subprocess
import os
from os.path import abspath
import re
from fabricate import *


TARGET = 'i'
SRCS = 'parserTree.cc import.cc parser.cc database.cc error.cc print.cc init.cc thread.cc function.cc value.cc identifier.cc object.cc variable.cc scope.cc variable_modifiers.cc c_class.cc'.split()
LIBS = '-lboost_filesystem -lboost_system -lboost_thread -lssl -lpthread -lsnappy -ltbb -lprotobuf -ldb_cxx -lcurl'

CONFIG = dict(
    CXX='g++',
    LD='g++ -L/usr/local/lib',
    PROTOC='protoc',
    BISON='bison',
    FLEX='flex',
    BUILDDIR=abspath('build'),
    SRCDIR=abspath('src'),
    PROJECTDIR=abspath('.'),
    GITVERSION=Shell('git describe --always --long --dirty --abbrev=12', silent=True).strip(),
    CXXFLAGS_BASE='-DILANG_VERSION=\\\\\\"{GITVERSION}\\\\\\" -std=c++1y -Wall -w -I{SRCDIR}/ -I{BUILDDIR}/ -I{PROJECTDIR}/deps/catch',
    CXXFLAGS='-ggdb -O0 -DILANG_STATIC_LIBRARY {{CXXFLAGS_BASE}}',
    CXXFLAGS_MODULES='{{{{CXXFLAGS}}}}',
    LDFLAGS='',
    LIBS=LIBS,
    SRCS=' '.join(SRCS),
)

# helpers

def _cxx(filename, *args, **kwargs):
    f = '{SRCDIR}/{filename}'.format(filename=filename, **CONFIG)
    if not os.path.isfile(f):
        # if not in src dir then must be generating the file from some other step
        # and there must be an after statement somewhere
        f = '{BUILDDIR}/{filename}'.format(filename=filename, **CONFIG)
    t = '{BUILDDIR}/{filename}'.format(
        filename=re.sub(r'\.[^\.]+$','.o',filename),
        **CONFIG
    )
    Run('{CXX} {CXXFLAGS} -o {to_file} -c {from_file}'.format(
        to_file=t,
        from_file=f,
        **CONFIG
    ), *args, **kwargs)
    return t

def _update_config():
    global CONFIG
    for k in CONFIG:
        v = os.environ.get(k)
        if v:
            CONFIG[k] = v
    # enable config variables to take values from other config variables
    old_config = None
    while old_config != CONFIG:
        old_config = CONFIG
        CONFIG = dict(
            (k,v.format(**old_config)) for k,v in old_config.iteritems()
        )

def _link(arr, TARGET=TARGET):
    after()
    Run('{LD} -o {TARGET} {LDFLAGS} {objs} {LIBS}'.format(
        objs=' '.join(arr),
        TARGET=TARGET,
        **CONFIG
    ))


# targets

def build():
    arr = (
        [_cxx('main.cc')] +
        compile_core() +
        compile_deps() +
        compile_modules()
    )
    _link(arr)


def unit():
    arr = (
        compile_core() +
        compile_deps() +
        compile_modules()
    )
    arr += compile_units()
    _link(arr, TARGET='{BUILDDIR}/unit'.format(**CONFIG))
    after()
    Shell('{BUILDDIR}/unit'.format(**CONFIG))

def check():
    build()
    after()
    Shell('cd checks && bash ./run', shell=True)

def compile_core():
    arr = compile_parser()
    arr += compile_database()
    after('parser', 'database')
    for s in SRCS:
        arr += [_cxx(s, group='core')]
    return arr

def compile_units():
    arr = []
    Shell('mkdir -p {BUILDDIR}/unit_tests'.format(**CONFIG))
    for s in glob.glob('unit_tests/*.cc'):
        Run('{CXX} {CXXFLAGS} -o {BUILDDIR}/{to_file} -c {from_file}'.format(
            from_file=s,
            to_file=s.replace('.cc', '.o'),
            **CONFIG
        ))
        arr.append('{BUILDDIR}/{fn}'.format(fn=s.replace('.cc', '.o'), **CONFIG))
    return arr

def compile_parser():
    Run('{BISON} -v -t -o {BUILDDIR}/parser.tab.cc {SRCDIR}/parser.y'.format(**CONFIG), group='parser-bison')
    Run('{FLEX} -o {BUILDDIR}/lex.yy.cc {SRCDIR}/scanner.l'.format(**CONFIG), after='parser-bison', group='parser-flex')
    return [
        _cxx('parser.tab.cc', after='parser-bison', group='parser'),
        _cxx('lex.yy.cc', after='parser-flex', group='parser')
    ]

def compile_database():
    Run('{PROTOC} {SRCDIR}/database.proto --cpp_out={BUILDDIR} --proto_path={SRCDIR}'.format(**CONFIG), group='database-proto')
    return [_cxx('database.pb.cc', group='database', after='database-proto')]

def compile_deps():
    libuv = '{PROJECTDIR}/deps/libuv/libuv.a'.format(**CONFIG)
    if not os.path.isfile(libuv):
        Shell('cd {PROJECTDIR}/deps/libuv && make'.format(**CONFIG), shell=True)
    return [libuv]

def compile_modules():
    arr = []
    for f in glob.glob('modules/**/*.cc'):
        Shell('mkdir -p {BUILDDIR}/{f}'.format(f=os.path.dirname(f), **CONFIG))
        Run('{CXX} {CXXFLAGS_MODULES} -o {BUILDDIR}/{to_file} -c {PROJECTDIR}/{from_file}'.format(
            from_file=f,
            to_file=f.replace('.cc', '.o'),
            **CONFIG
        ), group='modules')
        arr += ['{BUILDDIR}/{fn}'.format(fn=f.replace('.cc', '.o'), **CONFIG)]
    return arr

def clean():
    autoclean()

def clean_all():
    Shell('cd {PROJECTDIR}/deps/libuv && make clean'.format(**CONFIG), shell=True)
    clean()

def submodules():
    Shell('git submodule update --init --recursive')

if __name__ == '__main__':
    _update_config()
    main(parallel_ok=True)#, jobs=4)
