/* <ssdef.h>
 *
 *	System service status codes
 */
#ifndef _SSDEF_H
#define _SSDEF_H

/*
   system status codes

	layout of message space:
	msg #s	   range      # msgs	    description
	0-127	    0-3ff	128	fatal errors
       128-191	  400-5ff	 64	hardware defined fatal errors
       192-255	  600-7ff	 64	success codes
       256-383	  800-bff	128	warning codes
       384-447	  c00-dff	 64	informational codes
       448-511	  e00-fff	 64	nonfatal error codes
      512-1023	1000-1fff	512	shared messages (not here, see SHRMSG)
     1024-1279	2000-27ff	256	fatal errors (continued from 400)
     1280-1343	2800-29ff	 64	no privilege errors
     1344-1375	2a00-2aff	 32	exceeded quota errors
     1376-1395	2b00-2b98	 20	reserved for media mgmt extensions
     1396-4031	2ba0-7dff      2656	(unused)
     4032-4095	 7e00-7fff	 64	reserved for SCS sysaps
 */
#define SYSTEM$_FACILITY 0

#define SS$_NORMAL	1
/*
   Note that CONTINUE,WASCLR,WASSET,REMEMBER do not have any message text
   since these codes use the same message number as NORMAL and ACCVIO,
   they can never be made to have messages associated with them.
 */
#define SS$_CONTINUE	1
#define SS$_WASCLR	1
#define SS$_WASSET	9
#define SS$_REMEMBER	1

/* severe errors (hex 0-400, 2000-2800) */
#define SS$_ACCVIO	0x000C	/* 12 */
#define SS$_BADPARAM	0x0014	/* 20 */
#define SS$_EXQUOTA	0x001C	/* 28 */
#define SS$_NOPRIV	0x0024	/* 36 */
#define SS$_ABORT	0x002C	/* 44 */
#define SS$_BADATTRIB	0x0034	/* 52 */
#define SS$_BADESCAPE	0x003C	/* 60 */
#define SS$_BADIMGHDR	0x0044	/* 68 */
#define SS$_CHANINTLK	0x004C	/* 76 */
#define SS$_CTRLERR	0x0054	/* 84 */
#define SS$_DATACHECK	0x005C	/* 92 */
#define SS$_DEVFOREIGN	0x0064	/* 100 */
#define SS$_DEVMOUNT	0x006C	/* 108 */
#define SS$_DEVNOTMBX	0x0074	/* 116 */
#define SS$_DEVNOTMOUNT 0x007C	/* 124 */
#define SS$_DEVOFFLINE	0x0084	/* 132 */
#define SS$_DRVERR	0x008C	/* 140 */
#define SS$_DUPLNAM	0x0094	/* 148 */
#define SS$_FILACCERR	0x009C	/* 156 */
#define SS$_FILALRACC	0x00A4	/* 164 */
#define SS$_FILNOTACC	0x00AC	/* 172 */
#define SS$_FILNOTEXP	0x00B4	/* 180 */
#define SS$_FORMAT	0x00BC	/* 188 */
#define SS$_GPTFULL	0x00C4	/* 196 */
#define SS$_GSDFULL	0x00CC	/* 204 */
#define SS$_LCKPAGFUL	0x00D4	/* 212 */
#define SS$_ILLBLKNUM	0x00DC	/* 220 */
#define SS$_ILLCNTRFUNC 0x00E4	/* 228 */
#define SS$_ILLEFC	0x00EC	/* 236 */
#define SS$_ILLIOFUNC	0x00F4	/* 244 */
#define SS$_ILLPAGCNT	0x00FC	/* 252 */
#define SS$_ILLSER	0x0104	/* 260 */
#define SS$_INCVOLLABEL 0x010C	/* 268 */
#define SS$_INSFARG	0x0114	/* 276 */
#define SS$_INSFWSL	0x011C	/* 284 */
#define SS$_INSFMEM	0x0124	/* 292 */
#define SS$_INSFRAME	0x012C	/* 300 */
#define SS$_IVADDR	0x0134	/* 308 */
#define SS$_IVCHAN	0x013C	/* 316 */
#define SS$_IVDEVNAM	0x0144	/* 324 */
#define SS$_IVGSDNAM	0x014C	/* 332 */
#define SS$_IVLOGNAM	0x0154	/* 340 */
#define SS$_IVLOGTAB	0x015C	/* 348 */
#define SS$_IVQUOTAL	0x0164	/* 356 */
#define SS$_IVSECFLG	0x016C	/* 364 */
#define SS$_IVSSRQ	0x0174	/* 372 */
#define SS$_IVSTSFLG	0x017C	/* 380 */
#define SS$_IVTIME	0x0184	/* 388 */
#define SS$_LENVIO	0x018C	/* 396 */
#define SS$_LKWSETFUL	0x0194	/* 404 */
#define SS$_MBTOOSML	0x019C	/* 412 */
#define SS$_MEDOFL	0x01A4	/* 420 */
#define SS$_NODATA	0x01AC	/* 428 */
#define SS$_NOIOCHAN	0x01B4	/* 436 */
#define SS$_NOLOGNAM	0x01BC	/* 444 */
#define SS$_NONEXDRV	0x01C4	/* 452 */
#define SS$_NOTFILEDEV	0x01CC	/* 460 */
#define SS$_NOTINTBLSZ	0x01D4	/* 468 */
#define SS$_NOTLABELMT	0x01DC	/* 476 */
#define SS$_NOTSQDEV	0x01E4	/* 484 */
#define SS$_PAGOWNVIO	0x01EC	/* 492 */
#define SS$_PARITY	0x01F4	/* 500 */
#define SS$_PARTESCAPE	0x01FC	/* 508 */
#define SS$_PFMBSY	0x0204	/* 516 */
#define SS$_PSTFULL	0x020C	/* 524 */
#define SS$_RESULTOVF	0x0214	/* 532 */
#define SS$_SECTBLFUL	0x021C	/* 540 */
#define SS$_TAPEPOSLOST 0x0224	/* 548 */
#define SS$_TIMEOUT	0x022C	/* 556 */
#define SS$_UNASEFC	0x0234	/* 564 */
#define SS$_UNSAFE	0x023C	/* 572 */
#define SS$_VASFULL	0x0244	/* 580 */
#define SS$_VECINUSE	0x024C	/* 588 */
#define SS$_VOLINV	0x0254	/* 596 */
#define SS$_WRITLCK	0x025C	/* 604 */
#define SS$_NOTAPEOP	0x0264	/* 612 */
#define SS$_IVCHNLSEC	0x026C	/* 620 */
#define SS$_NOMBX	0x0274	/* 628 */
#define SS$_NOLINKS	0x027C	/* 636 */
#define SS$_NOSOLICIT	0x0284	/* 644 */
#define SS$_NOSUCHNODE	0x028C	/* 652 */
#define SS$_REJECT	0x0294	/* 660 */
#define SS$_TOOMUCHDATA 0x029C	/* 668 */
#define SS$_BUGCHECK	0x02A4	/* 676 */
#define SS$_FILNOTCNTG	0x02AC	/* 684 */
#define SS$_BADSTACK	0x02B4	/* 692 */
#define SS$_MCHECK	0x02BC	/* 700 */
#define SS$_DEVACTIVE	0x02C4	/* 708 */
#define SS$_HANGUP	0x02CC	/* 716 */
#define SS$_OPINCOMPL	0x02D4	/* 724 */
#define SS$_ILLSEQOP	0x02DC	/* 732 */
#define SS$_IVSECIDCTL	0x02E4	/* 740 */
#define SS$_NOTNETDEV	0x02EC	/* 748 */
#define SS$_IVPROTECT	0x02F4	/* 756 */
#define SS$_ACPVAFUL	0x02FC	/* 764 */
#define SS$_MTLBLLONG	0x0304	/* 772 */
#define SS$_BUFBYTALI	0x030C	/* 780 */
#define SS$_NOAQB	0x0314	/* 788 */
#define SS$_WRONGACP	0x031C	/* 796 */
#define SS$_BUFNOTALIGN 0x0324	/* 804 */
#define SS$_DEVCMDERR	0x032C	/* 812 */
#define SS$_DEVREQERR	0x0334	/* 820 */
#define SS$_INSFBUFDP	0x033C	/* 828 */
#define SS$_INSFMAPREG	0x0344	/* 836 */
#define SS$_IVBUFLEN	0x034C	/* 844 */
#define SS$_IVMODE	0x0354	/* 852 */
#define SS$_MCNOTVALID	0x035C	/* 860 */
#define SS$_POWERFAIL	0x0364	/* 868 */
#define SS$_SHMGSNOTMAP 0x036C	/* 876 */
#define SS$_TOOMANYLNAM 0x0374	/* 884 */
#define SS$_SHMNOTCNCT	0x037C	/* 892 */
#define SS$_NOTCREATOR	0x0384	/* 900 */
#define SS$_INTERLOCK	0x038C	/* 908 */
#define SS$_BADQUEUEHDR 0x0394	/* 916 */
#define SS$_NOSLOT	0x039C	/* 924 */
#define SS$_SUSPENDED	0x03A4	/* 932 */
#define SS$_EXPORTQUOTA 0x03AC	/* 940 */
#define SS$_NOSHMBLOCK	0x03B4	/* 948 */
#define SS$_BADQFILE	0x03BC	/* 956 */
#define SS$_NOQFILE	0x03C4	/* 964 */
#define SS$_QFACTIVE	0x03CC	/* 972 */
#define SS$_QFNOTACT	0x03D4	/* 980 */
#define SS$_DUPDSKQUOTA 0x03DC	/* 988 */
#define SS$_NODISKQUOTA 0x03E4	/* 996 */
#define SS$_EXDISKQUOTA 0x03EC	/* 1004 */
#define SS$_IDMISMATCH	0x03F4	/* 1012 */
#define SS$_NOWRT	0x03FC	/* 1020 */
/* Note:  the above set of severe error messages is full!!!! */
/* Additional severe error messages are in the range hex 2000-2800. */
#define SS$_PAGTYPVIO	0x0B00	/* 2816 */
#define SS$_NOSUCHPAG	0x0B08	/* 2824 */
#define SS$_BADISD	0x2004	/* 8196 */
#define SS$_RELINK	0x200C	/* 8204 */
#define SS$_NOTINSTALL	0x2014	/* 8212 */
#define SS$_SHARTOOBIG	0x201C	/* 8220 */
#define SS$_NOP1VA	0x2024	/* 8228 */
#define SS$_MULTRMS	0x202C	/* 8236 */
#define SS$_VECFULL	0x2034	/* 8244 */
#define SS$_IVLVEC	0x203C	/* 8252 */
#define SS$_INSFSPTS	0x2044	/* 8260 */
#define SS$_DISCONNECT	0x204C	/* 8268 */
#define SS$_PRIVINSTALL 0x2054	/* 8276 */
#define SS$_PROTINSTALL 0x205C	/* 8284 */
#define SS$_BADVEC	0x2064	/* 8292 */
#define SS$_REMRSRC	0x206C	/* 8300 */
#define SS$_PROTOCOL	0x2074	/* 8308 */
#define SS$_THIRDPARTY	0x207C	/* 8316 */
#define SS$_NOSUCHUSER	0x2084	/* 8324 */
#define SS$_SHUT	0x208C	/* 8332 */
#define SS$_UNREACHABLE 0x2094	/* 8340 */
#define SS$_INVLOGIN	0x209C	/* 8348 */
#define SS$_NOSUCHOBJ	0x20A4	/* 8356 */
#define SS$_EXCPUTIM	0x20AC	/* 8364 */
#define SS$_OPRABORT	0x20B4	/* 8372 */
#define SS$_SHRIDMISMAT 0x20BC	/* 8380 */
#define SS$_COMMHARD	0x20C4	/* 8388 */
#define SS$_IVCHAR	0x20CC	/* 8396 */
#define SS$_DEVINACT	0x20D4	/* 8404 */
#define SS$_CONNECFAIL	0x20DC	/* 8412 */
#define SS$_LINKABORT	0x20E4	/* 8420 */
#define SS$_LINKDISCON	0x20EC	/* 8428 */
#define SS$_LINKEXIT	0x20F4	/* 8436 */
#define SS$_PATHLOST	0x20FC	/* 8444 */
#define SS$_CLEARED	0x2104	/* 8452 */
#define SS$_RESET	0x210C	/* 8460 */
#define SS$_UNSOLICIT	0x2114	/* 8468 */
#define SS$_TOOMANYREDS 0x211C	/* 8476 */
#define SS$_IVLOCKID	0x2124	/* 8484 */
#define SS$_SUBLOCKS	0x212C	/* 8492 */
#define SS$_PARNOTGRANT 0x2134	/* 8500 */
#define SS$_CVTUNGRANT	0x213C	/* 8508 */
#define SS$_FORCEDERROR 0x2144	/* 8516 */
#define SS$_ILLSELF	0x214C	/* 8524 */
#define SS$_ILLCDTST	0x2154	/* 8532 */
#define SS$_NOLISTENER	0x215C	/* 8540 */
#define SS$_EXGBLPAGFIL 0x2164	/* 8548 */
#define SS$_BADRCT	0x216C	/* 8556 */
#define SS$_DIRNOTEMPTY 0x2174	/* 8564 */
#define SS$_FORCEDEXIT	0x217C	/* 8572 */
#define SS$_NOTPRINTED	0x2184	/* 8580 */
#define SS$_JBCERROR	0x218C	/* 8588 */
#define SS$_NOLICENSE	0x2194	/* 8596 */
#define SS$_VCBROKEN	0x219C	/* 8604 */
#define SS$_VCCLOSED	0x21A4	/* 8612 */
#define SS$_INSFCDT	0x21AC	/* 8620 */
#define SS$_DEVNOTDISM	0x21B4	/* 8628 */
#define SS$_NOSHRIMG	0x21BC	/* 8636 */
#define SS$_DUPUNIT	0x21C4	/* 8644 */
#define SS$_BADACLCTX	0x21CC	/* 8652 */
#define SS$_SERIOUSEXCP 0x21D4	/* 8660 */
#define SS$_TEMPLATEDEV 0x21DC	/* 8668 */
#define SS$_IVACL	0x21E4	/* 8676 */
#define SS$_NOSUCHID	0x21EC	/* 8684 */
#define SS$_NOLCLMEDA	0x21F4	/* 8692 */
#define SS$_NOREGAVIL	0x21FC	/* 8700 */
#define SS$_NOREGSUIT	0x2204	/* 8708 */
#define SS$_NOSUCHPGM	0x220C	/* 8716 */
#define SS$_PGMLDFAIL	0x2214	/* 8724 */
#define SS$_PGMSTDALN	0x221C	/* 8732 */
#define SS$_IVIDENT	0x2224	/* 8740 */
#define SS$_DUPIDENT	0x222C	/* 8748 */
#define SS$_INCSEGTRA	0x2234	/* 8756 */
#define SS$_NODELEAVE	0x223C	/* 8764 */
#define SS$_KERNELINV	0x2244	/* 8772 */
#define SS$_EXLNMQUOTA	0x224C	/* 8780 */
#define SS$_PARENT_DEL	0x2254	/* 8788 */
#define SS$_PARNOTSYS	0x225C	/* 8796 */
#define SS$_INSSWAPSPACE 0x2264 /* 8804 */
#define SS$_VOLOERR	0x226C	/* 8812 */
#define SS$_DATALATE	0x2274	/* 8820 */
#define SS$_OVRMAXARG	0x227C	/* 8828 */
#define SS$_SHACHASTA	0x2284	/* 8836 */
#define SS$_TERMNETDEV	0x228C	/* 8844 */
#define SS$_NOLOGTAB	0x2294	/* 8852 */
#define SS$_WRONGNAME	0x229C	/* 8860 */
#define SS$_NOVOLACC	0x22A4	/* 8868 */
#define SS$_NOFILACC	0x22AC	/* 8876 */
#define SS$_INVEXHLIST	0x22B4	/* 8884 */
#define SS$_NOACLSUPPORT 0x22BC /* 8892 */
#define SS$_INVSECLASS	0x22C4	/* 8900 */
#define SS$_INCSHAMEM	0x22CC	/* 8908 */
#define SS$_DEVNOTSHR	0x22D4	/* 8916 */
#define SS$_RUCONFLICT	0x22DC	/* 8924 */
#define SS$_DATALOST	0x22E4	/* 8932 */
#define SS$_REMOTE_PROC 0x22EC	/* 8940 */
#define SS$_CPUNOTACT	0x22F4	/* 8948 */
#define SS$_SRVMMAT	0x22FC	/* 8956 */
#define SS$_EXLICENSE	0x2304	/* 8964 */
#define SS$_INVLICENSE	0x230C	/* 8972 */
#define SS$_LICENSE_LEVEL 0x2314 /* 8980 */
#define SS$_INV_HW_ID	0x231C	/* 8988 */
#define SS$_BADCONTEXT	0x2324	/* 8996 */
#define SS$_TOOMUCHCTX	0x232C	/* 9004 */
#define SS$_VA_IN_USE	0x2334	/* 9012 */
#define SS$_NODELETE	0x233C	/* 9020 */
#define SS$_NOSUCHCPU	0x2344	/* 9028 */
#define SS$_IMGVEXC	0x234C	/* 9036 */
#define SS$_NOSAVPEXC	0x2354	/* 9044 */
#define SS$_NOSUCHTID	0x235C	/* 9052 */
#define SS$_NOSUCHRM	0x2364	/* 9060 */
#define SS$_NOCURTID	0x236C	/* 9068 */
#define SS$_WRONGSTATE	0x2374	/* 9076 */
#define SS$_VETO	0x237C	/* 9084 */
#define SS$_PWDSYNTAX	0x2384	/* 9092 */
#define SS$_PWDNOTDIF	0x238C	/* 9100 */
#define SS$_INVPWDLEN	0x2394	/* 9108 */
#define SS$_SYSVERDIF	0x239C	/* 9116 */
#define SS$_HWM_STALL	0x23A4	/* 9124 */
#define SS$_NOSUSPEND	0x23AC	/* 9132 */
#define SS$_NOSUCHPART	0x23B4	/* 9140 */
#define SS$_RMALRDCL	0x23BC	/* 9148 */
#define SS$_ALRCURTID	0x23C4	/* 9156 */
#define SS$_INVLOG	0x23CC	/* 9164 */
#define SS$_BADLOGVER	0x23D4	/* 9172 */
#define SS$_OPINPROG	0x23DC	/* 9180 */
#define SS$_WRONGACMODE 0x23E4	/* 9188 */
#define SS$_SUBTRACED	0x23EC	/* 9196 */
#define SS$_ARCHIVING	0x23F4	/* 9204 */
#define SS$_ARCHIVED	0x23FC	/* 9212 */
#define SS$_SITEPWDFAIL 0x2404	/* 9220 */
#define SS$_NOSUCHCPB	0x240C	/* 9228 */
#define SS$_CPUCAP	0x2414	/* 9236 */
#define SS$_LOADER	0x241C	/* 9244 */
#define SS$_HSTBUFACC	0x2424	/* 9252 */
#define SS$_INCONOLCK	0x242C	/* 9260 */
#define SS$_INVEVENT	0x2434	/* 9268 */
#define SS$_NOSUCHFAC	0x243C	/* 9276 */
#define SS$_NOCALLPRIV	0x2444	/* 9284 */
#define SS$_INSFLPGS	0x244C	/* 9292 */
#define SS$_INSFRPGS	0x2454	/* 9300 */
#define SS$_BADREASON	0x245C	/* 9308 */
#define SS$_NOSUCHBID	0x2464	/* 9316 */
#define SS$_NOTORIGIN	0x246C	/* 9324 */
#define SS$_NOLOG	0x2474	/* 9332 */
#define SS$_TPDISABLED	0x247C	/* 9340 */
#define SS$_BRANCHENDED 0x2484	/* 9348 */
#define SS$_BRANCHSTARTED 0x248C /* 9356 */
#define SS$_NOSUCHREPORT 0x2494 /* 9364 */
#define SS$_BADTIME	0x249C	/* 9372 */
#define SS$_PARTJOINED	0x24A4	/* 9380 */
#define SS$_NOPRESUME	0x24AC	/* 9388 */
#define SS$_RECOVERCANCEL 0x24B4 /* 9396 */
#define SS$_CURTIDCHANGE 0x24BC /* 9404 */
#define SS$_NOREADER	0x24C4	/* 9412 */
#define SS$_NOWRITER	0x24CC	/* 9420 */
#define SS$_DEVNOTWS	0x24D4	/* 9428 */

/* hardware fatal exception codes (hex 400-600) */
#define SS$_PLHLDR	0x0404	/* 1028 */
#define SS$_ASTFLT	0x040C	/* 1036 */
#define SS$_BREAK	0x0414	/* 1044 */
#define SS$_CMODSUPR	0x041C	/* 1052 */
#define SS$_CMODUSER	0x0424	/* 1060 */
#define SS$_COMPAT	0x042C	/* 1068 */
#define SS$_OPCCUS	0x0434	/* 1076 */
#define SS$_OPCDEC	0x043C	/* 1084 */
#define SS$_PAGRDERR	0x0444	/* 1092 */
#define SS$_RADRMOD	0x044C	/* 1100 */
#define SS$_ROPRAND	0x0454	/* 1108 */
#define SS$_SSFAIL	0x045C	/* 1116 */
#define SS$_TBIT	0x0464	/* 1124 */
#define SS$_DEBUG	0x046C	/* 1132 */
/* the following codes must be in order since they are calculated */
#define SS$_ARTRES	0x0474	/* 1140 */
#define SS$_INTOVF	0x047C	/* 1148 */
#define SS$_INTDIV	0x0484	/* 1156 */
#define SS$_FLTOVF	0x048C	/* 1164 */
#define SS$_FLTDIV	0x0494	/* 1172 */
#define SS$_FLTUND	0x049C	/* 1180 */
#define SS$_DECOVF	0x04A4	/* 1188 */
#define SS$_SUBRNG	0x04AC	/* 1196 */
#define SS$_FLTOVF_F	0x04B4	/* 1204 */
#define SS$_FLTDIV_F	0x04BC	/* 1212 */
#define SS$_FLTUND_F	0x04C4	/* 1220 */
/* the above codes must be in order since they are calculated */
#define SS$_INHCHMK	0x04CC	/* 1228 */
#define SS$_INHCHME	0x04D4	/* 1236 */
#define SS$_VECDIS	0x04DC	/* 1244 */
#define SS$_VARITH	0x04E4	/* 1252 */
#define SS$_ILLVECOP	0x04EC	/* 1260 */
#define SS$_VECALIGN	0x04F4	/* 1268 */

/* success codes (hex 600-800) */
#define SS$_BUFFEROVF	0x0601	/* 1537 */
#define SS$_CONTROLO	0x0609	/* 1545 */
#define SS$_CONTROLY	0x0611	/* 1553 */
#define SS$_CREATED	0x0619	/* 1561 */
#define SS$_MSGNOTFND	0x0621	/* 1569 */
#define SS$_NOTRAN	0x0629	/* 1577 */
#define SS$_SUPERSEDE	0x0631	/* 1585 */
#define SS$_WASECC	0x0639	/* 1593 */
#define SS$_DEVALRALLOC 0x0641	/* 1601 */
#define SS$_REMOTE	0x0649	/* 1609 */
#define SS$_CONTROLC	0x0651	/* 1617 */
#define SS$_NOTMODIFIED 0x0659	/* 1625 */
#define SS$_RDDELDATA	0x0661	/* 1633 */
#define SS$_OVRDSKQUOTA 0x0669	/* 1641 */
#define SS$_OBSOLETE_1	0x0671	/* 1649 */
#define SS$_FILEPURGED	0x0679	/* 1657 */
#define SS$_NOTALLPRIV	0x0681	/* 1665 */
#define SS$_SYNCH	0x0689	/* 1673 */
#define SS$_CONCEALED	0x0691	/* 1681 */
#define SS$_INCOMPAT	0x0699	/* 1689 */
#define SS$_DBGOPCREQ	0x06A1	/* 1697 */
#define SS$_ALRDYCLOSED 0x06A9	/* 1705 */
#define SS$_LNMCREATED	0x06B1	/* 1713 */
#define SS$_ACEIDMATCH	0x06B9	/* 1721 */
#define SS$_DBGEVENT	0x06C1	/* 1729 */
#define SS$_REMOVED	0x06C9	/* 1737 */
#define SS$_QUEUED	0x06D1	/* 1745 */
#define SS$_SUBDISABLED 0x06D9	/* 1753 */
#define SS$_FORGET	0x06E1	/* 1761 */
#define SS$_PREPARED	0x06E9	/* 1769 */
#define SS$_NOMOREITEMS 0x06F1	/* 1777 */
#define SS$_VOLATILE	0x06F9	/* 1785 */

/* warning codes (hex 800-c00) */
#define SS$_ACCONFLICT	0x0800	/* 2048 */
#define SS$_BADCHKSUM	0x0808	/* 2056 */
#define SS$_BADFILEHDR	0x0810	/* 2064 */
#define SS$_BADFILENAME 0x0818	/* 2072 */
#define SS$_BADFILEVER	0x0820	/* 2080 */
#define SS$_BADIRECTORY 0x0828	/* 2088 */
#define SS$_CANCEL	0x0830	/* 2096 */
#define SS$_DATAOVERUN	0x0838	/* 2104 */
#define SS$_DEVALLOC	0x0840	/* 2112 */
#define SS$_DEVASSIGN	0x0848	/* 2120 */
#define SS$_DEVICEFULL	0x0850	/* 2128 */
#define SS$_DEVNOTALLOC 0x0858	/* 2136 */
#define SS$_DIRFULL	0x0860	/* 2144 */
#define SS$_DUPFILENAME 0x0868	/* 2152 */
#define SS$_ENDOFFILE	0x0870	/* 2160 */
#define SS$_ENDOFTAPE	0x0878	/* 2168 */
#define SS$_EXTIDXFILE	0x0880	/* 2176 */
#define SS$_FCPREADERR	0x0888	/* 2184 */
#define SS$_FCPREWNDERR 0x0890	/* 2192 */
#define SS$_FCPSPACERR	0x0898	/* 2200 */
#define SS$_FCPWRITERR	0x08A0	/* 2208 */
#define SS$_FILELOCKED	0x08A8	/* 2216 */
#define SS$_FILENUMCHK	0x08B0	/* 2224 */
#define SS$_FILESEQCHK	0x08B8	/* 2232 */
#define SS$_FILESTRUCT	0x08C0	/* 2240 */
#define SS$_HEADERFULL	0x08C8	/* 2248 */
#define SS$_IDXFILEFULL 0x08D0	/* 2256 */
#define SS$_MBFULL	0x08D8	/* 2264 */
#define SS$_NOHOMEBLK	0x08E0	/* 2272 */
#define SS$_NONEXPR	0x08E8	/* 2280 */
#define SS$_NONLOCAL	0x08F0	/* 2288 */
#define SS$_NOHANDLER	0x08F8	/* 2296 */
#define SS$_NOSIGNAL	0x0900	/* 2304 */
#define SS$_NOSUCHDEV	0x0908	/* 2312 */
#define SS$_NOSUCHFILE	0x0910	/* 2320 */
#define SS$_RESIGNAL	0x0918	/* 2328 */
#define SS$_UNWIND	0x0920	/* 2336 */
#define SS$_UNWINDING	0x0928	/* 2344 */
#define SS$_NOMOREFILES 0x0930	/* 2352 */
#define SS$_BEGOFFILE	0x0938	/* 2360 */
#define SS$_BLOCKCNTERR 0x0940	/* 2368 */
#define SS$_MUSTCLOSEFL 0x0948	/* 2376 */
#define SS$_WAITUSRLBL	0x0950	/* 2384 */
#define SS$_ILLUSRLBLRD 0x0958	/* 2392 */
#define SS$_ILLUSRLBLWT 0x0960	/* 2400 */
#define SS$_ILLLBLAST	0x0968	/* 2408 */
#define SS$_ENDOFUSRLBL 0x0970	/* 2416 */
#define SS$_NOSUCHSEC	0x0978	/* 2424 */
#define SS$_CLIFRCEXT	0x0980	/* 2432 */
#define SS$_FCPREPSTN	0x0988	/* 2440 */
#define SS$_TOOMANYVER	0x0990	/* 2448 */
#define SS$_NOTVOLSET	0x0998	/* 2456 */
#define SS$_ENDOFVOLUME 0x09A0	/* 2464 */
#define SS$_NOMOREPROC	0x09A8	/* 2472 */
#define SS$_NODEVAVL	0x09B0	/* 2480 */
#define SS$_NOTQUEUED	0x09B8	/* 2488 */
#define SS$_DGQINCOMP	0x09C0	/* 2496 */
#define SS$_DIRALLOC	0x09C8	/* 2504 */
#define SS$_ACLEMPTY	0x09D0	/* 2512 */
#define SS$_NOENTRY	0x09D8	/* 2520 */
#define SS$_NOMOREACE	0x09E0	/* 2528 */
#define SS$_RIGHTSFULL	0x09E8	/* 2536 */
#define SS$_VALNOTVALID 0x09F0	/* 2544 */
#define SS$_ACLFULL	0x09F8	/* 2552 */
#define SS$_NOMORENODE	0x0A00	/* 2560 */
#define SS$_NOMORELOCK	0x0A08	/* 2568 */
#define SS$_BEGOFTAPE	0x0A10	/* 2576 */
#define SS$_OBJLOCKHELD 0x0A18	/* 2584 */
#define SS$_CPUSTARTIP	0x0A20	/* 2592 */
#define SS$_ALLSTARTED	0x0A28	/* 2600 */
#define SS$_ALRDYSTRT	0x0A30	/* 2608 */
#define SS$_ALRDYSTPPD	0x0A38	/* 2616 */
#define SS$_NSTPPD	0x0A40	/* 2624 */
#define SS$_UNKRESULT	0x0A48	/* 2632 */
#define SS$_ITEMNOTFOUND 0x0A50 /* 2640 */
#define SS$_NOMOREDEV	0x0A58	/* 2648 */
#define SS$_EFNOTSET	0x0A60	/* 2656 */
#define SS$_PRIMNOSTP	0x0A68	/* 2664 */
#define SS$_BOOTREJECT	0x0A70	/* 2672 */
#define SS$_RMTPATH	0x0A78	/* 2680 */
#define SS$_OBJECT_EXISTS 0x0A80 /* 2688 */
#define SS$_NOSUCHOBJECT 0x0A88 /* 2696 */
#define SS$_NOVOLDESC	0x0A90	/* 2704 */
#define SS$_NOPTBLDIR	0x0A98	/* 2712 */
#define SS$_DRVEXISTS	0x0AA0	/* 2720 */
#define SS$_DEVEXISTS	0x0AA8	/* 2728 */
#define SS$_GOTO_UNWIND	0x0AB0	/* 2736 */
#define SS$_EXIT_UNWIND	0x0AB8	/* 2744 */
#define SS$_SMALLHEADER	0x0AC0	/* 2752 */
#define SS$_FDT_COMPL	0x0AC8	/* 2760 */

/* informational codes (hex c00-e00) */
#define SS$_EOTIN	0x0C03	/* 3075 */
#define SS$_CHAINW	0x0C0B	/* 3083 */
#define SS$_NOTINSEC	0x0C13	/* 3091 */
#define SS$_NONXPAG	0x0C1B	/* 3099 */
#define SS$_LOGNAME	0x0C23	/* 3107 */
#define SS$_CPUSTARTD	0x0C2B	/* 3115 */
#define SS$_CPUSTOPPING 0x0C33	/* 3123 */
#define SS$_NOTALLCANCELED 0x0C3B /* 3131 */
#define SS$_NOTHINGDONE 0x0C43	/* 3139 */

/* nonfatal error codes (hex e00-1000) */
#define SS$_LINEABRT	0x0E02	/* 3586 */
#define SS$_DEADLOCK	0x0E0A	/* 3594 */
#define SS$_NOLOCKID	0x0E12	/* 3602 */
#define SS$_EXDEPTH	0x0E1A	/* 3610 */
#define SS$_PARTMAPPED	0x0E22	/* 3618 */
#define SS$_CANCELGRANT 0x0E2A	/* 3626 */
#define SS$_RETRY	0x0E32	/* 3634 */
#define SS$_BADACL	0x0E3A	/* 3642 */
#define SS$_ACEEXISTS	0x0E42	/* 3650 */
#define SS$_UNSUPPORTED 0x0E4A	/* 3658 */
#define SS$_NORIGHTSDB	0x0E52	/* 3666 */
#define SS$_LOGSTALL	0x0E5A	/* 3674 */
#define SS$_LOGFULL	0x0E62	/* 3682 */
#define SS$_PWDINDIC	0x0E6A	/* 3690 */
#define SS$_PWDINHIS	0x0E72	/* 3698 */
#define SS$_PWDWEAK	0x0E7A	/* 3706 */
#define SS$_USEGENPWD	0x0E82	/* 3714 */
#define SS$_INVBUSNAM	0x0E8A	/* 3722 */
#define SS$_INVCOMPTYPE 0x0E92	/* 3730 */
#define SS$_INVCOMPID	0x0E9A	/* 3738 */
#define SS$_INVCOMPLIST 0x0EA2	/* 3746 */
#define SS$_NOCOMPLSTS	0x0EAA	/* 3754 */
#define SS$_INVSECDOMAIN 0x0EB2 /* 3762 */
#define SS$_BADCHECKSUM 0x0EBA	/* 3770 */
#define SS$_ARBTOOBIG	0x0EC2	/* 3778 */
#define SS$_ORBTOOBIG	0x0ECA	/* 3786 */

#define SS$_NOSUCHVOL 0x0F2a /* 3882 */

#define SS$_UNSUPVOLSET 0x26B4 /* 9908 */

#define SS$_BADMEDIA 0x2784 /* 10116 */
#define SS$_IVPARAM 0x27a4 /* 10148 */

/* no privilege fatal error codes (hex 2800-2a00) */
#define SS$_NOPRIVSTRT	0x2800	/* 10240 */
#define SS$_NOCMKRNL	0x2804	/* 10244 */
#define SS$_NOCMEXEC	0x280C	/* 10252 */
#define SS$_NOSYSNAM	0x2814	/* 10260 */
#define SS$_NOGRPNAM	0x281C	/* 10268 */
#define SS$_NOALLSPOOL	0x2824	/* 10276 */
#define SS$_NODETACH	0x282C	/* 10284 */
#define SS$_NODIAGNOSE	0x2834	/* 10292 */
#define SS$_NOLOG_IO	0x283C	/* 10300 */
#define SS$_NOGROUP	0x2844	/* 10308 */
#define SS$_NOACNT	0x284C	/* 10316 */
#define SS$_NOPRMCEB	0x2854	/* 10324 */
#define SS$_NOPRMMBX	0x285C	/* 10332 */
#define SS$_NOPSWAPM	0x2864	/* 10340 */
#define SS$_NOALTPRI	0x286C	/* 10348 */
#define SS$_NOSETPRV	0x2874	/* 10356 */
#define SS$_NOTMPMBX	0x287C	/* 10364 */
#define SS$_NOWORLD	0x2884	/* 10372 */
#define SS$_NOMOUNT	0x288C	/* 10380 */
#define SS$_NOOPER	0x2894	/* 10388 */
#define SS$_NOEXQUOTA	0x289C	/* 10396 */
#define SS$_NONETMBX	0x28A4	/* 10404 */
#define SS$_NOVOLPRO	0x28AC	/* 10412 */
#define SS$_NOPHY_IO	0x28B4	/* 10420 */
#define SS$_NOBUGCHK	0x28BC	/* 10428 */
#define SS$_NOPRMGBL	0x28C4	/* 10436 */
#define SS$_NOSYSGBL	0x28CC	/* 10444 */
#define SS$_NOPFNMAP	0x28D4	/* 10452 */
#define SS$_NOSHMEM	0x28DC	/* 10460 */
#define SS$_NOSYSPRV	0x28E4	/* 10468 */
#define SS$_NOBYPASS	0x28EC	/* 10476 */
#define SS$_NOSYSLCK	0x28F4	/* 10484 */
#define SS$_UNUSED_4	0x28FC	/* 10492 */
#define SS$_UNUSED_5	0x2904	/* 10500 */
#define SS$_NODOWNGRADE 0x290C	/* 10508 */
#define SS$_NOUPGRADE	0x2914	/* 10516 */
#define SS$_NOGRPPRV	0x291C	/* 10524 */
#define SS$_NOREADALL	0x2924	/* 10532 */
#define SS$_NOSHARE	0x292C	/* 10540 */
#define SS$_NOSECURITY	0x2934	/* 10548 */
#define SS$_NOPRIVEND	0x29FF	/* 10751 */

/* exceeded quota fatal error codes (hex 2a00-2b00) */
#define SS$_EXQUOTASTRT 0x2A00	/* 10752 */
#define SS$_EXASTLM	0x2A04	/* 10756 */
#define SS$_EXBIOLM	0x2A0C	/* 10764 */
#define SS$_EXBYTLM	0x2A14	/* 10772 */
#define SS$_EXDIOLM	0x2A1C	/* 10780 */
#define SS$_EXFILLM	0x2A24	/* 10788 */
#define SS$_EXPGFLQUOTA 0x2A2C	/* 10796 */
#define SS$_EXPRCLM	0x2A34	/* 10804 */
#define SS$_EXTQELM	0x2A3C	/* 10812 */
#define SS$_EXENQLM	0x2A44	/* 10820 */
#define SS$_EXQUOTAEND	0x2AFF	/* 11007 */

/* Reserved for media management extensions (hex 2b00-2b98) */
/*	(MME messages will be defined here) */

/* reserved for SCA sysaps (hex 7e00-7fff) */
#define SS$_SYSAPMIN	0x7E00	/* 32256 */
#define SS$_SYSAPMAX	0x7FFF	/* 32767 */

#endif	/*_SSDEF_H*/
