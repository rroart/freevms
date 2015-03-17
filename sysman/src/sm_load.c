// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <descrip.h>
#include <cli$routines.h>

int init_module(void *, int, int);

int sm$load_driver()
{
    int retlen;
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
    char buf[1048576];
    int fd = open(c, 0);
    int size = read(fd, buf, 1048576);
    close(fd);

    init_module(buf, size, 0);
}

int sm$io_load(int mask)
{
    sm$load_driver();
}

int sm$io_connect(int mask)
{
    sm$io_load(0);
    // more;
}

