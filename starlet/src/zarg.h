#ifndef zarg_h
#define zarg_h

#undef _buildz
#define _buildz(...) __VA_ARGS__, 0

#undef __buildargz
#define __buildargz(a,...) a

#undef __buildargz2
#define __buildargz2(a,b,...) a, b

#undef __buildargz3
#define __buildargz3(a,b,c,...) a, b, c

#undef __buildargz4
#define __buildargz4(a,b,c,d,...) a, b, c, d

#undef __buildargz5
#define __buildargz5(a,b,c,d,e,...) a, b, c, d, e

#undef __buildargz6
#define __buildargz6(a,b,c,d,e,f,...) a, b, c, d, e, f

#undef __buildargz7
#define __buildargz7(a,b,c,d,e,f,g,...) a, b, c, d, e, f, g

#undef __buildargz8
#define __buildargz8(a,b,c,d,e,f,g,h,...) a, b, c, d, e, f, g, h

#undef __buildargz9
#define __buildargz9(a,b,c,d,e,f,g,h,i,...) a, b, c, d, e, f, g, h, i

#undef __buildargz10
#define __buildargz10(a,b,c,d,e,f,g,h,i,j,...) a, b, c, d, e, f, g, h, i, j

#undef __buildargz11
#define __buildargz11(a,b,c,d,e,f,g,h,i,j,k,...) a, b, c, d, e, f, g, h, i, j, k

#undef __buildargz12
#define __buildargz12(a,b,c,d,e,f,g,h,i,j,k,l,...) a, b, c, d, e, f, g, h, i, j, k, l

#undef __buildargz13
#define __buildargz13(a,b,c,d,e,f,g,h,i,j,k,l,m,...) a, b, c, d, e, f, g, h, i, j, k, l, m

#undef __buildargz14
#define __buildargz14(a,b,c,d,e,f,g,h,i,j,k,l,m,n,...) a, b, c, d, e, f, g, h, i, j, k, l, m, n

#undef __buildargz15
#define __buildargz15(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o...) a, b, c, d, e, f, g, h, i, j, k, l, m, n, o

#undef _buildargz
#define _buildargz(...) __buildargz(__VA_ARGS__, 0)

#undef _buildargz2
#define _buildargz2(...) __buildargz2(__VA_ARGS__, 0, 0)

#undef _buildargz3
#define _buildargz3(...) __buildargz3(__VA_ARGS__, 0, 0, 0)

#undef _buildargz4
#define _buildargz4(...) __buildargz4(__VA_ARGS__, 0, 0, 0, 0)

#undef _buildargz5
#define _buildargz5(...) __buildargz5(__VA_ARGS__, 0, 0, 0, 0, 0)

#undef _buildargz6
#define _buildargz6(...) __buildargz6(__VA_ARGS__, 0, 0, 0, 0, 0, 0)

#undef _buildargz7
#define _buildargz7(...) __buildargz7(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0)

#undef _buildargz8
#define _buildargz8(...) __buildargz8(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0)

#undef _buildargz9
#define _buildargz9(...) __buildargz9(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0)

#undef _buildargz10
#define _buildargz10(...) __buildargz10(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

#undef _buildargz11
#define _buildargz11(...) __buildargz11(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

#undef _buildargz12
#define _buildargz12(...) __buildargz12(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

#undef _buildargz13
#define _buildargz13(...) __buildargz13(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

#undef _buildargz14
#define _buildargz14(...) __buildargz14(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

#undef _buildargz15
#define _buildargz15(...) __buildargz15(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

#endif
