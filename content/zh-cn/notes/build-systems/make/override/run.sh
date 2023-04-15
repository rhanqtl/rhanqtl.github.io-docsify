#!/usr/bin/env bash

echo "=== Makefile Only ==="
make

echo "=== Makefile w/o override + Command-Line ==="
make XYZ:=CLI

echo "=== Makefile w/ override + Command-Line ==="
make XYZ:=CLI -f Makefile.override

echo "=== Makefile w/o override + Env w/o flag ==="
XYZ=Env make

echo "=== Makefile w/ override + Env w/o flag ==="
XYZ=Env make -f Makefile.override

echo "=== Makefile w/o override + Env w/ flag ==="
XYZ=Env make -e

echo "=== Makefile w/ override + Env w/ flag ==="
XYZ=Env make -e -f Makefile.override

echo "=== Makefile w/o override + Command-Line + Env w/o flag ==="
XYZ=Env make XYZ:=CLI

echo "=== Makefile w/o override + Command-Line + Env w/ flag ==="
XYZ=Env make XYZ:=CLI -e

echo "=== Makefile w/ override + Command-Line + Env w/o flag ==="
XYZ=Env make XYZ:=CLI -f Makefile.override

echo "=== Makefile w/ override + Command-Line + Env w/ flag ==="
XYZ=Env make XYZ:=CLI -e -f Makefile.override
