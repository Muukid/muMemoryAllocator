/*
muMemoryAllocator.h - Muukid
Public domain simple single-file C library for easy dynamic allocation.
No warranty implied; use at your own risk.

Licensed under MIT License or public domain, whichever you prefer.
More explicit license information at the end of file.

@TODO Prevent "allocation size ping pong", ie allocation size flicking
between, for example, 3 and 4, causing constant allocation.
*/

/* muu header commit 96a78ce */

#ifndef MUU_H
	#define MUU_H
	
	#if !defined(MU_SECURE_WARNINGS) && !defined(_CRT_SECURE_NO_WARNINGS)
		#define _CRT_SECURE_NO_WARNINGS
	#endif

	#ifdef __cplusplus
	extern "C" { // }
	#endif

	#define MUU_VERSION_MAJOR 1
	#define MUU_VERSION_MINOR 0
	#define MUU_VERSION_PATCH 0

	/* C standard library dependencies */

		#if !defined(int8_m)   || \
			!defined(uint8_m)  || \
			!defined(int16_m)  || \
			!defined(uint16_m) || \
			!defined(int32_m)  || \
			!defined(uint32_m) || \
			!defined(int64_m)  || \
			!defined(uint64_m)

			#define __STDC_LIMIT_MACROS
			#define __STDC_CONSTANT_MACROS
			#include <stdint.h>

			#ifndef int8_m
				#ifdef INT8_MAX
					#define int8_m int8_t
				#else
					#define int8_m char
				#endif
			#endif

			#ifndef uint8_m
				#ifdef UINT8_MAX
					#define uint8_m uint8_t
				#else
					#define uint8_m unsigned char
				#endif
			#endif

			#ifndef int16_m
				#ifdef INT16_MAX
					#define int16_m int16_t
				#else
					#define int16_m short
				#endif
			#endif

			#ifndef uint16_m
				#ifdef UINT16_MAX
					#define uint16_m uint16_t
				#else
					#define uint16_m unsigned short
				#endif
			#endif

			#ifndef int32_m
				#ifdef INT32_MAX
					#define int32_m int32_t
				#else
					#define int32_m long
				#endif
			#endif

			#ifndef uint32_m
				#ifdef UINT32_MAX
					#define uint32_m uint32_t
				#else
					#define uint32_m unsigned long
				#endif
			#endif

			#ifndef int64_m
				#ifdef INT64_MAX
					#define int64_m int64_t
				#else
					#define int64_m long long
				#endif
			#endif

			#ifndef uint64_m
				#ifdef UINT64_MAX
					#define uint64_m uint64_t
				#else
					#define uint64_m unsigned long long
				#endif
			#endif

		#endif

		#if !defined(size_m)

			#include <stddef.h>

			#ifndef size_m
				#define size_m size_t
			#endif

		#endif

		#if !defined(MU_SIZE_MAX)

			#include <stdint.h>

			#ifndef MU_SIZE_MAX
				#define MU_SIZE_MAX SIZE_MAX
			#endif

		#endif

		#if !defined(muBool)   || \
			!defined(MU_TRUE)  || \
			!defined(MU_FALSE)

			#include <stdbool.h>

			#ifndef muBool
				#define muBool bool
			#endif

			#ifndef MU_TRUE
				#define MU_TRUE true
			#endif

			#ifndef MU_FALSE
				#define MU_FALSE false
			#endif

		#endif

	/* Useful macros */

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

		#ifndef MU_NULL_PTR
			#define MU_NULL_PTR 0
		#endif

		#ifndef MU_NULL
			#define MU_NULL 0
		#endif

		#ifndef MU_NONE
			#define MU_NONE MU_SIZE_MAX
		#endif

		#ifndef MU_SET_RESULT
			#define MU_SET_RESULT(res, val) if(res!=MU_NULL_PTR){*res=val;}
		#endif

		#ifndef MU_ASSERT
			#define MU_ASSERT(cond, res, val, after) if(!(cond)){MU_SET_RESULT(res, val) after}
		#endif

		#define MU_ENUM(name, ...) enum _##name{__VA_ARGS__};typedef enum _##name _##name; typedef size_m name;

		#if !defined(MU_WIN32) && !defined(MU_UNIX)
			#if defined(WIN32) || defined(_WIN32)
				#define MU_WIN32
			#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
				#define MU_UNIX
			#endif
		#endif

		#define MU_HRARRAY_DEFAULT_FUNC(name) \
			muBool name##_comp(name t0, name t1) { \
				return t0.active == t1.active; \
			} \
			\
			void name##_on_creation(name* p) { \
				if (p != MU_NULL_PTR) { \
					MU_LOCK_CREATE(p->lock, p->lock_active) \
				} \
			} \
			void name##_on_destruction(name* p) { \
				if (p != MU_NULL_PTR) { \
					MU_LOCK_DESTROY(p->lock, p->lock_active) \
				} \
			} \
			void name##_on_hold(name* p) { \
				if (p != MU_NULL_PTR) { \
					MU_LOCK_LOCK(p->lock, p->lock_active) \
				} \
			} \
			void name##_on_release(name* p) { \
				if (p != MU_NULL_PTR) { \
					MU_LOCK_UNLOCK(p->lock, p->lock_active) \
				} \
			} \
			\
			mu_dynamic_hrarray_declaration( \
				name##_array, name, name##_, name##_comp, \
				name##_on_creation, name##_on_destruction, name##_on_hold, name##_on_release \
			)

		#define MU_SAFEFUNC(result, lib_prefix, context, fail_return) \
			MU_SET_RESULT(result, lib_prefix##SUCCESS) \
			MU_ASSERT(context != MU_NULL_PTR, result, lib_prefix##NOT_YET_INITIALIZED, fail_return) \

		#define MU_HOLD(result, item, da, context, lib_prefix, fail_return, da_prefix) \
			MU_ASSERT(item < da.length, result, lib_prefix##INVALID_ID, fail_return) \
			da_prefix##hold_element(0, &da, item); \
			MU_ASSERT(da.data[item].active, result, lib_prefix##INVALID_ID, da_prefix##release_element(0, &da, item); fail_return)

		#define MU_RELEASE(da, item, da_prefix) \
			da_prefix##release_element(0, &da, item);

	#ifdef __cplusplus
	}
	#endif

#endif /* MUU_H */

#ifndef MUMA_H
	#define MUMA_H
	
	#ifdef __cplusplus
		extern "C" {
	#endif

	#define MUMA_VERSION_MAJOR 1
	#define MUMA_VERSION_MINOR 0
	#define MUMA_VERSION_PATCH 0

	/* C standard library dependencies */

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

	/* Enums */

		MU_ENUM(mumaResult, 
			MUMA_SUCCESS,

			MUMA_FAILED_TO_ALLOCATE,
			MUMA_INVALID_TYPE_SIZE,
			MUMA_INVALID_INDEX,
			MUMA_INVALID_SHIFT_AMOUNT,
			MUMA_INVALID_COUNT,
			MUMA_NOT_FOUND
		)

	/* Functions */

		#ifdef MUMA_NAMES
			MUDEF const char* muma_result_get_name(mumaResult result);
		#endif

	/* API macro functionality */

		#define mu_dynamic_array_declaration(struct_name, type, function_name_prefix, type_comparison_func) \
			\
			struct struct_name { \
				type* data; \
				size_m allocated_length; \
				size_m length; \
			}; typedef struct struct_name struct_name; \
			\
			struct_name function_name_prefix##create(mumaResult* result, size_m length) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				struct_name s = MU_ZERO_STRUCT(struct_name); \
				s.data = MU_NULL_PTR; \
				s.allocated_length = length; \
				s.length = length; \
				\
				if (s.length == 0) { \
					return s; \
				} \
				\
				s.data = (type*)mu_malloc(sizeof(type)*s.allocated_length); \
				if (s.data == 0) { \
					MU_SET_RESULT(result, MUMA_FAILED_TO_ALLOCATE) \
					s.allocated_length = 0; \
					s.length = 0; \
					return s; \
				} \
				\
				mu_memset(s.data, 0, sizeof(type)*s.allocated_length);\
				return s;\
			} \
			\
			struct_name function_name_prefix##destroy(mumaResult* result, struct_name s) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				if (s.data != MU_NULL_PTR) { \
					mu_free(s.data); \
					s.data = MU_NULL_PTR; \
				} \
				\
				s.allocated_length = 0; \
				s.length = 0; \
				return s; \
			} \
			\
			struct_name function_name_prefix##resize(mumaResult* result, struct_name s, size_m length) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				if (s.length == length) { \
					return s; \
				} \
				\
				if (s.data == MU_NULL_PTR) { \
					if (length == 0) { \
						return s; \
					} \
					\
					mumaResult res = MUMA_SUCCESS; \
					s = function_name_prefix##create(&res, length); \
					if (res != MUMA_SUCCESS) { \
						MU_SET_RESULT(result, res) \
						return s; \
					} \
				} \
				\
				if (length == 0) { \
					s.length = 0; \
					mu_memset(s.data, 0, sizeof(type)*s.allocated_length); \
					return s; \
				} \
				\
				size_m old_length = s.length; \
				size_m old_allocated_length = s.allocated_length; \
				s.length = length; \
				\
				if ((s.length > s.allocated_length) || (s.length < s.allocated_length/2)) { \
					while (s.length > s.allocated_length) { \
						s.allocated_length *= 2; \
					} \
					while (s.length < s.allocated_length/2) { \
						s.allocated_length /= 2; \
					} \
					\
					type* new_data = (type*)mu_realloc(s.data, sizeof(type) * s.allocated_length); \
					if (new_data == 0) { \
						s.length = old_length; \
						s.allocated_length = old_allocated_length; \
						MU_SET_RESULT(result, MUMA_FAILED_TO_ALLOCATE) \
						return s; \
					} \
					\
					s.data = new_data; \
				} \
				\
				if (old_length < s.length) { \
					mu_memset(&s.data[old_length], 0, sizeof(type)*(s.allocated_length-old_length)); \
				} \
				\
				return s; \
			} \
			\
			struct_name function_name_prefix##lshift(mumaResult* result, struct_name s, size_m index, size_m amount) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				if (index >= s.length) { \
					MU_SET_RESULT(result, MUMA_INVALID_INDEX) \
					return s; \
				} \
				\
				if (amount == 0) { \
					return s; \
				} \
				\
				if (amount > index) { \
					MU_SET_RESULT(result, MUMA_INVALID_SHIFT_AMOUNT) \
					return s; \
				} \
				\
				mu_memcpy(&s.data[index-amount], &s.data[index], sizeof(type)*(s.length-index)); \
				\
				mumaResult res = MUMA_SUCCESS; \
				s = function_name_prefix##resize(&res, s, s.length-amount); \
				if (res != MUMA_SUCCESS) { \
					MU_SET_RESULT(result, res) \
					return s; \
				} \
				\
				return s; \
			} \
			\
			struct_name function_name_prefix##rshift(mumaResult* result, struct_name s, size_m index, size_m amount) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				if (index >= s.length) { \
					MU_SET_RESULT(result, MUMA_INVALID_INDEX) \
					return s; \
				} \
				\
				if (amount == 0) { \
					return s; \
				} \
				\
				mumaResult res = MUMA_SUCCESS; \
				s = function_name_prefix##resize(&res, s, s.length+amount); \
				if (res != MUMA_SUCCESS) { \
					MU_SET_RESULT(result, res) \
					return s; \
				} \
				\
				mu_memcpy(&s.data[index+amount], &s.data[index], sizeof(type)*(s.length-index)); \
				mu_memset(&s.data[index], 0, sizeof(type)*(amount)); \
				\
				return s; \
			} \
			\
			struct_name function_name_prefix##multiinsert(mumaResult* result, struct_name s, size_m index, type* insert, size_m count) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				mumaResult res = MUMA_SUCCESS; \
				s = function_name_prefix##rshift(&res, s, index, count); \
				if (res != MUMA_SUCCESS) { \
					MU_SET_RESULT(result, res) \
					return s; \
				} \
				\
				mu_memcpy(&s.data[index], insert, sizeof(type)*count); \
				return s; \
			} \
			\
			struct_name function_name_prefix##insert(mumaResult* result, struct_name s, size_m index, type insert) { \
				return function_name_prefix##multiinsert(result, s, index, &insert, 1); \
			} \
			\
			struct_name function_name_prefix##multierase(mumaResult* result, struct_name s, size_m index, size_m count) { \
				return function_name_prefix##lshift(result, s, index+count, count); \
			} \
			\
			struct_name function_name_prefix##erase(mumaResult* result, struct_name s, size_m index) { \
				return function_name_prefix##multierase(result, s, index, 1); \
			} \
			struct_name function_name_prefix##clear(mumaResult* result, struct_name s) { \
				return function_name_prefix##destroy(result, s); \
			} \
			\
			struct_name function_name_prefix##multipush(mumaResult* result, struct_name s, type* push, size_m count) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				mumaResult res = MUMA_SUCCESS; \
				s = function_name_prefix##resize(&res, s, s.length+count); \
				if (res != MUMA_SUCCESS) { \
					MU_SET_RESULT(result, res) \
					return s; \
				} \
				\
				mu_memcpy(&s.data[s.length-count], push, sizeof(type)*count); \
				return s; \
			} \
			\
			struct_name function_name_prefix##push(mumaResult* result, struct_name s, type push) { \
				return function_name_prefix##multipush(result, s, &push, 1); \
			} \
			\
			struct_name function_name_prefix##multipop(mumaResult* result, struct_name s, size_m count) { \
				return function_name_prefix##resize(result, s, s.length-count); \
			} \
			\
			struct_name function_name_prefix##pop(mumaResult* result, struct_name s) { \
				return function_name_prefix##multipop(result, s, 1); \
			} \
			\
			size_m function_name_prefix##find(mumaResult* result, struct_name s, type find) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				for (size_m i = 0; i < s.length; i++) { \
					if (type_comparison_func(find, s.data[i])) { \
						return i; \
					} \
				} \
				\
				MU_SET_RESULT(result, MUMA_NOT_FOUND) \
				return MU_NONE; \
			} \
			\
			struct_name function_name_prefix##find_push(mumaResult* result, struct_name s, type find, size_m* p_index) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				mumaResult res = MUMA_SUCCESS; \
				size_m index = function_name_prefix##find(&res, s, find); \
				if (index != MU_NONE) { \
					if (p_index != MU_NULL_PTR) { \
						*p_index = index; \
					} \
					return s; \
				} \
				if (res != MUMA_SUCCESS && res != MUMA_NOT_FOUND) { \
					if (p_index != MU_NULL_PTR) { \
						*p_index = MU_NONE; \
					} \
					MU_SET_RESULT(result, res) \
					return s; \
				} \
				\
				s = function_name_prefix##push(&res, s, find); \
				if (res != MUMA_SUCCESS) { \
					if (p_index != MU_NULL_PTR) { \
						*p_index = MU_NONE; \
					} \
					MU_SET_RESULT(result, res) \
					return s; \
				} \
				\
				if (p_index != MU_NULL_PTR) { \
					*p_index = s.length-1; \
				} \
				return s; \
			}

		#define mu_dynamic_hrarray_declaration( \
			struct_name, type, function_name_prefix, type_comparison_func, \
			on_creation, on_destruction, on_hold, on_release \
		) \
			\
			struct struct_name { \
				type* data; \
				size_m allocated_length; \
				size_m length; \
			}; typedef struct struct_name struct_name; \
			\
			struct_name function_name_prefix##create(mumaResult* result, size_m length) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				struct_name s = MU_ZERO_STRUCT(struct_name); \
				s.data = MU_NULL_PTR; \
				s.allocated_length = length; \
				s.length = length; \
				\
				if (s.length == 0) { \
					return s; \
				} \
				\
				s.data = (type*)mu_malloc(sizeof(type)*s.allocated_length); \
				if (s.data == 0) { \
					MU_SET_RESULT(result, MUMA_FAILED_TO_ALLOCATE) \
					s.allocated_length = 0; \
					s.length = 0; \
					return s; \
				} \
				\
				mu_memset(s.data, 0, sizeof(type)*s.allocated_length); \
				\
				for (size_m i = 0; i < s.length; i++) { \
					on_creation(&s.data[i]); \
				} \
				return s;\
			} \
			\
			void function_name_prefix##hold_element(mumaResult* result, struct_name* s, size_m index) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				if (index >= s->length) { \
					MU_SET_RESULT(result, MUMA_INVALID_INDEX) \
					return; \
				} \
				\
				on_hold(&s->data[index]); \
			} \
			\
			void function_name_prefix##release_element(mumaResult* result, struct_name* s, size_m index) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				if (index >= s->length) { \
					MU_SET_RESULT(result, MUMA_INVALID_INDEX) \
					return; \
				} \
				\
				on_release(&s->data[index]); \
			} \
			\
			void function_name_prefix##destroy(mumaResult* result, struct_name* s) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				for (size_m i = 0; i < s->length; i++) { \
					function_name_prefix##hold_element(0, s, i); \
				} \
				\
				for (size_m i = 0; i < s->length; i++) { \
					on_destruction(&s->data[i]); \
				} \
				\
				if (s->data != MU_NULL_PTR) { \
					mu_free(s->data); \
					s->data = MU_NULL_PTR; \
				} \
				\
				s->allocated_length = 0; \
				s->length = 0; \
			} \
			\
			void function_name_prefix##inner_resize(mumaResult* result, struct_name* s, size_m length, muBool cd) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				if (s->length == length) { \
					return; \
				} \
				\
				if (s->data == MU_NULL_PTR) { \
					if (length == 0) { \
						return; \
					} \
					\
					mumaResult res = MUMA_SUCCESS; \
					*s = function_name_prefix##create(&res, length); \
					if (res != MUMA_SUCCESS) { \
						MU_SET_RESULT(result, res) \
						return; \
					} \
				} \
				\
				if (length == 0) { \
					for (size_m i = 0; i < s->length; i++) { \
						on_destruction(&s->data[i]); \
					} \
					s->length = 0; \
					mu_memset(s->data, 0, sizeof(type)*s->allocated_length); \
					return; \
				} \
				\
				if (cd) { \
					for (size_m i = 0; i < s->length; i++) { \
						function_name_prefix##hold_element(0, s, i); \
					} \
				} \
				\
				size_m old_length = s->length; \
				size_m old_allocated_length = s->allocated_length; \
				s->length = length; \
				\
				/* Note: this is really dangerous, because it's not guaranteed that the  */ \
				/* reallocation will follow through. If it doesn't, we've now called the */ \
				/* destroy function on a bunch of elements that still exist. I can't     */ \
				/* really think of a better way of doing it than this right now, though. */ \
				if (cd && old_length > s->length) { \
					for (size_m i = s->length; i < old_length; i++) { \
						on_destruction(&s->data[i]); \
					} \
				} \
				\
				if ((s->length > s->allocated_length) || (s->length < s->allocated_length/2)) { \
					while (s->length > s->allocated_length) { \
						s->allocated_length *= 2; \
					} \
					while (s->length < s->allocated_length/2) { \
						s->allocated_length /= 2; \
					} \
					\
					type* new_data = (type*)mu_realloc(s->data, sizeof(type) * s->allocated_length); \
					if (new_data == 0) { \
						s->length = old_length; \
						s->allocated_length = old_allocated_length; \
						if (cd) { \
							for (size_m i = 0; i < s->length; i++) { \
								function_name_prefix##release_element(0, s, i); \
							} \
						} \
						MU_SET_RESULT(result, MUMA_FAILED_TO_ALLOCATE) \
						return; \
					} \
					\
					s->data = new_data; \
				} \
				\
				if (old_length < s->length) { \
					mu_memset(&s->data[old_length], 0, sizeof(type)*(s->allocated_length-old_length)); \
					\
					if (cd) { \
						for (size_m i = old_length; i < s->length; i++) { \
							on_creation(&s->data[i]); \
						} \
					} \
				} \
				\
				if (cd) { \
					for (size_m i = 0; i < s->length && i < old_length; i++) { \
						function_name_prefix##release_element(0, s, i); \
					} \
				} \
			} \
			\
			void function_name_prefix##resize(mumaResult* result, struct_name* s, size_m length) { \
				function_name_prefix##inner_resize(result, s, length, MU_TRUE); \
			} \
			\
			void function_name_prefix##inner_lshift(mumaResult* result, struct_name* s, size_m index, size_m amount, muBool cd) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				if (index >= s->length) { \
					MU_SET_RESULT(result, MUMA_INVALID_INDEX) \
					return; \
				} \
				\
				if (amount == 0) { \
					return; \
				} \
				\
				if (amount > index) { \
					MU_SET_RESULT(result, MUMA_INVALID_SHIFT_AMOUNT) \
					return; \
				} \
				\
				if (cd) { \
					for (size_m i = 0; i < s->length; i++) { \
						function_name_prefix##hold_element(0, s, i); \
					} \
				} \
				\
				/* Dangerous; resize may fail after this */ \
				if (cd) { \
					for (size_m i = index-amount; i < index; i++) { \
						on_destruction(&s->data[i]); \
					} \
				} \
				\
				mu_memcpy(&s->data[index-amount], &s->data[index], sizeof(type)*(s->length-index)); \
				\
				mumaResult res = MUMA_SUCCESS; \
				function_name_prefix##inner_resize(&res, s, s->length-amount, MU_FALSE); \
				\
				if (cd) { \
					for (size_m i = 0; i < s->length; i++) { \
						function_name_prefix##release_element(0, s, i); \
					} \
				} \
				\
				if (res != MUMA_SUCCESS) { \
					MU_SET_RESULT(result, res) \
					return; \
				} \
			} \
			\
			void function_name_prefix##lshift(mumaResult* result, struct_name* s, size_m index, size_m amount) { \
				function_name_prefix##inner_lshift(result, s, index, amount, MU_TRUE); \
			} \
			\
			void function_name_prefix##inner_rshift(mumaResult* result, struct_name* s, size_m index, size_m amount, muBool cd) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				if (index >= s->length) { \
					MU_SET_RESULT(result, MUMA_INVALID_INDEX) \
					return; \
				} \
				\
				if (amount == 0) { \
					return; \
				} \
				\
				if (cd) { \
					for (size_m i = 0; i < s->length; i++) { \
						function_name_prefix##hold_element(0, s, i); \
					} \
				} \
				\
				mumaResult res = MUMA_SUCCESS; \
				function_name_prefix##inner_resize(&res, s, s->length+amount, MU_FALSE); \
				if (res != MUMA_SUCCESS) { \
					if (cd) { \
						for (size_m i = 0; i < s->length; i++) { \
							function_name_prefix##release_element(0, s, i); \
						} \
					} \
					MU_SET_RESULT(result, res) \
					return; \
				} \
				\
				mu_memcpy(&s->data[index+amount], &s->data[index], sizeof(type)*(s->length-index)); \
				\
				if (cd) { \
					for (size_m i = 0; i < index; i++) { \
						function_name_prefix##release_element(0, s, i); \
					} \
					for (size_m i = index+amount; i < s->length; i++) { \
						function_name_prefix##release_element(0, s, i); \
					} \
				} \
				\
				mu_memset(&s->data[index], 0, sizeof(type)*(amount)); \
				\
				if (cd) { \
					for (size_m i = index; i < index+amount; i++) { \
						on_creation(&s->data[i]); \
					} \
				} \
			} \
			\
			void function_name_prefix##rshift(mumaResult* result, struct_name* s, size_m index, size_m amount) { \
				function_name_prefix##inner_rshift(result, s, index, amount, MU_TRUE); \
			} \
			\
			void function_name_prefix##multiinsert(mumaResult* result, struct_name* s, size_m index, type* insert, size_m count) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				for (size_m i = 0; i < s->length; i++) { \
					function_name_prefix##hold_element(0, s, i); \
				} \
				\
				mumaResult res = MUMA_SUCCESS; \
				function_name_prefix##inner_rshift(&res, s, index, count, MU_FALSE); \
				if (res != MUMA_SUCCESS) { \
					for (size_m i = 0; i < s->length; i++) { \
						function_name_prefix##release_element(0, s, i); \
					} \
					MU_SET_RESULT(result, res) \
					return; \
				} \
				\
				mu_memcpy(&s->data[index], insert, sizeof(type)*count); \
				\
				for (size_m i = 0; i < index; i++) { \
					function_name_prefix##release_element(0, s, i); \
				} \
				for (size_m i = index+count; i < s->length; i++) { \
					function_name_prefix##release_element(0, s, i); \
				} \
				\
				for (size_m i = index; i < index+count; i++) { \
					on_creation(&s->data[i]); \
				} \
			} \
			\
			void function_name_prefix##insert(mumaResult* result, struct_name* s, size_m index, type insert) { \
				function_name_prefix##multiinsert(result, s, index, &insert, 1); \
			} \
			\
			void function_name_prefix##multierase(mumaResult* result, struct_name* s, size_m index, size_m count) { \
				function_name_prefix##lshift(result, s, index+count, count); \
			} \
			\
			void function_name_prefix##erase(mumaResult* result, struct_name* s, size_m index) { \
				function_name_prefix##multierase(result, s, index, 1); \
			} \
			void function_name_prefix##clear(mumaResult* result, struct_name* s) { \
				function_name_prefix##destroy(result, s); \
			} \
			\
			void function_name_prefix##multipush(mumaResult* result, struct_name* s, type* push, size_m count) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				size_m old_length = s->length; \
				for (size_m i = 0; i < s->length; i++) { \
					function_name_prefix##hold_element(0, s, i); \
				} \
				\
				mumaResult res = MUMA_SUCCESS; \
				function_name_prefix##inner_resize(&res, s, s->length+count, MU_FALSE); \
				if (res != MUMA_SUCCESS) { \
					for (size_m i = 0; i < s->length; i++) { \
						function_name_prefix##release_element(0, s, i); \
					} \
					MU_SET_RESULT(result, res) \
					return; \
				} \
				\
				mu_memcpy(&s->data[s->length-count], push, sizeof(type)*count); \
				\
				for (size_m i = 0; i < old_length; i++) { \
					function_name_prefix##release_element(0, s, i); \
				} \
				\
				for (size_m i = s->length-count; i < s->length; i++) { \
					on_creation(&s->data[i]); \
				} \
			} \
			\
			void function_name_prefix##push(mumaResult* result, struct_name* s, type push) { \
				function_name_prefix##multipush(result, s, &push, 1); \
			} \
			\
			void function_name_prefix##multipop(mumaResult* result, struct_name* s, size_m count) { \
				function_name_prefix##resize(result, s, s->length-count); \
			} \
			\
			void function_name_prefix##pop(mumaResult* result, struct_name* s) { \
				function_name_prefix##multipop(result, s, 1); \
			} \
			\
			size_m function_name_prefix##find(mumaResult* result, struct_name* s, type find) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				for (size_m i = 0; i < s->length; i++) { \
					function_name_prefix##hold_element(0, s, i); \
					if (type_comparison_func(find, s->data[i])) { \
						function_name_prefix##release_element(0, s, i); \
						return i; \
					} \
					function_name_prefix##release_element(0, s, i); \
				} \
				\
				MU_SET_RESULT(result, MUMA_NOT_FOUND) \
				return MU_NONE; \
			} \
			\
			void function_name_prefix##find_push(mumaResult* result, struct_name* s, type find, size_m* p_index) { \
				MU_SET_RESULT(result, MUMA_SUCCESS) \
				\
				mumaResult res = MUMA_SUCCESS; \
				size_m index = function_name_prefix##find(&res, s, find); \
				if (index != MU_NONE) { \
					if (p_index != MU_NULL_PTR) { \
						*p_index = index; \
					} \
					return; \
				} \
				if (res != MUMA_SUCCESS && res != MUMA_NOT_FOUND) { \
					if (p_index != MU_NULL_PTR) { \
						*p_index = MU_NONE; \
					} \
					MU_SET_RESULT(result, res) \
					return; \
				} \
				\
				function_name_prefix##push(&res, s, find); \
				if (res != MUMA_SUCCESS) { \
					if (p_index != MU_NULL_PTR) { \
						*p_index = MU_NONE; \
					} \
					MU_SET_RESULT(result, res) \
					return; \
				} \
				\
				if (p_index != MU_NULL_PTR) { \
					*p_index = s->length-1; \
				} \
			}

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

