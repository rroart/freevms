// $Id$
// $Locker$

// Author. Roar Thronæs.

// temporary module. should really be at least one privileged shareable image

#include<linux/linkage.h>

asmlinkage void exe$check_access    (void) { }
asmlinkage void exe$getuai          (void) { }
asmlinkage void exe$setuai          (void) { }
asmlinkage void exe$idtoasc         (void) { }
asmlinkage void exe$asctoid         (void) { }
asmlinkage void exe$add_ident       (void) { }
asmlinkage void exe$rem_ident       (void) { }
asmlinkage void exe$find_held       (void) { }
asmlinkage void exe$find_holder     (void) { }
asmlinkage void exe$mod_ident       (void) { }
asmlinkage void exe$mod_holder      (void) { }
asmlinkage void exe$grantid      (void) { }
asmlinkage void exe$revokid      (void) { }
