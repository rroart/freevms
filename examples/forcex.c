#include <stdio.h> 
#include <ssdef.h> 
#include <descrip.h> 
#include <time.h>
#include <stdlib.h>

main(int argc, char ** argv){

unsigned int pid=atoi(argv[1]);
 
sys$forcex(&pid,0,42);

printf("after forcex %x\n",pid);

}
