#ifndef VMS_TYPES_H
#define VMS_TYPES_H

/* integer types */
typedef signed char _byte;
typedef unsigned char _ubyte;

typedef signed short _word;
typedef unsigned short _uword;

typedef signed int _longword;
typedef unsigned int _ulongword;

#if defined(__i386__)
typedef signed long long _quadword;
typedef unsigned long long _uquadword;
#elif defined(__x86_64__)
typedef signed long _quadword;
typedef unsigned long _uquadword;
#endif

typedef unsigned long _address;
typedef unsigned int _status;

typedef _quadword _vms_time;

#endif /* VMS_TYPES_H */
