#ifndef _HOSTFS_FS_I
#define _HOSTFS_FS_I

struct hostfs_inode_info {
	char *host_filename;
	int fd;
	int mode;
};

#endif

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
