/* <lib$routines.h>
 *
 *  General run-time library routines.
 */
#ifndef _LIB$ROUTINES_H
#define _LIB$ROUTINES_H

#include <descrip.h>

int lib$adawi(const short *, short *, short *);

int lib$add_times(const void *, const void *, void *);

int lib$addx(const void *, const void *, void *, const unsigned int *);

int lib$analyze_sdesc(const struct dsc$descriptor_s *, unsigned short *, char **);

int lib$analyze_sdesc_64(const struct dsc$descriptor_s* descrip, unsigned short *word_integer_length, char **data_address,
        unsigned long *descriptor_type);

int lib$ascii_to_uid(void);

int lib$asn_wth_mbx(const void *, const long *, const long *, unsigned short *, unsigned short *);

int lib$ast_in_prog(void);

int lib$attach(const unsigned long *);

int lib$bbcci(const int *, void *), lib$bbssi(const int *, void *);

int lib$build_nodespec(const struct dsc$descriptor_s *primary, struct dsc$descriptor_s *nodespec,
        const struct dsc$descriptor_s *acs, const struct dsc$descriptor_s *secondary, unsigned short *nodespec_length);

int lib$callg(const void *, const unsigned long (*)(void));

#if 0
// not now
int lib$char(void *,const char *);
#else
int lib$char(struct dsc$descriptor_s*, const char*);
#endif

int lib$compare_uid(void);

#if 0
int lib$convert_date_string(const void *,void *,...);
// not now?
#else
int lib$convert_date_string(const struct dsc$descriptor_s *, long long *, unsigned long *, unsigned long *, unsigned short *,
        unsigned long *);
#endif

int lib$crc(const void *, const long *, const void *);

void lib$crc_table(const unsigned long *, void *);

int lib$create_dir(const void *);

int lib$create_dir2(char *crea_file, int all_size);

int lib$create_user_vm_zone(unsigned long *, ...);

int lib$create_vm_zone(unsigned long *, ...);

void lib$crf_ins_key(const void *, const void *, const long *, const unsigned long *);

void lib$crf_ins_ref(const void *, const long *, const void *, long *, const long *);

void lib$crf_output(const void *, const long *, const long *, const long *, const long *, const long *);

int lib$currency(void *, ...);

int lib$cvt_dtb(int, const char *, long *), lib$cvt_htb(int, const char *, long *), lib$cvt_otb(int, const char *, long *);

int lib$cvt_dx_dx(const void *, void *, ...);

int lib$cvt_from_internal_time(const unsigned long *, long *, ...);

int lib$cvtf_from_internal_time(const unsigned long *, float *, const void *);

int lib$cvt_to_internal_time(const unsigned long *, const long *, void *);

int lib$cvtf_to_internal_time(const unsigned long *, const float *, void *);

int lib$cvt_vectim(const void *, void *);

#if 0
// not now
int lib$date_time(void *);
#else
int lib$date_time(struct dsc$descriptor_s *);
#endif

int lib$day(long *, const void*, int*);

int lib$day_of_week(const void *, int *);

int lib$dec_over(const unsigned long *);

int lib$decode_fault(const void *, const void *, ...);

int lib$delete_file(const void *, ...);

int lib$delete_logical(const void *, ...);

int lib$delete_symbol(const void *, ...);

int lib$delete_vm_zone(const unsigned long *);

#if 0
// not now
int lib$digit_sep(void *,...);
#else
int lib$digit_sep(struct dsc$descriptor_s *, unsigned short *);
#endif

int lib$disable_ctrl(const unsigned long *, ...);

int lib$do_command(const void *);

int lib$ediv(const signed long int *divisor, const signed long long *dividend, signed long int *quotient,
        signed long int *remainder);

int lib$emodd(const double *, const unsigned char *, const double *, long *, double *), lib$emodf(const float *,
        const unsigned char *, const float *, long *, float *), lib$emodg(const double *, const unsigned short *, const double *,
        long *, double *), lib$emodh(const void *, const unsigned short *, const void *, long *, void *);

int lib$emul(const long *, const long *, const long *, void *);

int lib$enable_ctrl(const unsigned long *, ...);

unsigned long (*lib$establish(const unsigned long (*)(void *, void *)))(void *,void *);

long lib$extv(const int *, const unsigned char *, const void *);

long lib$extzv(const int *, const unsigned char *, const void *);

int lib$ffc(const int *, const int *, const void *, int *), lib$ffs(const int *, const int *, const void *, int *);

int lib$fid_to_name(const void *, const void *, void *, ...);

struct FAB;
int lib$file_scan(const struct FAB *, const unsigned long (*)(struct FAB *), const unsigned long (*)(struct FAB *),...);

int lib$file_scan_end(void);

int lib$find_file(const void *, void *, unsigned long *, ...);

int lib$find_file_end(const unsigned long *);

int lib$find_image_symbol(const void *, const void *, void *, ...);

int lib$find_vm_zone(unsigned long *, unsigned long *);

int lib$fixup_flt(const void *, const void *, ...);

int lib$flt_under(const unsigned long *);

int lib$format_date_time(void *, ...);

int lib$free_date_time_context(void);

int lib$free_ef(const unsigned int *);

int lib$free_lun(const long *);

int lib$free_timer(long * context);

int lib$free_vm(const long *, const void *, ...);

int lib$free_vm_page(const long *, const void *);

int lib$get_command(void *, ...);

int lib$get_common(void *, ...);

int lib$get_date_format(void *, ...);

int lib$get_ef(unsigned int *);

int lib$get_foreign(void *, ...);

int lib$get_input(void *, ...);

int lib$get_lun(long *);

int lib$get_maximum_date_length(long *, ...);

int lib$get_symbol(const void *, void *, ...);

int lib$get_users_language(void *);

int lib$get_vm(const long *, void *, ...);

int lib$get_vm_page(const long *, void *);

// not now: int lib$getdvi(const long *,const unsigned short *,const void *,long *,...);
int lib$getdvi(signed int * item_code, unsigned short int * channel, void * device_name, signed int * longword_integer_value,
        void * resultant_string, unsigned short * resultant_length);

// not now: int lib$getjpi(const long *,const unsigned long *,const void *,long *,...);
int lib$getjpi(signed int * item_code, unsigned int * process_id, void * process_name, signed int * longword_integer_value,
        void * resultant_string, unsigned short * resultant_length);

int lib$getqui(const long *, const long *, const long *, const void *, const unsigned long *, long *, ...);

// not now: intg lib$getsyi(const long *,void *,...);
int lib$getsyi(signed int * item_code, void * resultant_value, void * resultant_string, unsigned short * resultant_length,
        unsigned int cluster_system_id, void * node_name);

unsigned int lib$ichar(const struct dsc$descriptor_s *);

unsigned int lib$index(const struct dsc$descriptor_s *, const struct dsc$descriptor_s *);

int lib$init_date_time_context(unsigned long *, const long *, const void *);

int lib$init_timer(long * context);

int lib$insert_tree(void *, void *, const unsigned long *, const int (*)(void *, void *, void *),
        const void (*)(void *, void *, void *),...);

int lib$insqhi(void *, void *);

int lib$insqti(void *, void *);

void lib$insv(const int *, const int *, const unsigned char *, void *);

int lib$int_over(const unsigned long *);

unsigned short lib$len(const struct dsc$descriptor_s *);

unsigned int lib$locc(const struct dsc$descriptor_s *char_string, const struct dsc$descriptor_s *source_string);

int lib$lookup_key(const void *, const void *, void *, ...);

int lib$lookup_tree(const void *, void *, const int (*)(void *, void *), void *);

int lib$lp_lines(void);

unsigned int lib$matchc(const struct dsc$descriptor_s *sub_string, const struct dsc$descriptor_s *search_string);

int lib$match_cond(const unsigned long *, const unsigned long *, ...);

void lib$movc3(const unsigned short *, const void *, void *);

void lib$movc5(const unsigned short *, const void *, const char *, const unsigned short *, void *);

int lib$movtc(const void *, const void *, const void *, void *);

int lib$movtuc(const void *, const void *, const void *, void *, ...);

int lib$mult_delta_time(const long *, void *);

int lib$multf_delta_time(const float *, void *);

int lib$pause(void);

int lib$polyd(const double *, const short *, const double *, double *), lib$polyf(const float *, const short *, const float *,
        float *), lib$polyg(const double *, const short *, const double *, double *), lib$polyh(const void *, const short *,
        const void *, void *);

int lib$put_common(const void *, ...);

int lib$put_output(const void *);

int lib$radix_point(void *, ...);

int lib$remqhi(void *, void *, ...);

int lib$remqti(void *, void *, ...);

int lib$rename_file(const void *, const void *, ...);

int lib$reserve_ef(const unsigned int *);

int lib$reset_vm_zone(const unsigned long *);

int (*lib$revert(void))(void *,void *);

int lib$run_program(const void *);

int lib$scanc(const void *, const unsigned char *, const unsigned char *);

int lib$scopy_dxdx(const void *, void *);

int lib$scopy_r_dx(const unsigned short *, const void *, void *);

int lib$set_logical(const void *, const void *, ...);

int lib$set_symbol(const void *, const void *, ...);

int lib$sfree1_dd(void *);

int lib$sfreen_dd(const unsigned *, void *);

int lib$sget1_dd(const unsigned short *, void *);

int lib$show_timer(void * handle_address, int * code, int (*user_action_procedure)(void), unsigned long user_argument_value);

int lib$show_vm(int * code, int (*user_action_procedure)(void), long * user_specified_argument);

int lib$show_vm_zone(const unsigned long *, ...);

void lib$signal(unsigned long, ...);

int lib$sig_to_ret(void *, void *);

int lib$sig_to_stop(void *, void *);

int lib$sim_trap(void *, void *);

int lib$skpc(const void *, const void *);

int lib$spanc(const void *, const unsigned char *, const unsigned char *);

int lib$spawn(void);

// not now: int lib$stat_timer(const int *,void *,...);
int lib$stat_timer(int * code, long * value_argument, void * handle_address);

int lib$stat_vm(const int *, void *);

int lib$stat_vm_zone(const unsigned long *, const int *, void *);

void lib$stop(unsigned long, ...);

int lib$sub_times(const void *, const void *, void *);

int lib$subx(const void *, const void *, void*, const unsigned int *);

int lib$sys_asctim(unsigned short *, void *, ...);

int lib$sys_fao(const void *, unsigned short *, void *, ...);

int lib$sys_faol(const void *, unsigned short *, void *, const void *);

int lib$sys_getmsg(const unsigned long *, unsigned short *, void *, ...);

int lib$sys_trnlog(const void *, unsigned short *, void *, ...);

int lib$tparse(void *, const void *, const void *);

int lib$tra_asc_ebc(const void *, void *);

int lib$tra_ebc_asc(const void *, void *);

int lib$traverse_tree(const void *, const unsigned long (*)(void *, void *),...);

int lib$trim_filespec(const void *, void *);

int lib$uid_to_ascii(void);

int lib$verify_vm_zone(const unsigned long *);

int lib$wait(float);

int lib$$ncompare(char *s1, char *s2);

int lib$compare_nodename(const struct dsc$descriptor_s *nodename1, const struct dsc$descriptor_s *nodename2, unsigned long *result);

int lib$expand_nodename(const struct dsc$descriptor_s *name, struct dsc$descriptor_s *xname, unsigned short *xname_len);

#endif /* _LIB$ROUTINES_H */
