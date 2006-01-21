$ write sys$output %STDRV-I-STARTUP,_FreeVMS_startup_begun
$ install add sys$system:authorize /priv=audit
$ install add sys$system:sda
$ install add sys$system:show /priv=(cmkrnl,world,netmbx,audit)
$ write sys$output %SET-I-INTSET,_login_interactive_limit_=_..,_current_interactive_value_=_0
