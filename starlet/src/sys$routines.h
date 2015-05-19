/* <sys$routines.h>
 *
 *  System Service routines from Starlet.
 */
#ifndef _SYS$ROUTINES_H
#define _SYS$ROUTINES_H

#include "iosbdef.h"
#include "lksbdef.h"

# ifdef __cplusplus
extern "C"
{
# endif

    unsigned long sys$abort_trans(void);
    unsigned long sys$abort_transw(void);

    unsigned long sys$ack_event(void);

    unsigned long sys$add_branch(void);
    unsigned long sys$add_branchw(void);

    unsigned long sys$add_holder(void);

    unsigned long sys$add_ident(void);

    unsigned long sys$adjstk(void);

    unsigned long sys$adjwsl(void);

    unsigned long sys$alloc(void);

    unsigned long sys$ascefc(void);

    unsigned long sys$asctim(void);

    unsigned long sys$asctoid(void);

    unsigned long sys$ascutc(void);

    unsigned long sys$assign(void);

    unsigned long sys$bintim(void);

    unsigned long sys$binutc(void);

    unsigned long sys$brdcst(void);

    unsigned long sys$brkthru(void);
    unsigned long sys$brkthruw(void);

    unsigned long sys$cancel(void);

    unsigned long sys$cancel_selective(void);

    unsigned long sys$canevtast(void);

    unsigned long sys$canexh(void);

    unsigned long sys$canrnh(void);

    unsigned long sys$cantim(void);

    unsigned long sys$canwak(void);

    unsigned long sys$change_acl(void);

    unsigned long sys$change_class(void);

    unsigned long sys$check_access(void);

    unsigned long sys$chkpro(void);

    unsigned long sys$clref(void);

    unsigned long sys$cmexec(void);

    unsigned long sys$cmkrnl(void);

    unsigned long sys$cntreg(void);

    unsigned long sys$create_branch(void);
    unsigned long sys$create_branchw(void);

    unsigned long sys$create_bufobj(void);

    unsigned long sys$create_rdb(void);

    unsigned long sys$create_uid(void);

    unsigned long sys$crelnm(void);

    unsigned long sys$crelnt(void);

    unsigned long sys$crelog(void);

    unsigned long sys$crembx(void);

    unsigned long sys$creprc(void);

    unsigned long sys$cretva(void);

    unsigned long sys$crmpsc(void);

    unsigned long sys$dacefc(void);

    unsigned long sys$dalloc(void);

    unsigned long sys$dassgn(void);

    unsigned long sys$declare_rm(void);
    unsigned long sys$declare_rmw(void);

    unsigned long sys$dclast(void);

    unsigned long sys$dclcmh(void);

    unsigned long sys$dclevt(void);

    unsigned long sys$dclexh(void);

    unsigned long sys$dclrnh(void);

    unsigned long sys$declare_rm(void);
    unsigned long sys$declare_rmw(void);

    unsigned long sys$delete_bufobj(void);

    unsigned long sys$dellnm(void);

    unsigned long sys$dellog(void);

    unsigned long sys$delmbx(void);

    unsigned long sys$delprc(void);

    unsigned long sys$deltva(void);

    unsigned long sys$deq(unsigned int lkid, void *valblk, unsigned int acmode, unsigned int flags);

    unsigned long sys$device_scan(void);

    unsigned long sys$dgblsc(void);

    unsigned long sys$diagnose(void);

    unsigned long sys$disable_vp_use(void);
    unsigned long sys$disable_vp_use_int(void);

    unsigned long sys$dismou(void);

    unsigned long sys$dlcefc(void);

    unsigned long sys$dns(void);
    unsigned long sys$dnsw(void);

    unsigned long sys$emaa(void);

    unsigned long sys$enable_vp_use(void);
    unsigned long sys$enable_vp_use_int(void);

    unsigned long sys$end_branch(void);
    unsigned long sys$end_branchw(void);

    unsigned long sys$end_trans(void);
    unsigned long sys$end_transw(void);

    unsigned long sys$enq(unsigned int efn, unsigned int lkmode, struct _lksb *lksb, unsigned int flags, void *resnam,
                          unsigned int parid, void (*astadr)(), unsigned long astprm, void (*blkastadr)(), unsigned int acmode, unsigned int rsdm_id);
    unsigned long sys$enqw(unsigned int efn, unsigned int lkmode, struct _lksb *lksb, unsigned int flags, void *resnam,
                           unsigned int parid, void (*astadr)(), unsigned long astprm, void (*blkastadr)(), unsigned int acmode, unsigned int rsdm_id);

    unsigned long sys$erapat(void);

    unsigned long sys$evdpostevent(void);

    unsigned long sys$exit(void);

    unsigned long sys$expreg(void);

    unsigned long sys$fao(void);

    unsigned long sys$faol(void);

    unsigned long sys$filescan(void);

    unsigned long sys$find_held(void);

    unsigned long sys$find_holder(void);

    unsigned long sys$finish_rdb(void);

    unsigned long sys$finish_rmop(void);
    unsigned long sys$finish_rmopw(void);

    unsigned long sys$forcex(void);

    unsigned long sys$forget_rm(void);
    unsigned long sys$forget_rmw(void);

    unsigned long sys$format_acl(void);

    unsigned long sys$format_audit(void);

    unsigned long sys$format_class(void);

    unsigned long sys$get_default_trans(void);

    unsigned long sys$getchn(void);

    unsigned long sys$getdev(void);

    unsigned long sys$getdti(void);
    unsigned long sys$getdtiw(void);

    unsigned long sys$getdvi(void);
    unsigned long sys$getdviw(void);

    unsigned long sys$getevi(void);

    unsigned long sys$getjpi(void);
    unsigned long sys$getjpiw(void);

    unsigned long sys$getlki(void);
    unsigned long sys$getlkiw(void);

    unsigned long sys$getmsg(void);

    unsigned long sys$getpti(void);

    unsigned long sys$getqui(void);
    unsigned long sys$getquiw(void);

    unsigned long sys$getsyi(void);
    unsigned long sys$getsyiw(void);

    unsigned long sys$gettim(unsigned long long * timadr);

    unsigned long sys$getuai(void);

    unsigned long sys$getutc(void);

    unsigned long sys$grantid(void);

    unsigned long sys$grant_license(void);

    unsigned long sys$hash_password(void);

    unsigned long sys$hiber(void);

    unsigned long sys$idtoasc(void);

    unsigned long sys$imgact(void);

    unsigned long sys$imgfix(void);

    unsigned long sys$init_vol(void);

    unsigned long sys$ipc(void);
    unsigned long sys$ipcw(void);

    unsigned long sys$join_rm(void);
    unsigned long sys$join_rmw(void);

    unsigned long sys$lckpag(void);

    unsigned long sys$lkwset(void);

    unsigned long sys$mgblsc(void);

    unsigned long sys$mod_holder(void);

    unsigned long sys$mod_ident(void);

    unsigned long sys$mount(void);

    unsigned long sys$mtaccess(void);

    unsigned long sys$numtim(void);

    unsigned long sys$numutc(void);

    unsigned long sys$parse_acl(void);

    unsigned long sys$parse_class(void);

    unsigned long sys$posix_fork_control(void);

    unsigned long sys$process_scan(void);

    unsigned long sys$purgws(void);

    unsigned long sys$putmsg(void);

    unsigned long sys$qio(unsigned int efn, unsigned short int chan, unsigned int func, struct _iosb *iosb,
                          void (*astadr)(__unknown_params), long astprm, void*p1, long p2, long p3, long p4, long p5, long p6);
    unsigned long sys$qiow(unsigned int efn, unsigned short int chan, unsigned int func, struct _iosb *iosb,
                           void (*astadr)(__unknown_params), long astprm, void*p1, long p2, long p3, long p4, long p5, long p6);

    unsigned long sys$readef(void);

    unsigned long sys$recover(void);
    unsigned long sys$recoverw(void);

    unsigned long sys$release_license(void);

    unsigned long sys$release_vp(void);
    unsigned long sys$release_vp_int(void);

    unsigned long sys$rem_holder(void);

    unsigned long sys$rem_ident(void);

    unsigned long sys$restore_vp_exception(void);
    unsigned long sys$restore_vp_exc_int(void);

    unsigned long sys$restore_vp_state(void);

    unsigned long sys$resume(void);

    unsigned long sys$revokid(void);

    unsigned long sys$rmsrundwn(void *, int);

    unsigned long sys$save_vp_exception(void);
    unsigned long sys$save_vp_exc_int(void);

    unsigned long sys$schdwk(void);

    unsigned long sys$set_default_trans(void);
    unsigned long sys$set_default_transw(void);

    unsigned long sys$setast(void);

    unsigned long sys$setddir(const void *, unsigned short *, void *);

    unsigned long sys$setdfprot(const unsigned short *, unsigned short *);

    unsigned long sys$setdti(void);
    unsigned long sys$setdtiw(void);

    unsigned long sys$setef(void);

    unsigned long sys$setevtast(void);
    unsigned long sys$setevtastw(void);

    unsigned long sys$setexv(void);

    unsigned long sys$setime(void);

    unsigned long sys$setimr(void);

    unsigned long sys$setpfm(void);

    unsigned long sys$setpra(void);

    unsigned long sys$setpri(void);

    unsigned long sys$setprn(void);

    unsigned long sys$setprt(void);

    unsigned long sys$setprv(void);

    unsigned long sys$setrwm(void);

    unsigned long sys$setsfm(void);

    unsigned long sys$setssf(void);

    unsigned long sys$setstk(void);

    unsigned long sys$setswm(void);

    unsigned long sys$setuai(void);

    unsigned long sys$sndacc(void);

    unsigned long sys$snderr(void);

    unsigned long sys$sndjbc(void);
    unsigned long sys$sndjbcw(void);

    unsigned long sys$sndopr(void);

    unsigned long sys$sndsmb(void);

    unsigned long sys$start_branch(void);
    unsigned long sys$start_branchw(void);

    unsigned long sys$start_trans(void);
    unsigned long sys$start_transw(void);

    unsigned long sys$subsystem(void);

    unsigned long sys$suspnd(void);

    unsigned long sys$synch(void);

    unsigned long sys$timcon(void);

    unsigned long sys$trans_event(void);
    unsigned long sys$trans_eventw(void);

    unsigned long sys$trnlnm(void);

    unsigned long sys$trnlog(void);

    unsigned long sys$ulkpag(void);

    unsigned long sys$ulwset(void);

    unsigned long sys$unwind(void);

    unsigned long sys$updsec(void);
    unsigned long sys$updsecw(void);

    unsigned long sys$waitfr(void);

    unsigned long sys$wake(void);

    unsigned long sys$wfland(void);

    unsigned long sys$wflor(void);

# ifdef __cplusplus
}
# endif

//  Utility Functions

int sys$$geterrno(char *errmsg);

int sys$$getmsg(unsigned int msgid, unsigned short int *msglen, char *fmt, ...);

#endif
