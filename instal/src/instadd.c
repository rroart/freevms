// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<descrip.h>
#include<kfedef.h>
#include<prvdef.h>
#include<cli$routines.h>

#include <stdio.h>
#include <string.h>

char * privs[] =
{
    "",
    "CMKRNL",
    "CMEXEC",
    "SYSNAM",
    "GRPNAM",
    "ALLSPOOL",
    "IMPERSONATE",
    "DIAGNOSE",
    "LOG_IO",
    "GROUP",
    "NOACNT",
    "PRMCEB",
    "PRMMBX",
    "PSWAPM",
    "SETPRI",
    "SETPRV",
    "TMPMBX",
    "WORLD",
    "MOUNT",
    "OPER",
    "EXQUOTA",
    "NETMBX",
    "VOLPRO",
    "PHY_IO",
    "BUGCHK",
    "PRMGBL",
    "SYSGBL",
    "PFNMAP",
    "SHMEM",
    "SYSPRV",
    "BYPASS",
    "SYSLCK",
    "SHARE",
    "UPGRADE",
    "DOWNGRADE",
    "GRPPRV",
    "READALL",
    "IMPORT",
    "AUDIT",
    "SECURITY",
    ""
};

int inst$getmem(const void *src, void *dest, int n);
int inst$alononpaged(int reqsize, int *alosize_p, void **pool_p);

int inst$find_priv(struct dsc$descriptor * name)
{
    int i;
    for (i=1; i<=PRV$K_NUMBER_OF_PRIVS; i++)
        if (0==strncasecmp(privs[i],name->dsc$a_pointer,name->dsc$w_length))
            return i;
    return 0;
}

long long inst$get_priv()
{
    long long priv=0;
    int retlen;
    $DESCRIPTOR(qpriv, "privileged");
    char d[80];
    struct dsc$descriptor q;
    q.dsc$a_pointer=d;
    q.dsc$w_length=80;
    memset (d, 0, 80);
    int sts = cli$present(&qpriv);
    if ((sts&1)==0)
        return 0;
    while (cli$get_value(&qpriv, &q, &retlen)&1)
    {
        q.dsc$w_length=retlen;
        int privno=inst$find_priv(&q);
        if (privno && privno < 32)
            priv|=1<<(privno-1);
        if (privno >= 32)
        {
            int priv2 = (1<<(privno-32-1));
            int * ppriv = &priv;
            ppriv++;
            * ppriv |= priv2; // check ugly ugly
        }
    }
    printf("priv %llx\n",priv);
    return priv;
}

int inst$add()
{
    int retlen=0;
    $DESCRIPTOR(p, "p1");
    char c[80];
    struct dsc$descriptor o;
    o.dsc$a_pointer=c;
    o.dsc$w_length=80;
    memset (c, 0, 80);
    int sts = cli$present(&p);
    if (sts&1)
    {
        sts = cli$get_value(&p, &o, &retlen);
        o.dsc$w_length=retlen;
    }
    printf("name %s\n",c);
    struct _kfe * new_kkfe, * kkfe;
    extern void * exe$gl_known_files; // simplifying straight to kfe
    sts = inst$getmem(&exe$gl_known_files, &kkfe, 0);
    struct _kfe kfe;
    memset(&kfe, 0, sizeof(struct _kfe));
    kfe.kfe$l_kfelink=kkfe;
    kfe.kfe$q_procpriv=inst$get_priv();
    printf("procpriv is %llx\n",kfe.kfe$q_procpriv);
    memcpy(&kfe.kfe$l_obsolete_1,o.dsc$a_pointer,o.dsc$w_length);
    int retsize=0;
    inst$alononpaged(sizeof(struct _kfe), &retsize, &new_kkfe);
    kfe.kfe$w_size=retsize;
    inst$getmem(&kfe,new_kkfe,sizeof(struct _kfe));
    sts = inst$getmem(&new_kkfe, &exe$gl_known_files, 0);
    return sts;
}
