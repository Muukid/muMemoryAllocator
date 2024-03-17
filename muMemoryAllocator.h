/*
muMemoryAllocator.h - Muukid
Public domain simple single-file C library for easy dynamic allocation.
No warranty implied; use at your own risk.

Licensed under MIT License or public domain, whichever you prefer.
More explicit license information at the end of file.

@TODO Prevent "allocation size ping pong", ie allocation size flicking
between, for example, 3 and 4, causing constant allocation. This is a
pertinent issue with threadsafe dynamic arrays.

@TODO Add allocation length setting, which allows us to optimize lock
creation/destruction calls within threadsafe dynamic arrays.

@MENTION Functions that modify multiple elements that take in "void* 
data" expect data to be as long as the array's type size multiplied by
the amount of elements being modified.

@MENTION Attempting to modify an out-of-index element in a threadsafe
dynamic array is undefined behavior, mostly due to the nature of 
thread locks.
*/

/* mum commit 6dd66f3 header */

#ifdef MU_THREADSAFE

#ifndef MUM_H
	#define MUM_H
	
	#if !defined(MU_SECURE_WARNINGS) && !defined(_CRT_SECURE_NO_WARNINGS)
		#define _CRT_SECURE_NO_WARNINGS
	#endif
	
	#ifdef __cplusplus
		extern "C" {
	#endif

	#define MUM_VERSION_MAJOR 1
	#define MUM_VERSION_MINOR 0
	#define MUM_VERSION_PATCH 0

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

		#if !defined(mu_memcpy)

			#include <string.h>

			#ifndef mu_memcpy
				#define mu_memcpy memcpy
			#endif

		#endif

	/* Enums */

		enum _mumResult {
			MUM_SUCCESS,
			MUM_FAILURE,
			MUM_CREATE_CALL_FAILED,
			MUM_DESTROY_CALL_FAILED,
			MUM_LOCK_CALL_FAILED,
			MUM_UNLOCK_CALL_FAILED,
			MUM_WAIT_CALL_FAILED,
			MUM_INVALID_THREAD,
			MUM_INVALID_MUTEX,
			MUM_INVALID_SPINLOCK,
			MUM_PREVIOUS_THREAD_CLOSED_BEFORE_LOCK,
			MUM_THREAD_TIMED_OUT
		};
		typedef enum _mumResult _mumResult;
		#define mumResult size_m

	/* Macros */

		#ifndef MU_NULL_PTR
			#define MU_NULL_PTR 0
		#endif

		#ifdef MU_MUTEX
			#define muLock muMutex
			#define mu_lock_create mu_mutex_create
			#define mu_lock_destroy mu_mutex_destroy
			#define mu_lock_lock mu_mutex_lock
			#define mu_lock_unlock mu_mutex_unlock
		#else
			#define muLock muSpinlock
			#define mu_lock_create mu_spinlock_create
			#define mu_lock_destroy mu_spinlock_destroy
			#define mu_lock_lock mu_spinlock_lock
			#define mu_lock_unlock mu_spinlock_unlock
		#endif

	/* Operating system */

		#if !defined(MU_WIN32) && !defined(MU_UNIX)
			#if defined(WIN32) || defined(_WIN32)
				#define MU_WIN32
			#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
				#define MU_UNIX
			#endif
		#endif

	/* Structs */

		#if defined(MU_WIN32)

			#include <windows.h>

			/* Thread */

				struct muThread {
					muBool active;
					HANDLE thread;
					DWORD id;
				};
				typedef struct muThread muThread;

				muThread mu_inner_thread_destroy(mumResult* result, muThread thread) {
					if (CloseHandle(thread.thread) != 0) {
						MU_SET_RESULT(result, MUM_DESTROY_CALL_FAILED)
						return thread;
					}
					return MU_ZERO_STRUCT(muThread);
				}

			/* Mutex */

				struct muMutex {
					muBool active;
					HANDLE mutex;
				};
				typedef struct muMutex muMutex;

				muMutex mu_inner_mutex_destroy(mumResult* result, muMutex mutex) {
					if (CloseHandle(mutex.mutex) != 0) {
						MU_SET_RESULT(result, MUM_DESTROY_CALL_FAILED)
						return mutex;
					}
					return MU_ZERO_STRUCT(muMutex);
				}

			/* Spinlock */

				struct muSpinlock {
					muBool active;
					LONG volatile locked;
				};
				typedef struct muSpinlock muSpinlock;

				muSpinlock mu_inner_spinlock_destroy(mumResult* result, muSpinlock spinlock) {
					MU_SET_RESULT(result, MUM_SUCCESS)
					if (spinlock.active) {

					}
					return MU_ZERO_STRUCT(muSpinlock);
				}

		#elif defined(MU_UNIX)

			#include <pthread.h>

			/* Thread */

				struct muThread {
					muBool active;
					pthread_t thread;
					void* ret;
				};
				typedef struct muThread muThread;

				muThread mu_inner_thread_destroy(mumResult* result, muThread thread) {
					if (pthread_cancel(thread.thread) != 0) {
						MU_SET_RESULT(result, MUM_DESTROY_CALL_FAILED)
						return thread;
					}
					return MU_ZERO_STRUCT(muThread);
				}

			/* Mutex */

				struct muMutex {
					muBool active;
					pthread_mutex_t mutex;
				};
				typedef struct muMutex muMutex;

				muMutex mu_inner_mutex_destroy(mumResult* result, muMutex mutex) {
					if (pthread_mutex_destroy(&mutex.mutex) != 0) {
						MU_SET_RESULT(result, MUM_DESTROY_CALL_FAILED)
						return mutex;
					}
					return MU_ZERO_STRUCT(muMutex);
				}

			/* Spinlock */

				struct muSpinlock {
					muBool active;
					int locked;
				};
				typedef struct muSpinlock muSpinlock;

				muSpinlock mu_inner_spinlock_destroy(mumResult* result, muSpinlock spinlock) {
					MU_SET_RESULT(result, MUM_SUCCESS)
					if (spinlock.active) {

					}
					return MU_ZERO_STRUCT(muSpinlock);
				}

		#endif

	/* Functions */

		#ifdef MUM_NAMES
			MUDEF const char* mum_result_get_name(mumResult result);
		#endif

		/* Thread */

			MUDEF muThread mu_thread_create(mumResult* result, void (*start)(void* args), void* args);
			MUDEF muThread mu_thread_destroy(mumResult* result, muThread thread);

			MUDEF void mu_thread_exit(void* ret);
			MUDEF void mu_thread_wait(mumResult* result, muThread* p_thread);
			MUDEF void* mu_thread_get_return_value(mumResult* result, muThread thread);

		/* Mutex */

			MUDEF muMutex mu_mutex_create(mumResult* result);
			MUDEF muMutex mu_mutex_destroy(mumResult* result, muMutex mutex);

			MUDEF void mu_mutex_lock(mumResult* result, muMutex* p_mutex);
			MUDEF void mu_mutex_unlock(mumResult* result, muMutex* p_mutex);

		/* Spinlock */

			MUDEF muSpinlock mu_spinlock_create(mumResult* result);
			MUDEF muSpinlock mu_spinlock_destroy(mumResult* result, muSpinlock spinlock);

			MUDEF void mu_spinlock_lock(mumResult* result, muSpinlock* p_spinlock);
			MUDEF void mu_spinlock_unlock(mumResult* result, muSpinlock* p_spinlock);

	#ifdef __cplusplus
		}
	#endif

#endif /* MUM_H */

#endif

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

		#ifdef MU_THREADSAFE
		struct muTsDynamicArray {
			muDynamicArray da;
			muDynamicArray lock_da;
		};
		typedef struct muTsDynamicArray muTsDynamicArray;
		#endif

	/* Enums */

		enum _mumaResult {
			MUMA_SUCCESS,

			#ifdef MU_THREADSAFE
			MUMA_MUM_FAILURE,
			MUMA_MUM_CREATE_CALL_FAILED,
			MUMA_MUM_DESTROY_CALL_FAILED,
			MUMA_MUM_LOCK_CALL_FAILED,
			MUMA_MUM_UNLOCK_CALL_FAILED,
			MUMA_MUM_WAIT_CALL_FAILED,
			MUMA_MUM_INVALID_THREAD,
			MUMA_MUM_INVALID_MUTEX,
			MUMA_MUM_INVALID_SPINLOCK,
			MUMA_MUM_PREVIOUS_THREAD_CLOSED_BEFORE_LOCK,
			MUMA_MUM_THREAD_TIMED_OUT,
			#endif

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

	#ifdef __cplusplus
		}
	#endif

#endif /* MUMA_H */

#ifdef MUMA_IMPLEMENTATION

	/* mum commit 6dd66f3 implementation */

	#ifdef MU_THREADSAFE

	#ifndef MUM_IMPLEMENTATION
		#define MUM_IMPLEMENTATION
	#endif

	#ifdef MUM_IMPLEMENTATION

		#ifdef __cplusplus
			extern "C" {
		#endif

		/* Shared API functions */

			#ifdef MUM_NAMES
				MUDEF const char* mum_result_get_name(mumResult result) {
					switch (result) {
						default: return "MUM_UNKNOWN"; break;
						case MUM_SUCCESS: return "MUM_SUCCESS"; break;
						case MUM_CREATE_CALL_FAILED: return "MUM_CREATE_CALL_FAILED"; break;
						case MUM_DESTROY_CALL_FAILED: return "MUM_DESTROY_CALL_FAILED"; break;
						case MUM_LOCK_CALL_FAILED: return "MUM_LOCK_CALL_FAILED"; break;
						case MUM_UNLOCK_CALL_FAILED: return "MUM_UNLOCK_CALL_FAILED"; break;
						case MUM_WAIT_CALL_FAILED: return "MUM_WAIT_CALL_FAILED"; break;
						case MUM_INVALID_THREAD: return "MUM_INVALID_THREAD"; break;
						case MUM_INVALID_MUTEX: return "MUM_INVALID_MUTEX"; break;
						case MUM_INVALID_SPINLOCK: return "MUM_INVALID_SPINLOCK"; break;
						case MUM_PREVIOUS_THREAD_CLOSED_BEFORE_LOCK: return "MUM_PREVIOUS_THREAD_CLOSED_BEFORE_LOCK"; break;
						case MUM_FAILURE: return "MUM_FAILURE"; break;
						case MUM_THREAD_TIMED_OUT: return "MUM_THREAD_TIMED_OUT"; break;
					}
				}
			#endif

		/* Windows implementation */

		#ifdef MU_WIN32

			/* API functions */

				/* Thread */

					#define MU_WIN32_CHECK_THREAD_ACTIVE(ret) if(thread.active!=MU_TRUE){if(result!=MU_NULL_PTR){*result=MUM_INVALID_THREAD;}return ret;}
					#define MU_WIN32_CHECK_PTHREAD_ACTIVE(ret) if(p_thread->active!=MU_TRUE){if(result!=MU_NULL_PTR){*result=MUM_INVALID_THREAD;}return ret;}

					MUDEF muThread mu_thread_create(mumResult* result, void (*start)(void* args), void* args) {
						MU_SET_RESULT(result, MUM_SUCCESS)

						muThread thread = MU_ZERO_STRUCT(muThread);
						thread.active = MU_TRUE;

						LPTHREAD_START_ROUTINE lp_start;
						mu_memcpy(&lp_start, &start, sizeof(void*));
						thread.thread = CreateThread(0, 0, lp_start, args, 0, &thread.id);

						if (thread.thread == 0) {
							MU_SET_RESULT(result, MUM_CREATE_CALL_FAILED)
							return MU_ZERO_STRUCT(muThread);
						}

						return thread;
					}

					MUDEF muThread mu_thread_destroy(mumResult* result, muThread thread) {
						MU_WIN32_CHECK_THREAD_ACTIVE(thread)

						mumResult res = MUM_SUCCESS;
						thread = mu_inner_thread_destroy(&res, thread);
						MU_SET_RESULT(result, res)
						return thread;
					}

					MUDEF void mu_thread_exit(void* ret) {
						DWORD d;
						mu_memcpy(&d, &ret, sizeof(DWORD));
						ExitThread(d);
					}

					MUDEF void mu_thread_wait(mumResult* result, muThread* p_thread) {
						MU_SET_RESULT(result, MUM_SUCCESS)

						MU_WIN32_CHECK_PTHREAD_ACTIVE()

						DWORD wait_result = WaitForSingleObject(p_thread->thread, INFINITE);

						switch (wait_result) {
							case WAIT_TIMEOUT: {
								MU_SET_RESULT(result, MUM_THREAD_TIMED_OUT)
							} break;

							case WAIT_FAILED: {
								MU_SET_RESULT(result, MUM_FAILURE)
							} break;
						}
					}

					MUDEF void* mu_thread_get_return_value(mumResult* result, muThread thread) {
						MU_SET_RESULT(result, MUM_SUCCESS)

						MU_WIN32_CHECK_THREAD_ACTIVE(MU_NULL_PTR)

						DWORD exit_code = 0;
						GetExitCodeThread(thread.thread, &exit_code);

						void* p;
						mu_memcpy(&p, &exit_code, sizeof(DWORD));
						return p;
					}

				/* Mutex */

					#define MU_WIN32_CHECK_MUTEX_ACTIVE(ret) if(mutex.active!=MU_TRUE){if(result!=MU_NULL_PTR){*result=MUM_INVALID_MUTEX;}return ret;}
					#define MU_WIN32_CHECK_PMUTEX_ACTIVE(ret) if(p_mutex->active!=MU_TRUE){if(result!=MU_NULL_PTR){*result=MUM_INVALID_MUTEX;}return ret;}

					MUDEF muMutex mu_mutex_create(mumResult* result) {
						MU_SET_RESULT(result, MUM_SUCCESS)

						muMutex mutex = MU_ZERO_STRUCT(muMutex);
						mutex.active = MU_TRUE;

						mutex.mutex = CreateMutex(0, MU_FALSE, 0);
						if (mutex.mutex == 0) {
							MU_SET_RESULT(result, MUM_CREATE_CALL_FAILED)
							return MU_ZERO_STRUCT(muMutex);
						}

						return mutex;
					}

					MUDEF muMutex mu_mutex_destroy(mumResult* result, muMutex mutex) {
						MU_WIN32_CHECK_MUTEX_ACTIVE(mutex)

						mumResult res = MUM_SUCCESS;
						mutex = mu_inner_mutex_destroy(&res, mutex);
						MU_SET_RESULT(result, res)
						return mutex;
					}

					MUDEF void mu_mutex_lock(mumResult* result, muMutex* p_mutex) {
						MU_SET_RESULT(result, MUM_SUCCESS)
						MU_WIN32_CHECK_PMUTEX_ACTIVE()

						DWORD wait_result = WaitForSingleObject(p_mutex->mutex, INFINITE);

						switch (wait_result) {
							// The mutex has most likely been closed. This should pretty much never happen with
							// the way mum is set up, but if it has, that's really bad. Most likely, rather mum
							// is not working as intended at all, or the user has modified things that they
							// shouldn't.
							case WAIT_FAILED: {
								MU_SET_RESULT(result, MUM_INVALID_MUTEX)
							} break;

							// The thread holding the mutex has died. This can be due to a few things:
							// * The thread crashed or otherwise imploded in on itself.
							// * I forgot to put an unlock call on an error return case.
							// * The user has fiddled around with values they shouldn't.
							// Either way, this is REALLY, REALLY bad, and will lead to sporadic random bugs &
							// crashes.
							// Note: we still have ownership due to this, but ehhhhh.
							// https://devblogs.microsoft.com/oldnewthing/20050912-14/?p=34253
							// (Raymond Chen is awesome btw)
							case WAIT_ABANDONED: {
								MU_SET_RESULT(result, MUM_PREVIOUS_THREAD_CLOSED_BEFORE_LOCK)
							} break;
						}
					}

					MUDEF void mu_mutex_unlock(mumResult* result, muMutex* p_mutex) {
						MU_SET_RESULT(result, MUM_SUCCESS)
						MU_WIN32_CHECK_PMUTEX_ACTIVE()

						if (!ReleaseMutex(p_mutex->mutex)) {
							MU_SET_RESULT(result, MUM_UNLOCK_CALL_FAILED)
							return;
						}
					}

				/* Spinlock */

					static inline muBool mum_atomic_compare_exchange(LONG volatile* ptr, LONG compare, LONG exchange) {
						return InterlockedCompareExchange(ptr, exchange, compare) != exchange;
					}

					// Kind of a hack
					static inline void mum_atomic_store(LONG volatile* ptr, long value) {
						if (value == 0) {
							_interlockedbittestandreset(ptr, 0);
						} else {
							_interlockedbittestandset(ptr, 0);
						}
					}

					#define MU_WIN32_CHECK_SPINLOCK_ACTIVE(ret) if(spinlock.active!=MU_TRUE){if(result!=MU_NULL_PTR){*result=MUM_INVALID_SPINLOCK;}return ret;}
					#define MU_WIN32_CHECK_PSPINLOCK_ACTIVE(ret) if(p_spinlock->active!=MU_TRUE){if(result!=MU_NULL_PTR){*result=MUM_INVALID_SPINLOCK;}return ret;}

					MUDEF muSpinlock mu_spinlock_create(mumResult* result) {
						MU_SET_RESULT(result, MUM_SUCCESS)

						muSpinlock spinlock = MU_ZERO_STRUCT(muSpinlock);
						spinlock.active = MU_TRUE;

						return spinlock;
					}

					MUDEF muSpinlock mu_spinlock_destroy(mumResult* result, muSpinlock spinlock) {
						MU_WIN32_CHECK_SPINLOCK_ACTIVE(spinlock)

						mumResult res = MUM_SUCCESS;
						spinlock = mu_inner_spinlock_destroy(&res, spinlock);
						MU_SET_RESULT(result, res)
						return spinlock;
					}

					MUDEF void mu_spinlock_lock(mumResult* result, muSpinlock* p_spinlock) {
						MU_SET_RESULT(result, MUM_SUCCESS)

						while (!mum_atomic_compare_exchange(&p_spinlock->locked, 0, 1)) {

						}
					}

					MUDEF void mu_spinlock_unlock(mumResult* result, muSpinlock* p_spinlock) {
						MU_SET_RESULT(result, MUM_SUCCESS)

						mum_atomic_store(&p_spinlock->locked, 0);
					}

		#endif

		/* Unix implementation */

		#ifdef MU_UNIX

			/* API functions */

				/* Thread */

					#define MU_UNIX_CHECK_THREAD_ACTIVE(ret) if(thread.active!=MU_TRUE){if(result!=MU_NULL_PTR){*result=MUM_INVALID_THREAD;}return ret;}
					#define MU_UNIX_CHECK_PTHREAD_ACTIVE(ret) if(p_thread->active!=MU_TRUE){if(result!=MU_NULL_PTR){*result=MUM_INVALID_THREAD;}return ret;}

					MUDEF muThread mu_thread_create(mumResult* result, void (*start)(void* args), void* args) {
						MU_SET_RESULT(result, MUM_SUCCESS)

						muThread thread = MU_ZERO_STRUCT(muThread);
						thread.active = MU_TRUE;

						void* (*func)(void*);
						mu_memcpy(&func, &start, sizeof(void*));
						if (pthread_create(&thread.thread, 0, func, args) != 0) {
							MU_SET_RESULT(result, MUM_CREATE_CALL_FAILED)
							return MU_ZERO_STRUCT(muThread);
						}

						thread.ret = MU_NULL_PTR;
						return thread;
					}

					MUDEF muThread mu_thread_destroy(mumResult* result, muThread thread) {
						MU_UNIX_CHECK_THREAD_ACTIVE(thread)

						mumResult res = MUM_SUCCESS;
						thread = mu_inner_thread_destroy(&res, thread);
						MU_SET_RESULT(result, res)
						return thread;
					}

					MUDEF void mu_thread_exit(void* ret) {
						pthread_exit(ret);
					}

					MUDEF void mu_thread_wait(mumResult* result, muThread* p_thread) {
						MU_SET_RESULT(result, MUM_SUCCESS)

						MU_UNIX_CHECK_PTHREAD_ACTIVE()

						if (pthread_join(p_thread->thread, &p_thread->ret) != 0) {
							MU_SET_RESULT(result, MUM_WAIT_CALL_FAILED)
							return;
						}
					}

					MUDEF void* mu_thread_get_return_value(mumResult* result, muThread thread) {
						MU_SET_RESULT(result, MUM_SUCCESS)

						MU_UNIX_CHECK_THREAD_ACTIVE(MU_NULL_PTR)

						return thread.ret;
					}

				/* Mutex */

					#define MU_UNIX_CHECK_MUTEX_ACTIVE(ret) if(mutex.active!=MU_TRUE){if(result!=MU_NULL_PTR){*result=MUM_INVALID_MUTEX;}return ret;}
					#define MU_UNIX_CHECK_PMUTEX_ACTIVE(ret) if(p_mutex->active!=MU_TRUE){if(result!=MU_NULL_PTR){*result=MUM_INVALID_MUTEX;}return ret;}

					MUDEF muMutex mu_mutex_create(mumResult* result) {
						MU_SET_RESULT(result, MUM_SUCCESS)

						muMutex mutex = MU_ZERO_STRUCT(muMutex);
						mutex.active = MU_TRUE;

						if (pthread_mutex_init(&mutex.mutex, 0) != 0) {
							MU_SET_RESULT(result, MUM_CREATE_CALL_FAILED)
							return MU_ZERO_STRUCT(muMutex);
						}

						return mutex;
					}

					MUDEF muMutex mu_mutex_destroy(mumResult* result, muMutex mutex) {
						MU_UNIX_CHECK_MUTEX_ACTIVE(mutex)

						mumResult res = MUM_SUCCESS;
						mutex = mu_inner_mutex_destroy(&res, mutex);
						MU_SET_RESULT(result, res)
						return mutex;
					}

					MUDEF void mu_mutex_lock(mumResult* result, muMutex* p_mutex) {
						MU_SET_RESULT(result, MUM_SUCCESS)
						MU_UNIX_CHECK_PMUTEX_ACTIVE()

						if (pthread_mutex_lock(&p_mutex->mutex) != 0) {
							MU_SET_RESULT(result, MUM_LOCK_CALL_FAILED)
							return;
						}
					}

					MUDEF void mu_mutex_unlock(mumResult* result, muMutex* p_mutex) {
						MU_SET_RESULT(result, MUM_SUCCESS)
						MU_UNIX_CHECK_PMUTEX_ACTIVE()

						if (pthread_mutex_unlock(&p_mutex->mutex) != 0) {
							MU_SET_RESULT(result, MUM_UNLOCK_CALL_FAILED)
							return;
						}
					}

				/* Spinlock */

					// A lot of this code is stolen from:
					// https://github.com/stepancheg/no-mutex-c

					static inline muBool mum_atomic_compare_exchange(int* ptr, int compare, int exchange) {
						return __atomic_compare_exchange_n(ptr, &compare, exchange, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
					}

					static inline void mum_atomic_store(int* ptr, int value) {
						if (value) {}
						__atomic_store_n(ptr, 0, __ATOMIC_SEQ_CST);
					}

					#define MU_UNIX_CHECK_SPINLOCK_ACTIVE(ret) if(spinlock.active!=MU_TRUE){if(result!=MU_NULL_PTR){*result=MUM_INVALID_SPINLOCK;}return ret;}
					#define MU_UNIX_CHECK_PSPINLOCK_ACTIVE(ret) if(p_spinlock->active!=MU_TRUE){if(result!=MU_NULL_PTR){*result=MUM_INVALID_SPINLOCK;}return ret;}

					MUDEF muSpinlock mu_spinlock_create(mumResult* result) {
						MU_SET_RESULT(result, MUM_SUCCESS)

						muSpinlock spinlock = MU_ZERO_STRUCT(muSpinlock);
						spinlock.active = MU_TRUE;

						return spinlock;
					}

					MUDEF muSpinlock mu_spinlock_destroy(mumResult* result, muSpinlock spinlock) {
						MU_UNIX_CHECK_SPINLOCK_ACTIVE(spinlock)

						mumResult res = MUM_SUCCESS;
						spinlock = mu_inner_spinlock_destroy(&res, spinlock);
						MU_SET_RESULT(result, res)
						return spinlock;
					}

					MUDEF void mu_spinlock_lock(mumResult* result, muSpinlock* p_spinlock) {
						MU_SET_RESULT(result, MUM_SUCCESS)

						while (!mum_atomic_compare_exchange(&p_spinlock->locked, 0, 1)) {

						}
					}

					MUDEF void mu_spinlock_unlock(mumResult* result, muSpinlock* p_spinlock) {
						MU_SET_RESULT(result, MUM_SUCCESS)

						mum_atomic_store(&p_spinlock->locked, 0);
					}

		#endif /* MU_UNIX */

		#ifdef __cplusplus
			}
		#endif

	#endif /* MUM_IMPLEMENTATION */

	#endif

	#ifdef __cplusplus
		extern "C" {
	#endif

	#ifdef MU_THREADSAFE
		mumaResult mum_result_to_muma_result(mumResult result) {
			switch (result) {
				default: case MUM_FAILURE: return MUMA_MUM_FAILURE; break;
				case MUM_CREATE_CALL_FAILED: return MUMA_MUM_CREATE_CALL_FAILED; break;
				case MUM_DESTROY_CALL_FAILED: return MUMA_MUM_DESTROY_CALL_FAILED; break;
				case MUM_LOCK_CALL_FAILED: return MUMA_MUM_LOCK_CALL_FAILED; break;
				case MUM_UNLOCK_CALL_FAILED: return MUMA_MUM_UNLOCK_CALL_FAILED; break;
				case MUM_WAIT_CALL_FAILED: return MUMA_MUM_WAIT_CALL_FAILED; break;
				case MUM_INVALID_THREAD: return MUMA_MUM_INVALID_THREAD; break;
				case MUM_INVALID_MUTEX: return MUMA_MUM_INVALID_MUTEX; break;
				case MUM_INVALID_SPINLOCK: return MUMA_MUM_INVALID_SPINLOCK; break;
				case MUM_PREVIOUS_THREAD_CLOSED_BEFORE_LOCK: return MUMA_MUM_PREVIOUS_THREAD_CLOSED_BEFORE_LOCK; break;
				case MUM_THREAD_TIMED_OUT: return MUMA_MUM_THREAD_TIMED_OUT; break;
			}
		}
	#endif

	#ifdef MUMA_NAMES
		MUDEF const char* muma_result_get_name(mumaResult result) {
			switch (result) {
				default: return "MUMA_UNKNOWN"; break;
				case MUMA_SUCCESS: return "MUMA_SUCCESS"; break;
				#ifdef MU_THREADSAFE
				case MUMA_MUM_FAILURE: return "MUMA_MUM_FAILURE"; break;
				case MUMA_MUM_CREATE_CALL_FAILED: return "MUMA_MUM_CREATE_CALL_FAILED"; break;
				case MUMA_MUM_DESTROY_CALL_FAILED: return "MUMA_MUM_DESTROY_CALL_FAILED"; break;
				case MUMA_MUM_LOCK_CALL_FAILED: return "MUMA_MUM_LOCK_CALL_FAILED"; break;
				case MUMA_MUM_UNLOCK_CALL_FAILED: return "MUMA_MUM_UNLOCK_CALL_FAILED"; break;
				case MUMA_MUM_WAIT_CALL_FAILED: return "MUMA_MUM_WAIT_CALL_FAILED"; break;
				case MUMA_MUM_INVALID_THREAD: return "MUMA_MUM_INVALID_THREAD"; break;
				case MUMA_MUM_INVALID_MUTEX: return "MUMA_MUM_INVALID_MUTEX"; break;
				case MUMA_MUM_INVALID_SPINLOCK: return "MUMA_MUM_INVALID_SPINLOCK"; break;
				case MUMA_MUM_PREVIOUS_THREAD_CLOSED_BEFORE_LOCK: return "MUMA_MUM_PREVIOUS_THREAD_CLOSED_BEFORE_LOCK"; break;
				case MUMA_MUM_THREAD_TIMED_OUT: return "MUMA_MUM_THREAD_TIMED_OUT"; break;
				#endif
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

