EXE =
# EXE = .exe

all: directory${EXE} directoryum${EXE}

directory${EXE}: directory.c
	gcc -g -I../../starlet/src -I../../librtl/src/ -I../../lib/src directory.c ../../starlet/src/starlet.o -o directory${EXE}
	cp -p directory mount

UMDEF=-Dcompile_um -I.

directoryum${EXE}: directory.c
	gcc -g $(UMDEF) -I../../starlet/src -I../../librtl/src/ -I../../lib/src directory.c -I../../examples ../../starlet/src/starlet.c -o directoryum${EXE}
	cp -p directory mount
