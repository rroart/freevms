/* <libdef.h>
 *
 *	Message/status codes for LIBRTL
 */
#ifndef _LIBDEF_H
#define _LIBDEF_H

/*
   Symbols are defined following the standard for global names:
	LIB$_ABCMNOXYZ
   In addition, the library standards specify that the letters "ABC", "MNO",
   and "XYZ" are the first three letters of the first three words of the error
   message, not counting articles and prepositions.
 */
#define LIB$_FACILITY	21	/* 0x0015 */

#define LIB$_NORMAL		0x00158001	/* 1409025 */
#define LIB$_STRTRU		0x00158011	/* 1409041 */
#define LIB$_ONEENTQUE		0x00158019	/* 1409049 lib$insqhi, lib$insqti, lib$remqhi, lib$remqti */
#define LIB$_KEYALRINS		0x00158021	/* 1409057 lib$insert_tree */
#define LIB$_ERRROUCAL		0x00158029	/* 1409065 lib$delete_file, lib$rename_file */
#define LIB$_RESTART		0x00158031	/* 1409073 lib$decode_fault */
#define LIB$_BADSTA		0x00158204	/* 1409540 lib$fixup_flt */
#define LIB$_FATERRLIB		0x0015820C	/* 1409548 lib$vm_get */
#define LIB$_INSVIRMEM		0x00158214	/* 1409556 lib$vm_get, lib$lookup_key */
#define LIB$_INPSTRTRU		0x0015821C	/* 1409564 lib$get_input */
#define LIB$_INVSTRDES		0x00158224	/* 1409572 lib$scopy_xxxx */
#define LIB$_INTLOGERR		0x0015822C	/* 1409580 */
#define LIB$_INVARG		0x00158234	/* 1409588 lib$lookup_key */
#define LIB$_AMBKEY		0x0015823C	/* 1409596 lib$lookup_key */
#define LIB$_UNRKEY		0x00158244	/* 1409604 */
#define LIB$_PUSSTAOVE		0x0015824C	/* 1409612 lib$fixup_flt */
#define LIB$_USEFLORES		0x00158254	/* 1409620 lib$lookup_key */
#define LIB$_SIGNO_ARG		0x0015825C	/* 1409628 lib$signal, lib$stop */
#define LIB$_BADBLOADR		0x00158264	/* 1409636 lib$free_vm, lib$vm_free[_page] */
#define LIB$_BADBLOSIZ		0x0015826C	/* 1409644 lib$get_vm */
#define LIB$_NOTFOU		0x00158274	/* 1409652 lib$ffs, lib$ffc */
#define LIB$_ATTCONSTO		0x0015827C	/* 1409660 lib$signal, lib$stop */
#define LIB$_SYNTAXERR		0x00158284	/* 1409668 */
#define LIB$_INVTYPE		0x0015828C	/* 1409676 lib$tparse */
#define LIB$_INSEF		0x00158294	/* 1409684 lib$get_ef */
#define LIB$_EF_ALRFRE		0x0015829C	/* 1409692 lib$free_ef */
#define LIB$_EF_ALRRES		0x001582A4	/* 1409700 lib$reserve_ef */
#define LIB$_EF_RESSYS		0x001582AC	/* 1409708 lib$free_ef, lib$reserve_ef */
#define LIB$_INSLUN		0x001582B4	/* 1409716 lib$get_lun */
#define LIB$_LUNALRFRE		0x001582BC	/* 1409724 lib$free_lun */
#define LIB$_LUNRESSYS		0x001582C4	/* 1409732 lib$free_lun, lib$reserve_lun */
#define LIB$_SCRBUFOVF		0x001582CC	/* 1409740 scr package */
#define LIB$_INVSCRPOS		0x001582D4	/* 1409748 scr package */
#define LIB$_SECINTFAI		0x001582DC	/* 1409756 lib$insqhi, lib$insqti, lib$remqhi, lib$remqti */
#define LIB$_INVCHA		0x001582E4	/* 1409764 lib$tra_asc_ebc, lib$tra_ebc_asc */
#define LIB$_QUEWASEMP		0x001582EC	/* 1409772 lib$remqhi, lib$remqti */
#define LIB$_STRIS_INT		0x001582F4	/* 1409780 lib$scopy, etc. */
#define LIB$_KEYNOTFOU		0x001582FC	/* 1409788 lib$lookup_tree */
#define LIB$_INVCVT		0x00158304	/* 1409796 lib$cvtdxdx */
#define LIB$_INVDTYDSC		0x0015830C	/* 1409804 lib$cvtdxdx */
#define LIB$_INVCLADSC		0x00158314	/* 1409812 lib$cvtdxdx */
#define LIB$_INVCLADTY		0x0015831C	/* 1409820 lib$cvtdxdx */
#define LIB$_INVNBDS		0x00158324	/* 1409828 lib$cvtdxdx */
#define LIB$_DESSTROVF		0x0015832C	/* 1409836 lib$cvtdxdx */
#define LIB$_INTOVF		0x00158334	/* 1409844 lib$cvtdxdx */
#define LIB$_DECOVF		0x0015833C	/* 1409852 lib$cvtdxdx */
#define LIB$_FLTOVF		0x00158344	/* 1409860 lib$cvtdxdx */
#define LIB$_FLTUND		0x0015834C	/* 1409868 lib$cvtdxdx */
#define LIB$_ROPRAND		0x00158354	/* 1409876 lib$cvtdxdx */
#define LIB$_WRONUMARG		0x0015835C	/* 1409884 lib$analyze_sdesc */
#define LIB$_NOSUCHSYM		0x00158364	/* 1409892 lib$cli_callback */
#define LIB$_INSCLIMEM		0x0015836C	/* 1409900 lib$cli_callback */
#define LIB$_AMBSYMDEF		0x00158374	/* 1409908 lib$cli_callback */
#define LIB$_NOCLI		0x0015837C	/* 1409916 lib$cli_callback */
#define LIB$_UNECLIERR		0x00158384	/* 1409924 lib$cli_callback */
#define LIB$_INVSYMNAM		0x0015838C	/* 1409932 lib$cli_callback */
#define LIB$_ATTREQREF		0x00158394	/* 1409940 lib$attach */
#define LIB$_INVFILSPE		0x0015839C	/* 1409948 lib$fid_to_name, lib$extract_concealed, */
					/* lib$set_file_prot, lib$unlock_file, lib$create_dir */
#define LIB$_INVXAB		0x001583A4	/* 1409956 lib$qual_file_parse */
#define LIB$_NO_STRACT		0x001583AC	/* 1409964 scr package */
#define LIB$_BADTAGVAL		0x001583B4	/* 1409972 lib$vm */
#define LIB$_INVOPEZON		0x001583BC	/* 1409980 lib$vm */
#define LIB$_PAGLIMEXC		0x001583C4	/* 1409988 lib$vm */
#define LIB$_NOTIMP		0x001583CC	/* 1409996 lib$not_implemented */
#define LIB$_BADZONE		0x001583D4	/* 1410004 lib$verify_vm_zone, lib$show_vm_zone, lib$stat_vm_zone, lib$find_vm_zone */
#define LIB$_IVTIME		0x001583DC	/* 1410012 lib$add_times, lib$sub_times, lib$muliply_delta_time */
					/* lib$multiply_delta_time_f, lib$cvt_to_internal_time */
					/* lib$cvt_from_internal_time, lib$convert_date_string */
#define LIB$_ONEDELTIM		0x001583E4	/* 1410020 lib$add_times */
#define LIB$_NEGTIM		0x001583EC	/* 1410028 lib$sub_times */
#define LIB$_INVARGORD		0x001583F4	/* 1410036 lib$sub_times */
#define LIB$_ABSTIMREQ		0x001583FC	/* 1410044 lib$cvt_from_internal_time */
#define LIB$_DELTIMREQ		0x00158404	/* 1410052 lib$cvt_from_internal_time */
#define LIB$_INVOPER		0x0015840C	/* 1410060 lib$cvt_xxx_internal_time */
#define LIB$_REENTRANCY		0x00158414	/* 1410068 lib$format_date_time, lib$init_date_time_context, */
					/* lib$convert_date_string, lib$get_date_format */
#define LIB$_UNRFORCOD		0x0015841C	/* 1410076 lib$format_date_time, lib$convert_date_string, lib$get_date_format */
#define LIB$_ILLINISTR		0x00158424	/* 1410084 lib$init_date_time_context */
#define LIB$_NUMELEMENTS	0x0015842C	/* 1410092 lib$init_date_time_context */
#define LIB$_ELETOOLON		0x00158434	/* 1410100 lib$init_date_time_context */
#define LIB$_ILLCOMPONENT	0x0015843C	/* 1410108 lib$init_date_time_context */
#define LIB$_AMBDATTIM		0x00158444	/* 1410116 lib$convert_date_string */
#define LIB$_INCDATTIM		0x0015844C	/* 1410124 lib$convert_date_string */
#define LIB$_ILLFORMAT		0x00158454	/* 1410132 lib$convert_date_string, lib$get_date_format */
#define LIB$_UNSUPVER		0x0015845C	/* 1410140 lib$ascii_to_uid, lib$compare_uid, lib$uid_to_ascii */
#define LIB$_BADFORMAT		0x00158464	/* 1410148 lib$ascii_to_uid */
#define LIB$_OUTSTRTRU		0x00159000	/* 1413120 lib$cvtdxdx */
#define LIB$_FILFAIMAT		0x00159008	/* 1413128 lib$qual_file_match */
#define LIB$_NEGANS		0x00159010	/* 1413136 lib$confirm_act */
#define LIB$_QUIPRO		0x00159018	/* 1413144 lib$confirm_act, lib$qual_file_match */
#define LIB$_QUICONACT		0x00159020	/* 1413152 lib$confirm_act, lib$qual_file_match */
#define LIB$_ENGLUSED		0x00159028	/* 1413160 lib$format_date_time, lib$convert_date_string, lib$get_date_format */
#define LIB$_DEFFORUSE		0x00159030	/* 1413168 lib$format_date_time, lib$convert_date_string, lib$get_date_format */
#define LIB$_MEMALRFRE		0x00159038	/* 1413176 lib$vm_free */
#define LIB$_BADCCC		0x0015C000	/* 1425408 lib$$read_object */
#define LIB$_EOMERROR		0x0015C008	/* 1425416 lib$$read_object */
#define LIB$_EOMFATAL		0x0015C010	/* 1425424 lib$$read_object */
#define LIB$_EOMWARN		0x0015C018	/* 1425432 lib$$read_object */
#define LIB$_GSDTYP		0x0015C020	/* 1425440 lib$$read_object */
#define LIB$_ILLFMLCNT		0x0015C028	/* 1425448 lib$$read_object */
#define LIB$_ILLMODNAM		0x0015C030	/* 1425456 lib$$read_object */
#define LIB$_ILLPSCLEN		0x0015C038	/* 1425464 lib$$read_object */
#define LIB$_ILLRECLEN		0x0015C040	/* 1425472 lib$$read_object */
#define LIB$_ILLRECLN2		0x0015C048	/* 1425480 lib$$read_object */
#define LIB$_ILLRECTYP		0x0015C050	/* 1425488 lib$$read_object */
#define LIB$_ILLRECTY2		0x0015C058	/* 1425496 lib$$read_object */
#define LIB$_ILLSYMLEN		0x0015C060	/* 1425504 lib$$read_object */
#define LIB$_NOEOM		0x0015C068	/* 1425512 lib$$read_object */
#define LIB$_RECTOOSML		0x0015C070	/* 1425520 lib$$read_object */
#define LIB$_SEQUENCE		0x0015C078	/* 1425528 lib$$read_object */
#define LIB$_SEQUENCE2		0x0015C080	/* 1425536 lib$$read_object */
#define LIB$_STRLVL		0x0015C088	/* 1425544 lib$$read_object */
#define LIB$_NOWILD		0x0015112A	/* 1380650 */
#define LIB$_ACTIMAGE		0x001512BA	/* 1381050 */
#define LIB$_OPENIN		0x0015109A	/* 1380506 */
#define LIB$_CLOSEIN		0x00151050	/* 1380432 */
#define LIB$_READERR		0x001510B2	/* 1380530 */

#endif	/*_LIBDEF_H*/
