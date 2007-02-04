$ write sys$output "%STDRV-I-STARTUP, FreeVMS startup begun"
$ install add sys$system:authorize /priv=audit
$ install add sys$system:sda
$ install add sys$system:show /priv=(cmkrnl,world,netmbx,audit)
$ write sys$output "%SET-I-INTSET, login interactive limit = .., current interactive value = 0"
