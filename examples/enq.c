#include <stdio.h> 
#include <descrip.h> 
#include <lckdef.h> 
#include <misc.h>
#include <starlet.h>
  
main(){

struct _lksb lksb;

         unsigned int status, lkmode, flags; 
         $DESCRIPTOR(resource,"STRUCTURE_1"); 

         lkmode = LCK$K_NLMODE; 
  
         status = sys$enq(0,  
                   lkmode,      
                   &lksb,        
                   0,            
                    &resource,   
                   0, 0, 0, 0, 0, 0); 

sleep(5);
printf("%x %x\n",lksb.lksb$w_status,lksb.lksb$l_lkid);

         lkmode = LCK$K_PWMODE; 
         flags = LCK$M_CONVERT; 
  
         status = sys$enq(0,     
                   lkmode,        
                   &lksb,         
                   flags,         
                   0, 0, 0, 0, 0, 0, 0); 

sleep(5);
printf("%x %x\n",lksb.lksb$w_status,lksb.lksb$l_lkid);

 } 
