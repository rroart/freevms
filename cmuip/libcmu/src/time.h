#ifndef __TIME_LOADED
#define __TIME_LOADED   1

/*  TIME - V3.0 - Localtime() RTL Routine Return Structure Definition   */
/*
 * MODIFICATIONS:
 *  19-JAN-1994  mlo -- removed reference to sys/ in include statements
 *  19-MAY-1992  mlo -- extended tm structure to hold GMT and DST stuff
 */
# define CLK_TCK (100)

typedef  long int clock_t;

#ifndef __TYPES_LOADED
#include <types.h>
#endif

struct tbuffer
{
    int proc_user_time, proc_system_time;
    int child_user_time, child_system_time;
};

typedef struct tbuffer tbuffer_t;

#include <timeb.h>

struct tm
{
    int tm_sec, tm_min, tm_hour;
    int tm_mday, tm_mon, tm_year;
    int tm_wday, tm_yday, tm_isdst;
    long    tm_gmtoff;
    char    *tm_zone;
};

/*
 * Structure returned by gettimeofday(2) system call,
 * and used in other calls.
 */
struct timeval
{
    long    tv_sec;     /* seconds */
    long    tv_usec;    /* and microseconds */
};

struct timezone
{
    int tz_minuteswest; /* minutes west of Greenwich */
    int tz_dsttime; /* type of dst correction */
};
#define DST_NONE    0   /* not on dst */
#define DST_USA     1   /* USA style dst */
#define DST_AUST    2   /* Australian style dst */
#define DST_WET     3   /* Western European dst */
#define DST_MET     4   /* Middle European dst */
#define DST_EET     5   /* Eastern European dst */

typedef struct tm tm_t;

clock_t clock (void);

#undef difftime
double difftime (time_t time2, time_t time1);

time_t time (time_t *time_location);

void times (tbuffer_t *buffer);

char *asctime (const tm_t *timeptr);

char *ctime (const time_t *bintim);

tm_t *gmtime (const time_t *timer);

struct tm *localtime (const time_t *bintim);

time_t mktime (tm_t *timeptr);

void ftime (timeb_t *time_pointer);

#if defined(CC$mixed_float) || defined(CC$VAXCSHR)
/*
 * CC$gfloat's definition was modified so that it's defined when
 * g_float is in effect, and undefined otherwise.  The following makes
 * the new behavior compatible with the old.
 */
#ifndef CC$gfloat
#define CC$gfloat 0
#endif

#if CC$gfloat

double vaxc$gdifftime(time_t time2, time_t time1);
#define difftime vaxc$gdifftime

#else

double vaxc$ddifftime(time_t time2, time_t time1);
#define difftime vaxc$ddifftime

#endif
#endif

#endif                  /* __TIME_LOADED */
