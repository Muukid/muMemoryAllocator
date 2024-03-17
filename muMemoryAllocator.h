/*
muMemoryAllocator.h - Muukid
Public domain simple single-file C library for easy dynamic allocation.
No warranty implied; use at your own risk.

Licensed under MIT License or public domain, whichever you prefer.
More explicit license information at the end of file.
*/

#ifndef MUMA_H
	#define MUMA_H

	#if !defined(MU_SECURE_WARNINGS) && !defined(_CRT_SECURE_NO_WARNINGS)
		#define _CRT_SECURE_NO_WARNINGS
	#endif
	
	#ifdef __cplusplus
		extern "C" {
	#endif

	#define MUMA_VERSION_MAJOR 1
	#define MUMA_VERSION_MINOR 0
	#define MUMA_VERSION_PATCH 0

	#ifndef MUDEF
		#ifdef MU_STATIC
			#define MUDEF static
		#else
			#define MUDEF extern
		#endif
	#endif

	#ifndef MU_ZERO_STRUCT
		#ifdef __cplusplus
			#define MU_ZERO_STRUCT(s) {}
		#else
			#define MU_ZERO_STRUCT(s) (s){0}
		#endif
	#endif

	#ifndef MU_ZERO_STRUCT_CONST
		#ifdef __cplusplus
			#define MU_ZERO_STRUCT_CONST(s) {}
		#else
			#define MU_ZERO_STRUCT_CONST(s) {0}
		#endif
	#endif

	#ifndef MU_SET_RESULT
		#define MU_SET_RESULT(res, val) if(res!=MU_NULL_PTR){*res=val;}
	#endif

	/* C standard library dependencies */

		#if !defined(size_m)

			#include <stddef.h>

			#ifndef size_m
				#define size_m size_t
			#endif

		#endif

		#if !defined(mu_malloc)  || \
			!defined(mu_free)    || \
			!defined(mu_realloc)

			#include <stdlib.h>

			#ifndef mu_malloc
				#define mu_malloc malloc
			#endif

			#ifndef mu_free
				#define mu_free free
			#endif

			#ifndef mu_realloc
				#define mu_realloc realloc
			#endif

		#endif

		#if !defined(mu_memset) || \
			!defined(mu_memcpy)

			#include <string.h>

			#ifndef mu_memset
				#define mu_memset memset
			#endif

			#ifndef mu_memcpy
				#define mu_memcpy memcpy
			#endif

		#endif

		#if !defined(MU_SIZE_MAX)

			#include <stdint.h>

			#ifndef MU_SIZE_MAX
				#define MU_SIZE_MAX SIZE_MAX
			#endif

		#endif

	/* Structs */

		struct muDynamicArray {
			void* data;
			size_m type_size;
			size_m length;
			size_m allocated_length;
		};
		typedef struct muDynamicArray muDynamicArray;

	/* Enums */

		enum _mumaResult {
			MUMA_SUCCESS,
			MUMA_FAILED_TO_ALLOCATE,
			MUMA_INVALID_TYPE_SIZE,
			MUMA_INVALID_INDEX,
			MUMA_INVALID_SHIFT_AMOUNT,
			MUMA_INVALID_COUNT,
			MUMA_NOT_FOUND
		};
		typedef enum _mumaResult _mumaResult;
		#define mumaResult size_m

	/* Macros */

		#ifndef MU_NULL_PTR
			#define MU_NULL_PTR 0
		#endif

		#ifndef MU_NONE
			#define MU_NONE MU_SIZE_MAX
		#endif

	/* Functions */

		#ifdef MUMA_NAMES
			MUDEF const char* muma_result_get_name(mumaResult result);
		#endif

		MUDEF muDynamicArray mu_dynamic_array_create(mumaResult* result, size_m type_size, size_m length);
		MUDEF muDynamicArray mu_dynamic_array_destroy(mumaResult* result, muDynamicArray da);

		MUDEF muDynamicArray mu_dynamic_array_multiset(mumaResult* result, muDynamicArray da, void* data, size_m count, size_m index);
		MUDEF muDynamicArray mu_dynamic_array_set(mumaResult* result, muDynamicArray da, void* data, size_m index);

		MUDEF void* mu_dynamic_array_get(mumaResult* result, muDynamicArray da, size_m index);

		MUDEF muDynamicArray mu_dynamic_array_resize(mumaResult* result, muDynamicArray da, size_m length);
		MUDEF muDynamicArray mu_dynamic_array_lshift(mumaResult* result, muDynamicArray da, size_m index, size_m amount);
		MUDEF muDynamicArray mu_dynamic_array_rshift(mumaResult* result, muDynamicArray da, size_m index, size_m amount);

		MUDEF muDynamicArray mu_dynamic_array_multiinsert(mumaResult* result, muDynamicArray da, void* insert, size_m count, size_m index);
		MUDEF muDynamicArray mu_dynamic_array_insert(mumaResult* result, muDynamicArray da, void* insert, size_m index);

		MUDEF muDynamicArray mu_dynamic_array_multierase(mumaResult* result, muDynamicArray da, size_m index, size_m count);
		MUDEF muDynamicArray mu_dynamic_array_erase(mumaResult* result, muDynamicArray da, size_m index);

		MUDEF muDynamicArray mu_dynamic_array_clear(mumaResult* result, muDynamicArray da);

		MUDEF muDynamicArray mu_dynamic_array_multipush(mumaResult* result, muDynamicArray da, void* push, size_m count);
		MUDEF muDynamicArray mu_dynamic_array_push(mumaResult* result, muDynamicArray da, void* push);

		MUDEF muDynamicArray mu_dynamic_array_multipop(mumaResult* result, muDynamicArray da, size_m count);
		MUDEF muDynamicArray mu_dynamic_array_pop(mumaResult* result, muDynamicArray da);

		MUDEF size_m mu_dynamic_array_find(mumaResult* result, muDynamicArray da, void* find);
		MUDEF muDynamicArray mu_dynamic_array_find_push(mumaResult* result, muDynamicArray da, void* find, size_m* p_index);

	/* Macro functions */

		#ifndef muda_i
			#define muda_i(arr, type, index) ((type*)arr.data)[index]
		#endif

	#ifdef __cplusplus
		}
	#endif

#endif /* MUMA_H */

#ifdef MUMA_IMPLEMENTATION

	#ifdef __cplusplus
		extern "C" {
	#endif

	#ifdef MUMA_NAMES
		MUDEF const char* muma_result_get_name(mumaResult result) {
			switch (result) {
				default: return "MUMA_UNKNOWN"; break;
				case MUMA_SUCCESS: return "MUMA_SUCCESS"; break;
				case MUMA_FAILED_TO_ALLOCATE: return "MUMA_FAILED_TO_ALLOCATE"; break;
				case MUMA_INVALID_TYPE_SIZE: return "MUMA_INVALID_TYPE_SIZE"; break;
				case MUMA_INVALID_INDEX: return "MUMA_INVALID_INDEX"; break;
				case MUMA_INVALID_SHIFT_AMOUNT: return "MUMA_INVALID_SHIFT_AMOUNT"; break;
				case MUMA_INVALID_COUNT: return "MUMA_INVALID_COUNT"; break;
			}
		}
	#endif

	MUDEF muDynamicArray mu_dynamic_array_create(mumaResult* result, size_m type_size, size_m length) {
		MU_SET_RESULT(result, MUMA_SUCCESS)

		muDynamicArray da = MU_ZERO_STRUCT(muDynamicArray);

		if (type_size <= 0) {
			MU_SET_RESULT(result, MUMA_INVALID_TYPE_SIZE)
			return da;
		}

		da.type_size = type_size;
		da.length = length;
		da.allocated_length = length;

		if (da.allocated_length == 0) {
			da.data = MU_NULL_PTR;
			return da;
		}

		da.data = mu_malloc(da.type_size * da.allocated_length);
		if (da.data == 0) {
			MU_SET_RESULT(result, MUMA_FAILED_TO_ALLOCATE)
			return da;
		}
		mu_memset(da.data, 0, da.type_size * da.allocated_length);
		return da;
	}

	MUDEF muDynamicArray mu_dynamic_array_destroy(mumaResult* result, muDynamicArray da) {
		MU_SET_RESULT(result, MUMA_SUCCESS)

		if (da.data != MU_NULL_PTR) {
			mu_free(da.data);
			da.data = MU_NULL_PTR;
		}
		da.length = 0;
		da.allocated_length = 0;
		return da;
	}

	MUDEF muDynamicArray mu_dynamic_array_multiset(mumaResult* result, muDynamicArray da, void* data, size_m count, size_m index) {
		MU_SET_RESULT(result, MUMA_SUCCESS)

		if (index >= da.length) {
			MU_SET_RESULT(result, MUMA_INVALID_INDEX)
			return da;
		}

		if (index+count >= da.length+1) {
			MU_SET_RESULT(result, MUMA_INVALID_COUNT)
			return da;
		}

		if (count == 0) {
			return da;
		}

		char* c = (char*)da.data;
		mu_memcpy(&c[da.type_size*index], data, da.type_size*count);

		return da;
	}

	MUDEF muDynamicArray mu_dynamic_array_set(mumaResult* result, muDynamicArray da, void* data, size_m index) {
		return mu_dynamic_array_multiset(result, da, data, 1, index);
	}

	MUDEF void* mu_dynamic_array_get(mumaResult* result, muDynamicArray da, size_m index) {
		MU_SET_RESULT(result, MUMA_SUCCESS)

		if (index >= da.length) {
			MU_SET_RESULT(result, MUMA_INVALID_INDEX)
			return MU_NULL_PTR;
		}

		char* c = (char*)da.data;
		return &c[da.type_size*index];
	}

	MUDEF muDynamicArray mu_dynamic_array_resize(mumaResult* result, muDynamicArray da, size_m length) {
		MU_SET_RESULT(result, MUMA_SUCCESS)

		if (da.type_size <= 0) {
			MU_SET_RESULT(result, MUMA_INVALID_TYPE_SIZE)
			return da;
		}

		if (da.length == length) {
			return da;
		}

		if (da.data == MU_NULL_PTR) {
			da.length = length;
			da.allocated_length = length;
			da.data = mu_malloc(da.allocated_length * da.type_size);

			if (da.data == 0) {
				MU_SET_RESULT(result, MUMA_FAILED_TO_ALLOCATE)
				return da;
			}
			mu_memset(da.data, 0, da.type_size * da.allocated_length);
			return da;
		}

		size_m old_length = da.length;
		size_m old_allocated_length = da.allocated_length;
		da.length = length;

		if (da.length == 0) {
			mu_memset(da.data, 0, da.type_size*da.allocated_length);
			return da;
		}

		if ((da.length > da.allocated_length) || (da.length < da.allocated_length/2)) {
			while (da.length > da.allocated_length) {
				da.allocated_length *= 2;
			}
			while (da.length < da.allocated_length/2) {
				da.allocated_length /= 2;
			}

			void* new_data = mu_realloc(da.data, da.type_size * da.allocated_length);
			if (new_data == 0) {
				da.length = old_length;
				da.allocated_length = old_allocated_length;
				MU_SET_RESULT(result, MUMA_FAILED_TO_ALLOCATE)
				return da;
			}

			da.data = new_data;
		}

		if (old_length < da.length) {
			char* c = (char*)da.data;
			mu_memset(&c[da.type_size*old_length], 0, da.type_size*((da.allocated_length-old_length)));
		}

		return da;
	}

	MUDEF muDynamicArray mu_dynamic_array_lshift(mumaResult* result, muDynamicArray da, size_m index, size_m amount) {
		MU_SET_RESULT(result, MUMA_SUCCESS)

		if (index >= da.length) {
			MU_SET_RESULT(result, MUMA_INVALID_INDEX)
			return da;
		}

		if (amount == 0) {
			return da;
		}

		if (amount > index) {
			MU_SET_RESULT(result, MUMA_INVALID_SHIFT_AMOUNT)
			return da;
		}

		char* c = (char*)da.data;
		mu_memcpy(&c[da.type_size*(index-amount)], &c[da.type_size*index], da.type_size*(da.length-index));

		mumaResult res = MUMA_SUCCESS;
		da = mu_dynamic_array_resize(&res, da, da.length-amount);
		if (res != MUMA_SUCCESS) {
			MU_SET_RESULT(result, res)
			return da;
		}

		return da;
	}

	MUDEF muDynamicArray mu_dynamic_array_rshift(mumaResult* result, muDynamicArray da, size_m index, size_m amount) {
		MU_SET_RESULT(result, MUMA_SUCCESS)

		if (index >= da.length) {
			MU_SET_RESULT(result, MUMA_INVALID_INDEX)
			return da;
		}

		if (amount == 0) {
			return da;
		}

		mumaResult res = MUMA_SUCCESS;
		da = mu_dynamic_array_resize(&res, da, da.length+amount);
		if (res != MUMA_SUCCESS) {
			MU_SET_RESULT(result, res)
			return da;
		}

		char* c = (char*)da.data;
		mu_memcpy(&c[da.type_size*(index+amount)], &c[da.type_size*(index)], da.type_size*(da.length-index));
		mu_memset(&c[da.type_size*(index)], 0, da.type_size*(amount));

		return da;
	}

	MUDEF muDynamicArray mu_dynamic_array_multiinsert(mumaResult* result, muDynamicArray da, void* insert, size_m count, size_m index) {
		MU_SET_RESULT(result, MUMA_SUCCESS)

		mumaResult res = MUMA_SUCCESS;
		da = mu_dynamic_array_rshift(&res, da, index, count);
		if (res != MUMA_SUCCESS) {
			MU_SET_RESULT(result, res)
			return da;
		}

		return mu_dynamic_array_multiset(result, da, insert, count, index);
	}

	MUDEF muDynamicArray mu_dynamic_array_insert(mumaResult* result, muDynamicArray da, void* insert, size_m index) {
		return mu_dynamic_array_multiinsert(result, da, insert, 1, index);
	}

	MUDEF muDynamicArray mu_dynamic_array_multierase(mumaResult* result, muDynamicArray da, size_m index, size_m count) {
		return mu_dynamic_array_lshift(result, da, index+count, count);
	}

	MUDEF muDynamicArray mu_dynamic_array_erase(mumaResult* result, muDynamicArray da, size_m index) {
		return mu_dynamic_array_multierase(result, da, index, 1);
	}

	MUDEF muDynamicArray mu_dynamic_array_clear(mumaResult* result, muDynamicArray da) {
		return mu_dynamic_array_destroy(result, da);
	}

	MUDEF muDynamicArray mu_dynamic_array_multipush(mumaResult* result, muDynamicArray da, void* push, size_m count) {
		MU_SET_RESULT(result, MUMA_SUCCESS)

		mumaResult res = MUMA_SUCCESS;
		da = mu_dynamic_array_resize(&res, da, da.length+count);
		if (res != MUMA_SUCCESS) {
			MU_SET_RESULT(result, res)
			return da;
		}

		// @TODO test if da.length-count is correct
		return mu_dynamic_array_multiset(result, da, push, count, da.length-count);
	}

	MUDEF muDynamicArray mu_dynamic_array_push(mumaResult* result, muDynamicArray da, void* push) {
		return mu_dynamic_array_multipush(result, da, push, 1);
	}

	MUDEF muDynamicArray mu_dynamic_array_multipop(mumaResult* result, muDynamicArray da, size_m count) {
		return mu_dynamic_array_resize(result, da, da.length-count);
	}

	MUDEF muDynamicArray mu_dynamic_array_pop(mumaResult* result, muDynamicArray da) {
		return mu_dynamic_array_multipop(result, da, 1);
	}

	MUDEF size_m mu_dynamic_array_find(mumaResult* result, muDynamicArray da, void* find) {
		MU_SET_RESULT(result, MUMA_SUCCESS)

		char* c = (char*)da.data;
		char* cf = (char*)find;
		for (size_m i = 0; i < da.length; i++) {
			char found = 1;
			for (size_m j = 0; j < da.type_size; j++) {
				if (c[(da.type_size*i)+j] != cf[j]) {
					found = 0;
					break;
				}
			}
			if (found == 1) {
				return i;
			}
		}

		MU_SET_RESULT(result, MUMA_NOT_FOUND)
		return MU_NONE;
	}

	MUDEF muDynamicArray mu_dynamic_array_find_push(mumaResult* result, muDynamicArray da, void* find, size_m* p_index) {
		MU_SET_RESULT(result, MUMA_SUCCESS)

		mumaResult res = MUMA_SUCCESS;
		size_m index = mu_dynamic_array_find(&res, da, find);
		if (index != MU_NONE) {
			if (p_index != MU_NULL_PTR) {
				*p_index = index;
			}
			return da;
		}
		if (res != MUMA_SUCCESS && res != MUMA_NOT_FOUND) {
			if (p_index != MU_NULL_PTR) {
				*p_index = MU_NONE;
			}
			MU_SET_RESULT(result, res)
			return da;
		}

		da = mu_dynamic_array_push(&res, da, find);
		if (res != MUMA_SUCCESS) {
			if (p_index != MU_NULL_PTR) {
				*p_index = MU_NONE;
			}
			MU_SET_RESULT(result, res)
			return da;
		}

		if (p_index != MU_NULL_PTR) {
			*p_index = da.length-1;
		}
		return da;
	}

	#ifdef __cplusplus
		}
	#endif

#endif /* MUMA_IMPLEMENTATION */

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

