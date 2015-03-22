#ifndef VARARGS_H
#define VARARGS_H

typedef char *va_list;

#if __GNUC__ > 1
#define va_dcl  int va_alist; ...
#else
#define va_dcl  int va_alist;
#endif

#ifdef __GNUC__
#define va_alist        __builtin_va_alist
#endif

#endif
