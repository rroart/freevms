//+++2001-10-06
//    Copyright (C) 2001, Mike Rieker, Beverly, MA USA
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; version 2 of the License.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---2001-10-06

/************************************************************************/
/*									*/
/*  Write to the screen							*/
/*									*/
/************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "edt.h"

static char outputbuf[1024];
static int outputlen = 0;

void outerr (int extra, const char *format, ...)

{
  char *buff;
  va_list ap;

  buff = malloc (extra + strlen (format) + 1);
  va_start (ap, format);
  vsprintf (buff, format, ap);
  va_end (ap);
  outstr (buff);
  free (buff);

  output ();
}

void outfmt (int extra, const char *format, ...)

{
  char *buff;
  va_list ap;

  buff = malloc (extra + strlen (format) + 1);
  va_start (ap, format);
  vsprintf (buff, format, ap);
  va_end (ap);
  outstr (buff);
  free (buff);
}

void outstr (const char *string)

{
  outbuf (strlen (string), string);
}

void outchr (char c)

{
  outbuf (1, &c);
}

void outbuf (int size, const char *buff)

{
  while (size + outputlen >= sizeof outputbuf) {
    memcpy (outputbuf + outputlen, buff, sizeof outputbuf - outputlen);
    size -= sizeof outputbuf - outputlen;
    buff += sizeof outputbuf - outputlen;
    outputlen = sizeof outputbuf;
    output ();
  }
  memcpy (outputbuf + outputlen, buff, size);
  outputlen += size;

#if 00
  output (); /** ?? debugging ?? **/
#endif
}

void output (void)

{
  if (outputlen > 0) {
    os_writebuffer (outputlen, outputbuf);
    outputlen = 0;
  }
}
