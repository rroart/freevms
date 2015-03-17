/*  Kernel module help for i386.
    Copyright (C) 2001 Rusty Russell.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Shdr Elf32_Shdr
#define Elf_Sym Elf32_Sym
#include <linux/moduleloader.h>
#include <linux/elf.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/kernel.h>

#if 0
#define DEBUGP printk
#else
#define DEBUGP(fmt...)
#endif

void *module_alloc(unsigned long size)
{
    if (size == 0)
        return NULL;
#if 0
    return vmalloc_exec(size);
#else
    return kmalloc(size, GFP_KERNEL);
#endif
}


/* Free memory returned from module_alloc */
void module_free(struct module *mod, void *module_region)
{
#if 0
    vfree(module_region);
#else
    kfree(module_region);
#endif
    /* FIXME: If module_region == mod->init_region, trim exception
           table entries. */
}

/* We don't need anything special. */
int module_frob_arch_sections(Elf_Ehdr *hdr,
                              Elf_Shdr *sechdrs,
                              char *secstrings,
                              struct module *mod)
{
    return 0;
}

int apply_relocate(Elf32_Shdr *sechdrs,
                   const char *strtab,
                   unsigned int symindex,
                   unsigned int relsec,
                   struct module *me)
{
    unsigned int i;
    Elf32_Rel *rel = (void *)sechdrs[relsec].sh_addr;
    Elf32_Sym *sym;
    uint32_t *location;

    DEBUGP("Applying relocate section %u to %u\n", relsec,
           sechdrs[relsec].sh_info);
    for (i = 0; i < sechdrs[relsec].sh_size / sizeof(*rel); i++)
    {
        /* This is where to make the change */
        location = (void *)sechdrs[sechdrs[relsec].sh_info].sh_addr
                   + rel[i].r_offset;
        /* This is the symbol it is referring to.  Note that all
           undefined symbols have been resolved.  */
        sym = (Elf32_Sym *)sechdrs[symindex].sh_addr
              + ELF32_R_SYM(rel[i].r_info);

        switch (ELF32_R_TYPE(rel[i].r_info))
        {
        case R_386_32:
            /* We add the value into the location given */
            *location += sym->st_value;
            break;
        case R_386_PC32:
            /* Add the value, subtract its postition */
            *location += sym->st_value - (uint32_t)location;
            break;
        default:
            printk(KERN_ERR "module %s: Unknown relocation: %u\n",
                   me->name, ELF32_R_TYPE(rel[i].r_info));
            return -ENOEXEC;
        }
    }
    return 0;
}

int apply_relocate_add(Elf32_Shdr *sechdrs,
                       const char *strtab,
                       unsigned int symindex,
                       unsigned int relsec,
                       struct module *me)
{
    printk(KERN_ERR "module %s: ADD RELOCATION unsupported\n",
           me->name);
    return -ENOEXEC;
}

int module_finalize(const Elf_Ehdr *hdr,
                    const Elf_Shdr *sechdrs,
                    struct module *me)
{
    const Elf_Shdr *s, *text = NULL, *alt = NULL, *locks = NULL;
    char *secstrings = (void *)hdr + sechdrs[hdr->e_shstrndx].sh_offset;

    for (s = sechdrs; s < sechdrs + hdr->e_shnum; s++)
    {
        if (!strcmp(".text", secstrings + s->sh_name))
            text = s;
        if (!strcmp(".altinstructions", secstrings + s->sh_name))
            alt = s;
        if (!strcmp(".smp_locks", secstrings + s->sh_name))
            locks= s;
    }

    if (alt)
    {
        /* patch .altinstructions */
        void *aseg = (void *)alt->sh_addr;
#if 0
        apply_alternatives(aseg, aseg + alt->sh_size);
#endif
    }
    if (locks && text)
    {
        void *lseg = (void *)locks->sh_addr;
        void *tseg = (void *)text->sh_addr;
#if 0
        alternatives_smp_module_add(me, me->name,
                                    lseg, lseg + locks->sh_size,
                                    tseg, tseg + text->sh_size);
#endif
    }
    return 0;
}

void module_arch_cleanup(struct module *mod)
{
#if 0
    alternatives_smp_module_del(mod);
#endif
}
