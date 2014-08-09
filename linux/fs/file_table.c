// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004  

/*
 *  linux/fs/file_table.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *  Copyright (C) 1997 David S. Miller (davem@caip.rutgers.edu)
 */

#include <linux/string.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/smp_lock.h>
#include <linux/iobuf.h>

/* sysctl tunables... */
struct files_stat_struct files_stat = {0, 0, NR_FILE};

/* Here the new files go */
static LIST_HEAD(anon_list);
/* And here the free ones sit */
static LIST_HEAD(free_list);
/* public *and* exported. Not pretty! */
spinlock_t files_lock = SPIN_LOCK_UNLOCKED;

/* Find an unused file structure and return a pointer to it.
 * Returns NULL, if there are no more free file structures or
 * we run out of memory.
 *
 * SMP-safe.
 */
struct file * get_empty_filp(void)
{
	static int old_max = 0;
	struct file * f;

	file_list_lock();
	if (files_stat.nr_free_files > NR_RESERVED_FILES) {
	used_one:
		f = list_entry(free_list.next, struct file, f_list);
		list_del(&f->f_list);
		files_stat.nr_free_files--;
	new_one:
		memset(f, 0, sizeof(*f));
		atomic_set(&f->f_count,1);
		f->f_version = ++event;
		f->f_uid = current->fsuid;
		f->f_gid = current->fsgid;
		list_add(&f->f_list, &anon_list);
		file_list_unlock();
		return f;
	}
	/*
	 * Use a reserved one if we're the superuser
	 */
	if (files_stat.nr_free_files && !current->euid)
		goto used_one;
	/*
	 * Allocate a new one if we're below the limit.
	 */
	if (files_stat.nr_files < files_stat.max_files) {
		file_list_unlock();
		f = kmalloc(sizeof(*f), GFP_KERNEL);
		memset(f, 0, sizeof(*f));
		file_list_lock();
		if (f) {
			files_stat.nr_files++;
			goto new_one;
		}
		/* Big problems... */
		printk(KERN_WARNING "VFS: filp allocation failed\n");

	} else if (files_stat.max_files > old_max) {
		printk(KERN_INFO "VFS: file-max limit %d reached\n", files_stat.max_files);
		old_max = files_stat.max_files;
	}
	file_list_unlock();
	return NULL;
}

void fastcall fput(struct file * file)
{
}

struct file * fastcall fget(unsigned int fd)
{
	struct file * file;
	struct files_struct *files = current->files;

	read_lock(&files->file_lock);
	file = fcheck(fd);
	if (file)
		get_file(file);
	read_unlock(&files->file_lock);
	return file;
}

/* Here. put_filp() is SMP-safe now. */

void put_filp(struct file *file)
{
}

void file_move(struct file *file, struct list_head *list)
{
	if (!list)
		return;
	file_list_lock();
	list_del(&file->f_list);
	list_add(&file->f_list, list);
	file_list_unlock();
}

int fs_may_remount_ro(struct super_block *sb)
{
	struct list_head *p;

	/* Check that no files are currently opened for writing. */
	file_list_lock();
	for (p = sb->s_files.next; p != &sb->s_files; p = p->next) {
		struct file *file = list_entry(p, struct file, f_list);
		struct inode *inode = file->f_dentry->d_inode;

		/* File with pending delete? */
		if (inode->i_nlink == 0)
			goto too_bad;

		/* Writable file? */
		if (S_ISREG(inode->i_mode) && (file->f_mode & FMODE_WRITE))
			goto too_bad;
	}
	file_list_unlock();
	return 1; /* Tis' cool bro. */
too_bad:
	file_list_unlock();
	return 0;
}
