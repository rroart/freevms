VMSSUBDIRS = starlet librtl cliutl backup dir init login dcl edt dfu
VMSSUBDIRSINST = cliutl dir dcl login init edt dfu
VMSSUBDIRS2 = examples

export LIBGCC = $(shell gcc -print-libgcc-file-name)

export LIBC = /usr/lib/libc.a

export VMSLIBS = $(TOPDIR)/librtl/src/librtl.a $(TOPDIR)/starlet/src/starlet.a
export VMSLIBSUM = $(TOPDIR)/librtl/src/librtl.a $(TOPDIR)/starlet/src/starletum.a 
export LINKER = $(TOPDIR)/linker/src/linker

export LINKPRE = -Bstatic
#export LINKPRE = -Bstatic -s 
export LINKPOST = /usr/lib/crt1.o /usr/lib/crti.o /usr/lib/libc.a

export EXE = .exe
export NOEXE =

export ROOTI386 = $(TOPDIR)/rooti386
export ROOTUM = $(TOPDIR)/rootum
export ROOTI386_COMMON = $(TOPDIR)/rooti386/vms"$$$$"common
export ROOTUM_COMMON = $(TOPDIR)/rootum/vms"$$$$"common
export ROOTI386_COMMON_SYSEXE = $(TOPDIR)/rooti386/vms"$$$$"common/sysexe
export ROOTUM_COMMON_SYSEXE = $(TOPDIR)/rootum/vms"$$$$"common/sysexe

vmsall:
	for X in $(VMSSUBDIRS); do \
		cd $$X/src; \
		make; \
		cd ../..; \
	done; \
	for X in $(VMSSUBDIRS2); do \
		cd $$X; \
		make; \
		cd ..; \
	done;

vmsclean:
	for X in $(VMSSUBDIRS); do \
		cd $$X/src; \
		make clean; \
		cd ../..; \
	done; \
	for X in $(VMSSUBDIRS2); do \
		cd $$X; \
		make clean; \
		cd ..; \
	done;

rootinstall:
	for X in $(VMSSUBDIRSINST); do \
		cd $$X/src; \
		make install; \
		cd ../..; \
	done; \
	for X in $(VMSSUBDIRS2); do \
		cd $$X; \
		make install; \
		cd ..; \
	done; 

image-install:
	make rootinstall; \
	cd diskimage; \
	./instmnt; \
	cd ..

image:
	cd diskimage; \
	make image; \
	cd ..

image-kernel-iomm:
	cd diskimage; \
	make image-kernel-iomm; \
	cd ..

image-kernel-noiomm:
	cd diskimage; \
	make image-kernel-noiomm; \
	cd ..

environment:
	./envscript

download-linker:
	cd linker/src; \
	make download-linker; \
	cd ../..

build-linker:
	cd linker/src; \
	make build-linker; \
	cd ../..

download-bliss:
	cd bliss/src; \
	make download-bliss; \
	cd ../..

build-bliss:
	cd bliss/src; \
	make build-bliss; \
	cd ../..

download-grub:
	cd grub/src; \
	make download-grub; \
	cd ../..

build-grub:
	cd grub/src; \
	make build-grub; \
	cd ../..

include Makefile.kernel
