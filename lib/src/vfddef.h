#ifndef vfddef_h
#define vfddef_h

struct vms_fd
{
    int vfd$l_is_cmu;
    void * vfd$l_fd_p;
    int vfd$l_refcnt;
};

#endif
