/* <sys$routines.h>
 *
 *	System Service routines from Starlet.
 */
#ifndef _SYS$ROUTINES_H
#define _SYS$ROUTINES_H
# ifdef __cplusplus
extern "C" {
# endif
	




  unsigned long sys$abort_trans(), sys$abort_transw();



  unsigned long sys$ack_event();


  unsigned long sys$add_branch(), sys$add_branchw();


  unsigned long sys$add_holder();


  unsigned long sys$add_ident();


  unsigned long sys$adjstk();


  unsigned long sys$adjwsl();


  unsigned long sys$alloc();


  unsigned long sys$ascefc();


  unsigned long sys$asctim();


  unsigned long sys$asctoid();


  unsigned long sys$ascutc();


  unsigned long sys$assign();


  unsigned long sys$bintim();


  unsigned long sys$binutc();


  unsigned long sys$brdcst();



  unsigned long sys$brkthru(), sys$brkthruw();


  unsigned long sys$cancel();


  unsigned long sys$cancel_selective();


  unsigned long sys$canevtast();


  unsigned long sys$canexh();


  unsigned long sys$canrnh();


  unsigned long sys$cantim();


  unsigned long sys$canwak();



  unsigned long sys$change_acl();


  unsigned long sys$change_class();


  unsigned long sys$check_access();


  unsigned long sys$chkpro();


  unsigned long sys$clref();


  unsigned long sys$cmexec();


  unsigned long sys$cmkrnl();


  unsigned long sys$cntreg();



  unsigned long sys$create_branch(), sys$create_branchw();


  unsigned long sys$create_bufobj();


  unsigned long sys$create_rdb();


  unsigned long sys$create_uid();


  unsigned long sys$crelnm();


  unsigned long sys$crelnt();


  unsigned long sys$crelog();


  unsigned long sys$crembx();



  unsigned long sys$creprc();


  unsigned long sys$cretva();



  unsigned long sys$crmpsc();


  unsigned long sys$dacefc();


  unsigned long sys$dalloc();


  unsigned long sys$dassgn();



  unsigned long sys$declare_rm(), sys$declare_rmw();


  unsigned long sys$dclast();


  unsigned long sys$dclcmh();



  unsigned long sys$dclevt();


  unsigned long sys$dclexh();


  unsigned long sys$dclrnh();



  unsigned long sys$declare_rm(), sys$declare_rmw();


  unsigned long sys$delete_bufobj();


  unsigned long sys$dellnm();


  unsigned long sys$dellog();


  unsigned long sys$delmbx();


  unsigned long sys$delprc();


  unsigned long sys$deltva();


  unsigned long sys$deq();


  unsigned long sys$device_scan();


  unsigned long sys$dgblsc();


  unsigned long sys$diagnose();


  unsigned long sys$disable_vp_use(), sys$disable_vp_use_int();


  unsigned long sys$dismou();


  unsigned long sys$dlcefc();


  unsigned long sys$dns(), sys$dnsw();


  unsigned long sys$emaa();


  unsigned long sys$enable_vp_use(), sys$enable_vp_use_int();


  unsigned long sys$end_branch(), sys$end_branchw();


  unsigned long sys$end_trans(), sys$end_transw();



  unsigned long sys$enq(), sys$enqw();


  unsigned long sys$erapat();


  unsigned long sys$evdpostevent();


  unsigned long sys$exit();


  unsigned long sys$expreg();


  unsigned long sys$fao();


  unsigned long sys$faol();


  unsigned long sys$filescan();


  unsigned long sys$find_held();


  unsigned long sys$find_holder();


  unsigned long sys$finish_rdb();



  unsigned long sys$finish_rmop(), sys$finish_rmopw();


  unsigned long sys$forcex();


  unsigned long sys$forget_rm(), sys$forget_rmw();



  unsigned long sys$format_acl();



  unsigned long sys$format_audit();


  unsigned long sys$format_class();


  unsigned long sys$get_default_trans();


  unsigned long sys$getchn();


  unsigned long sys$getdev();



  unsigned long sys$getdti(), sys$getdtiw();



  unsigned long sys$getdvi(), sys$getdviw();


  unsigned long sys$getevi();


  unsigned long sys$getjpi(), sys$getjpiw();


  unsigned long sys$getlki(), sys$getlkiw();


  unsigned long sys$getmsg();


  unsigned long sys$getpti();


  unsigned long sys$getqui(), sys$getquiw();


  unsigned long sys$getsyi(), sys$getsyiw();


  unsigned long sys$gettim();


  unsigned long sys$getuai();


  unsigned long sys$getutc();


  unsigned long sys$grantid();


  unsigned long sys$grant_license();


  unsigned long sys$hash_password();


  unsigned long sys$hiber();


  unsigned long sys$idtoasc();


  unsigned long sys$imgact();


  unsigned long sys$imgfix();


  unsigned long sys$init_vol();


  unsigned long sys$ipc(), sys$ipcw();



  unsigned long sys$join_rm(), sys$join_rmw();


  unsigned long sys$lckpag();


  unsigned long sys$lkwset();


  unsigned long sys$mgblsc();


  unsigned long sys$mod_holder();


  unsigned long sys$mod_ident();


  unsigned long sys$mount();


  unsigned long sys$mtaccess();


  unsigned long sys$numtim();


  unsigned long sys$numutc();


  unsigned long sys$parse_acl();


  unsigned long sys$parse_class();


  unsigned long sys$posix_fork_control();


  unsigned long sys$process_scan();


  unsigned long sys$purgws();


  unsigned long sys$putmsg();



  unsigned long sys$qio(), sys$qiow();


  unsigned long sys$readef();



  unsigned long sys$recover(), sys$recoverw();


  unsigned long sys$release_license();


  unsigned long sys$release_vp(), sys$release_vp_int();


  unsigned long sys$rem_holder();


  unsigned long sys$rem_ident();


  unsigned long sys$restore_vp_exception(), sys$restore_vp_exc_int();


  unsigned long sys$restore_vp_state();


  unsigned long sys$resume();


  unsigned long sys$revokid();


  unsigned long sys$rmsrundwn(void *,int);	


  unsigned long sys$save_vp_exception(), sys$save_vp_exc_int();


  unsigned long sys$schdwk();


  unsigned long sys$set_default_trans(), sys$set_default_transw();


  unsigned long sys$setast();


  unsigned long sys$setddir(const void *,unsigned short *,void *);


  unsigned long sys$setdfprot(const unsigned short *,unsigned short *);


  unsigned long sys$setdti(), sys$setdtiw();


  unsigned long sys$setef();



  unsigned long sys$setevtast(), sys$setevtastw();


  unsigned long sys$setexv();


  unsigned long sys$setime();


  unsigned long sys$setimr();


  unsigned long sys$setpfm();


  unsigned long sys$setpra();


  unsigned long sys$setpri();


  unsigned long sys$setprn();


  unsigned long sys$setprt();


  unsigned long sys$setprv();


  unsigned long sys$setrwm();


  unsigned long sys$setsfm();


  unsigned long sys$setssf();


  unsigned long sys$setstk();


  unsigned long sys$setswm();


  unsigned long sys$setuai();


  unsigned long sys$sndacc();


  unsigned long sys$snderr();


  unsigned long sys$sndjbc(), sys$sndjbcw();


  unsigned long sys$sndopr();


  unsigned long sys$sndsmb();



  unsigned long sys$start_branch(), sys$start_branchw();



  unsigned long sys$start_trans(), sys$start_transw();


  unsigned long sys$subsystem();


  unsigned long sys$suspnd();


  unsigned long sys$synch();


  unsigned long sys$timcon();



  unsigned long sys$trans_event(), sys$trans_eventw();


  unsigned long sys$trnlnm();


  unsigned long sys$trnlog();


  unsigned long sys$ulkpag();


  unsigned long sys$ulwset();


  unsigned long sys$unwind();


  unsigned long sys$updsec(), sys$updsecw();


  unsigned long sys$waitfr();


  unsigned long sys$wake();


  unsigned long sys$wfland();


  unsigned long sys$wflor();

# ifdef __cplusplus
}
# endif

//	Utility Functions

int sys$$geterrno(char *errmsg);

int sys$$getmsg (unsigned int msgid, unsigned short int *msglen, char *fmt,...);

#endif	
