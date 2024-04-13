/*
============================================================
                        DEMO INFO

DEMO NAME:          dynamic_array.c
DEMO WRITTEN BY:    Muukid
CREATION DATE:      2024-04-13
LAST UPDATED:       2024-04-13

============================================================
                        DEMO PURPOSE

This demo shows the basics behind creating, modifying, and
destroying a normal dynamic array using muma.

============================================================

============================================================
                        LICENSE INFO

All code is licensed under MIT License or public domain, 
whichever you prefer.
More explicit license information at the end of file.

============================================================
*/

// Include muma
#define MUMA_NAMES // (for muma_result_get_name)
#define MUMA_IMPLEMENTATION
#include <muMemoryAllocator.h>

// Include stdio for printing
#include <stdio.h>

/* Create a dynamic float array */

// Specify our comparison function

muBool float_comp(float f0, float f1) {
	return f0 == f1;
}

// Create our dynamic array

mu_dynamic_array_declaration(
	float_arr, // dynamic array struct name
	float, // element type
	float_arr_, // dynamic array function name prefix
	float_comp // comparison function
)

// Create a function to print our list, which we'll do a few times

void print_floats(float_arr floats) {
	printf("{ ");

	for (size_m i = 0; i < floats.length; i++) {
		printf("%.1f", floats.data[i]);

		if (i+1 < floats.length) {
			printf(", ");
		}
	}

	printf(" }\n");
}

int main(void) {
	// Create a result variable to check result of functions

	mumaResult result;

	/* Set up dynamic array */

		// Create a float dynamic array of length 5

		float_arr floats = float_arr_create(&result, 5);
		if (result != MUMA_SUCCESS)
			printf("WARNING: float_arr_create returned %s\n", muma_result_get_name(result));

		// Set elements to be: { 2.f, 3.f, 9.f, 6.f, 7.f }

		floats.data[0] = 2.f;
		floats.data[1] = 3.f;
		floats.data[2] = 9.f;
		floats.data[3] = 6.f;
		floats.data[4] = 7.f;

		// Print our array

		printf("Initial array: \n");
		print_floats(floats);

	/* Shifting */

		// Shift element 2 (9.f) right by 3
		// This should result in the array now being:
		// { 2.f, 3.f, 0.f, 0.f, 0.f, 9.f, 6.f, 7.f }

		floats = float_arr_rshift(&result, floats, 2, 3);
		if (result != MUMA_SUCCESS)
			printf("WARNING: float_arr_rshift returned %s\n", muma_result_get_name(result));

		// Print our array

		printf("Array after right shift: \n");
		print_floats(floats);

		// Shift element 5 (9.f) left by 4
		// This should result in:
		// { 2.f, 9.f, 6.f, 7.f }

		floats = float_arr_lshift(&result, floats, 5, 4);
		if (result != MUMA_SUCCESS)
			printf("WARNING: float_arr_lshift returned %s\n", muma_result_get_name(result));

		// Print our array

		printf("Array after left shift: \n");
		print_floats(floats);

	/* Insert */

		// Declare another array

		float other_arr[] = {
			4.f, 3.f, 1.f
		};

		// Insert that array into the dynamic array at index 2
		// This should result in:
		// { 2.f, 9.f, 4.f, 3.f, 1.f, 6.f, 7.f }

		floats = float_arr_multiinsert(&result, floats, 2, other_arr, sizeof(other_arr) / sizeof(float));
		if (result != MUMA_SUCCESS)
			printf("WARNING: float_arr_multiinsert returned %s\n", muma_result_get_name(result));

		// Print our array

		printf("Array after insertion: \n");
		print_floats(floats);

	/* Erasing */

		// Erase from index 2 (4.f) to 5 (6.f)
		// Ie, erase at index 2, length 4 elements
		// This should result in:
		// { 2.f, 9.f, 7.f }

		floats = float_arr_multierase(&result, floats, 2, 4);
		if (result != MUMA_SUCCESS)
			printf("WARNING: float_arr_multierase returned %s\n", muma_result_get_name(result));

		// Print our array

		printf("Array after erasing: \n");
		print_floats(floats);

	/* Push / Pop */

		// Push other_arr on top of dynamic array
		// This should result in:
		// { 2.f, 9.f, 7.f, 4.f, 3.f, 1.f }

		floats = float_arr_multipush(&result, floats, other_arr, sizeof(other_arr) / sizeof(float));
		if (result != MUMA_SUCCESS)
			printf("WARNING: float_arr_multipush returned %s\n", muma_result_get_name(result));

		// Print our array

		printf("Array after pushing: \n");
		print_floats(floats);

		// Pop 2 elements off of dynamic array
		// This should result in:
		// { 2.f, 9.f, 7.f, 4.f }

		floats = float_arr_multipop(&result, floats, 2);
		if (result != MUMA_SUCCESS)
			printf("WARNING: float_arr_multipop returned %s\n", muma_result_get_name(result));

		// Print our array

		printf("Array after popping: \n");
		print_floats(floats);

	/* Find */

		// Try and find 9.f
		// This should return index 1

		size_m index = float_arr_find(&result, floats, 9.f);
		if (result != MUMA_SUCCESS)
			printf("WARNING: float_arr_find returned %s\n", muma_result_get_name(result));

		// Print index (should be 1)
		if (index != MU_NONE) {
			printf("Index that contains 9.f: %i\n", (int)index);
		} else {
			printf("No element is 9.f\n");
		}

		// Try and find 3.f, which doesn't exist in the array
		// This should return MU_NONE

		index = float_arr_find(&result, floats, 3.f);

		// Result should be MUMA_NOT_FOUND since it, indeed, shouldn't exist
		if (result != MUMA_SUCCESS)
			printf("float_arr_find returned %s, which we expect\n", muma_result_get_name(result));

		// Print index (should be MU_NONE)
		if (index == MU_NONE) {
			printf("No element is 3.f\n");
		} else {
			printf("Index that contains 3.f: %i\n", (int)index);
		}

	/* Find/Push */

		// Try and find/push 7.f
		// Because 7.f exists, this should not modify our array

		floats = float_arr_find_push(&result, floats, 7.f, &index);
		if (result != MUMA_SUCCESS)
			printf("WARNING: float_arr_find_push returned %s\n", muma_result_get_name(result));

		// Print our array (which should be the same)

		printf("Array after find/pushing 7.f: \n");
		print_floats(floats);

		// Print our index (which should be 2)

		printf("Index that contains 7.f: %i\n", (int)index);

		// Try and find/push 8.f
		// Because 8.f doesn't exist, this should automatically push it onto our array

		floats = float_arr_find_push(&result, floats, 8.f, &index);
		if (result != MUMA_SUCCESS)
			printf("WARNING: float_arr_find_push returned %s\n", muma_result_get_name(result));

		// Print our array, which should now be the same but with 8.f at the end:
		// { 2.f, 9.f, 7.f, 4.f, 8.f }

		printf("Array after find/pushing 8.f: \n");
		print_floats(floats);

		// Print our index (which should be 4)

		printf("Index that contains 8.f: %i\n", (int)index);

	/* End */

		// Destroy the array (necessary to free memory)

		floats = float_arr_destroy(&result, floats);

	// In total, the program should've printed:
	/*
	Initial array: 
	{ 2.0, 3.0, 9.0, 6.0, 7.0 }
	Array after right shift: 
	{ 2.0, 3.0, 0.0, 0.0, 0.0, 9.0, 6.0, 7.0 }
	Array after left shift: 
	{ 2.0, 9.0, 6.0, 7.0 }
	Array after insertion: 
	{ 2.0, 9.0, 4.0, 3.0, 1.0, 6.0, 7.0 }
	Array after erasing: 
	{ 2.0, 9.0, 7.0 }
	Array after pushing: 
	{ 2.0, 9.0, 7.0, 4.0, 3.0, 1.0 }
	Array after popping: 
	{ 2.0, 9.0, 7.0, 4.0 }
	Index that contains 9.f: 1
	float_arr_find returned MUMA_NOT_FOUND, which we expect
	No element is 3.f
	Array after find/pushing 7.f: 
	{ 2.0, 9.0, 7.0, 4.0 }
	Index that contains 7.f: 2
	Array after find/pushing 8.f: 
	{ 2.0, 9.0, 7.0, 4.0, 8.0 }
	Index that contains 8.f: 4
	*/

	return 0;
}

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2024 Hum
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/

