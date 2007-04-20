#
# Make file for STR$ functions
#
# History
#
#	Oct 10, 1997 - Kevin Handy
#		Initial version
#
#	Feb 4, 1997 - Christof Zeile
#		Change '.0' to '.o'.
#		Change 'str_analyse_sdesc' to 'str_analyze_sdesc'
#
#	Feb 4, 1997 - Kevin Handy
#		Added '-Wall' and '-Wmissing-prototypes' so I could
#		find more problems.
#
#	01/26/1998 - Kevin Handy
#		Merged together my basic and string libraries, so I
#		only has one subdirectory to worry about.
#
CFLAGS = -fdollars-in-identifiers -Wall -Wmissing-prototypes -Wcast-qual -Wconversion -I. -I../../starlet/src -I../../lib/src -DNOKERNEL

STROBJ = \
	str_analyze_sdesc.o stranalyze_sdesc_64.o \
	str_append.o str_blind_compare.o \
	str_compare.o str_compare_eql.o str_concat.o \
	str_copy_dx.o str_copy_r.o \
	str_dupl_char.o \
	str_free1_dx.o str_get1_dx.o str_left.o \
	str_pos_extr.o str_right.o \
	str_trim.o str_upcase.o str_find_first_in_set.o \
	str_find_first_not_in_set.o str_position.o \
	str_translate.o str_element.o \
	str_len_extr.o str_prefix.o \
	strcompare_multi.o strcopy_r_64.o strdivide.o \
	strfind_first_substring.o strget1_dx_64.o \
	strmatch_wild.o strmul.o strrecip.o strreplace.o \
	strround.o strsub.o strutil.o stradd.o

LIBOBJ = lib_analyze_sdesc.o lib_cvt_dtb.o lib_cvt_htb.o \
	lib_cvt_otb.o lib_ichar.o lib_index.o lib_len.o \
	lib_ab_asc_ebc.o lib_ab_asc_ebc_rev.o \
	lib_ab_cvt_o_u.o lib_ab_cvt_u_o.o lib_ab_cvtpt_o.o \
	lib_ab_cvtpt_u.o lib_ab_cvtpt_z.o lib_ab_cvttp_o.o \
	lib_ab_cvttp_u.o lib_ab_cvttp_z.o lib_ab_ebc_asc.o \
	lib_ab_ebc_asc_rev.o lib_ab_lowercase.o lib_ab_upcase.o \
	lib_add_times.o lib_addx.o lib_cvt_vectim.o lib_day.o \
	lib_day_of_week.o lib_mult_delta_time.o lib_sub_times.o \
	lib_subx.o libcredir.o libgetinp.o libputout.o libgetef.o \
	libffs.o libffc.o libinsv.o libextv.o libextzv.o \
	libbuild_nodespec.o libcompare_nodename.o libconvert_date_string.o \
	libdate_time.o libdigit_sep.o libutil.o libanalyze_sdesc_64.o \
	libasnmbx.o lib_char.o \
	libcli.o \
	liblocc.o libmatchc.o  libmovc3.o libediv.o \
	libgetdvi.o libgetjpi.o libgetsyi.o \
	libgetlog.o libtparse.o libgetfor.o \
	libwait.o libebcdic.o libbintree.o liblookey.o libtimer.o libvm.o \
	libexpand_nodename.o sys__combine_date_time.o \
	sysgetmsg.o sysgeterrno.o

SYSOBJ = sys__combine_date_time.o sys_asctim.o sys_bintim.o \
	sys_gettim.o sys_numtim.o

CVTOBJ = cvt_convert_float.o

QUELIB = libinsqhi.o

OTSOBJ = otscvt.o

STROBJ_PIC = $(addprefix pic/,$(STROBJ))
LIBOBJ_PIC = $(addprefix pic/,$(LIBOBJ))
SYSOBJ_PIC = $(addprefix pic/,$(SYSOBJ))
CVTOBJ_PIC = $(addprefix pic/,$(STROBJ))
QUELIB_PIC = $(addprefix pic/,$(QUELIB))
OTSOBJ_PIC = $(addprefix pic/,$(OTSOBJ))

STRLIB = librtl.a
STRLIB_PIC = librtl_pic.a
ELFLIB = i386/librtl$(EXE_ELF)
EXELIB = i386/librtl$(EXE)

all : $(STRLIB) $(STRLIB_PIC) $(ELFLIB) $(EXELIB) tests

install: all
	install -d ../../rooti386/vms"$$"common/syslib
	install i386/*.ele ../../rooti386/vms"$$"common/syslib
	install i386/*.exe ../../rooti386/vms"$$"common/syslib

clean :
	- rm *.o
	- rm *.a
	- rm teststr
	- rm test_str_element
	- rm test_addx
	- rm test_date

tests: teststr test_str_element test_addx rtl_lib_test rtl_str_test test_ile test_libgetef
# not: test_date

teststr : teststr.c $(STRLIB)
test_str_element : test_str_element.c $(STRLIB)
test_addx : test_addx.c $(STRLIB)
test_date : test_date.c $(STRLIB)
rtl_str_test : rtl_str_test.c $(STRLIB) $(SYSLIB)
rtl_lib_test : rtl_lib_test.c $(STRLIB) $(SYSLIB)
test_ile : test_ile.c $(STRLIB) $(SYSLIB)
test_libgetef : test_libgetef.c $(STRLIB) $(SYSLIB)

$(STRLIB) : $(STROBJ) $(LIBOBJ) $(CVTOBJ) $(QUELIB) $(OTSOBJ)
	ar -rv $(STRLIB) $?

$(STRLIB_PIC) : $(STROBJ_PIC) $(LIBOBJ_PIC) $(CVTOBJ_PIC) $(QUELIB_PIC) $(OTSOBJ)
	ar -rv $(STRLIB_PIC) $?

$(EXELIB) : $(STROBJ) $(LIBOBJ) $(CVTOBJ) $(QUELIB) $(OTSOBJ)
	install -d i386
	$(LINKER) -Bshareable $? ../../starlet/src/i386/starlet$(EXE) -o $(EXELIB)

starlet$(EXE_ELF): $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)
	ln -s $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)

$(ELFLIB) : $(STROBJ_PIC) $(LIBOBJ_PIC) $(CVTOBJ_PIC) $(QUELIB_PIC) $(OTSOBJ_PIC) starlet$(EXE_ELF)
	install -d i386
	ld -Bshareable $(VMS_LD_RPATH) $? starlet$(EXE_ELF) $(LIBGCC) -o $(ELFLIB)

pic/%.o: %.c 
	install -d pic
	$(CC) -fpic -fPIC $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -I$(HPATH) -c -o $@ $<

str.o : str.c ../../starlet/src/strdef.h
str__const.o : str__const.c ../../starlet/src/strdef.h
str__is_string_class.o : str__is_string_class.c ../../starlet/src/strdef.h
str__copy_fill.o : str__copy_fill.c ../../starlet/src/strdef.h
str__resize.o : str__resize.c ../../starlet/src/strdef.h
str_analyze_sdesc.o : str_analyze_sdesc.c ../../starlet/src/strdef.h
str_append.o : str_append.c ../../starlet/src/strdef.h
str_blind_compare.o : str_blind_compare.c ../../starlet/src/strdef.h
str_compare.o : str_compare.c ../../starlet/src/strdef.h
str_compare_eql.o : str_compare_eql.c ../../starlet/src/strdef.h
str_concat.o : str_concat.c ../../starlet/src/strdef.h
str_copy_dx.o : str_copy_dx.c ../../starlet/src/strdef.h
str_copy_r.o : str_copy_r.c ../../starlet/src/strdef.h
str_dupl_char.o : str_dupl_char.c ../../starlet/src/strdef.h
str_element.o : str_element.c ../../starlet/src/strdef.h
str_free1_dx.o : str_free1_dx.c ../../starlet/src/strdef.h
str_get1_dx.o : str_get1_dx.c ../../starlet/src/strdef.h
str_left.o : str_left.c ../../starlet/src/strdef.h
str_len_extr.o : str_len_extr.c ../../starlet/src/strdef.h
str_pos_extr.o : str_pos_extr.c ../../starlet/src/strdef.h
str_position.o : str_position.c ../../starlet/src/strdef.h
str_prefix.o : str_prefix.c ../../starlet/src/strdef.h
str_right.o : str_right.c ../../starlet/src/strdef.h
str_find_first_in_set.o : str_find_first_in_set.c ../../starlet/src/strdef.h
str_find_first_not_in_set.o : str_find_first_not_in_set.c ../../starlet/src/strdef.h
str_translate.o : str_translate.c ../../starlet/src/strdef.h
str_trim.o : str_trim.c ../../starlet/src/strdef.h
str_upcase.o : str_upcase.c ../../starlet/src/strdef.h
cvt_convert_float.o: cvt_convert_float.c
libinsqhi.o: libinsqhi.c
libcredir.o: libcredir.c
libgetinp.o: libgetinp.c
libputout.o: libputout.c
libgetef.o: libgetef.c
libffc.o: libffc.c
libffs.o: libffs.c
libinsv.o: libinsv.c
libextv.o: libextv.c
libextzv.o: libextzv.c

libasnmbx.o: libasnmbx.c

