#ifndef ILEUTIL_H
#define ILEUTIL_H

#include <iledef.h>

#include <linux/types.h>

inline int check_ile3(struct _ile3 *ile);

inline void set_ile3(struct _ile3 *ile, void *p, unsigned short length);

inline void set_ile3_from_string(struct _ile3 *ile, char *s);

inline void set_ile3_from_char(struct _ile3 *ile, char c);
inline void set_ile3_from_short(struct _ile3 *ile, short n);
inline void set_ile3_from_int(struct _ile3 *ile, int n);
inline void set_ile3_from_long(struct _ile3 *ile, long n);
inline void set_ile3_from_long_long(struct _ile3 *ile, long long n);

inline void set_ile3_from_unsigned_char(struct _ile3 *ile, unsigned char c);
inline void set_ile3_from_unsigned_short(struct _ile3 *ile, unsigned short n);
inline void set_ile3_from_unsigned_int(struct _ile3 *ile, unsigned int n);
inline void set_ile3_from_unsigned_long(struct _ile3 *ile, unsigned long n);
inline void set_ile3_from_unsigned_long_long(struct _ile3 *ile, unsigned long long n);

#endif /* ILEUTIL_H */
