/*
 * Copyright (C) 2001 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

extern int modify_ldt(int func, void *ptr, unsigned long bytecount);

int sys_modify_ldt(int func, void *ptr, unsigned long bytecount)
{
       return modify_ldt(func, ptr, bytecount);
}

/*
 * Overrides for Emacs so that we follow Linus's tabbing style.
 * Emacs will notice this stuff at the end of the file and automatically
 * adjust the settings for this buffer only.  This must remain at the end
 * of the file.
 * ---------------------------------------------------------------------------
 * Local variables:
 * c-file-style: "linux"
 * End:
 */
