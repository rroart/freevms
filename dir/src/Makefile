EXE =
# EXE = .exe

all: directory${EXE} directoryum${EXE}

directory${EXE}: directory.c
	cc -g -I../../starlet/src -I../../librtl/src/ -I../../lib/src directory.c ../../starlet/src/starlet.a -o directory${EXE}
	cp -p directory mount

UMDEF=-Dcompile_um -I.

directoryum${EXE}: directory.c
	cc -g $(UMDEF) -I../../starlet/src -I../../librtl/src/ -I../../lib/src directory.c -I../../examples ../../starlet/src/starlet.c ../../starlet/src/rms.c -o directoryum${EXE}
	cp -p directory mount
