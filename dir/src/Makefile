EXE =
# EXE = .exe

LIB = ../../starlet/src/starlet.a
LIBUM = ../../starlet/src/starletum.a

all: directory${EXE} directoryum${EXE}

directory${EXE}: directory.c $(LIB)
	cc -g -I../../starlet/src -I../../librtl/src/ -I../../lib/src directory.c ../../starlet/src/starlet.a -o directory${EXE}
	mkdir i386
	cp -p directory i386/copy
	cp -p directory i386/import
	cp -p directory i386/delete
	cp -p directory i386/difference
	cp -p directory i386/directory
	cp -p directory i386/extend
	cp -p directory i386/help
	cp -p directory i386/show
	cp -p directory i386/search
	cp -p directory i386/type

UMDEF=-Dcompile_um -I.

directoryum${EXE}: directory.c $(LIBUM)
	cc -g $(UMDEF) -I../../starlet/src -I../../librtl/src/ -I../../lib/src directory.c ../../starlet/src/starletum.a -o directoryum${EXE}
	mkdir um
	cp -p directoryum um/copy
	cp -p directoryum um/import
	cp -p directoryum um/delete
	cp -p directoryum um/difference
	cp -p directoryum um/directory
	cp -p directoryum um/extend
	cp -p directoryum um/help
	cp -p directoryum um/show
	cp -p directoryum um/search
	cp -p directoryum um/type
