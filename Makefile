VMSSUBDIRS = crtl starlet librtl smgrtl sort cld cdu msgfil cliutl backup dir init login dcl edt dfu mount copy delete dif util32 sda instal uaf
VMSSUBDIRSINST = crtl starlet librtl smgrtl sort cld cliutl dir dcl login init edt dfu mount copy delete dif util32 sda instal uaf
VMSSUBDIRS2 = examples
VMSSUBDIRS3 = cmuip/ipacp/src/

export BLISS = $(TOPDIR)/bliss/bin/gcc

TMPARCH := $(shell uname -m | sed -e s/i.86/i386/ -e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/)

export LIBGCC = $(shell gcc -print-libgcc-file-name)
ifeq ($(TMPARCH),x86_64)
export LIBGCC_EH = $(shell gcc -print-file-name=libgcc_eh.a)
export LIBGCC += $(LIBGCC_EH)
endif

export LIBC = /usr/lib/libc.a

export EXE = .exe
export NOEXE =
export EXE_ELF = .ele

export VMS_RPATH = -Wl,-rpath,/vms"$$$$"common/syslib
export VMS_LD_RPATH = -rpath /vms"$$$$"common/syslib

export VMSLIBS = $(TOPDIR)/librtl/src/librtl.a $(TOPDIR)/starlet/src/starlet.a
export VMSLIBS_PIC = $(TOPDIR)/librtl/src/librtl_pic.a $(TOPDIR)/starlet/src/starlet_pic.a
export VMSDLIBS_EXE = $(TOPDIR)/librtl/src/i386/librtl$(EXE) $(TOPDIR)/starlet/src/i386/starlet$(EXE)
export VMSDLIBS_EXE_ELF = $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF) $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)
export VMSLIBSUM = $(TOPDIR)/librtl/src/librtl.a $(TOPDIR)/starlet/src/starletum.a 
export VMSLIBSUM_PIC = $(TOPDIR)/librtl/src/librtl_pic.a $(TOPDIR)/starlet/src/starletum_pic.a 
export LINKER = $(TOPDIR)/linker/src/linker

export LINKPRE = -Bstatic
#export LINKPRE = -Bstatic -s 
export LINKPOST = /usr/lib/crt1.o /usr/lib/crti.o /usr/lib/libc.a

export ROOTI386 = $(TOPDIR)/rooti386
export ROOTUM = $(TOPDIR)/rootum
export ROOTI386_COMMON = $(TOPDIR)/rooti386/vms"$$$$"common
export ROOTUM_COMMON = $(TOPDIR)/rootum/vms"$$$$"common
export ROOTI386_COMMON_SYSEXE = $(TOPDIR)/rooti386/vms"$$$$"common/sysexe
export ROOTUM_COMMON_SYSEXE = $(TOPDIR)/rootum/vms"$$$$"common/sysexe

export EXTERNAL = $(TOPDIR)/cdu/src/cdu.a
export EXTERNAL_PIC = $(TOPDIR)/cdu/src/cdu_pic.a
export EXTERNAL2 = $(TOPDIR)/cdu/src/cdu2.a
export EXTERNAL2_PIC = $(TOPDIR)/cdu/src/cdu2_pic.a

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
	done; \
	for X in $(VMSSUBDIRS3); do \
		cd $$X; \
		make; \
		cd ../../..; \
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
	done; \
	for X in $(VMSSUBDIRS3); do \
		cd $$X; \
		make clean; \
		cd ../../..; \
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
	done; \
	for X in $(VMSSUBDIRS3); do \
		cd $$X; \
		make install; \
		cd ../../..; \
	done; \
	make -f Makefile.linux install

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

telnet:
	cd cmuip/telnet/src; \
	make; \
	cd ../../..

include Makefile.kernel
