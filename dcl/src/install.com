$ write sys$output "ODS2 install starting"
$ write sys$output "creating vms$common"
$ mount dqa2
$ set def dqa2:
$ write sys$output "creating vms$common"
$ crea /dire vms$common.dir
$ set def [.vms$common]
$ write sys$output "creating syslib"
$ crea /dire syslib.dir
$ write sys$output "creating sysexe"
$ crea /dire sysexe.dir
$ write sys$output "creating systest"
$ crea /dire systest.dir
$ set def dqa1:[000000]
$ write sys$output "copying kernel"
$ copy [boot]freevms.iomm dqa2:[000000]freevms1.elf
$ set def dqa2:[vms$common.sysexe]
$ write sys$output "copying authorize"
$ copy sys$system:authorize authorize
$ write sys$output "copying copy"
$ copy sys$system:copy copy
$ write sys$output "copying create"
$ copy sys$system:create create
$ write sys$output "copying dcl"
$ copy sys$system:dcl dcl
$ write sys$output "copying delete"
$ copy sys$system:delete delete
$ write sys$output "copying dfu"
$ copy sys$system:dfu dfu
$ write sys$output "copying diff"
$ copy sys$system:diff diff
$ write sys$output "copying directory"
$ copy sys$system:directory directory
$ write sys$output "copying edt"
$ copy sys$system:edt edt
$ write sys$output "copying init"
$ copy sys$system:init init
$ write sys$output "copying install"
$ copy sys$system:install install
$ write sys$output "copying install.com"
$ copy sys$system:install.com install.com
$ write sys$output "copying ipacp"
$ copy sys$system:ipacp ipacp
$ write sys$output "copying loginout"
$ copy sys$system:loginout loginout
$ write sys$output "copying pagefile.sys"
$ copy sys$system:pagefile.sys pagefile.sys
$ write sys$output "copying sda"
$ copy sys$system:sda sda
$ write sys$output "copying search"
$ copy sys$system:search search
$ write sys$output "copying set"
$ copy sys$system:set set
$ write sys$output "copying show"
$ copy sys$system:show show
$ write sys$output "copying startup.com"
$ copy sys$system:startup.com startup.com
$ write sys$output "copying sysman"
$ copy sys$system:sysman sysman
$ write sys$output "copying sysuaf.dat"
$ copy sys$system:sysuaf.dat sysuaf.dat
$ write sys$output "copying type"
$ copy sys$system:type type
$ write sys$output "copying vmount"
$ copy sys$system:vmount vmount
$ set def [vms$common.systest]
$ write sys$output "copying test1.com"
$ copy sys$system:test1.com test1.com
$ write sys$output "copying test2.com"
$ copy sys$system:test2.com test2.com
$ write sys$output "copying test3.com"
$ copy sys$system:test3.com test3.com
$ write sys$output "copying test4.com"
$ copy sys$system:test4.com test4.com
$ write sys$output "copying test5.com"
$ copy sys$system:test5.com test5.com
$ write sys$output "copying test6.com"
$ copy sys$system:test6.com test6.com
$ write sys$output "copying test7.com"
$ copy sys$system:test7.com test7.com
$ write sys$output "copying test8.com"
$ copy sys$system:test8.com test8.com
$ write sys$output "%INSTALL-I-STARTUP, FreeVMS limited ODS2 install done"
