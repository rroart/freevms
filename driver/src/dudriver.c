// $Id$
// $Locker$

// Author. Roar Thronæs.
// Author. NetBSD people.

#if 0
/*
 * Copyright (c) 1996 Ludd, University of Lule}, Sweden.
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)mscp.c	7.5 (Berkeley) 12/16/90
 */

#include<mscpdef.h>
#include<scs_routines.h>
#include<sysap_macros.h>

#if 0
#include <sys/param.h>
#include <sys/device.h>
#include <sys/buf.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <machine/bus.h>
#include <machine/sid.h>
#include <dev/mscp/mscp.h>
#include <dev/mscp/mscpreg.h>
#include <dev/mscp/mscpvar.h>
#include "ra.h"
#include "mt.h"
#define b_forw	b_hash.le_next
#endif

int	mscp_match __P((struct device *, struct cfdata *, void *));
void	mscp_attach __P((struct device *, struct device *, void *));
void	mscp_start __P((struct	mscp_softc *));
int	mscp_init __P((struct  mscp_softc *));
void	mscp_initds __P((struct mscp_softc *));
int	mscp_waitstep __P((struct mscp_softc *, int, int));

struct	cfattach mscpbus_ca = {
	sizeof(struct mscp_softc), mscp_match, mscp_attach
};

#define	READ_SA		(bus_space_read_2(mi->mi_iot, mi->mi_sah, 0))
#define	READ_IP		(bus_space_read_2(mi->mi_iot, mi->mi_iph, 0))
#define	WRITE_IP(x)	bus_space_write_2(mi->mi_iot, mi->mi_iph, 0, (x))
#define	WRITE_SW(x)	bus_space_write_2(mi->mi_iot, mi->mi_swh, 0, (x))

struct	mscp slavereply;

/*
 * This function is for delay during init. Some MSCP clone card (Dilog)
 * can't handle fast read from its registers, and therefore need
 * a delay between them.
 */

#define DELAYTEN 1000
int
mscp_waitstep(mi, mask, result)
	struct mscp_softc *mi;
	int mask, result;
{
	int	status = 1;

	if ((READ_SA & mask) != result) {
		volatile int count = 0;
		while ((READ_SA & mask) != result) {
			DELAY(10000);
			count += 1;
			if (count > DELAYTEN)
				break;
		}
		if (count > DELAYTEN)
			status = 0;
	}
	return status;
}

int
mscp_match(parent, match, aux)
	struct device *parent;
	struct cfdata *match;
	void *aux;
{
	struct	mscp_attach_args *ma = aux;

#if NRA || NRX
	if (ma->ma_type & MSCPBUS_DISK)
		return 1;
#endif
#if NMT
	if (ma->ma_type & MSCPBUS_TAPE)
		return 1;
#endif
	return 0;
};

void
mscp_attach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct	mscp_attach_args *ma = aux;
	struct	mscp_softc *mi = (void *)self;
	volatile struct mscp *mp;
	volatile int i;
	int	timeout, next = 0;

	mi->mi_mc = ma->ma_mc;
	mi->mi_me = NULL;
	mi->mi_type = ma->ma_type;
	mi->mi_uda = ma->ma_uda;
	mi->mi_dmat = ma->ma_dmat;
	mi->mi_dmam = ma->ma_dmam;
	mi->mi_iot = ma->ma_iot;
	mi->mi_iph = ma->ma_iph;
	mi->mi_sah = ma->ma_sah;
	mi->mi_swh = ma->ma_swh;
	mi->mi_ivec = ma->ma_ivec;
	mi->mi_adapnr = ma->ma_adapnr;
	mi->mi_ctlrnr = ma->ma_ctlrnr;
	*ma->ma_softc = mi;
	/*
	 * Go out to init the bus, so that we can give commands
	 * to its devices.
	 */
	mi->mi_cmd.mri_size = NCMD;
	mi->mi_cmd.mri_desc = mi->mi_uda->mp_ca.ca_cmddsc;
	mi->mi_cmd.mri_ring = mi->mi_uda->mp_cmd;
	mi->mi_rsp.mri_size = NRSP;
	mi->mi_rsp.mri_desc = mi->mi_uda->mp_ca.ca_rspdsc;
	mi->mi_rsp.mri_ring = mi->mi_uda->mp_rsp;
	BUFQ_INIT(&mi->mi_resq);

	if (mscp_init(mi)) {
		printf("%s: can't init, controller hung\n",
		    mi->mi_dev.dv_xname);
		return;
	}
	for (i = 0; i < NCMD; i++) {
		mi->mi_mxiuse |= (1 << i);
		if (bus_dmamap_create(mi->mi_dmat, (64*1024), 16, (64*1024),
		    0, BUS_DMA_NOWAIT, &mi->mi_xi[i].mxi_dmam)) {
			printf("Couldn't alloc dmamap %d\n", i);
			return;
		}
	}
	

#if NRA
	if (ma->ma_type & MSCPBUS_DISK) {
		extern	struct mscp_device ra_device;

		mi->mi_me = &ra_device;
	}
#endif
#if NMT
	if (ma->ma_type & MSCPBUS_TAPE) {
		extern	struct mscp_device mt_device;

		mi->mi_me = &mt_device;
	}
#endif
	/*
	 * Go out and search for sub-units on this MSCP bus,
	 * and call config_found for each found.
	 */
findunit:
	mp = mscp_getcp(mi, MSCP_DONTWAIT);
	if (mp == NULL)
		panic("mscpattach: no packets");
	mp->mscp_opcode = MSCP$K_OP_GTUNT;
	mp->mscp_unit = next;
	mp->mscp_modifier = M_GUM_NEXTUNIT;
	*mp->mscp_addr |= MSCP_OWN | MSCP_INT;
	slavereply.mscp_opcode = 0;

	i = bus_space_read_2(mi->mi_iot, mi->mi_iph, 0);
	mp = &slavereply;
	timeout = 1000;
	while (timeout-- > 0) {
		DELAY(10000);
		if (mp->mscp_opcode)
			goto gotit;
	}
	printf("%s: no response to Get Unit Status request\n",
	    mi->mi_dev.dv_xname);
	return;

gotit:	/*
	 * Got a slave response.  If the unit is there, use it.
	 */
	switch (mp->mscp_status & M_ST_MASK) {

	case M_ST_SUCCESS:	/* worked */
	case M_ST_AVAILABLE:	/* found another drive */
		break;		/* use it */

	case M_ST_OFFLINE:
		/*
		 * Figure out why it is off line.  It may be because
		 * it is nonexistent, or because it is spun down, or
		 * for some other reason.
		 */
		switch (mp->mscp_status & ~M_ST_MASK) {

		case M_OFFLINE_UNKNOWN:
			/*
			 * No such drive, and there are none with
			 * higher unit numbers either, if we are
			 * using M_GUM_NEXTUNIT.
			 */
			mi->mi_ierr = 3;
			return;

		case M_OFFLINE_UNMOUNTED:
			/*
			 * The drive is not spun up.  Use it anyway.
			 *
			 * N.B.: this seems to be a common occurrance
			 * after a power failure.  The first attempt
			 * to bring it on line seems to spin it up
			 * (and thus takes several minutes).  Perhaps
			 * we should note here that the on-line may
			 * take longer than usual.
			 */
			break;

		default:
			/*
			 * In service, or something else equally unusable.
			 */
			printf("%s: unit %d off line: ", mi->mi_dev.dv_xname,
				mp->mscp_unit);
			mscp_printevent((struct mscp *)mp);
			next++;
			goto findunit;
		}
		break;

	default:
		printf("%s: unable to get unit status: ", mi->mi_dev.dv_xname);
		mscp_printevent((struct mscp *)mp);
		return;
	}

	/*
	 * If we get a lower number, we have circulated around all
	 * devices and are finished, otherwise try to find next unit.
	 * We shouldn't ever get this, it's a workaround.
	 */
	if (mp->mscp_unit < next)
		return;

	next = mp->mscp_unit + 1;
	goto findunit;
}


/*
 * The ctlr gets initialised, normally after boot but may also be 
 * done if the ctlr gets in an unknown state. Returns 1 if init
 * fails, 0 otherwise.
 */
int
mscp_init(mi)
	struct	mscp_softc *mi;
{
	struct	mscp *mp;
	volatile int i;
	int	status, count;
	unsigned int j = 0;

	/*
	 * While we are thinking about it, reset the next command
	 * and response indicies.
	 */
	mi->mi_cmd.mri_next = 0;
	mi->mi_rsp.mri_next = 0;

	mi->mi_flags |= MSC_IGNOREINTR;

	if ((mi->mi_type & MSCPBUS_KDB) == 0)
		WRITE_IP(0); /* Kick off */;

	status = mscp_waitstep(mi, MP_STEP1, MP_STEP1);/* Wait to it wakes up */
	if (status == 0)
		return 1; /* Init failed */
	if (READ_SA & MP_ERR) {
		(*mi->mi_mc->mc_saerror)(mi->mi_dev.dv_parent, 0);
		return 1;
	}

	/* step1 */
	WRITE_SW(MP_ERR | (NCMDL2 << 11) | (NRSPL2 << 8) |
	    MP_IE | (mi->mi_ivec >> 2));
	status = mscp_waitstep(mi, STEP1MASK, STEP1GOOD);
	if (status == 0) {
		(*mi->mi_mc->mc_saerror)(mi->mi_dev.dv_parent, 0);
		return 1;
	}

	/* step2 */
	WRITE_SW(((mi->mi_dmam->dm_segs[0].ds_addr & 0xffff) + 
	    offsetof(struct mscp_pack, mp_ca.ca_rspdsc[0])) |
	    (vax_cputype == VAX_780 || vax_cputype == VAX_8600 ? MP_PI : 0));
	status = mscp_waitstep(mi, STEP2MASK, STEP2GOOD(mi->mi_ivec >> 2));
	if (status == 0) {
		(*mi->mi_mc->mc_saerror)(mi->mi_dev.dv_parent, 0);
		return 1;
	}

	/* step3 */
	WRITE_SW((mi->mi_dmam->dm_segs[0].ds_addr >> 16));
	status = mscp_waitstep(mi, STEP3MASK, STEP3GOOD);
	if (status == 0) { 
		(*mi->mi_mc->mc_saerror)(mi->mi_dev.dv_parent, 0);
		return 1;
	}
	i = READ_SA & 0377;
	printf(": version %d model %d\n", i & 15, i >> 4);

#define BURST 4 /* XXX */
	if (mi->mi_type & MSCPBUS_UDA) {
		WRITE_SW(MP_GO | (BURST - 1) << 2);
		printf("%s: DMA burst size set to %d\n", 
		    mi->mi_dev.dv_xname, BURST);
	}
	WRITE_SW(MP_GO);

	mscp_initds(mi);
	mi->mi_flags &= ~MSC_IGNOREINTR;

	/*
	 * Set up all necessary info in the bus softc struct, get a
	 * mscp packet and set characteristics for this controller.
	 */
	mi->mi_credits = MSCP_MINCREDITS + 1;
	mp = mscp_getcp(mi, MSCP_DONTWAIT);

	mi->mi_credits = 0;
	mp->mscp_opcode = MSCP$K_OP_STCON;
	mp->mscp_unit = mp->mscp_modifier = mp->mscp_flags =
	    mp->mscp_sccc.sccc_version = mp->mscp_sccc.sccc_hosttimo = 
	    mp->mscp_sccc.sccc_time = mp->mscp_sccc.sccc_time1 =
	    mp->mscp_sccc.sccc_errlgfl = 0;
	mp->mscp_sccc.sccc_ctlrflags = M_CF_ATTN | M_CF_MISC | M_CF_THIS;
	*mp->mscp_addr |= MSCP_OWN | MSCP_INT;
	i = READ_IP;

	count = 0;
	while (count < DELAYTEN) {
		if (((volatile int)mi->mi_flags & MSC_READY) != 0)
			break;
		if ((j = READ_SA) & MP_ERR)
			goto out;
		DELAY(10000);
		count += 1;
	}
	if (count == DELAYTEN) {
out:
		printf("%s: couldn't set ctlr characteristics, sa=%x\n", 
		    mi->mi_dev.dv_xname, j);
		return 1;
	}
	return 0;
}

/*
 * Initialise the various data structures that control the mscp protocol.
 */
void
mscp_initds(mi)
	struct mscp_softc *mi;
{
	struct mscp_pack *ud = mi->mi_uda;
	struct mscp *mp;
	int i;

	for (i = 0, mp = ud->mp_rsp; i < NRSP; i++, mp++) {
		ud->mp_ca.ca_rspdsc[i] = MSCP_OWN | MSCP_INT |
		    (mi->mi_dmam->dm_segs[0].ds_addr +
		    offsetof(struct mscp_pack, mp_rsp[i].mscp_cmdref));
		mp->mscp_addr = &ud->mp_ca.ca_rspdsc[i];
		mp->mscp_msglen = MSCP_MSGLEN;
	}
	for (i = 0, mp = ud->mp_cmd; i < NCMD; i++, mp++) {
		ud->mp_ca.ca_cmddsc[i] = MSCP_INT |
		    (mi->mi_dmam->dm_segs[0].ds_addr +
		    offsetof(struct mscp_pack, mp_cmd[i].mscp_cmdref));
		mp->mscp_addr = &ud->mp_ca.ca_cmddsc[i];
		mp->mscp_msglen = MSCP_MSGLEN;
		if (mi->mi_type & MSCPBUS_TAPE)
			mp->mscp_vcid = 1;
	}
}

static	void mscp_kickaway(struct mscp_softc *);

void
mscp_intr(mi)
	struct mscp_softc *mi;
{
	struct mscp_pack *ud = mi->mi_uda;

	if (mi->mi_flags & MSC_IGNOREINTR)
		return;
	/*
	 * Check for response and command ring transitions.
	 */
	if (ud->mp_ca.ca_rspint) {
		ud->mp_ca.ca_rspint = 0;
		mscp_dorsp(mi);
	}
	if (ud->mp_ca.ca_cmdint) {
		ud->mp_ca.ca_cmdint = 0;
		MSCP_DOCMD(mi);
	}

	/*
	 * If there are any not-yet-handled request, try them now.
	 */
	if (BUFQ_FIRST(&mi->mi_resq))
		mscp_kickaway(mi);
}

int
mscp_print(aux, name)
	void *aux;
	const char *name;
{
	struct drive_attach_args *da = aux;
	struct	mscp *mp = da->da_mp;
	int type = mp->mscp_guse.guse_mediaid;

	if (name) {
		printf("%c%c", MSCP_MID_CHAR(2, type), MSCP_MID_CHAR(1, type));
		if (MSCP_MID_ECH(0, type))
			printf("%c", MSCP_MID_CHAR(0, type));
		printf("%d at %s drive %d", MSCP_MID_NUM(type), name,
		    mp->mscp_unit);
	}
	return UNCONF;
}

/*
 * common strategy routine for all types of MSCP devices.
 */
void
mscp_strategy(bp, usc)
	struct buf *bp;
	struct device *usc;
{
	struct	mscp_softc *mi = (void *)usc;
	int s = splimp();

	BUFQ_INSERT_TAIL(&mi->mi_resq, bp);
	mscp_kickaway(mi);
	splx(s);
}


void
mscp_kickaway(mi)
	struct	mscp_softc *mi;
{
	struct buf *bp;
	struct	mscp *mp;
	int next;

	while ((bp = BUFQ_FIRST(&mi->mi_resq)) != NULL) {
		/*
		 * Ok; we are ready to try to start a xfer. Get a MSCP packet
		 * and try to start...
		 */
		if ((mp = mscp_getcp(mi, MSCP_DONTWAIT)) == NULL) {
			if (mi->mi_credits > MSCP_MINCREDITS)
				printf("%s: command ring too small\n",
				    mi->mi_dev.dv_parent->dv_xname);
			/*
			 * By some (strange) reason we didn't get a MSCP packet.
			 * Just return and wait for free packets.
			 */
			return;
		}
	
		if ((next = (ffs(mi->mi_mxiuse) - 1)) < 0)
			panic("no mxi buffers");
		mi->mi_mxiuse &= ~(1 << next);
		if (mi->mi_xi[next].mxi_inuse)
			panic("mxi inuse");
		/*
		 * Set up the MSCP packet and ask the ctlr to start.
		 */
		mp->mscp_opcode =
		    (bp->b_flags & B_READ) ? MSCP$K_OP_READ : MSCP$K_OP_WRITE;
		mp->mscp_cmdref = next;
		mi->mi_xi[next].mxi_bp = bp;
		mi->mi_xi[next].mxi_mp = mp;
		mi->mi_xi[next].mxi_inuse = 1;
		bp->b_resid = next;
		(*mi->mi_me->me_fillin)(bp, mp);
		(*mi->mi_mc->mc_go)(mi->mi_dev.dv_parent, &mi->mi_xi[next]);
		BUFQ_REMOVE(&mi->mi_resq, bp);
	}
}

void
mscp_dgo(mi, mxi)
	struct mscp_softc *mi;
	struct mscp_xi *mxi;
{
	volatile int i;
	struct	mscp *mp;

	/*
	 * Fill in the MSCP packet and move the buffer to the I/O wait queue.
	 */
	mp = mxi->mxi_mp;
	mp->mscp_seq.seq_buffer = mxi->mxi_dmam->dm_segs[0].ds_addr;

	*mp->mscp_addr |= MSCP_OWN | MSCP_INT;
	i = READ_IP;
}

#ifdef DIAGNOSTIC
/*
 * Dump the entire contents of an MSCP packet in hex.  Mainly useful
 * for debugging....
 */
void
mscp_hexdump(mp)
	struct mscp *mp;
{
	long *p = (long *) mp;
	int i = mp->mscp_msglen;

	if (i > 256)		/* sanity */
		i = 256;
	i /= sizeof (*p);	/* ASSUMES MULTIPLE OF sizeof(long) */
	while (--i >= 0)
		printf("0x%x ", (int)*p++);
	printf("\n");
}
#endif

/*
 * MSCP error reporting
 */

/*
 * Messages for the various subcodes.
 */
static char unknown_msg[] = "unknown subcode";

/*
 * Subcodes for Success (0)
 */
static char *succ_msgs[] = {
	"normal",		/* 0 */
	"spin down ignored",	/* 1 = Spin-Down Ignored */
	"still connected",	/* 2 = Still Connected */
	unknown_msg,
	"dup. unit #",		/* 4 = Duplicate Unit Number */
	unknown_msg,
	unknown_msg,
	unknown_msg,
	"already online",	/* 8 = Already Online */
	unknown_msg,
	unknown_msg,
	unknown_msg,
	unknown_msg,
	unknown_msg,
	unknown_msg,
	unknown_msg,
	"still online",		/* 16 = Still Online */
};

/*
 * Subcodes for Invalid Command (1)
 */
static char *icmd_msgs[] = {
	"invalid msg length",	/* 0 = Invalid Message Length */
};

/*
 * Subcodes for Command Aborted (2)
 */
/* none known */

/*
 * Subcodes for Unit Offline (3)
 */
static char *offl_msgs[] = {
	"unknown drive",	/* 0 = Unknown, or online to other ctlr */
	"not mounted",		/* 1 = Unmounted, or RUN/STOP at STOP */
	"inoperative",		/* 2 = Unit Inoperative */
	unknown_msg,
	"duplicate",		/* 4 = Duplicate Unit Number */
	unknown_msg,
	unknown_msg,
	unknown_msg,
	"in diagnosis",		/* 8 = Disabled by FS or diagnostic */
};

/*
 * Subcodes for Unit Available (4)
 */
/* none known */

/*
 * Subcodes for Media Format Error (5)
 */
static char *media_fmt_msgs[] = {
	"fct unread - edc",	/* 0 = FCT unreadable */
	"invalid sector header",/* 1 = Invalid Sector Header */
	"not 512 sectors",	/* 2 = Not 512 Byte Sectors */
	"not formatted",	/* 3 = Not Formatted */
	"fct ecc",		/* 4 = FCT ECC */
};

/*
 * Subcodes for Write Protected (6)
 * N.B.:  Code 6 subcodes are 7 bits higher than other subcodes
 * (i.e., bits 12-15).
 */
static char *wrprot_msgs[] = {
	unknown_msg,
	"software",		/* 1 = Software Write Protect */
	"hardware",		/* 2 = Hardware Write Protect */
};

/*
 * Subcodes for Compare Error (7)
 */
/* none known */

/*
 * Subcodes for Data Error (8)
 */
static char *data_msgs[] = {
	"forced error",		/* 0 = Forced Error (software) */
	unknown_msg,
	"header compare",	/* 2 = Header Compare Error */
	"sync timeout",		/* 3 = Sync Timeout Error */
	unknown_msg,
	unknown_msg,
	unknown_msg,
	"uncorrectable ecc",	/* 7 = Uncorrectable ECC */
	"1 symbol ecc",		/* 8 = 1 bit ECC */
	"2 symbol ecc",		/* 9 = 2 bit ECC */
	"3 symbol ecc",		/* 10 = 3 bit ECC */
	"4 symbol ecc",		/* 11 = 4 bit ECC */
	"5 symbol ecc",		/* 12 = 5 bit ECC */
	"6 symbol ecc",		/* 13 = 6 bit ECC */
	"7 symbol ecc",		/* 14 = 7 bit ECC */
	"8 symbol ecc",		/* 15 = 8 bit ECC */
};

/*
 * Subcodes for Host Buffer Access Error (9)
 */
static char *host_buffer_msgs[] = {
	unknown_msg,
	"odd xfer addr",	/* 1 = Odd Transfer Address */
	"odd xfer count",	/* 2 = Odd Transfer Count */
	"non-exist. memory",	/* 3 = Non-Existent Memory */
	"memory parity",	/* 4 = Memory Parity Error */
};

/*
 * Subcodes for Controller Error (10)
 */
static char *cntlr_msgs[] = {
	unknown_msg,
	"serdes overrun",	/* 1 = Serialiser/Deserialiser Overrun */
	"edc",			/* 2 = Error Detection Code? */
	"inconsistant internal data struct",/* 3 = Internal Error */
};

/*
 * Subcodes for Drive Error (11)
 */
static char *drive_msgs[] = {
	unknown_msg,
	"sdi command timeout",	/* 1 = SDI Command Timeout */
	"ctlr detected protocol",/* 2 = Controller Detected Protocol Error */
	"positioner",		/* 3 = Positioner Error */
	"lost rd/wr ready",	/* 4 = Lost R/W Ready Error */
	"drive clock dropout",	/* 5 = Lost Drive Clock */
	"lost recvr ready",	/* 6 = Lost Receiver Ready */
	"drive detected error", /* 7 = Drive Error */
	"ctlr detected pulse or parity",/* 8 = Pulse or Parity Error */
};

/*
 * The following table correlates message codes with the
 * decoding strings.
 */
struct code_decode {
	char	*cdc_msg;
	int	cdc_nsubcodes;
	char	**cdc_submsgs;
} code_decode[] = {
#define SC(m)	sizeof (m) / sizeof (m[0]), m
	{"success",			SC(succ_msgs)},
	{"invalid command",		SC(icmd_msgs)},
	{"command aborted",		0, 0},
	{"unit offline",		SC(offl_msgs)},
	{"unit available",		0, 0},
	{"media format error",		SC(media_fmt_msgs)},
	{"write protected",		SC(wrprot_msgs)},
	{"compare error",		0, 0},
	{"data error",			SC(data_msgs)},
	{"host buffer access error",	SC(host_buffer_msgs)},
	{"controller error",		SC(cntlr_msgs)},
	{"drive error",			SC(drive_msgs)},
#undef SC
};

/*
 * Print the decoded error event from an MSCP error datagram.
 */
void
mscp_printevent(mp)
	struct mscp *mp;
{
	int event = mp->mscp_event;
	struct code_decode *cdc;
	int c, sc;
	char *cm, *scm;

	/*
	 * The code is the lower six bits of the event number (aka
	 * status).  If that is 6 (write protect), the subcode is in
	 * bits 12-15; otherwise, it is in bits 5-11.
	 * I WONDER WHAT THE OTHER BITS ARE FOR.  IT SURE WOULD BE
	 * NICE IF DEC SOLD DOCUMENTATION FOR THEIR OWN CONTROLLERS.
	 */
	c = event & M_ST_MASK;
	sc = (c != 6 ? event >> 5 : event >> 12) & 0x7ff;
	if (c >= sizeof code_decode / sizeof code_decode[0])
		cm = "- unknown code", scm = "??";
	else {
		cdc = &code_decode[c];
		cm = cdc->cdc_msg;
		if (sc >= cdc->cdc_nsubcodes)
			scm = unknown_msg;
		else
			scm = cdc->cdc_submsgs[sc];
	}
	printf(" %s (%s) (code %d, subcode %d)\n", cm, scm, c, sc);
}

static char *codemsg[16] = {
	"lbn", "code 1", "code 2", "code 3",
	"code 4", "code 5", "rbn", "code 7",
	"code 8", "code 9", "code 10", "code 11",
	"code 12", "code 13", "code 14", "code 15"
};
/*
 * Print the code and logical block number for an error packet.
 * THIS IS PROBABLY PECULIAR TO DISK DRIVES.  IT SURE WOULD BE
 * NICE IF DEC SOLD DOCUMENTATION FOR THEIR OWN CONTROLLERS.
 */
int
mscp_decodeerror(name, mp, mi)
	char *name;
	struct mscp *mp;
	struct mscp_softc *mi;
{
	int issoft;
	/* 
	 * We will get three sdi errors of type 11 after autoconfig
	 * is finished; depending of searching for non-existing units.
	 * How can we avoid this???
	 */
	if (((mp->mscp_event & M_ST_MASK) == 11) && (mi->mi_ierr++ < 3))
		return 1;
	/*
	 * For bad blocks, mp->mscp_erd.erd_hdr identifies a code and
	 * the logical block number.  Code 0 is a regular block; code 6
	 * is a replacement block.  The remaining codes are currently
	 * undefined.  The code is in the upper four bits of the header
	 * (bits 0-27 are the lbn).
	 */
	issoft = mp->mscp_flags & (M_LF_SUCC | M_LF_CONT);
#define BADCODE(h)	(codemsg[(unsigned)(h) >> 28])
#define BADLBN(h)	((h) & 0xfffffff)

	printf("%s: drive %d %s error datagram%s:", name, mp->mscp_unit,
		issoft ? "soft" : "hard",
		mp->mscp_flags & M_LF_CONT ? " (continuing)" : "");
	switch (mp->mscp_format & 0377) {

	case M_FM_CTLRERR:	/* controller error */
		break;

	case M_FM_BUSADDR:	/* host memory access error */
		printf(" memory addr 0x%x:", (int)mp->mscp_erd.erd_busaddr);
		break;

	case M_FM_DISKTRN:
		printf(" unit %d: level %d retry %d, %s %d:",
			mp->mscp_unit,
			mp->mscp_erd.erd_level, mp->mscp_erd.erd_retry,
			BADCODE(mp->mscp_erd.erd_hdr),
			(int)BADLBN(mp->mscp_erd.erd_hdr));
		break;

	case M_FM_SDI:
		printf(" unit %d: %s %d:", mp->mscp_unit,
			BADCODE(mp->mscp_erd.erd_hdr),
			(int)BADLBN(mp->mscp_erd.erd_hdr));
		break;

	case M_FM_SMLDSK:
		printf(" unit %d: small disk error, cyl %d:",
			mp->mscp_unit, mp->mscp_erd.erd_sdecyl);
		break;

	case M_FM_TAPETRN:
		printf(" unit %d: tape transfer error, grp 0x%x event 0%o:",
		    mp->mscp_unit, mp->mscp_erd.erd_sdecyl, mp->mscp_event);
		break;

	case M_FM_STIERR:
		printf(" unit %d: STI error, event 0%o:", mp->mscp_unit,
		    mp->mscp_event);
		break;

	default:
		printf(" unit %d: unknown error, format 0x%x:",
			mp->mscp_unit, mp->mscp_format);
	}
	mscp_printevent(mp);
	return 0;
#undef BADCODE
#undef BADLBN
}
/*
 * RA disk device driver
 * RX MSCP floppy disk device driver
 */

/*
 * TODO
 *	write bad block forwarding code
 */

#include <sys/param.h>
#include <sys/buf.h>
#include <sys/device.h>
#include <sys/disk.h>
#include <sys/disklabel.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/reboot.h>
#include <sys/proc.h>
#include <sys/systm.h>

#include <ufs/ufs/dinode.h>
#include <ufs/ffs/fs.h>

#include <machine/bus.h>
#include <machine/cpu.h>

#include <dev/mscp/mscp.h>
#include <dev/mscp/mscpreg.h>
#include <dev/mscp/mscpvar.h>

#include "locators.h"
#include "ioconf.h"
#include "ra.h"

#define RAMAJOR 9	/* RA major device number XXX */

/*
 * Drive status, per drive
 */
struct ra_softc {
	struct	device ra_dev;	/* Autoconf struct */
	struct	disk ra_disk;
	int	ra_state;	/* open/closed state */
	u_long	ra_mediaid;	/* media id */
	int	ra_hwunit;	/* Hardware unit number */
	int	ra_havelabel;	/* true if we have a label */
	int	ra_wlabel;	/* label sector is currently writable */
};

#define rx_softc ra_softc

void	rxattach __P((struct device *, struct device *, void *));
int	rx_putonline __P((struct rx_softc *));
void	rrmakelabel __P((struct disklabel *, long));

#if NRA

int	ramatch __P((struct device *, struct cfdata *, void *));
void	raattach __P((struct device *, struct device *, void *));
int	raopen __P((dev_t, int, int, struct proc *));
int	raclose __P((dev_t, int, int, struct proc *));
void	rastrategy __P((struct buf *));
int	raread __P((dev_t, struct uio *));
int	rawrite __P((dev_t, struct uio *));
int	raioctl __P((dev_t, int, caddr_t, int, struct proc *));
int	radump __P((dev_t, daddr_t, caddr_t, size_t));
int	rasize __P((dev_t));
int	ra_putonline __P((struct ra_softc *));

struct	cfattach ra_ca = {
	sizeof(struct ra_softc), ramatch, rxattach
};

/*
 * More driver definitions, for generic MSCP code.
 */

int
ramatch(parent, cf, aux)
	struct	device *parent;
	struct	cfdata *cf;
	void	*aux;
{
	struct	drive_attach_args *da = aux;
	struct	mscp *mp = da->da_mp;

	if ((da->da_typ & MSCPBUS_DISK) == 0)
		return 0;
	if (cf->cf_loc[MSCPBUSCF_DRIVE] != MSCPBUSCF_DRIVE_DEFAULT &&
	    cf->cf_loc[MSCPBUSCF_DRIVE] != mp->mscp_unit)
		return 0;
	/*
	 * Check if this disk is a floppy; then don't configure it.
	 * Seems to be a safe way to test it per Chris Torek.
	 */
	if (MSCP_MID_ECH(1, mp->mscp_guse.guse_mediaid) == 'X' - '@')
		return 0;
	return 1;
}

/* 
 * (Try to) put the drive online. This is done the first time the
 * drive is opened, or if it har fallen offline.
 */
int
ra_putonline(ra)
	struct ra_softc *ra;
{
	struct	disklabel *dl;
	char *msg;

	if (rx_putonline(ra) != MSCP_DONE)
		return MSCP_FAILED;

	dl = ra->ra_disk.dk_label;

	ra->ra_state = DK_RDLABEL;
	printf("%s", ra->ra_dev.dv_xname);
	if ((msg = readdisklabel(MAKEDISKDEV(RAMAJOR, ra->ra_dev.dv_unit,
	    RAW_PART), rastrategy, dl, NULL)) != NULL)
		printf(": %s", msg);
	else {
		ra->ra_havelabel = 1;
		ra->ra_state = DK_OPEN;
	}

	printf(": size %d sectors\n", dl->d_secperunit);

	return MSCP_DONE;
}

/*
 * Open a drive.
 */
/*ARGSUSED*/
int
raopen(dev, flag, fmt, p)
	dev_t dev;
	int flag, fmt;
	struct	proc *p;
{
	struct ra_softc *ra;
	int part, unit, mask;
	/*
	 * Make sure this is a reasonable open request.
	 */
	unit = DISKUNIT(dev);
	if (unit >= ra_cd.cd_ndevs)
		return ENXIO;
	ra = ra_cd.cd_devs[unit];
	if (ra == 0)
		return ENXIO;

	/*
	 * If this is the first open; we must first try to put
	 * the disk online (and read the label).
	 */
	if (ra->ra_state == DK_CLOSED)
		if (ra_putonline(ra) == MSCP_FAILED)
			return ENXIO;

	/* If the disk has no label; allow writing everywhere */
	if (ra->ra_havelabel == 0)
		ra->ra_wlabel = 1;

	part = DISKPART(dev);
	if (part >= ra->ra_disk.dk_label->d_npartitions)
		return ENXIO;

	/*
	 * Wait for the state to settle
	 */
#if notyet
	while (ra->ra_state != DK_OPEN)
		if ((error = tsleep((caddr_t)ra, (PZERO + 1) | PCATCH,
		    devopn, 0))) {
			splx(s);
			return (error);
		}
#endif

	mask = 1 << part;

	switch (fmt) {
	case S_IFCHR:
		ra->ra_disk.dk_copenmask |= mask;
		break;
	case S_IFBLK:
		ra->ra_disk.dk_bopenmask |= mask;
		break;
	}
	ra->ra_disk.dk_openmask |= mask;
	return 0;
}

/* ARGSUSED */
int
raclose(dev, flags, fmt, p)
	dev_t dev;
	int flags, fmt;
	struct	proc *p;
{
	int unit = DISKUNIT(dev);
	struct ra_softc *ra = ra_cd.cd_devs[unit];
	int mask = (1 << DISKPART(dev));

	switch (fmt) {
	case S_IFCHR:
		ra->ra_disk.dk_copenmask &= ~mask;
		break;
	case S_IFBLK:
		ra->ra_disk.dk_bopenmask &= ~mask;
		break;
	}
	ra->ra_disk.dk_openmask =
	    ra->ra_disk.dk_copenmask | ra->ra_disk.dk_bopenmask;

	/*
	 * Should wait for I/O to complete on this partition even if
	 * others are open, but wait for work on blkflush().
	 */
#if notyet
	if (ra->ra_openpart == 0) {
		s = splimp();
		while (BUFQ_FIRST(&udautab[unit]) != NULL)
			(void) tsleep(&udautab[unit], PZERO - 1,
			    "raclose", 0);
		splx(s);
		ra->ra_state = CLOSED;
		ra->ra_wlabel = 0;
	}
#endif
	return (0);
}

/*
 * Queue a transfer request, and if possible, hand it to the controller.
 */
void
rastrategy(bp)
	struct buf *bp;
{
	int unit;
	struct ra_softc *ra;
	/*
	 * Make sure this is a reasonable drive to use.
	 */
	unit = DISKUNIT(bp->b_dev);
	if (unit > ra_cd.cd_ndevs || (ra = ra_cd.cd_devs[unit]) == NULL) {
		bp->b_error = ENXIO;
		bp->b_flags |= B_ERROR;
		goto done;
	}
	/*
	 * If drive is open `raw' or reading label, let it at it.
	 */
	if (ra->ra_state == DK_RDLABEL) {
		mscp_strategy(bp, ra->ra_dev.dv_parent);
		return;
	}

	/* If disk is not online, try to put it online */
	if (ra->ra_state == DK_CLOSED)
		if (ra_putonline(ra) == MSCP_FAILED) {
			bp->b_flags |= B_ERROR;
			bp->b_error = EIO;
			goto done;
		}

	/*
	 * Determine the size of the transfer, and make sure it is
	 * within the boundaries of the partition.
	 */
	if (bounds_check_with_label(bp, ra->ra_disk.dk_label,
	    ra->ra_wlabel) <= 0)
		goto done;

	/* Make some statistics... /bqt */
	ra->ra_disk.dk_xfer++;
	ra->ra_disk.dk_bytes += bp->b_bcount;
	mscp_strategy(bp, ra->ra_dev.dv_parent);
	return;

done:
	biodone(bp);
}

int
raread(dev, uio)
	dev_t dev;
	struct uio *uio;
{

	return (physio(rastrategy, NULL, dev, B_READ, minphys, uio));
}

int
rawrite(dev, uio)
	dev_t dev;
	struct uio *uio;
{

	return (physio(rastrategy, NULL, dev, B_WRITE, minphys, uio));
}

/*
 * I/O controls.
 */
int
raioctl(dev, cmd, data, flag, p)
	dev_t dev;
	int cmd;
	caddr_t data;
	int flag;
	struct proc *p;
{
	int unit = DISKUNIT(dev);
	struct disklabel *lp, *tp;
	struct ra_softc *ra = ra_cd.cd_devs[unit];
	int error = 0;
#ifdef __HAVE_OLD_DISKLABEL
	struct disklabel newlabel;
#endif

	lp = ra->ra_disk.dk_label;

	switch (cmd) {

	case DIOCGDINFO:
		bcopy(lp, data, sizeof (struct disklabel));
		break;
#ifdef __HAVE_OLD_DISKLABEL
	case ODIOCGDINFO:
		bcopy(lp, &newlabel, sizeof disklabel);
		if (newlabel.d_npartitions > OLDMAXPARTITIONS)
			return ENOTTY;
		bcopy(&newlabel, data, sizeof (struct olddisklabel));
		break;
#endif

	case DIOCGPART:
		((struct partinfo *)data)->disklab = lp;
		((struct partinfo *)data)->part =
		    &lp->d_partitions[DISKPART(dev)];
		break;

	case DIOCWDINFO:
	case DIOCSDINFO:
#ifdef __HAVE_OLD_DISKLABEL
	case ODIOCWDINFO:
	case ODIOCSDINFO:
		if (cmd == ODIOCSDINFO || xfer == ODIOCWDINFO) {
			memset(&newlabel, 0, sizeof newlabel);
			memcpy(&newlabel, data, sizeof (struct olddisklabel));
			tp = &newlabel;
		} else
#endif
		tp = (struct disklabel *)data;

		if ((flag & FWRITE) == 0)
			error = EBADF;
		else {
			error = setdisklabel(lp, tp, 0, 0);
			if ((error == 0) && (cmd == DIOCWDINFO
#ifdef __HAVE_OLD_DISKLABEL
			    || cmd == ODIOCWDINFO
#else
			    )) {
#endif
				ra->ra_wlabel = 1;
				error = writedisklabel(dev, rastrategy, lp,0);
				ra->ra_wlabel = 0;
			}
		}
		break;

	case DIOCWLABEL:
		if ((flag & FWRITE) == 0)
			error = EBADF;
		else
			ra->ra_wlabel = 1;
		break;

	case DIOCGDEFLABEL:
#ifdef __HAVE_OLD_DISKLABEL
	case ODIOCGDEFLABEL:
		if (cmd == ODIOCGDEFLABEL)
			tp = &newlabel;
		else
#else
		tp = (struct disklabel *)data;
#endif
		bzero(tp, sizeof(struct disklabel));
		tp->d_secsize = lp->d_secsize;
		tp->d_nsectors = lp->d_nsectors;
		tp->d_ntracks = lp->d_ntracks;
		tp->d_ncylinders = lp->d_ncylinders;
		tp->d_secpercyl = lp->d_secpercyl;
		tp->d_secperunit = lp->d_secperunit;
		tp->d_type = DTYPE_MSCP;
		tp->d_rpm = 3600;
		rrmakelabel(tp, ra->ra_mediaid);
#ifdef __HAVE_OLD_DISKLABEL
		if (cmd == ODIOCGDEFLABEL) {
			if (tp->d_npartitions > OLDMAXPARTITIONS)
				return ENOTTY;
			memcpy(data, tp, sizeof (struct olddisklabel));
		}
#endif
		break;

	default:
		error = ENOTTY;
		break;
	}
	return (error);
}


int
radump(dev, blkno, va, size)
	dev_t	dev;
	daddr_t blkno;
	caddr_t va;
	size_t	size;
{
	return ENXIO;
}

/*
 * Return the size of a partition, if known, or -1 if not.
 */
int
rasize(dev)
	dev_t dev;
{
	int unit = DISKUNIT(dev);
	struct ra_softc *ra;

	if (unit >= ra_cd.cd_ndevs || ra_cd.cd_devs[unit] == 0)
		return -1;

	ra = ra_cd.cd_devs[unit];

	if (ra->ra_state == DK_CLOSED)
		if (ra_putonline(ra) == MSCP_FAILED)
			return -1;

	return ra->ra_disk.dk_label->d_partitions[DISKPART(dev)].p_size *
	    (ra->ra_disk.dk_label->d_secsize / DEV_BSIZE);
}

#endif /* NRA */

#if NRX

int	rxmatch __P((struct device *, struct cfdata *, void *));
int	rxopen __P((dev_t, int, int, struct proc *));
int	rxclose __P((dev_t, int, int, struct proc *));
void	rxstrategy __P((struct buf *));
int	rxread __P((dev_t, struct uio *));
int	rxwrite __P((dev_t, struct uio *));
int	rxioctl __P((dev_t, int, caddr_t, int, struct proc *));
int	rxdump __P((dev_t, daddr_t, caddr_t, size_t));
int	rxsize __P((dev_t));

struct	cfattach rx_ca = {
	sizeof(struct rx_softc), rxmatch, rxattach
};

/*
 * More driver definitions, for generic MSCP code.
 */

int
rxmatch(parent, cf, aux)
	struct	device *parent;
	struct	cfdata *cf;
	void	*aux;
{
	struct	drive_attach_args *da = aux;
	struct	mscp *mp = da->da_mp;

	if ((da->da_typ & MSCPBUS_DISK) == 0)
		return 0;
	if (cf->cf_loc[MSCPBUSCF_DRIVE] != MSCPBUSCF_DRIVE_DEFAULT &&
	    cf->cf_loc[MSCPBUSCF_DRIVE] != mp->mscp_unit)
		return 0;
	/*
	 * Check if this disk is a floppy; then configure it.
	 * Seems to be a safe way to test it per Chris Torek.
	 */
	if (MSCP_MID_ECH(1, mp->mscp_guse.guse_mediaid) == 'X' - '@')
		return 1;
	return 0;
}

#endif /* NRX */

/*
 * The attach routine only checks and prints drive type.
 * Bringing the disk online is done when the disk is accessed
 * the first time. 
 */
void
rxattach(parent, self, aux)
	struct	device *parent, *self;
	void	*aux; 
{
	struct	rx_softc *rx = (void *)self;
	struct	drive_attach_args *da = aux;
	struct	mscp *mp = da->da_mp;
	struct	mscp_softc *mi = (void *)parent;
	struct	disklabel *dl;

	rx->ra_mediaid = mp->mscp_guse.guse_mediaid;
	rx->ra_state = DK_CLOSED;
	rx->ra_hwunit = mp->mscp_unit;
	mi->mi_dp[mp->mscp_unit] = self;

	rx->ra_disk.dk_name = rx->ra_dev.dv_xname;
	disk_attach((struct disk *)&rx->ra_disk);

	/* Fill in what we know. The actual size is gotten later */
	dl = rx->ra_disk.dk_label;

	dl->d_secsize = DEV_BSIZE;
	dl->d_nsectors = mp->mscp_guse.guse_nspt;
	dl->d_ntracks = mp->mscp_guse.guse_ngpc * mp->mscp_guse.guse_group;
	dl->d_secpercyl = dl->d_nsectors * dl->d_ntracks;
	disk_printtype(mp->mscp_unit, mp->mscp_guse.guse_mediaid);
#ifdef DEBUG
	printf("%s: nspt %d group %d ngpc %d rct %d nrpt %d nrct %d\n",
	    self->dv_xname, mp->mscp_guse.guse_nspt, mp->mscp_guse.guse_group,
	    mp->mscp_guse.guse_ngpc, mp->mscp_guse.guse_rctsize,
	    mp->mscp_guse.guse_nrpt, mp->mscp_guse.guse_nrct);
#endif
}

/* 
 * (Try to) put the drive online. This is done the first time the
 * drive is opened, or if it har fallen offline.
 */
int
rx_putonline(rx)
	struct rx_softc *rx;
{
	struct	mscp *mp;
	struct	mscp_softc *mi = (struct mscp_softc *)rx->ra_dev.dv_parent;
	volatile int i;

	rx->ra_state = DK_CLOSED;
	mp = mscp_getcp(mi, MSCP_WAIT);
	mp->mscp_opcode = M_OP_ONLINE;
	mp->mscp_unit = rx->ra_hwunit;
	mp->mscp_cmdref = 1;
	*mp->mscp_addr |= MSCP_OWN | MSCP_INT;

	/* Poll away */
	i = bus_space_read_2(mi->mi_iot, mi->mi_iph, 0);
	if (tsleep(&rx->ra_dev.dv_unit, PRIBIO, "rxonline", 100*100))
		rx->ra_state = DK_CLOSED;

	if (rx->ra_state == DK_CLOSED)
		return MSCP_FAILED;

	return MSCP_DONE;
}

#if NRX

/*
 * Open a drive.
 */
/*ARGSUSED*/
int
rxopen(dev, flag, fmt, p)
	dev_t dev;
	int flag, fmt;
	struct	proc *p;
{
	struct rx_softc *rx;
	int unit;

	/*
	 * Make sure this is a reasonable open request.
	 */
	unit = DISKUNIT(dev);
	if (unit >= rx_cd.cd_ndevs)
		return ENXIO;
	rx = rx_cd.cd_devs[unit];
	if (rx == 0)
		return ENXIO;

	/*
	 * If this is the first open; we must first try to put
	 * the disk online (and read the label).
	 */
	if (rx->ra_state == DK_CLOSED)
		if (rx_putonline(rx) == MSCP_FAILED)
			return ENXIO;

	return 0;
}

/* ARGSUSED */
int
rxclose(dev, flags, fmt, p)
	dev_t dev;
	int flags, fmt;
	struct	proc *p;
{
	return (0);
}

/*
 * Queue a transfer request, and if possible, hand it to the controller.
 *
 * This routine is broken into two so that the internal version
 * udastrat1() can be called by the (nonexistent, as yet) bad block
 * revectoring routine.
 */
void
rxstrategy(bp)
	struct buf *bp;
{
	int unit;
	struct rx_softc *rx;

	/*
	 * Make sure this is a reasonable drive to use.
	 */
	unit = DISKUNIT(bp->b_dev);
	if (unit > rx_cd.cd_ndevs || (rx = rx_cd.cd_devs[unit]) == NULL) {
		bp->b_error = ENXIO;
		bp->b_flags |= B_ERROR;
		goto done;
	}

	/* If disk is not online, try to put it online */
	if (rx->ra_state == DK_CLOSED)
		if (rx_putonline(rx) == MSCP_FAILED) {
			bp->b_flags |= B_ERROR;
			bp->b_error = EIO;
			goto done;
		}

	/*
	 * Determine the size of the transfer, and make sure it is
	 * within the boundaries of the partition.
	 */
	if (bp->b_blkno >= rx->ra_disk.dk_label->d_secperunit) {
		bp->b_resid = bp->b_bcount;
		goto done;
	}

	/* Make some statistics... /bqt */
	rx->ra_disk.dk_xfer++;
	rx->ra_disk.dk_bytes += bp->b_bcount;
	mscp_strategy(bp, rx->ra_dev.dv_parent);
	return;

done:
	biodone(bp);
}

int
rxread(dev, uio)
	dev_t dev;
	struct uio *uio;
{

	return (physio(rxstrategy, NULL, dev, B_READ, minphys, uio));
}

int
rxwrite(dev, uio)
	dev_t dev;
	struct uio *uio;
{

	return (physio(rxstrategy, NULL, dev, B_WRITE, minphys, uio));
}

/*
 * I/O controls.
 */
int
rxioctl(dev, cmd, data, flag, p)
	dev_t dev;
	int cmd;
	caddr_t data;
	int flag;
	struct proc *p;
{
	int unit = DISKUNIT(dev);
	struct disklabel *lp;
	struct rx_softc *rx = rx_cd.cd_devs[unit];
	int error = 0;

	lp = rx->ra_disk.dk_label;

	switch (cmd) {

	case DIOCGDINFO:
		bcopy(lp, data, sizeof (struct disklabel));
		break;

	case DIOCGPART:
		((struct partinfo *)data)->disklab = lp;
		((struct partinfo *)data)->part =
		    &lp->d_partitions[DISKPART(dev)];
		break;


	case DIOCWDINFO:
	case DIOCSDINFO:
	case DIOCWLABEL:
		break;

	default:
		error = ENOTTY;
		break;
	}
	return (error);
}

int
rxdump(dev, blkno, va, size)
	dev_t dev;
	daddr_t blkno;
	caddr_t va;
	size_t size;
{

	/* Not likely. */
	return ENXIO;
}

int
rxsize(dev)
	dev_t dev;
{

	return -1;
}

#endif /* NRX */

void	rrdgram __P((struct device *, struct mscp *, struct mscp_softc *));
void	rriodone __P((struct device *, struct buf *));
int	rronline __P((struct device *, struct mscp *));
int	rrgotstatus __P((struct device *, struct mscp *));
void	rrreplace __P((struct device *, struct mscp *));
int	rrioerror __P((struct device *, struct mscp *, struct buf *));
void	rrfillin __P((struct buf *, struct mscp *));
void	rrbb __P((struct device *, struct mscp *, struct buf *));


struct	mscp_device ra_device = {
	rrdgram,
	rriodone,
	rronline,
	rrgotstatus,
	rrreplace,
	rrioerror,
	rrbb,
	rrfillin,
};

/*
 * Handle an error datagram.
 * This can come from an unconfigured drive as well.
 */	
void	    
rrdgram(usc, mp, mi)
	struct device *usc;
	struct mscp *mp; 
	struct mscp_softc *mi;
{	 
	if (mscp_decodeerror(usc == NULL?"unconf disk" : usc->dv_xname, mp, mi))
		return;	 
	/*
	 * SDI status information bytes 10 and 11 are the microprocessor
	 * error code and front panel code respectively.  These vary per
	 * drive type and are printed purely for field service information.
	 */
	if (mp->mscp_format == M_FM_SDI) 
		printf("\tsdi uproc error code 0x%x, front panel code 0x%x\n",
			mp->mscp_erd.erd_sdistat[10],
			mp->mscp_erd.erd_sdistat[11]);
}

void	
rriodone(usc, bp)
	struct device *usc;
	struct buf *bp;
{

	biodone(bp);
}

/*
 * A drive came on line.  Check its type and size.  Return DONE if
 * we think the drive is truly on line.	 In any case, awaken anyone
 * sleeping on the drive on-line-ness.
 */
int
rronline(usc, mp)
	struct device *usc;
	struct mscp *mp;
{
	struct rx_softc *rx = (struct rx_softc *)usc;
	struct disklabel *dl;

	wakeup((caddr_t)&usc->dv_unit);
	if ((mp->mscp_status & M_ST_MASK) != M_ST_SUCCESS) {
		printf("%s: attempt to bring on line failed: ", usc->dv_xname);
		mscp_printevent(mp);
		return (MSCP_FAILED);
	}

	rx->ra_state = DK_OPEN;
 
	dl = rx->ra_disk.dk_label;
	dl->d_secperunit = (daddr_t)mp->mscp_onle.onle_unitsize;

	if (dl->d_secpercyl) {
		dl->d_ncylinders = dl->d_secperunit/dl->d_secpercyl;
		dl->d_type = DTYPE_MSCP;
		dl->d_rpm = 3600;
	} else {
		dl->d_type = DTYPE_FLOPPY;
		dl->d_rpm = 300;
	}
	rrmakelabel(dl, rx->ra_mediaid);

	return (MSCP_DONE);
}

void
rrmakelabel(dl, type)
	struct disklabel *dl;
	long type;
{
	int n, p = 0;

	dl->d_bbsize = BBSIZE;
	dl->d_sbsize = SBSIZE;

	/* Create the disk name for disklabel. Phew... */
	dl->d_typename[p++] = MSCP_MID_CHAR(2, type);
	dl->d_typename[p++] = MSCP_MID_CHAR(1, type);
	if (MSCP_MID_ECH(0, type))
		dl->d_typename[p++] = MSCP_MID_CHAR(0, type);
	n = MSCP_MID_NUM(type);
	if (n > 99) {
		dl->d_typename[p++] = '1';
		n -= 100;
	}
	if (n > 9) {
		dl->d_typename[p++] = (n / 10) + '0';
		n %= 10;
	}
	dl->d_typename[p++] = n + '0';
	dl->d_typename[p] = 0;
	dl->d_npartitions = MAXPARTITIONS;
	dl->d_partitions[0].p_size = dl->d_partitions[2].p_size =
	    dl->d_secperunit;
	dl->d_partitions[0].p_offset = dl->d_partitions[2].p_offset = 0;
	dl->d_interleave = dl->d_headswitch = 1;
	dl->d_magic = dl->d_magic2 = DISKMAGIC;
	dl->d_checksum = dkcksum(dl);
}

/*	
 * We got some (configured) unit's status.  Return DONE if it succeeded.
 */
int
rrgotstatus(usc, mp)
	struct device *usc;
	struct mscp *mp;
{	
	if ((mp->mscp_status & M_ST_MASK) != M_ST_SUCCESS) {
		printf("%s: attempt to get status failed: ", usc->dv_xname);
		mscp_printevent(mp);
		return (MSCP_FAILED);
	}
	/* record for (future) bad block forwarding and whatever else */
#ifdef notyet
	uda_rasave(ui->ui_unit, mp, 1);
#endif
	return (MSCP_DONE);
}

/*	
 * A replace operation finished.
 */
/*ARGSUSED*/
void	
rrreplace(usc, mp)
	struct device *usc;
	struct mscp *mp;
{

	panic("udareplace");
}

/*
 * A transfer failed.  We get a chance to fix or restart it.
 * Need to write the bad block forwaring code first....
 */
/*ARGSUSED*/
int 
rrioerror(usc, mp, bp)
	struct device *usc;
	struct mscp *mp;
	struct buf *bp;
{
	struct ra_softc *ra = (void *)usc;
	int code = mp->mscp_event;

	switch (code & M_ST_MASK) {
	/* The unit has fallen offline. Try to figure out why. */
	case M_ST_OFFLINE:
		bp->b_flags |= B_ERROR;
		bp->b_error = EIO;
		ra->ra_state = DK_CLOSED;
		if (code & M_OFFLINE_UNMOUNTED)
			printf("%s: not mounted/spun down\n", usc->dv_xname);
		if (code & M_OFFLINE_DUPLICATE)
			printf("%s: duplicate unit number!!!\n", usc->dv_xname);
		return MSCP_DONE;

	case M_ST_AVAILABLE:
		ra->ra_state = DK_CLOSED; /* Force another online */
		return MSCP_DONE;

	default:
		printf("%s:", usc->dv_xname);
		break;
	}
	return (MSCP_FAILED);
}

/*
 * Fill in disk addresses in a mscp packet waiting for transfer.
 */
void
rrfillin(bp, mp)
	struct buf *bp;
	struct mscp *mp;
{
	struct rx_softc *rx = 0; /* Wall */
	struct disklabel *lp;
	int unit = DISKUNIT(bp->b_dev);
	int part = DISKPART(bp->b_dev);

#if NRA
	if (major(bp->b_dev) == RAMAJOR)
		rx = ra_cd.cd_devs[unit];
#endif
#if NRX
	if (major(bp->b_dev) != RAMAJOR)
		rx = rx_cd.cd_devs[unit];
#endif
	lp = rx->ra_disk.dk_label;

	mp->mscp_seq.seq_lbn = lp->d_partitions[part].p_offset + bp->b_blkno;
	mp->mscp_unit = rx->ra_hwunit;
	mp->mscp_seq.seq_bytecount = bp->b_bcount;
}

/*
 * A bad block related operation finished.
 */
/*ARGSUSED*/
void
rrbb(usc, mp, bp)
	struct device *usc;
	struct mscp *mp;
	struct buf *bp;
{

	panic("udabb");
}

/*
 * MSCP generic driver routines
 */

#include <sys/param.h>
#include <sys/buf.h>
#include <sys/malloc.h>
#include <sys/device.h>
#include <sys/proc.h>
#include <sys/systm.h>

#include <machine/bus.h>

#include <dev/mscp/mscp.h>
#include <dev/mscp/mscpreg.h>
#include <dev/mscp/mscpvar.h>

#define PCMD	PSWP		/* priority for command packet waits */

/*
 * Get a command packet.  Second argument is true iff we are
 * to wait if necessary.  Return NULL if none are available and
 * we cannot wait.
 */
struct mscp *
mscp_getcp(mi, canwait)
	struct mscp_softc *mi;
	int canwait;
{
#define mri	(&mi->mi_cmd)
	struct mscp *mp;
	int i;
	int s = splimp();

again:
	/*
	 * Ensure that we have some command credits, and
	 * that the next command packet is free.
	 */
	if (mi->mi_credits <= MSCP_MINCREDITS) {
		if (!canwait) {
			splx(s);
			return (NULL);
		}
		mi->mi_wantcredits = 1;
		(void) tsleep(&mi->mi_wantcredits, PCMD, "mscpwcrd", 0);
		goto again;
	}
	i = mri->mri_next;
	if (mri->mri_desc[i] & MSCP_OWN) {
		if (!canwait) {
			splx(s);
			return (NULL);
		}
		mi->mi_wantcmd = 1;
		(void) tsleep(&mi->mi_wantcmd, PCMD, "mscpwcmd", 0);
		goto again;
	}
	mi->mi_credits--;
	mri->mri_desc[i] &= ~MSCP_INT;
	mri->mri_next = (mri->mri_next + 1) % mri->mri_size;
	splx(s);
	mp = &mri->mri_ring[i];

	/*
	 * Initialise some often-zero fields.
	 * ARE THE LAST TWO NECESSARY IN GENERAL?  IT SURE WOULD BE
	 * NICE IF DEC SOLD DOCUMENTATION FOR THEIR OWN CONTROLLERS.
	 */
	mp->mscp_msglen = MSCP_MSGLEN;
	mp->mscp_flags = 0;
	mp->mscp_modifier = 0;
	mp->mscp_seq.seq_bytecount = 0;
	mp->mscp_seq.seq_buffer = 0;
	mp->mscp_seq.seq_mapbase = 0;
/*???*/ mp->mscp_sccc.sccc_errlgfl = 0;
/*???*/ mp->mscp_sccc.sccc_copyspd = 0;
	return (mp);
#undef	mri
}

#ifdef AVOID_EMULEX_BUG
int	mscp_aeb_xor = 0x8000bb80;
#endif

/*
 * Handle a response ring transition.
 */
void
mscp_dorsp(mi)
	struct mscp_softc *mi;
{
	struct device *drive;
	struct mscp_device *me = mi->mi_me;
	struct mscp_ctlr *mc = mi->mi_mc;
	struct buf *bp;
	struct mscp *mp;
	struct mscp_xi *mxi;
	int nextrsp;
	int st, error;
	extern int cold;
	extern struct mscp slavereply;

	nextrsp = mi->mi_rsp.mri_next;
loop:
	if (mi->mi_rsp.mri_desc[nextrsp] & MSCP_OWN) {
		/*
		 * No more responses.  Remember the next expected
		 * response index.  Check to see if we have some
		 * credits back, and wake up sleepers if so.
		 */
		mi->mi_rsp.mri_next = nextrsp;
		if (mi->mi_wantcredits && mi->mi_credits > MSCP_MINCREDITS) {
			mi->mi_wantcredits = 0;
			wakeup((caddr_t) &mi->mi_wantcredits);
		}
		return;
	}

	mp = &mi->mi_rsp.mri_ring[nextrsp];
	mi->mi_credits += MSCP_CREDITS(mp->mscp_msgtc);
	/*
	 * Controllers are allowed to interrupt as any drive, so we
	 * must check the command before checking for a drive.
	 */
	if (mp->mscp_opcode == (MSCP$K_OP_STCON | M_OP_END)) {
		if ((mp->mscp_status & M_ST_MASK) == M_ST_SUCCESS) {
			mi->mi_flags |= MSC_READY;
		} else {
			printf("%s: SETCTLRC failed: %d ",
			    mi->mi_dev.dv_xname, mp->mscp_status);
			mscp_printevent(mp);
		}
		goto done;
	}

	/*
	 * Found a response.  Update credit information.  If there is
	 * nothing else to do, jump to `done' to get the next response.
	 */
	if (mp->mscp_unit >= mi->mi_driveno) { /* Must expand drive table */
		int tmpno = ((mp->mscp_unit + 32) & 0xffe0) * sizeof(void *);
		struct device **tmp = (struct device **)
		    malloc(tmpno, M_DEVBUF, M_NOWAIT);
		bzero(tmp, tmpno);
		if (mi->mi_driveno) {
			bcopy(mi->mi_dp, tmp, mi->mi_driveno);
			free(mi->mi_dp, mi->mi_driveno);
		}
		mi->mi_driveno = tmpno;
		mi->mi_dp = tmp;
	}

	drive = mi->mi_dp[mp->mscp_unit];

	switch (MSCP_MSGTYPE(mp->mscp_msgtc)) {

	case MSCPT_SEQ:
		break;

	case MSCPT_DATAGRAM:
		(*me->me_dgram)(drive, mp, mi);
		goto done;

	case MSCPT_CREDITS:
		goto done;

	case MSCPT_MAINTENANCE:
	default:
		printf("%s: unit %d: unknown message type 0x%x ignored\n",
			mi->mi_dev.dv_xname, mp->mscp_unit,
			MSCP_MSGTYPE(mp->mscp_msgtc));
		goto done;
	}

	/*
	 * Handle individual responses.
	 */
	st = mp->mscp_status & M_ST_MASK;
	error = 0;
	switch (mp->mscp_opcode) {

	case M_OP_END:
		/*
		 * The controller presents a bogus END packet when
		 * a read/write command is given with an illegal
		 * block number.  This is contrary to the MSCP
		 * specification (ENDs are to be given only for
		 * invalid commands), but that is the way of it.
		 */
		if (st == M_ST_INVALCMD && mp->mscp_cmdref != 0) {
			printf("%s: bad lbn (%d)?\n", drive->dv_xname,
				(int)mp->mscp_seq.seq_lbn);
			error = EIO;
			goto rwend;
		}
		goto unknown;

	case M_OP_ONLINE | M_OP_END:
		/*
		 * Finished an ON LINE request.	 Call the driver to
		 * find out whether it succeeded.  If so, mark it on
		 * line.
		 */
		(*me->me_online)(drive, mp);
		break;

	case MSCP$K_OP_GTUNT | M_OP_END:
		/*
		 * Got unit status.  If we are autoconfiguring, save
		 * the mscp struct so that mscp_attach know what to do.
		 * If the drive isn't configured, call config_found()
		 * to set it up, otherwise it's just a "normal" unit
		 * status.
		 */
		if (cold)
			bcopy(mp, &slavereply, sizeof(struct mscp));

		if (mp->mscp_status == (M_ST_OFFLINE|M_OFFLINE_UNKNOWN))
			break;

		if (drive == 0) {
			struct	drive_attach_args da;

			da.da_mp = (struct mscp *)mp;
			da.da_typ = mi->mi_type;
			config_found(&mi->mi_dev, (void *)&da, mscp_print);
		} else
			/* Hack to avoid complaints */
			if (!(((mp->mscp_event & M_ST_MASK) == M_ST_AVAILABLE)
			    && cold))
				(*me->me_gotstatus)(drive, mp);
		break;

	case M_OP_AVAILATTN:
		/*
		 * The drive went offline and we did not notice.
		 * Mark it off line now, to force an on line request
		 * next, so we can make sure it is still the same
		 * drive.
		 *
		 * IF THE UDA DRIVER HAS A COMMAND AWAITING UNIBUS
		 * RESOURCES, THAT COMMAND MAY GO OUT BEFORE THE ON
		 * LINE.  IS IT WORTH FIXING??
		 */
#ifdef notyet
		(*md->md_offline)(ui, mp);
#endif
		break;

	case M_OP_POS | M_OP_END:
	case M_OP_WRITM | M_OP_END:
	case M_OP_AVAILABLE | M_OP_END:
		/*
		 * A non-data transfer operation completed.
		 */
		(*me->me_cmddone)(drive, mp);
		break;

	case M_OP_READ | M_OP_END:
	case M_OP_WRITE | M_OP_END:
		/*
		 * A transfer finished.	 Get the buffer, and release its
		 * map registers via ubadone().	 If the command finished
		 * with an off line or available status, the drive went
		 * off line (the idiot controller does not tell us until
		 * it comes back *on* line, or until we try to use it).
		 */
rwend:
#ifdef DIAGNOSTIC
		if (mp->mscp_cmdref >= NCMD) {
			/*
			 * No buffer means there is a bug somewhere!
			 */
			printf("%s: io done, but bad xfer number?\n",
			    drive->dv_xname);
			mscp_hexdump(mp);
			break;
		}
#endif

		if (mp->mscp_cmdref == -1) {
			(*me->me_cmddone)(drive, mp);
			break;
		}
		mxi = &mi->mi_xi[mp->mscp_cmdref];
		if (mxi->mxi_inuse == 0)
			panic("mxi not inuse");
		bp = mxi->mxi_bp;
		/*
		 * Mark any error-due-to-bad-LBN (via `goto rwend').
		 * WHAT STATUS WILL THESE HAVE?	 IT SURE WOULD BE NICE
		 * IF DEC SOLD DOCUMENTATION FOR THEIR OWN CONTROLLERS.
		 */
		if (error) {
			bp->b_flags |= B_ERROR;
			bp->b_error = error;
		}
		if (st == M_ST_OFFLINE || st == M_ST_AVAILABLE) {
#ifdef notyet
			(*md->md_offline)(ui, mp);
#endif
		}

		/*
		 * If the transfer has something to do with bad
		 * block forwarding, let the driver handle the
		 * rest.
		 */
		if ((bp->b_flags & B_BAD) != 0 && me->me_bb != NULL) {
			(*me->me_bb)(drive, mp, bp);
			goto out;
		}

		/*
		 * If the transfer failed, give the driver a crack
		 * at fixing things up.
		 */
		if (st != M_ST_SUCCESS) {
			switch ((*me->me_ioerr)(drive, mp, bp)) {

			case MSCP_DONE:		/* fixed */
				break;

			case MSCP_RESTARTED:	/* still working on it */
				goto out;

			case MSCP_FAILED:	/* no luck */
				/* XXX must move to ra.c */
				mscp_printevent(mp);
				break;
			}
		}

		/*
		 * Set the residual count and mark the transfer as
		 * done.  If the I/O wait queue is now empty, release
		 * the shared BDP, if any.
		 */
		bp->b_resid = bp->b_bcount - mp->mscp_seq.seq_bytecount;
		bus_dmamap_unload(mi->mi_dmat, mxi->mxi_dmam);

		(*mc->mc_ctlrdone)(mi->mi_dev.dv_parent);
		(*me->me_iodone)(drive, bp);
out:
		mxi->mxi_inuse = 0;
		mi->mi_mxiuse |= (1 << mp->mscp_cmdref);
		break;
		
	case M_OP_REPLACE | M_OP_END:
		/*
		 * A replace operation finished.  Just let the driver
		 * handle it (if it does replaces).
		 */
		if (me->me_replace == NULL)
			printf("%s: bogus REPLACE end\n", drive->dv_xname);
		else
			(*me->me_replace)(drive, mp);
		break;

	default:
		/*
		 * If it is not one of the above, we cannot handle it.
		 * (And we should not have received it, for that matter.)
		 */
unknown:
		printf("%s: unknown opcode 0x%x status 0x%x ignored\n",
			drive->dv_xname, mp->mscp_opcode, mp->mscp_status);
#ifdef DIAGNOSTIC
		mscp_hexdump(mp);
#endif
		break;
	}

	/*
	 * If the drive needs to be put back in the controller queue,
	 * do that now.	 (`bp' below ought to be `dp', but they are all
	 * struct buf *.)  Note that b_active was cleared in the driver;
	 * we presume that there is something to be done, hence reassert it.
	 */
#ifdef notyet /* XXX */
	if (ui->ui_flags & UNIT_REQUEUE) {
		...
	}
#endif
done:
	/*
	 * Give back the response packet, and take a look at the next.
	 */
	mp->mscp_msglen = MSCP_MSGLEN;
	mi->mi_rsp.mri_desc[nextrsp] |= MSCP_OWN;
	nextrsp = (nextrsp + 1) % mi->mi_rsp.mri_size;
	goto loop;
}

/*
 * Requeue outstanding transfers, e.g., after bus reset.
 * Also requeue any drives that have on line or unit status
 * info pending.
 */
void
mscp_requeue(mi)
	struct mscp_softc *mi;
{
	panic("mscp_requeue");
}

#endif /* if 0 */

#include"../../freevms/lib/src/cdtdef.h"
#include"../../freevms/lib/src/ddtdef.h"
#include"../../freevms/lib/src/dptdef.h"
#include"../../freevms/lib/src/fdtdef.h"
#include"../../freevms/lib/src/pdtdef.h"
#include"../../freevms/lib/src/irpdef.h"
#include"../../freevms/lib/src/ucbdef.h"
#include"../../freevms/starlet/src/iodef.h"
#include"../../freevms/starlet/src/devdef.h"

void dumyerr(void) {
  /* do nothing yet */
}

int dulisten(void * packet, struct _cdt * c, struct _pdt * p) {

}

void dudaemonize(void) { }

/* mscp.exe mscp$disk let it be a kernel_thread? maybe not... */
int dumscp(void) {
  char myname[]="mscp$disk";
  char myinfo[]="mscp disk";
  dudaemonize(); /* find out what this does */
  
  //  listen(msgbuf,err,cdt,pdt,cdt);
  scs$listen(dulisten,dumyerr,myname,myinfo);
}

struct _fdt fdt_du = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

void  du_startio (struct _irp * i, struct _ucb * u) { 
  if (u->ucb$l_devchar2&DEV$M_CDP) {
    /* not local? */
    struct _cdrp * c;
    c=(struct _cdrp *) i->irp$l_fqfl;
    c->cdrp$l_cdt=((struct _mscp_ucb *)u)->ucb$l_cdt;
    
  } else {
    /* local */
    /* error if it gets here, got no real mscp */
    panic("no real mscp\n");
  }
};

/* more yet undefined dummies */
void  du_unsolint (void) { };
void  du_functb (void) { };
void  du_cancel (void) { };
void  du_regdump (void) { };
void  du_diagbuf (void) { };
void  du_errorbuf (void) { };
void  du_unitinit (void) { };
void  du_altstart (void) { };
void  du_mntver (void) { };
void  du_cloneducb (void) { };
void  du_mntv_sssc (void) { };
void  du_mntv_for (void) { };
void  du_mntv_sqd (void) { };
void  du_aux_storage (void) { };
void  du_aux_routine (void) { };


struct _ddt ddt_du = {
  ddt$l_start: du_startio,
  ddt$l_unsolint: du_unsolint,
  ddt$l_functb: du_functb,
  ddt$l_cancel: du_cancel,
  ddt$l_regdump: du_regdump,
  ddt$l_diagbuf: du_diagbuf,
  ddt$l_errorbuf: du_errorbuf,
  ddt$l_unitinit: du_unitinit,
  ddt$l_altstart: du_altstart,
  ddt$l_mntver: du_mntver,
  ddt$l_cloneducb: du_cloneducb,
  ddt$w_fdtsize: 0,
  ddt$l_mntv_sssc: du_mntv_sssc,
  ddt$l_mntv_for: du_mntv_for,
  ddt$l_mntv_sqd: du_mntv_sqd,
  ddt$l_aux_storage: du_aux_storage,
  ddt$l_aux_routine: du_aux_routine
};

static struct _fdt du_fdt;

void acp_std$readblk();
void acp_std$writeblk();

void du_init(void) {
  /* a lot of these? */
  ini_fdt_act(&du_fdt,IO$_READLBLK,acp_std$readblk);
  ini_fdt_act(&du_fdt,IO$_READPBLK,acp_std$readblk);
  ini_fdt_act(&du_fdt,IO$_READVBLK,acp_std$readblk);
}

char dudriverstring[]="DUDRIVER";

int testme = 3;

struct _dpt du_dpt = {
  //  dpt$t_name:"DUDRIVER"
  //  dpt$b_type:&testme,
  // dpt$t_name:dudriverstring  // have no idea why this won't compile
};

