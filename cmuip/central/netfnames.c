/*
    ****************************************************************

        Copyright (c) 1992, Carnegie Mellon University

        All Rights Reserved

    Permission  is  hereby  granted   to  use,  copy,  modify,  and
    distribute  this software  provided  that the  above  copyright
    notice appears in  all copies and that  any distribution be for
    noncommercial purposes.

    Carnegie Mellon University disclaims all warranties with regard
    to this software.  In no event shall Carnegie Mellon University
    be liable for  any special, indirect,  or consequential damages
    or any damages whatsoever  resulting from loss of use, data, or
    profits  arising  out of  or in  connection  with  the  use  or
    performance of this software.

    ****************************************************************
*/
#include stdio
#include ctype

int FNAME_NET_TO_VMS(unix_fn,ulen,vms_fn,dirp)
char *unix_fn,*vms_fn;
int ulen;
int dirp;
{
    /* !!!HACK!!! Why don't we check for the end of vms_fn??? (cause were lazy) */
    int i;
    char c;
    int upmode_flag = 0;
    int dotp = 0, lastdot = -1;
    char *vs;

    /* find last dot in file (for directory translations) */
    for (i=0; i<ulen; i++)
        if (unix_fn[i]=='.') lastdot = i;

    for (i=0,vs = vms_fn; i<ulen; i++)
    {
        c = unix_fn[i];

        if (isalpha(c))
        {
            if (!!islower(c) == upmode_flag)
            {
                *vs++ = '$';
                upmode_flag = !upmode_flag;
            }
            *vs++ = toupper(c);
            continue;
        }

        /* Leave digits alone */
        if (isdigit(c))
        {
            *vs++ = c;
            continue;
        }

        /* Check for hyphens and underscores - they are OK */
        if ((c == '-') || (c == '_'))
        {
            *vs++ = c;
            continue;
        }

        /* A dot? */
        if (c=='.')
            if (!dotp && !dirp)
            {
                *vs++ = '.';
                dotp = 1;
                continue;
            }

        /* escape the escape */
        if (c=='$')
        {
            *vs++ = '$';
            *vs++ = '$';
            continue;
        }


        /* Since it's not alpha numeric, it must be escaped */
        *vs++ = '$';

        /* run the gauntlet */
        if (c==0)
        {
            *vs++ = '6';
            *vs++ = 'A';
            continue;
        }
        if (c<=032)
        {
            *vs++ = '4';
            *vs++ = 'A'+c-001;
            continue;
        }
        if (c<040)
        {
            *vs++ = '6';
            *vs++ = 'B'+(c-033);
            continue;
        }
        if (c==' ')
        {
            *vs++ = '7';
            *vs++ = 'A';
            continue;
        }
        if (c<058)
        {
            *vs++ = '5';
            *vs++ = 'A'+(c-041);
            continue;
        }
        if (c==':')
        {
            *vs++ = '5';
            *vs++ = 'Z';
            continue;
        }
        if (c<077)
        {
            *vs++ = '7';
            *vs++ = 'B'+(c-073);
            continue;
        }
        if (c<0140)
        {
            *vs++ = '8';
            *vs++ = 'A'+(c-0100);
            continue;
        }
        if (c==0140)
        {
            *vs++ = '9';
            *vs++ = 'A';
            continue;
        }
        if (c<200)
        {
            *vs++ = '9';
            *vs++ = 'B'+(c-0172);
            continue;
        }

        sprintf(vs,"%3o",c);
        vs += 3;
    }

    if ((lastdot == -1) || dirp)
        *vs++ = '.';

    *vs = 0;

    return(vs-vms_fn);
}


int FNAME_VMS_TO_NET(vms_fn,vlen,unix_fn,dirp,vclip)
char *vms_fn,*unix_fn;
int vlen;
int dirp,vclip;
{
    char c,*us=unix_fn;
    int i,upmode_flag=0;

    for (i=0,us = unix_fn; i<vlen; i++)
    {
        c = vms_fn[i];

        /* handle letters */
        if (isalpha(c))
        {
            if (upmode_flag)
                *us++ = toupper(c);
            else *us++ = tolower(c);
            continue;
        }

        /* Leave digits alone */
        if (isdigit(c))
        {
            *us++ = c;
            continue;
        }

        /* clip version number */
        if ((c==';') && vclip)
        {
            *us=0;
            return (us-unix_fn);
        }

        /* handle escape character ($) */
        if (c=='$')
        {
            char c2 = vms_fn[i+1], c3 = vms_fn[i+2], c4 = vms_fn[i+3];

            /* a double escape */
            if (c2=='$')
            {
                *us++ = vms_fn[++i];
                continue;
            }

            /* a case switch */
            if (isalpha(c2))
            {
                upmode_flag = !upmode_flag;
                continue;
            }

            if ((c2>='4') && (c2<='9'))
            {
                int rc = -1;

                switch (c2)
                {
                case '4' :
                {
                    if (isupper(c3)) rc = c3+001-'A';
                    break;
                }
                case '5' :
                {
                    if (c3=='Z') rc = ':';
                    else if (isupper(c3)) rc = c3+041-'A';
                    break;
                }
                case '6' :
                {
                    if (c3=='A') rc = 0;
                    else if (isupper(c3)) rc = c3+033-'B';
                    break;
                }
                case '7' :
                {
                    if (c3=='A') rc = ' ';
                    else if (isupper(c3)) rc = c3+073-'B';
                    break;
                }
                case '8' :
                {
                    if (isupper(c3)) rc = c3+0100-'A';
                    break;
                }
                case '9' :
                {
                    if (c3=='A') rc = 0140;
                    else if (isupper(c3)) rc = c3+0172-'B';
                    break;
                }
                }
                if (rc>=0)
                {
                    i += 2;
                    *us++ = rc;
                    continue;
                }
            }

            /* got an octal code? */
            if (isdigit(c2) && isdigit(c3) && isdigit(c4))
            {
                *us++ = c2*0100 + c3*010 + c4;
                i += 3;
                continue;
            }

        }

        *us++ = c;
    }

    if (*(us-1) == '.')
        us -= 1;
    else if (!strncmp(us-4, ".dir", 4) || !strncmp(us-4, ".DIR", 4))
        us -= 4;

    *us = 0;

    return (us-unix_fn);
}

