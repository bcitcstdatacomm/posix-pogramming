#!/usr/bin/env bash

compile()
{
  echo "Building $1/$2"
  pushd "$2" > /dev/null || exit
  make clean all || exit
  popd > /dev/null || exit
}

compile_all()
{
  pushd "$1" > /dev/null || exit
  for dir in */ ; do
    compile "$1" "$dir"
  done
  popd > /dev/null || exit
}

if [ -z ${1} ]; then echo "usage: ./build-all.sh <c compiler>"; exit 1; fi

declare -a dirs=( "io" "processes" )
export LINTFLAGS="--quiet --checks=*,-llvmlibc-restrict-system-libc-headers,-cppcoreguidelines-init-variables,-clang-analyzer-security.insecureAPI.strcpy,-concurrency-mt-unsafe,-android-cloexec-accept,-android-cloexec-dup,-google-readability-todo,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-cert-dcl03-c,-hicpp-static-assert,-misc-static-assert,-altera-struct-pack-align"
export LINT=clang-tidy
export CFLAGS="-g -std=c11 -fstack-protector-all -ftrapv -Wpedantic -Wall -Wextra -Wdouble-promotion -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wnull-dereference -Winit-self -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wunused-local-typedefs -Wstrict-overflow=5 -Wmissing-noreturn -Walloca -Wfloat-equal -Wdeclaration-after-statement -Wshadow -Wpointer-arith -Wabsolute-value -Wundef -Wexpansion-to-defined -Wunused-macros -Wno-endif-labels -Wbad-function-cast -Wcast-qual -Wwrite-strings -Wconversion -Wdangling-else -Wdate-time -Wempty-body -Wsign-conversion -Wfloat-conversion -Waggregate-return -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wpacked -Wredundant-decls -Wnested-externs -Winline -Winvalid-pch -Wlong-long -Wvariadic-macros -Wdisabled-optimization -Wstack-protector -Woverlength-strings"
export CC=$1

for i in "${dirs[@]}"
do
  compile_all "$i"
done
