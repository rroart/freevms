// $Id$
// $Locker$

// Author. Roar Thron�s.

/* This program is derived from 4.3 BSD software and is
 subject to the copyright notice below.

 The port to HP-UX has been motivated by the incapability
 of 'rlogin'/'rlogind' as per HP-UX 6.5 (and 7.0) to transfer window sizes.

 Changes:

 - General HP-UX portation. Use of facilities not available
 in HP-UX (e.g. setpriority) has been eliminated.
 Utmp/wtmp handling has been ported.

 - The program uses BSD command line options to be used
 in connection with e.g. 'rlogind' i.e. 'new login'.

 - HP features left out:      password expiry
 '*' as login shell, add it if you need it

 - BSD features left out:         quota checks
 password expiry
 analysis of terminal type (tset feature)

 - BSD features thrown in:        Security logging to syslogd.
 This requires you to have a (ported) syslog
 system -- 7.0 comes with syslog

 'Lastlog' feature.

 - A lot of nitty gritty details have been adjusted in favour of
 HP-UX, e.g. /etc/securetty, default paths and the environment
 variables assigned by 'login'.

 - We do *nothing* to setup/alter tty state, under HP-UX this is
 to be done by getty/rlogind/telnetd/some one else.

 Michael Glad (glad@daimi.dk)
 Computer Science Department
 Aarhus University
 Denmark

 1990-07-04

 1991-09-24 glad@daimi.aau.dk: HP-UX 8.0 port:
 - now explictly sets non-blocking mode on descriptors
 - strcasecmp is now part of HP-UX

 1992-02-05 poe@daimi.aau.dk: Ported the stuff to Linux 0.12
 From 1992 till now (1997) this code for Linux has been maintained at
 ftp.daimi.aau.dk:/pub/linux/poe/

 1999-02-22 Arkadiusz Mi�kiewicz <misiek@pld.ORG.PL>
 - added Native Language Support
 Sun Mar 21 1999 - Arnaldo Carvalho de Melo <acme@conectiva.com.br>
 - fixed strerr(errno) in gettext calls
 */

/*
 * Copyright (c) 1980, 1987, 1988 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * login [ name ]
 * login -h hostname    (for telnetd, etc.)
 * login -f name    (for pre-authenticated login: datakit, xterm, etc.)
 */

/* #define TESTING */

#ifdef TESTING
#include "param.h"
#else
#include <sys/param.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include <memory.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/file.h>
#include <termios.h>
#include <string.h>
#define index strchr
#define rindex strrchr
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <utmp.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syslog.h>
#include <sys/sysmacros.h>
#include <netdb.h>

static void
getloginname(void);
void
badlogin(const char *name);
void
sleepexit(int eval);

#ifdef __linux__
#  include <sys/sysmacros.h>
#  include <linux/major.h>
#endif

#ifdef TESTING
#  include "utmp.h"
#else
#  include <utmp.h>
#endif

#ifdef SHADOW_PWD
#  include <shadow.h>
#endif

#ifndef __linux__
#  include <tzfile.h>
#endif
#include <lastlog.h>

#define SLEEP_EXIT_TIMEOUT 5

#if 0
/* from before we had a lastlog.h file in linux */
struct lastlog
{
    long ll_time;
    char ll_line[12];
    char ll_host[16];
};
#endif

static void timedout(int);
static void sigint(int);
static void motd(void);

#ifdef CRYPTOCARD
#include "cryptocard.h"
#endif

#ifdef  KERBEROS
#include <kerberos/krb.h>
#include <sys/termios.h>
char realm[REALM_SZ];
int kerror = KSUCCESS, notickets = 1;
#endif

#ifdef USE_TTY_GROUP
#  define TTY_MODE 0620
#else
#  define TTY_MODE 0600
#endif

#define TTYGRPNAME  "tty"       /* name of group to own ttys */

#ifndef MAXPATHLEN
#  define MAXPATHLEN 1024
#endif

/*
 * This bounds the time given to login.  Not a define so it can
 * be patched on machines where it's too small.
 */
#ifndef __linux__
int timeout = 300;
#else
int timeout = 60; /* used in cryptocard.c */
#endif

struct passwd *pwd; /* used in cryptocard.c */
struct passwd mypwd =
{
pw_name: "system"
, pw_uid: 0, pw_gid: 0, pw_dir: "/root"
    , pw_shell
: "/vms$common/sysexe/dcl"
};

char hostaddress[4]; /* used in checktty.c */
char *hostname; /* idem */
static char *username/*, *tty_name, *tty_number*/;
static char thishost[100];
static int failures = 1;
static pid_t pid;

#ifndef __linux__
struct sgttyb sgttyb;
struct tchars tc =
{
    CINTR, CQUIT, CSTART, CSTOP, CEOT, CBRK
};
struct ltchars ltc =
{
    CSUSP, CDSUSP, CRPRNT, CFLUSH, CWERASE, CLNEXT
};
#endif

#define _(x) x

/* Nice and simple code provided by Linus Torvalds 16-Feb-93 */
/* Nonblocking stuff by Maciej W. Rozycki, macro@ds2.pg.gda.pl, 1999.
 He writes: "Login performs open() on a tty in a blocking mode.
 In some cases it may make login wait in open() for carrier infinitely,
 for example if the line is a simplistic case of a three-wire serial
 connection. I believe login should open the line in the non-blocking mode
 leaving the decision to make a connection to getty (where it actually
 belongs). */
static void opentty(const char * tty)
{
    int i, fd, flags;

    fd = open(tty, O_RDWR | O_NONBLOCK);
    if (fd == -1)
    {
        syslog(LOG_ERR, _("FATAL: can't reopen tty: %s"), strerror(errno));
        sleep(1);
        exit(1);
    }

    flags = fcntl(fd, F_GETFL);
    flags &= ~O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);

    for (i = 0; i < fd; i++)
        close(i);
    for (i = 0; i < 3; i++)
        if (fd != i)
            dup2(fd, i);
    if (fd >= 3)
        close(fd);
}

/* In case login is suid it was possible to use a hardlink as stdin
 and exploit races for a local root exploit. (Wojciech Purczynski). */
/* More precisely, the problem is  ttyn := ttyname(0); ...; chown(ttyn);
 here ttyname() might return "/tmp/x", a hardlink to a pseudotty. */
/* All of this is a problem only when login is suid, which it isnt. */
static void check_ttyname(char *ttyn)
{
    struct stat statbuf;

    if (lstat(ttyn, &statbuf) || !S_ISCHR(statbuf.st_mode)
            || (statbuf.st_nlink > 1 && strncmp(ttyn, "/dev/", 5)))
    {
        syslog(LOG_ERR, _("FATAL: bad tty"));
        sleep(1);
        exit(1);
    }
}

/* true if the filedescriptor fd is a console tty, very Linux specific */
static int consoletty(int fd)
{
#ifdef __linux__
    struct stat stb;

    if ((fstat(fd, &stb) >= 0) && (major(stb.st_rdev) == TTY_MAJOR)
            && (minor(stb.st_rdev) < 64))
    {
        return 1;
    }
#endif
    return 0;
}

#define _PATH_DEFPATH_ROOT "/root"

int main(int argc, char **argv)
{
    extern int optind;
    extern char *optarg, **environ;
    struct group *gr;
    register int ch;
    register char *p;
    int ask, fflag, hflag, pflag, cnt, errsv;
    int passwd_req;
    char *domain/* , *ttyn*/;
    char tbuf[MAXPATHLEN + 2], tname[sizeof(_PATH_TTY) + 10];
    char *termenv;
    char *childArgv[10];
    char *buff;
    int childArgc = 0;
    char *salt, *pp;
#ifdef CHOWNVCS
    char vcsn[20], vcsan[20];
#endif

#if 0
    int stsflg = ctl$gl_creprc_flags;
    // check for PRC$M_NOUAF sometime
#endif

    pid = getpid();

    signal(SIGALRM, timedout);
    alarm((unsigned int) timeout);
    signal(SIGQUIT, SIG_IGN );
    signal(SIGINT, SIG_IGN );

#if 0
    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);
#endif

    setpriority(PRIO_PROCESS, 0, 0);
#ifdef HAVE_QUOTA
    quota(Q_SETUID, 0, 0, 0);
#endif
#ifdef DO_PS_FIDDLING
    initproctitle(argc, argv);
#endif

    /*
     * -p is used by getty to tell login not to destroy the environment
     * -f is used to skip a second login authentication
     * -h is used by other servers to pass the name of the remote
     *    host to login so that it may be placed in utmp and wtmp
     */
    gethostname(tbuf, sizeof(tbuf));
    strncpy(thishost, tbuf, sizeof(thishost));
    domain = index(tbuf, '.');

    username = /*tty_name =*/hostname = NULL;
    fflag = hflag = pflag = 0;
    passwd_req = 1;

#if 0
    while ((ch = getopt(argc, argv, "fh:p")) != -1)
        switch (ch)
        {
        case 'f':
            fflag = 1;
            break;

        case 'h':
            if (getuid())
            {
                fprintf(stderr,
                        _("login: -h for super-user only.\n"));
                exit(1);
            }
            hflag = 1;
            if (domain && (p = index(optarg, '.')) &&
                    strcasecmp(p, domain) == 0)
                *p = 0;

            hostname = strdup(optarg); /* strdup: Ambrose C. Li */
            {
                struct hostent *he = gethostbyname(hostname);

                /* he points to static storage; copy the part we use */
                hostaddress[0] = 0;
                if (he && he->h_addr_list && he->h_addr_list[0])
                    memcpy(hostaddress, he->h_addr_list[0],
                           sizeof(hostaddress));
            }
            break;

        case 'p':
            pflag = 1;
            break;

        case '?':
        default:
            fprintf(stderr,
                    _("usage: login [-fp] [username]\n"));
            exit(1);
        }
    argc -= optind;
    argv += optind;
#endif
#if 0
    if (0==strcmp(argv[0],"[vms$common.sysexe]startup.com"))
    {
        execve("/vms$common/sysexe/dcl",argv,0);
    }
    if (0==strcmp(argv[0],"[vms$common.sysexe]install.com"))
    {
        execve("/vms$common/sysexe/dcl",argv,0);
    }
#else
    if (0 == strcmp(argv[0], "[vms$common.sysexe]startup.com"))
    {
        execve("[vms$common.sysexe]dcl", argv, 0);
    }
    if (0 == strcmp(argv[0], "[vms$common.sysexe]install.com"))
    {
        execve("[vms$common.sysexe]dcl", argv, 0);
    }
#endif
#if 0
    if (*argv)
    {
        char *p = *argv;
        username = strdup(p);
        ask = 0;
        /* wipe name - some people mistype their password here */
        /* (of course we are too late, but perhaps this helps a little ..) */
        while(*p)
            *p++ = ' ';
    }
    else
#endif
        ask = 1;

    for (cnt = getdtablesize(); cnt > 2; cnt--)
        close(cnt);

#if 0
    ttyn = ttyname(0);

    if (ttyn == NULL || *ttyn == '\0')
    {
        /* no snprintf required - see definition of tname */
        sprintf(tname, "%s??", _PATH_TTY);
        ttyn = tname;
    }

    check_ttyname(ttyn);

    if (strncmp(ttyn, "/dev/", 5) == 0)
        tty_name = ttyn+5;
    else
        tty_name = ttyn;

    if (strncmp(ttyn, "/dev/tty", 8) == 0)
        tty_number = ttyn+8;
    else
    {
        char *p = ttyn;
        while (*p && !isdigit(*p)) p++;
        tty_number = p;
    }

#ifdef CHOWNVCS
    /* find names of Virtual Console devices, for later mode change */
    snprintf(vcsn, sizeof(vcsn), "/dev/vcs%s", tty_number);
    snprintf(vcsan, sizeof(vcsan), "/dev/vcsa%s", tty_number);
#endif

    /* set pgid to pid */
    setpgrp();
    /* this means that setsid() will fail */
#endif

    {
#if 0
        struct termios tt, ttt;

        tcgetattr(0, &tt);
        ttt = tt;
        ttt.c_cflag &= ~HUPCL;
#endif

#if 0
        /* These can fail, e.g. with ttyn on a read-only filesystem */
        chown(ttyn, 0, 0);
        chmod(ttyn, TTY_MODE);
#endif

#if 0
        // not yet?
        /* Kill processes left on this tty */
        tcsetattr(0,TCSAFLUSH,&ttt);
        signal(SIGHUP, SIG_IGN); /* so vhangup() wont kill us */
        vhangup();
        signal(SIGHUP, SIG_DFL);
#endif

#if 0
        /* open stdin,stdout,stderr to the tty */
        opentty(ttyn);

        /* restore tty modes */
        tcsetattr(0,TCSAFLUSH,&tt);
#endif
    }

    openlog("login", LOG_ODELAY, LOG_AUTHPRIV);

#if 0
    /* other than iso-8859-1 */
    printf("\033(K");
    fprintf(stderr,"\033(K");
#endif

    for (cnt = 0;; ask = 1)
    {

        if (ask)
        {
            fflag = 0;
            getloginname();
        }

        /* Dirty patch to fix a gigantic security hole when using
         yellow pages. This problem should be solved by the
         libraries, and not by programs, but this must be fixed
         urgently! If the first char of the username is '+', we
         avoid login success.
         Feb 95 <alvaro@etsit.upm.es> */

        if (username[0] == '+')
        {
            puts(_("Illegal username"));
            badlogin(username);
            sleepexit(1);
        }

        /* (void)strcpy(tbuf, username); why was this here? */
        pwd = &mypwd;
        goto nocheck;
        if ((pwd = getpwnam(username)))
        {
#  ifdef SHADOW_PWD
            struct spwd *sp;

            if ((sp = getspnam(username)))
                pwd->pw_passwd = sp->sp_pwdp;
#  endif
            salt = pwd->pw_passwd;
        }
        else
            salt = "xx";

nocheck:
        if (pwd)
        {
            initgroups(username, pwd->pw_gid);
            //checktty(username, tty_name, pwd); /* in checktty.c */
        }

        /*
         * Disallow automatic login to root; if not invoked by
         * root, disallow if the uid's differ.
         */
        if (fflag && pwd)
        {
            int uid = getuid();

            passwd_req = pwd->pw_uid == 0 || (uid && uid != pwd->pw_uid);
        }

        break; // goto out of this

#if 0
        /*
         * If trying to log in as root, but with insecure terminal,
         * refuse the login attempt.
         */
        if (pwd && pwd->pw_uid == 0 && !rootterm(tty_name))
        {
            fprintf(stderr,
                    _("%s login refused on this terminal.\n"),
                    pwd->pw_name);

            if (hostname)
                syslog(LOG_NOTICE,
                       _("LOGIN %s REFUSED FROM %s ON TTY %s"),
                       pwd->pw_name, hostname, tty_name);
            else
                syslog(LOG_NOTICE,
                       _("LOGIN %s REFUSED ON TTY %s"),
                       pwd->pw_name, tty_name);
            continue;
        }
#endif

        /*
         * If no pre-authentication and a password exists
         * for this user, prompt for one and verify it.
         */
        if (!passwd_req || (pwd && !*pwd->pw_passwd))
            break;

        setpriority(PRIO_PROCESS, 0, -4);
        pp = getpass(_("Password: "));

#  ifdef CRYPTOCARD
        if (strncmp(pp, "CRYPTO", 6) == 0)
        {
            if (pwd && cryptocard()) break;
        }
#  endif /* CRYPTOCARD */

        //p = crypt(pp, salt);
        setpriority(PRIO_PROCESS, 0, 0);

#  ifdef KERBEROS
        /*
         * If not present in pw file, act as we normally would.
         * If we aren't Kerberos-authenticated, try the normal
         * pw file for a password.  If that's ok, log the user
         * in without issueing any tickets.
         */

        if (pwd && !krb_get_lrealm(realm,1))
        {
            /*
             * get TGT for local realm; be careful about uid's
             * here for ticket file ownership
             */
            setreuid(geteuid(),pwd->pw_uid);
            kerror = krb_get_pw_in_tkt(pwd->pw_name, "", realm,
                                       "krbtgt", realm, DEFAULT_TKT_LIFE, pp);
            setuid(0);
            if (kerror == INTK_OK)
            {
                memset(pp, 0, strlen(pp));
                notickets = 0; /* user got ticket */
                break;
            }
        }
#  endif /* KERBEROS */
        memset(pp, 0, strlen(pp));

        if (pwd && !strcmp(p, pwd->pw_passwd))
            break;

        printf(_("Login incorrect\n"));
        badlogin(username); /* log ALL bad logins */
        failures++;

        /* we allow 10 tries, but after 3 we start backing off */
        if (++cnt > 3)
        {
            if (cnt >= 10)
            {
                sleepexit(1);
            }
            sleep((unsigned int) ((cnt - 3) * 5));
        }
    }

    /* committed to login -- turn off timeout */
    alarm((unsigned int) 0);

#ifdef HAVE_QUOTA
    if (quota(Q_SETUID, pwd->pw_uid, 0, 0) < 0 && errno != EINVAL)
    {
        switch(errno)
        {
        case EUSERS:
            fprintf(stderr,
                    _("Too many users logged on already.\nTry again later.\n"));
            break;
        case EPROCLIM:
            fprintf(stderr,
                    _("You have too many processes running.\n"));
            break;
        default:
            perror("quota (Q_SETUID)");
        }
        sleepexit(0); /* %% */
    }
#endif

    /* paranoia... */
#ifdef SHADOW_PWD
    endspent();
#endif
    endpwent();

#if 0
    chown(ttyn, pwd->pw_uid,
          (gr = getgrnam(TTYGRPNAME)) ? gr->gr_gid : pwd->pw_gid);
    chmod(ttyn, TTY_MODE);
#endif

#ifdef CHOWNVCS
    /* if tty is one of the VC's then change owner and mode of the
     special /dev/vcs devices as well */
    if (consoletty(0))
    {
        chown(vcsn, pwd->pw_uid, (gr ? gr->gr_gid : pwd->pw_gid));
        chown(vcsan, pwd->pw_uid, (gr ? gr->gr_gid : pwd->pw_gid));
        chmod(vcsn, TTY_MODE);
        chmod(vcsan, TTY_MODE);
    }
#endif

    setgid(pwd->pw_gid);

#ifdef HAVE_QUOTA
    quota(Q_DOWARN, pwd->pw_uid, (dev_t)-1, 0);
#endif

    if (*pwd->pw_shell == '\0')
        pwd->pw_shell = _PATH_BSHELL;

    /* preserve TERM even without -p flag */
    {
        char *ep;

        if (!((ep = getenv("TERM")) && (termenv = strdup(ep))))
            termenv = "dumb";
    }

    /* destroy environment unless user has requested preservation */
    if (!pflag)
    {
        environ = (char**) malloc(sizeof(char*));
        memset(environ, 0, sizeof(char*));
    }

    setenv("HOME", pwd->pw_dir, 0); /* legal to override */
    if (pwd->pw_uid)
        setenv("PATH", _PATH_DEFPATH, 1);
    else
        setenv("PATH", _PATH_DEFPATH_ROOT, 1);

    setenv("SHELL", pwd->pw_shell, 1);
    setenv("TERM", termenv, 1);

    /* mailx will give a funny error msg if you forget this one */
    {
        char tmp[MAXPATHLEN];
        /* avoid snprintf */
        if (sizeof(_PATH_MAILDIR) + strlen(pwd->pw_name) + 1 < MAXPATHLEN)
        {
            sprintf(tmp, "%s/%s", _PATH_MAILDIR, pwd->pw_name);
            setenv("MAIL", tmp, 0);
        }
    }

    /* LOGNAME is not documented in login(1) but
     HP-UX 6.5 does it. We'll not allow modifying it.
     */
    setenv("LOGNAME", pwd->pw_name, 1);

#ifdef DO_PS_FIDDLING
    setproctitle("login", username);
#endif

#if 0
    if (!strncmp(tty_name, "ttyS", 4))
        syslog(LOG_INFO, _("DIALUP AT %s BY %s"), tty_name, pwd->pw_name);
#endif

    /* allow tracking of good logins.
     -steve philp (sphilp@mail.alliance.net) */

    if (pwd->pw_uid == 0)
    {
        if (hostname)
            syslog(LOG_NOTICE, _("ROOT LOGIN ON %s FROM %s"),
                   "tty" /*tty_name*/, hostname);
        else
            syslog(LOG_NOTICE, _("ROOT LOGIN ON %s"), "tty" /*tty_name*/);
    }
    else
    {
        if (hostname)
            syslog(LOG_INFO, _("LOGIN ON %s BY %s FROM %s"), "tty" /*tty_name*/,
                   pwd->pw_name, hostname);
        else
            syslog(LOG_INFO, _("LOGIN ON %s BY %s"), "tty" /*tty_name*/,
                   pwd->pw_name);
    }

    signal(SIGALRM, SIG_DFL );
    signal(SIGQUIT, SIG_DFL );
    signal(SIGTSTP, SIG_IGN );

    signal(SIGINT, SIG_DFL );

    /* discard permissions last so can't get killed and drop core */
    if (setuid(pwd->pw_uid) < 0 && pwd->pw_uid)
    {
        syslog(LOG_ALERT, _("setuid() failed"));
        exit(1);
    }

    /* wait until here to change directory! */
    if (chdir(pwd->pw_dir) < 0)
    {
        printf(_("No directory %s!\n"), pwd->pw_dir);
        if (chdir("/"))
            exit(0);
        pwd->pw_dir = "/";
        printf(_("Logging in with home = \"/\".\n"));
    }

    /* if the shell field has a space: treat it like a shell script */
    if (strchr(pwd->pw_shell, ' '))
    {
        buff = malloc(strlen(pwd->pw_shell) + 6);

        if (!buff)
        {
            fprintf(stderr, _("login: no memory for shell script.\n"));
            exit(0);
        }

        strcpy(buff, "exec ");
        strcat(buff, pwd->pw_shell);
        childArgv[childArgc++] = "/bin/sh";
        childArgv[childArgc++] = "-sh";
        childArgv[childArgc++] = "-c";
        childArgv[childArgc++] = buff;
    }
    else
    {
        tbuf[0] = '-';
        strncpy(tbuf + 1,
                ((p = rindex(pwd->pw_shell, '/')) ? p + 1 : pwd->pw_shell),
                sizeof(tbuf) - 1);

        childArgv[childArgc++] = pwd->pw_shell;
        childArgv[childArgc++] = tbuf;
    }

    childArgv[childArgc++] = NULL;

    execvp(childArgv[0], childArgv + 1);

    errsv = errno;

    if (!strcmp(childArgv[0], "/bin/sh"))
        fprintf(stderr, _("login: couldn't exec shell script: %s.\n"),
                strerror(errsv));
    else
        fprintf(stderr, _("login: no shell: %s.\n"), strerror(errsv));

    exit(0);
}

static void getloginname(void)
{
    int ch, cnt, cnt2;
    char *p;
    static char nbuf[UT_NAMESIZE + 1];

    cnt2 = 0;
    for (;;)
    {
        cnt = 0;
        printf(_("\nUsername: "));
        fflush(stdout);
        for (p = nbuf; (ch = getchar()) != '\n';)
        {
            if (ch == EOF)
            {
                badlogin("EOF");
                exit(0);
            }
            if (p < nbuf + UT_NAMESIZE)
                *p++ = ch;

            cnt++;
            if (cnt > UT_NAMESIZE + 20)
            {
                fprintf(stderr, _("login name much too long.\n"));
                badlogin(_("NAME too long"));
                exit(0);
            }
        }
        if (p > nbuf)
        {
            if (nbuf[0] == '-')
                fprintf(stderr, _("login names may not start with '-'.\n"));
            else
            {
                *p = '\0';
                username = nbuf;
                break;
            }
        }

        cnt2++;
        if (cnt2 > 50)
        {
            fprintf(stderr, _("too many bare linefeeds.\n"));
            badlogin(_("EXCESSIVE linefeeds"));
            exit(0);
        }
    }
}

/*
 * Robert Ambrose writes:
 * A couple of my users have a problem with login processes hanging around
 * soaking up pts's.  What they seem to hung up on is trying to write out the
 * message 'Login timed out after %d seconds' when the connection has already
 * been dropped.
 * What I did was add a second timeout while trying to write the message so
 * the process just exits if the second timeout expires.
 */

static void timedout2(int sig)
{
    struct termio ti;

    /* reset echo */
    ioctl(0, TCGETA, &ti);
    ti.c_lflag |= ECHO;
    ioctl(0, TCSETA, &ti);
    exit(0); /* %% */
}

static void timedout(int sig)
{
    signal(SIGALRM, timedout2);
    alarm(10);
    fprintf(stderr, _("Login timed out after %d seconds\n"), timeout);
    signal(SIGALRM, SIG_IGN );
    alarm(0);
    timedout2(0);
}

#define SECURETTY "/dev/console"

#ifndef USE_PAM
int rootterm(char * ttyn)
{
    int fd;
    char buf[100], *p;
    int cnt, more = 0;

    fd = open(SECURETTY, O_RDONLY);
    if (fd < 0)
        return 1;

    /* read each line in /etc/securetty, if a line matches our ttyline
     then root is allowed to login on this tty, and we should return
     true. */
    for (;;)
    {
        p = buf;
        cnt = 100;
        while (--cnt >= 0 && (more = read(fd, p, 1)) == 1 && *p != '\n')
            p++;
        if (more && *p == '\n')
        {
            *p = '\0';
            if (!strcmp(buf, ttyn))
            {
                close(fd);
                return 1;
            }
            else
                continue;
        }
        else
        {
            close(fd);
            return 0;
        }
    }
}
#endif /* !USE_PAM */

jmp_buf motdinterrupt;

#define _PATH_MOTDFILE "/etc/motd"

void motd(void)
{
    int fd, nchars;
    void (*oldint)(int);
    char tbuf[8192];

    if ((fd = open(_PATH_MOTDFILE, O_RDONLY, 0)) < 0)
        return;
    oldint = signal(SIGINT, sigint);
    if (setjmp(motdinterrupt) == 0)
        while ((nchars = read(fd, tbuf, sizeof(tbuf))) > 0)
            write(fileno(stdout), tbuf, nchars);
    signal(SIGINT, oldint);
    close(fd);
}

void sigint(int sig)
{
    longjmp(motdinterrupt, 1);
}

void badlogin(const char *name)
{
    if (failures == 1)
    {
        if (hostname)
            syslog(LOG_NOTICE, _("LOGIN FAILURE FROM %s, %s"), hostname, name);
        else
            syslog(LOG_NOTICE, _("LOGIN FAILURE ON %s, %s"), "tty" /*tty_name*/,
                   name);
    }
    else
    {
        if (hostname)
            syslog(LOG_NOTICE, _("%d LOGIN FAILURES FROM %s, %s"), failures,
                   hostname, name);
        else
            syslog(LOG_NOTICE, _("%d LOGIN FAILURES ON %s, %s"), failures,
                   "tty" /*tty_name*/, name);
    }
}

/* Should not be called from PAM code... */
void sleepexit(int eval)
{
    sleep(SLEEP_EXIT_TIMEOUT);
    exit(eval);
}
