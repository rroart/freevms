/*
 *      This is part of ODS2 initialize utility written by Zakaria Yassine,
 *      email address:  zakaria@sympatico.ca
 *
 *      This work is based on ODS2 specifications (see file ods2.doc). As
 *      part of the FreeVMS project, (see http://freevms.free.fr/indexGB.html),
 *      this work is under GNU GPL license.
 *
 * Modifications:
 *
 *   30-SEP-2003        Zakaria Yassine <zakaria@sympatico.ca>
 *      Initiale version. Device size hardcoded, geometry not set, and many
 *      other things :-)        Usage: init /dev/hdb4
 *
 * Report Bugs:
 *   To: FreeVMS project mailing list <http://freevms.nvg.org/>
 *       or directely to me.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned char in_line[512];

int main(int argc, char *argv[])
{
    int i;
    if (argc>1){
        FILE *fout = fopen(argv[1],"wb");
        create_ods2(fout);
        close(fout);
	return 0;
    }
  return 0;
}

