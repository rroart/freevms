#ifndef far_pointers_h
#define far_pointers_h

/* these few does not belong here really */
typedef int int32;
typedef short int int16;
typedef unsigned int uint32;
typedef unsigned short int uint16;

typedef int     	 * INT_PQ;	
typedef unsigned int	 * UINT_PQ;	
typedef unsigned int     * UINT32_PQ;	
typedef void             * VOID_PQ;	
typedef void            ** VOID_PPQ;	
typedef long long          * INT64_PQ;	
typedef unsigned long long * UINT64_PQ; 	
typedef unsigned long long ** UINT64_PPQ; 
typedef char		 * CHAR_PQ;	
typedef char		** CHAR_PPQ; 	
typedef short		 * SHORT_PQ;	
typedef unsigned short	 * USHORT_PQ;   

#if 0
typedef unsigned long long   INT_PQ;	
typedef unsigned long long   UINT_PQ;	
typedef unsigned long long   UINT32_PQ;	
typedef unsigned long long   VOID_PQ;	
typedef unsigned long long   VOID_PPQ;	
typedef unsigned long long   INT64_PQ;	
typedef unsigned long long   UINT64_PQ; 	
typedef unsigned long long   UINT64_PPQ; 	
typedef unsigned long long   CHAR_PQ;	
typedef unsigned long long   CHAR_PPQ; 	
typedef unsigned long long   SHORT_PQ;	
typedef unsigned long long   USHORT_PQ;	
#endif 

#endif 
