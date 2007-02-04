$ mount dqa2
$ set def dqa2:
$ crea /dire vms$common.dir
$ set def [.vms$common]
$ crea /dire sysexe.dir
$ set def dqa1:[000000]
$ copy [boot]freevms.iomm dqa2:[000000]freevms1.elf
$ copy [vms$common.sysexe]dcl dqa2:[vms$common.sysexe]dcl
$ write sys$output "%INSTALL-I-STARTUP, FreeVMS limited ODS2 install done"
$ write sys$output "%SET-I-INTSET, login interactive limit = .., current interactive value = 0"
