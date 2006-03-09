// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/config.h>
#include <linux/linkage.h>
#include <linux/string.h>

#include <clidef.h>
#include <cliservdef.h>
#include <descrip.h>
#include <ssdef.h>
#include <system_data_cells.h>
#include <linux/slab.h>

// remember ctl$ag_clidata

#define MAXSYMNAM 64

typedef struct symbol symbol;

struct symbol { symbol *next;
  int namelen;
  char name[MAXSYMNAM];
  //Symtype symtype;
  //unsigned long (*func) (Symbol *symbol, char *strp, char **rtnp, void *valuep);
  //unsigned long ivalue;
  int svaluelen;
  char svalue[1];
};

int add_sym(struct dsc$descriptor * sym, struct dsc$descriptor * val) {
  symbol * s = kmalloc(sizeof(symbol)+64/*sym->dsc$w_length*/, GFP_KERNEL);
  s->namelen=sym->dsc$w_length;
  s->svaluelen=val->dsc$w_length;
  memcpy(s->name,sym->dsc$a_pointer,sym->dsc$w_length);
  memcpy(s->svalue,val->dsc$a_pointer,val->dsc$w_length);
  s->next=((symbol *)ctl$ag_clidata)/*->next*/;
  ctl$ag_clidata=s;
}

int mod_sym(symbol * sym, struct dsc$descriptor * val) {
  symbol * s = sym;
  s->svaluelen=val->dsc$w_length;
  memcpy(s->svalue,val->dsc$a_pointer,val->dsc$w_length);
}

int find_sym(struct dsc$descriptor * sym) {
  symbol * tmp = ctl$ag_clidata;
  while (tmp) {
    if (tmp->namelen==sym->dsc$w_length && 0==strncmp(sym->dsc$a_pointer,&tmp->name,tmp->namelen))
      return tmp;
    tmp=tmp->next;
  }
  return 0;
}

int del_sym(struct dsc$descriptor * sym) {
  int a = find_sym(sym);
  if (a==0) return 0;
  symbol ** tmp = &ctl$ag_clidata;
  while (*tmp) {
    if ((*tmp)->namelen==sym->dsc$w_length && 0==strncmp(sym->dsc$a_pointer,&(*tmp)->name,(*tmp)->namelen))
      break;
    tmp=(*tmp); // next
  }
  void * tmp2=*tmp;
  *tmp=(*tmp)->next;
  kfree(tmp2);
  return 0;
}

cre_or_mod_sym(struct dsc$descriptor * sym, struct dsc$descriptor * val) {
  symbol * s = find_sym(sym);
  if (s)
    mod_sym(s,val);
  else
    add_sym(sym,val);
}

asmlinkage int exe$cli(void * cliv, int par1, int par2) {
  struct _clidef1 * cli = cliv;
  struct _clidef2 * cli2 = cliv;
  if (cli->cli$b_rqtype!=CLI$K_CLISERV) {
    printk("cli rqtype is not CLISERV\n");
    return 0;
  }
  switch (cli->cli$w_servcod) {
  case CLI$K_DEFLOCAL:
    cre_or_mod_sym(&cli2->cli$q_namdesc,&cli2->cli$q_valdesc);
    break;
  case CLI$K_CREALOG:
    break;
  case CLI$K_DELELOG:
    printk("no delete logical yet\n");
    break;
  case CLI$K_GETSYM:
    {
      symbol * s = find_sym(&cli2->cli$q_namdesc);
      if (s) {
	struct dsc$descriptor * d = &cli2->cli$q_valdesc;
	d->dsc$w_length=s->svaluelen;
	memcpy(d->dsc$a_pointer,s->svalue,d->dsc$w_length);
      }
    }
    break;
  case CLI$K_DELELCL:
      del_sym(&cli2->cli$q_namdesc);
    break;
  default:
    printk("cli servcod %x not known\n",cli->cli$w_servcod);
    break;
  }
  return SS$_NORMAL;
}
