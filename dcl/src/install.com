$ mount dqa2
$ copy [boot]freevms.iomm dqa2:[000000]freevms1.elf
$ copy [vms$common.sysexe]dcl dqa2:[000000]dcl
$ write sys$output %INSTALL-I-STARTUP,_FreeVMS_limited_ODS2_install_done
$ write sys$output %SET-I-INTSET,_login_interactive_limit_=_..,_current_interactive_value_=_0
