// $Id$
// $Locker$

// Author. Roar Thronæs.

int vms_mm=0;

main(int argc, char ** argv) {
  extern int yydebug;
  yydebug=1;
  return cdumain(argc,argv);
}
