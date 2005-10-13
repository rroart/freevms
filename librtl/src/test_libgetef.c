/*
** If this test routine prints out "BROKEN" - something is wrong.
*/

#define __NEW_STARLET
#include <lib$routines.h>
#include <libdef.h>						/* lib messages */
#include <ssdef.h>						/* system messages */

#include <stdio.h>

int main(void)
    {

    /* Test out initial freeing of event flags */

    unsigned int efn = 0;
    unsigned int status = lib$free_ef(&efn);
    if (status != LIB$_EF_RESSYS) printf("Broken: Can never free event flag %d, status %u\n",efn,status);
    for (efn = 1; efn <= 23; efn++)
	{
	status = lib$free_ef(&efn);
	if (status != SS$_NORMAL) printf("Broken: Should be able to free 1-13, efn %d, status %u\n",efn,status);
	}
    for (efn = 24; efn <= 31; efn++)
	{
	status = lib$free_ef(&efn);
	if (status != LIB$_EF_RESSYS) printf("Broken: Can never free event flag %d, status %u\n",efn,status);
	}
    for (efn = 32; efn <= 63; efn++)
	{
	status = lib$free_ef(&efn);
	if (status != LIB$_EF_ALRFRE) printf("Broken: Should already be free 32-63, efn %d, status %u\n",efn,status);
	}
    efn = 64;
    status = lib$free_ef(&efn);
    if (status != LIB$_EF_RESSYS) printf("Broken: Can never free event flag %d, status %u\n",efn,status);

    /* Now try to get event flags */

    int cnt = 0;
    do
	{
        status = lib$get_ef(&efn);
	if (status == SS$_NORMAL) cnt++;
        } while (status == SS$_NORMAL);
    if (cnt != 55)
        printf("Broken:  Couldn't reserve 55 event flags\n");
    if (status != LIB$_INSEF || efn != (unsigned int)-1)
	printf("Broken: wrong status returned from lib$get_ef, status = %u\n",status);

    /* Free them all again */

    efn = 0;
    status = lib$free_ef(&efn);
    if (status != LIB$_EF_RESSYS) printf("Broken: Can never free event flag %d, status %u\n",efn,status);
    for (efn = 1; efn <= 23; efn++)
	{
	status = lib$free_ef(&efn);
	if (status != SS$_NORMAL) printf("Broken: Should be able to free 1-13, efn %d, status %u\n",efn,status);
	}
    for (efn = 24; efn <= 31; efn++)
	{
	status = lib$free_ef(&efn);
	if (status != LIB$_EF_RESSYS) printf("Broken: Can never free event flag %d, status %u\n",efn,status);
	}
    for (efn = 32; efn <= 63; efn++)
	{
	status = lib$free_ef(&efn);
	if (status != SS$_NORMAL) printf("Broken: Should be able to free 32-63, efn %d, status %u\n",efn,status);
	}
    efn = 64;
    status = lib$free_ef(&efn);
    if (status != LIB$_EF_RESSYS) printf("Broken: Can never free event flag %d, status %u\n",efn,status);

    /* free them again and check status values */

    for (efn = 1; efn <= 23; efn++)
	{
	status = lib$free_ef(&efn);
	if (status != LIB$_EF_ALRFRE) printf("Broken: Should already be free 1-23, efn %d, status %u\n",efn,status);
	}
    for (efn = 24; efn <= 31; efn++)
	{
	status = lib$free_ef(&efn);
	if (status != LIB$_EF_RESSYS) printf("Broken: Can never free event flag %d, status %u\n",efn,status);
	}
    for (efn = 32; efn <= 63; efn++)
	{
	status = lib$free_ef(&efn);
	if (status != LIB$_EF_ALRFRE) printf("Broken: Should already be free 32-63, efn %d, status %u\n",efn,status);
	}

    efn = 0;
    status = lib$reserve_ef(&efn);
    if (status != LIB$_EF_RESSYS) printf("Broken: Can never reserve event flag %d, status %u\n",efn,status);
    for (efn = 1; efn <= 23; efn++)
        {
	status = lib$reserve_ef(&efn);
	if (status != SS$_NORMAL) printf("Broken: Should be able to reserve 1-23, efn %d, status %u\n",efn,status);
        }
    for (efn = 24; efn <= 31; efn++)
	{
	status = lib$reserve_ef(&efn);
	if (status != LIB$_EF_RESSYS) printf("Broken: Can never reserve event flag %d, status %u\n",efn,status);
	}
    for (efn = 32; efn <= 63; efn++)
	{
	status = lib$reserve_ef(&efn);
	if (status != SS$_NORMAL) printf("Broken: Should be able to reserve 32-63, efn %d, status %u\n",efn,status);
	}
    efn = 64;
    status = lib$reserve_ef(&efn);
    if (status != LIB$_EF_RESSYS) printf("Broken: Can never reserve event flag %d, status %u\n",efn,status);

    /* Try to reserve them twice */

    for (efn = 1; efn <= 23; efn++)
        {
	status = lib$reserve_ef(&efn);
	if (status != LIB$_EF_ALRRES) printf("Broken: Should be able to reserve 32-63, efn %d, status %u\n",efn,status);
        }
    for (efn = 24; efn <= 31; efn++)
	{
	status = lib$reserve_ef(&efn);
	if (status != LIB$_EF_RESSYS) printf("Broken: Can never reserve event flag %d, status %u\n",efn,status);
	}
    for (efn = 32; efn <= 63; efn++)
	{
	status = lib$reserve_ef(&efn);
	if (status != LIB$_EF_ALRRES) printf("Broken: Should be able to reserve 32-63, efn %d, status %u\n",efn,status);
	}

    return 0;
    }
