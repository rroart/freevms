OBJ = sys.o

OBJS = checksum.o ldt.o old-checksum.o ptrace.o ptrace_user.o semaphore.o \
	sigcontext.o syscalls.o sysrq.o
export-objs = ksyms.o

SYMLINKS = semaphore.c old-checksum.c checksum.S

all: $(OBJ)

$(OBJ): $(OBJS) $(export-objs)
	rm -f $@
	$(LD) $(LINKFLAGS) --start-group $^ --end-group -o $@

sigcontext.o: sigcontext.c
	$(CC) $(USER_CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

ldt.o: ldt.c
	$(CC) $(USER_CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

ptrace_user.o: ptrace_user.c
	$(CC) $(USER_CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

checksum.S old-checksum.c: 
	-rm -f $@
	-ln -s $(TOPDIR)/linux/arch/i386/lib/$@ $@

semaphore.c:
	-rm -f $@
	-ln -s $(TOPDIR)/linux/arch/i386/kernel/$@ $@

clean:
	rm -f $(OBJS) $(export-objs)

fastdep:

archmrproper:
	rm -f $(SYMLINKS)

archclean:
	rm -f link.ld
	@$(MAKEBOOT) clean

archdep:
	@$(MAKEBOOT) dep

modules:

include $(TOPDIR)/Rules.make
