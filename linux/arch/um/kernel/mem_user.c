/*
 * arch/um/kernel/mem_user.c
 *
 * BRIEF MODULE DESCRIPTION
 * user side memory routines for supporting IO memory inside user mode linux
 *
 * Copyright (C) 2001 RidgeRun, Inc.
 * Author: RidgeRun, Inc.
 *         Greg Lonnon glonnon@ridgerun.com or info@ridgerun.com
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "kern_util.h"
#include "user.h"
#include "user_util.h"
#include "init.h"

struct mem_region {
	struct mem_region *next;
	char *driver;
	unsigned long start;
	unsigned long usable;
	unsigned long total;
	int fd;
};

struct mem_region physmem_region;

struct mem_region *mem_list = &physmem_region;

#define TEMPNAME_TEMPLATE "vm_file-XXXXXX"

int create_mem_file(unsigned long len)
{
	int fd;
	char zero;

	fd = make_tempfile(TEMPNAME_TEMPLATE, NULL, 1);
	if (fchmod(fd, 0777) < 0){
		perror("fchmod");
		exit(1);
	}
	if(lseek(fd, len, SEEK_SET) < 0){
		perror("lseek");
		exit(1);
	}
	zero = 0;
	if(write(fd, &zero, 1) != 1){
		perror("write");
		exit(1);
	}
	if(fcntl(fd, F_SETFD, 1) != 0)
		perror("Setting FD_CLOEXEC failed");
	return(fd);
}

void setup_range(int fd, char *driver, unsigned long start, 
		 unsigned long usable, unsigned long total)
{
	struct mem_region *region, *next;

	if(fd == -1){
		fd = create_mem_file(usable);
		region = &physmem_region;
		next = physmem_region.next;
	}
	else {
		region = malloc(sizeof(*region));
		if(region == NULL){
			perror("Allocating iomem struct");
			exit(1);
		}
		next = physmem_region.next;
	}
	*region = ((struct mem_region) { next, driver, start, usable, 
					 total, fd } );
	if(region != &physmem_region) physmem_region.next = region;
}

void __init setup_memory(void)
{
	struct mem_region *region;
	void *loc;
	unsigned long start;
	int page;

	start = -1;
	region = mem_list;
	page = page_size();
	while(region){
		if(region->start != -1) start = region->start;
		else region->start = start;
		loc = mmap((void *) region->start, region->usable,
			   PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED,
			   region->fd, 0);
		if(loc != (void *) region->start){
			perror("Mapping memory");
			exit(1);
		}
		start += region->total;
		start = (start + page - 1) & ~(page - 1);
		region = region->next;
	}
}

static int __init parse_iomem(char *str, int *add)
{
	struct stat buf;
	char *file, *driver;
	int fd;

	driver = str;
	file = strchr(str,',');
	if(file == NULL){
		printk(__FUNCTION__ " failed to parse iomem\n");
		return 1;
	}
	*file = '\0';
	file++;
	fd = open(file, O_RDWR);
	if(fd < 0){
		perror("Couldn't open io file");
		return 1;
	}
	if(fstat(fd, &buf) < 0) {
		perror(__FUNCTION__ "fstat - cannot fstat file");
		exit(1);
	}
	setup_range(fd, driver, -1, buf.st_size, buf.st_size);
	return 0;
}
__uml_setup("iomem=",parse_iomem,
"iomem=<name>,<file>\n"
"    Configure <file> as a named IO memory region named <name>.\n\n"
);

#ifdef notdef
int logging = 0;
int logging_fd = -1;

int logging_line = 0;
char logging_buf[256];

void log(char *fmt, ...)
{
	va_list ap;
	struct timeval tv;

	if(logging == 0) return;
	if(logging_fd == -1)
		logging_fd = open("log", O_RDWR | O_CREAT | O_TRUNC, 0644);
	gettimeofday(&tv, NULL);
	sprintf(logging_buf, "%d\t %u.%u  ", logging_line++, tv.tv_sec, 
		tv.tv_usec);
	va_start(ap, fmt);
	vsprintf(&logging_buf[strlen(logging_buf)], fmt, ap);
	va_end(ap);
	write(logging_fd, logging_buf, strlen(logging_buf));
}
#endif

void map(unsigned long virt, void *p, unsigned long len, 
	 int r, int w, int x)
{
	struct mem_region *region;
	unsigned long phys = (unsigned long) p;
	void *loc;
	int prot;

	prot = (r ? PROT_READ : 0) | (w ? PROT_WRITE : 0) | 
		(x ? PROT_EXEC : 0);
	for(region = mem_list; region ; region = region->next) {
		if((phys < region->start) || 
		   (phys >= region->start + region->usable))
			continue;
		phys -= region->start;
		loc = mmap((void *) virt, len, prot, MAP_SHARED | MAP_FIXED, 
			   region->fd, phys);
		if(loc != (void *) virt){
			panic("Error mapping a page - errno = %d", errno);
		}
		return;
	}
	panic("No physical or IO memory region for address 0x%x\n", phys);
}

int unmap(void *addr, int len)
{
	int err;

	err = munmap(addr, len);
	if(err < 0) return(-errno);
	else return(err);
}

int protect(unsigned long addr, unsigned long len, int r, int w, int x,
	     int must_succeed)
{
	int prot;

	prot = (r ? PROT_READ : 0) | (w ? PROT_WRITE : 0) | 
		(x ? PROT_EXEC : 0);
	if(mprotect((void *) addr, len, prot) == -1){
		if(must_succeed)
			panic("protect failed, errno = %d", errno);
		else return(-errno);
	}
	return(0);
}

unsigned long find_iomem(char *driver, unsigned long *len_out)
{
	struct mem_region *region;

	for(region = mem_list; region ; region = region->next) {
		if((region->driver != NULL) &&
		   !strcmp(region->driver, driver)){
			*len_out = region->usable;
			return(region->start);
		}
	}
	*len_out = 0;
	return 0;
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
