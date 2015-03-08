$ mysymbol="this is my symbol"
$ show symbol mysymbol
$ delete /symbol mysymbol
$ show symbol mysymbol
$ show logical sys$system
$ define mylogical test
$ show logical mylogical
$ show status
