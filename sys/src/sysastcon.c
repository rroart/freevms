int exe$setast(char enbflg) {

}

int exe$dclast(void (*astadr)(__unknown_params), unsigned long astprm, unsigned int acmode) {
  struct _acb * a=vmalloc(sizeof(struct _acb));
  a->acb$l_pid=current->pid;
  a->acb$l_ast=astadr;
  a->acb$l_astprm=astprm;
  sch$qast(current->pid,PRI$_NULL,a);
}
