#ifndef __CVT_ROUTINES_H
#define __CVT_ROUTINES_H

/* From: David Wijnants */

  int cvt$convert_float (void*, int, void*, int, int);

  #ifdef __VMS

    #define cvt$convert_float(ival,ityp,oval,otyp,opt) \
           (cvt_convert_float)(ival,ityp,oval,otyp,opt)

    int cvt$convert_float (void*, int, void*, int, int);

  #endif

#endif
