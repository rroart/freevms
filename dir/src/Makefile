EXE =
# EXE = .exe

LIB = ../../starlet/src/starlet.a
LIBUM = ../../starlet/src/starletum.a

FLAGS = -static

all: directory${EXE} directoryum${EXE}

clean:
	rm *.o um/* i386/* directory

install: all
	[ ! -d ../../rootum/vms$$common.sysexe ] && mkdir -p ../../rootum/vms"$$"common/sysexe
	[ ! -d ../../rooti386/vms$$common.sysexe ] && mkdir -p ../../rooti386/vms"$$"common/sysexe
	cp -p um/* ../../rootum/vms"$$"common/sysexe
	tar -C i386 -clf - .| tar -C ../../rooti386/vms"$$"common/sysexe -xpf -

directory.o: directory.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ directory.c -o directory.o

directory${EXE}: directory.o $(LIB)
	gcc -s -static directory.o ../../starlet/src/starlet.a -o directory${EXE}
	../../linker/src/linker $(LINKPRE) directory.o ../../starlet/src/starlet.a $(LIBGCC) $(LINKPOST) $(LIBGCC) -o directory.exe
	mkdir i386 || echo i386 exists
	cp -p directory i386/directory
	[ -f directory.exe ] && cp -p directory.exe i386/directory.exe
	cd i386
	cd i386; ln -f directory copy
	cd i386; ln -f directory import
	cd i386; ln -f directory export
	cd i386; ln -f directory delete
	cd i386; ln -f directory difference
	cd i386; ln -f directory extend
	cd i386; ln -f directory help
	cd i386; ln -f directory show
	cd i386; ln -f directory search
	cd i386; ln -f directory type
	cd i386; ln -f directory mount
	cd i386; [ -f directory.exe ] && ln -f directory.exe copy.exe
	cd i386; [ -f directory.exe ] && ln -f directory.exe import.exe
	cd i386; [ -f directory.exe ] && ln -f directory.exe export.exe
	cd i386; [ -f directory.exe ] && ln -f directory.exe delete.exe
	cd i386; [ -f directory.exe ] && ln -f directory.exe difference.exe
	cd i386; [ -f directory.exe ] && ln -f directory.exe extend.exe
	cd i386; [ -f directory.exe ] && ln -f directory.exe help.exe
	cd i386; [ -f directory.exe ] && ln -f directory.exe show.exe
	cd i386; [ -f directory.exe ] && ln -f directory.exe search.exe
	cd i386; [ -f directory.exe ] && ln -f directory.exe type.exe
	cd i386; [ -f directory.exe ] && ln -f directory.exe mount.exe
	cd ..

UMDEF=-Dcompile_um -I.

directoryum${EXE}: directory.c $(LIBUM)
	gcc $(FLAGS) -g $(UMDEF) -I../../starlet/src -I../../librtl/src/ -I../../lib/src directory.c ../../starlet/src/starletum.a -o directoryum${EXE}
	mkdir um || echo um exists
	cp -p directoryum um/copy
	cp -p directoryum um/import
	cp -p directoryum um/export
	cp -p directoryum um/delete
	cp -p directoryum um/difference
	cp -p directoryum um/directory
	cp -p directoryum um/extend
	cp -p directoryum um/help
	cp -p directoryum um/show
	cp -p directoryum um/search
	cp -p directoryum um/type
	cp -p directoryum um/mount
