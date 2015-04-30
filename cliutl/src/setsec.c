// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <acedef.h>
#include <starlet.h>
#include <descrip.h>
#include <iledef.h>
#include <misc.h>
#include <ossdef.h>
#include <ssdef.h>
#include <cli$routines.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Author: Roar Thron�s */

/* Don't know anywhere else to put this */

char * acl_types[] =
{ "", "IDENTIFIER", "" };

char * access_types[] =
{ "", "READ", "WRITE", "EXECUTE", "DELETE", "CONTROL", "" };

int access_types_values[] =
{ 0, ACE$M_READ, ACE$M_WRITE, ACE$M_EXECUTE, ACE$M_DELETE, ACE$M_CONTROL };

int acl_find_type(struct dsc$descriptor * name)
{
    if (name->dsc$w_length == 0)
        return 0;
    int i;
    for (i = 1; i <= 1; i++)
        if (0
                == strncasecmp(acl_types[i], name->dsc$a_pointer,
                               name->dsc$w_length))
            return i;
    return 0;
}

int acl_find_access_type(struct dsc$descriptor * name)
{
    if (name->dsc$w_length == 0)
        return 0;
    int i;
    for (i = 1; i <= 5; i++)
        if (0
                == strncasecmp(access_types[i], name->dsc$a_pointer,
                               name->dsc$w_length))
            return i;
    return 0;
}

set_security(int argc, char**argv)
{
    $DESCRIPTOR(p, "p1");
    $DESCRIPTOR(acl, "acl");

    char aclstr[80];
    struct dsc$descriptor aclval;
    aclval.dsc$a_pointer = aclstr;
    aclval.dsc$w_length = 80;

    char nastr[80];
    struct dsc$descriptor naval;
    naval.dsc$a_pointer = nastr;
    naval.dsc$w_length = 80;

    char prstr[80];
    struct dsc$descriptor prval;
    prval.dsc$a_pointer = prstr;
    prval.dsc$w_length = 80;

    int sts, aclsts, nasts, prsts;
    int retlen;

    sts = cli$present(&p);
#if 0
    if (0)
        sts = cli$get_value(&p, &o, &retlen);
#endif

    aclsts = cli$present(&acl);

    int pid = 0;

    if (aclsts & 1)
    {
#if 0
        aclsts = cli$get_value(&acl, &aclval, &retlen);
#endif

        int id;
        int ace_access = 0;
        int ace_id = 0;
        int retlen;
#if 0
        $DESCRIPTOR(qacl, "acl");
#endif
        char d[80];
        struct dsc$descriptor q;
        q.dsc$a_pointer = d;
        q.dsc$w_length = 80;
        memset(d, 0, 80);
#if 0
        int sts = cli$present(&qacl);
        if ((sts&1)==0)
            return 0;
#else
        int sts;
#endif
#if 0
        $DESCRIPTOR(acc, "ACCESS");
        while (cli$get_value(&acc, &q, &retlen)&1)
        {
            q.dsc$w_length=retlen;
            d[retlen]=0;
            printf("text %s\n",d);
        }
        return;
#endif
        if (/*cli$get_value(&qacl, &q, &retlen)&*/1)
        {
#if 0
            q.dsc$w_length=retlen;
            d[retlen]=0;
            printf("ac %s\n",d);
            struct dsc$descriptor q1 = q;
#endif
#if 0
            char * eq = strchr(q1.dsc$a_pointer, '=');
            if (eq == 0)
                break;
            q1.dsc$w_length = ((long)eq) - ((long)q1.dsc$a_pointer);
#endif
#if 0
            int acl = acl_find_type(&q1);
            acl=1;
            printf("acl %x\n", acl);
#endif
            if (/*acl ==*/1)
            {
#if 0
                struct dsc$descriptor q2;
                q2.dsc$a_pointer = eq + 1;
                q2.dsc$w_length = ((long)q.dsc$a_pointer) + q.dsc$w_length - ((long)q2.dsc$a_pointer);
                int id = atoi(eq+1);
                printf("id %x %s\n",id,eq+1);
                if (cli$get_value(&qacl, &q, &retlen)&1)
                {
                    q.dsc$w_length=retlen;
                    struct dsc$descriptor q1 = q;
                    char * eq = strchr(q1.dsc$a_pointer, '=');
                    if (eq == 0)
                        break;
                    q1.dsc$w_length = ((long)eq) - ((long)q1.dsc$a_pointer);
                    if (0==strncmp("ACCESS", q1.dsc$a_pointer, q1.dsc$w_length))
                    {
                        struct dsc$descriptor q2;
                        q2.dsc$a_pointer = eq + 1;
                        q2.dsc$w_length = ((long)q.dsc$a_pointer) + q.dsc$w_length - ((long)q2.dsc$a_pointer);
                        ace_access = access_types_values [acl_find_access_type(&q2)];
                    }
                }
#endif
                $DESCRIPTOR(ident, "IDENTIFIER");
                $DESCRIPTOR(access, "ACCESS");
                if (cli$get_value(&ident, &q, &retlen) & 1)
                {
                    d[retlen] = 0;
                    int id = atoi(d);
                    ace_id = id;
                    printf("id = %x\n", id);
                }
                else
                    printf("noid\n");
                while (cli$get_value(&access, &q, &retlen) & 1)
                {
                    q.dsc$w_length = retlen;
                    d[retlen] = 0;
                    ace_access |= access_types_values[acl_find_access_type(&q)];
                    printf("access = %s %x\n", d, ace_access);
                }/* else
                 printf("noac\n");*/
            }
        }
        printf("access %x\n", ace_access);
#if 0
        return 1;
#endif

        char rsa[80];

        $DESCRIPTOR(file, "FILE");
        struct dsc$descriptor filename;
        filename.dsc$a_pointer = rsa;
        filename.dsc$w_length = 80;

        sts = cli$present(&p);
        printf("sec %x\n", sts);
        if (sts & 1)
        {
            sts = cli$get_value(&p, &filename, &retlen);
            filename.dsc$w_length = retlen;
        }
        else
            return 0;

        struct _ile3 itmlst[2];
        memset(itmlst, 0, 2 * sizeof(struct _ile3));
        retlen = 0;
        char buf[512];
        struct _acedef * ace = buf;
        ace->ace$b_size = 16; // should be 12?
        ace->ace$b_type = ACE$C_KEYID;
        ace->ace$l_access = ace_access;
        ace->ace$l_key = ace_id;
        itmlst[0].ile3$w_length = 512;
        itmlst[0].ile3$w_code = OSS$_ACL_ADD_ENTRY;
        itmlst[0].ile3$ps_bufaddr = buf;
        itmlst[0].ile3$ps_retlen_addr = &retlen;
        sts = sys$set_security(&file, &filename, 0, 0, &itmlst, 0, 0);
    }

    return SS$_NORMAL;
}

