docproc.o: docproc.c
	$(HOSTCC) $(HOSTCFLAGS) -c -o $@ $(@:.o=.c)

docproc: docproc.o
	${HOSTCC} -o docproc docproc.o

clean:
	rm -f *~ *.o mkdep split-include docproc

include $(TOPDIR)/Rules.make
