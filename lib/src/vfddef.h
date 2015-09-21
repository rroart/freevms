#ifndef VFDDEF_H
#define VFDDEF_H

struct vms_fd
{
    INT32 vfd$l_is_cmu;
    void * vfd$l_fd_p;
    INT32 vfd$l_refcnt;
};

#endif
