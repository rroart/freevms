/*
 * This file tests the lib$addx, lib$subx, and other associated
 * functions
 */
#include <stdio.h>
#include <stdlib.h>
#include "lib$routines.h"


void print_array(const unsigned char* Array, int length);

int main()
{
	unsigned char array1[8] = {100,200, 80, 90,125,215, 10,  2};
	unsigned char array2[8] = { 50,100,250, 10, 10, 88, 10,  1};
	unsigned char array3[8] = {  0,  0,  0,  0,  0,  0,  0,  0};
	const long arraysize = 2;		/* Array size in longwords */
	unsigned long status;

	printf("Array1: ");
	print_array(array1, arraysize);

	printf("Array2: ");
	print_array(array2, arraysize);

	status = lib$addx(array1, array2, array3, &arraysize);
	printf("Status from Array3 = Array1 + Arrary2: %ld\n", status);

	printf("Addx  : ");
	print_array(array3, arraysize);

	status = lib$subx(array1, array2, array3, &arraysize);
	printf("Status from Array3 = Array1 - Arrary2: %ld\n", status);

	printf("Subx  : ");
	print_array(array3, arraysize);

	return EXIT_SUCCESS;
}

void print_array(const unsigned char* Array, int length)
{
	int loop;

	for (loop = 0; loop < (length * 4); loop++)
	{
		printf("%4d", Array[loop]);
	}
	printf("\n");
}

