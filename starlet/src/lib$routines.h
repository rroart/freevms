/* <lib$routines.h>
 *
 *	General run-time library routines.
 */
#ifndef _LIB$ROUTINES_H
#define _LIB$ROUTINES_H

#include <descrip.h>


unsigned long lib$adawi(const short *,short *,short *);

unsigned long lib$add_times(const void *,const void *,void *);

unsigned long lib$addx(const void *,const void *,void *, const long *);

unsigned long lib$analyze_sdesc(const struct dsc$descriptor_s *,
	unsigned short *,char **);

unsigned long lib$analyze_sdesc_64 (const struct dsc$descriptor_s* descrip,
                                unsigned short  *word_integer_length,
                                char            **data_address,
                                unsigned long   *descriptor_type );

unsigned long lib$ascii_to_uid();

unsigned long lib$asn_wth_mbx(const void *,const long *,const long *,
			      unsigned short *,unsigned short *);

int lib$ast_in_prog(void);		

unsigned long lib$attach(const unsigned long *);

int lib$bbcci(const int *,void *), lib$bbssi(const int *,void *);  

unsigned long lib$build_nodespec(const  struct  dsc$descriptor_s *primary,
                                        struct  dsc$descriptor_s *nodespec,
                                 const  struct  dsc$descriptor_s *acs,
                                 const  struct  dsc$descriptor_s *secondary,
                                 unsigned short *nodespec_length );

unsigned long lib$callg(const void *,const unsigned long (*)()); 

#if 0
// not now
unsigned long lib$char(void *,const char *);
#else
unsigned long lib$char(struct dsc$descriptor_s*, const char*);
#endif

unsigned long lib$compare_uid();

#if 0
unsigned long lib$convert_date_string(const void *,void *,...);
// not now?
#else
unsigned long lib$convert_date_string(const struct dsc$descriptor_s *,long long *,unsigned long *,unsigned long *, unsigned short *,unsigned long *);
#endif

unsigned long lib$crc(const void *,const long *,const void *);

void lib$crc_table(const unsigned long *,void *);

unsigned long lib$create_dir(const void *,...);

unsigned long lib$create_user_vm_zone(unsigned long *,...);

unsigned long lib$create_vm_zone(unsigned long *,...);

void lib$crf_ins_key(const void *,const void *,const long *,const unsigned long *);

void lib$crf_ins_ref(const void *,const long *,const void *,long *,const long *);

void lib$crf_output(const void *,const long *,const long *,const long *,const long *,const long *);

unsigned long lib$currency(void *,...);

unsigned long lib$cvt_dtb(int,const char *,long *),
	      lib$cvt_htb(int,const char *,long *),
	      lib$cvt_otb(int,const char *,long *);

unsigned long lib$cvt_dx_dx(const void *,void *,...);

unsigned long lib$cvt_from_internal_time(const unsigned long *,long *,...);

unsigned long lib$cvtf_from_internal_time(const unsigned long *,float *,const void *);

unsigned long lib$cvt_to_internal_time(const unsigned long *,const long *,void *);

unsigned long lib$cvtf_to_internal_time(const unsigned long *,const float *,void *);

unsigned long lib$cvt_vectim(const void *,void *);

#if 0
// not now
unsigned long lib$date_time(void *);
#else
unsigned long lib$date_time(struct dsc$descriptor_s *);
#endif

unsigned long lib$day(long *, const void*, int*);

unsigned long lib$day_of_week(const void *,int *);

unsigned long lib$dec_over(const unsigned long *);

unsigned long lib$decode_fault(const void *,const void *,...);

unsigned long lib$delete_file(const void *,...);

unsigned long lib$delete_logical(const void *,...);

unsigned long lib$delete_symbol(const void *,...);

unsigned long lib$delete_vm_zone(const unsigned long *);

#if 0
// not now
unsigned long lib$digit_sep(void *,...);
#else
unsigned long lib$digit_sep(struct dsc$descriptor_s *,unsigned short *);
#endif

unsigned long lib$disable_ctrl(const unsigned long *,...);

unsigned long lib$do_command(const void *);

unsigned long lib$ediv( const   signed long int *divisor,
                        const   signed long long *dividend,
                                signed long int *quotient,
                                signed long int *remainder );

unsigned long lib$emodd(const double *,const unsigned char *,const double *,long *,double *),
	      lib$emodf(const float *,const unsigned char *,const float *,long *,float *),
	      lib$emodg(const double *,const unsigned short *,const double *,long *,double *),
	      lib$emodh(const void *,const unsigned short *,const void *,long *,void *);

unsigned long lib$emul(const long *,const long *,const long *,void *);

unsigned long lib$enable_ctrl(const unsigned long *,...);

unsigned long (*lib$establish(const unsigned long (*)(void *,void *)))(void *,void *);

long lib$extv(const int *,const unsigned char *,const void *);

long lib$extzv(const int *,const unsigned char *,const void *);

unsigned long lib$ffc(const int *,const int *,const void *,int *),
	      lib$ffs(const int *,const int *,const void *,int *);

unsigned long lib$fid_to_name(const void *,const void *,void *,...);

struct FAB;	
unsigned long lib$file_scan(const struct FAB *,const unsigned long (*)(struct FAB *),
			    const unsigned long (*)(struct FAB *),...);

unsigned long lib$file_scan_end();

unsigned long lib$find_file(const void *,void *,unsigned long *,...);

unsigned long lib$find_file_end(const unsigned long *);

unsigned long lib$find_image_symbol(const void *,const void *,void *,...);

unsigned long lib$find_vm_zone(unsigned long *,unsigned long *);

unsigned long lib$fixup_flt(const void *,const void *,...);

unsigned long lib$flt_under(const unsigned long *);

unsigned long lib$format_date_time(void *,...);

unsigned long lib$free_date_time_context();

unsigned int lib$free_ef(const unsigned int *);

unsigned long lib$free_lun(const long *);

unsigned long lib$free_timer(void *);

unsigned long lib$free_vm(const long *,const void *,...);

unsigned long lib$free_vm_page(const long *,const void *);

unsigned long lib$get_command(void *,...);

unsigned long lib$get_common(void *,...);

unsigned long lib$get_date_format(void *,...);

unsigned int lib$get_ef(unsigned int *);

unsigned long lib$get_foreign(void *,...);

unsigned long lib$get_input(void *,...);

unsigned long lib$get_lun(long *);

unsigned long lib$get_maximum_date_length(long *,...);

unsigned long lib$get_symbol(const void *,void *,...);

unsigned long lib$get_users_language(void *);

unsigned long lib$get_vm(const long *,void *,...);

unsigned long lib$get_vm_page(const long *,void *);

unsigned long lib$getdvi(const long *,const unsigned short *,const void *,long *,...);

unsigned long lib$getjpi(const long *,const unsigned long *,const void *,long *,...);

unsigned long lib$getqui(const long *,const long *,const long *,const void *,
			 const unsigned long *,long *,...);

unsigned long lib$getsyi(const long *,void *,...);

int lib$ichar(const struct dsc$descriptor_s *);

unsigned long lib$index(const struct dsc$descriptor_s *,
	const struct dsc$descriptor_s *);

unsigned long lib$init_date_time_context(unsigned long *,const long *,const void *);

unsigned long lib$init_timer();

long int lib$insert_tree(void *,void *,const unsigned long *,
			 const int (*)(void *,void *,void *),
			 const void (*)(void *,void *,void *),...);

unsigned long lib$insqhi(void *,void *,...);

unsigned long lib$insqti(void *,void *,...);

void lib$insv(const int *,const int *,const unsigned char *,void *);

unsigned long lib$int_over(const unsigned long *);

unsigned short lib$len(const struct dsc$descriptor_s *);

unsigned long lib$locc (const struct dsc$descriptor_s *char_string,
			const struct dsc$descriptor_s *source_string);

unsigned long lib$lookup_key(const void *,const void *,void *,...);

unsigned long lib$lookup_tree(const void *,void *,const int (*)(void *,void *),void *);

long lib$lp_lines(void);

unsigned long lib$matchc ( const struct dsc$descriptor_s *sub_string,
			   const struct dsc$descriptor_s *search_string);

int lib$match_cond(const unsigned long *,const unsigned long *,...);

void lib$movc3(const unsigned short *,const void *,void *);

void lib$movc5(const unsigned short *,const void *,const char *,
	       const unsigned short *,void *);

unsigned long lib$movtc(const void *,const void *,const void *,void *);

int lib$movtuc(const void *,const void *,const void *,void *,...);

unsigned long lib$mult_delta_time(const long *,void *);

unsigned long lib$multf_delta_time(const float *,void *);

unsigned long lib$pause(void);

unsigned long lib$polyd(const double *,const short *,const double *,double *),
	      lib$polyf(const float *,const short *,const float *,float *),
	      lib$polyg(const double *,const short *,const double *,double *),
	      lib$polyh(const void *,const short *,const void *,void *);

unsigned long lib$put_common(const void *,...);

unsigned long lib$put_output(const void *);

unsigned long lib$radix_point(void *,...);

unsigned long lib$remqhi(void *,void *,...);

unsigned long lib$remqti(void *,void *,...);

unsigned long lib$rename_file(const void *,const void *,...);

unsigned int lib$reserve_ef(const unsigned int *);

unsigned long lib$reset_vm_zone(const unsigned long *);

unsigned long (*lib$revert(void))(void *,void *);

unsigned long lib$run_program(const void *);

int lib$scanc(const void *,const unsigned char *,const unsigned char *);

unsigned long lib$scopy_dxdx(const void *,void *);

unsigned long lib$scopy_r_dx(const unsigned short *,const void *,void *);

unsigned long lib$set_logical(const void *,const void *,...);

unsigned long lib$set_symbol(const void *,const void *,...);

unsigned long lib$sfree1_dd(void *);

unsigned long lib$sfreen_dd(const unsigned *,void *);

unsigned long lib$sget1_dd(const unsigned short *,void *);

unsigned long lib$show_timer();

unsigned long lib$show_vm();

unsigned long lib$show_vm_zone(const unsigned long *,...);

void lib$signal(unsigned long,...);

unsigned long lib$sig_to_ret(void *,void *);

unsigned long lib$sig_to_stop(void *,void *);

unsigned long lib$sim_trap(void *,void *);

int lib$skpc(const void *,const void *);

int lib$spanc(const void *,const unsigned char *,const unsigned char *);

unsigned long lib$spawn();

unsigned long lib$stat_timer(const int *,void *,...);

unsigned long lib$stat_vm(const int *,void *);

unsigned long lib$stat_vm_zone(const unsigned long *,const int *,void *);

void lib$stop(unsigned long,...);

unsigned long lib$sub_times(const void *,const void *,void *);

unsigned long lib$subx(const void *,const void *,void*, const long *);

unsigned long lib$sys_asctim(unsigned short *,void *,...);

unsigned long lib$sys_fao(const void *,unsigned short *,void *,...);

unsigned long lib$sys_faol(const void *,unsigned short *,void *,const void *);

unsigned long lib$sys_getmsg(const unsigned long *,unsigned short *,void *,...);

unsigned long lib$sys_trnlog(const void *,unsigned short *,void *,...);

unsigned long lib$tparse(void *,const void *,const void *);

unsigned long lib$tra_asc_ebc(const void *,void *);

unsigned long lib$tra_ebc_asc(const void *,void *);

unsigned long lib$traverse_tree(const void *,const unsigned long (*)(void *,void *),...);

unsigned long lib$trim_filespec(const void *,void *);

unsigned long lib$uid_to_ascii();

unsigned long lib$verify_vm_zone(const unsigned long *);

unsigned long lib$wait(float);

#endif	

int lib$$ncompare (char *s1,char *s2);

