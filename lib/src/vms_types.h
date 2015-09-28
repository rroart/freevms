#ifndef VMS_TYPES_H
#define VMS_TYPES_H

/* integer types */
typedef signed char INT8;
typedef unsigned char UINT8;

typedef signed short INT16;
typedef unsigned short UINT16;

typedef signed int INT32;
typedef unsigned int UINT32;

#if defined(__i386__)
typedef signed long long INT64;
typedef unsigned long long UINT64;
#elif defined(__x86_64__)
typedef signed long INT64;
typedef unsigned long UINT64;
#endif

/* other types */
typedef unsigned long ADDRESS; /* address/pointer value (TODO: is this really needed?) */
typedef int _STATUS; /* return status */

typedef UINT32 PID; /* process id */
typedef UINT16 CHAN_NUM; /* channel number */

/* TODO: merge with struct TIME into struct _generic_64 */
typedef INT64 _vms_time;

#endif /* VMS_TYPES_H */
