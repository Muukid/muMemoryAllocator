/*
muMemoryAllocator.h - Muukid
Public domain simple single-file C library for easy dynamic allocation.
No warranty implied; use at your own risk.

Licensed under MIT License or public domain, whichever you prefer.
More explicit license information at the end of file.

@TODO Prevent "allocation size ping pong", ie allocation size flicking
between, for example, 3 and 4, causing constant allocation.
*/

/* @DOCBEGIN

# muMemoryAllocator v1.1.0

muMemoryAllocator (acrynomized to muma) is a public domain header-only single-file C library for easy management of dynamic allocation. To use it, download the `muMemoryAllocator.h` file, add it to your include path, and include it like so:

```c
#define MUMA_IMPLEMENTATION
#include "muMemoryAllocator.h"
```

More information about the general structure of a mu library is provided at [the mu library information GitHub repository.](https://github.com/Muukid/mu-library-information)

# Demos

Demos that quickly show the gist of the library and how it works are available in the `demos` folder.

# Licensing

muma is licensed under public domain or MIT, whichever you prefer. More information is provided in the accompanying file `license.md` and at the bottom of `muMemoryAllocator.h`.

# Other library dependencies

muma has a dependency on:

[muUtility v1.1.0](https://github.com/Muukid/muUtility/releases/tag/v1.1.0)

Note that mu libraries store their dependencies within their files, so you don't need to import these dependencies yourself.

@DOCEND */

#ifndef MUMA_H
	#define MUMA_H
	
	/* muUtility commit v1.1.0 header */
	
		#if !defined(MU_CHECK_VERSION_MISMATCHING) && defined(MUU_H) && \
			(MUU_VERSION_MAJOR != 1 || MUU_VERSION_MINOR != 1 || MUU_VERSION_PATCH != 0)

			#pragma message("[MUMA] muUtility's header has already been defined, but version doesn't match the version that this library is built for. This may lead to errors, warnings, or unexpected behavior. Define MU_CHECK_VERSION_MISMATCHING before this to turn off this message.")

		#endif

		// @IGNORE
		#ifndef MUU_H
			#define MUU_H
			
			// @DOCLINE # Secure warnings
			// @DOCLINE mu libraries often use non-secure functions that will trigger warnings on certain compilers. These warnings are, to put it lightly, dumb, so the header section of muUtility defines `_CRT_SECURE_NO_WARNINGS`. However, it is not guaranteed that this definition will actually turn the warnings off, which at that point, they have to be manually turned off by the user.
			#if !defined(MU_SECURE_WARNINGS) && !defined(_CRT_SECURE_NO_WARNINGS)
				#define _CRT_SECURE_NO_WARNINGS
			#endif

			#ifdef __cplusplus
			extern "C" { // }
			#endif

			// @DOCLINE # C standard library dependencies

				// @DOCLINE muUtility has several C standard library dependencies, all of which are overridable by defining them before the inclusion of its header. The following is a list of those dependencies:

				// @DOCLINE ## `stdint.h` dependencies
				#if !defined(int8_m)      || \
					!defined(uint8_m)     || \
					!defined(int16_m)     || \
					!defined(uint16_m)    || \
					!defined(int32_m)     || \
					!defined(uint32_m)    || \
					!defined(int64_m)     || \
					!defined(uint64_m)    || \
					!defined(MU_SIZE_MAX)

					#define __STDC_LIMIT_MACROS
					#define __STDC_CONSTANT_MACROS
					#include <stdint.h>

					// @DOCLINE `int8_m`: equivalent to `int8_t` if `INT8_MAX` is defined, `char` if otherwise
					#ifndef int8_m
						#ifdef INT8_MAX
							#define int8_m int8_t
						#else
							#define int8_m char
						#endif
					#endif

					// @DOCLINE `uint8_m`: equivalent to `uint8_t` if `UINT8_MAX` is defined, `unsigned char` if otherwise
					#ifndef uint8_m
						#ifdef UINT8_MAX
							#define uint8_m uint8_t
						#else
							#define uint8_m unsigned char
						#endif
					#endif

					// @DOCLINE `int16_m`: equivalent to `int16_t` if `INT16_MAX` is defined, `short` if otherwise
					#ifndef int16_m
						#ifdef INT16_MAX
							#define int16_m int16_t
						#else
							#define int16_m short
						#endif
					#endif

					// @DOCLINE `uint16_m`: equivalent to `uint16_t` if `UINT16_MAX` is defined, `unsigned short` if otherwise
					#ifndef uint16_m
						#ifdef UINT16_MAX
							#define uint16_m uint16_t
						#else
							#define uint16_m unsigned short
						#endif
					#endif

					// @DOCLINE `int32_m`: equivalent to `int32_t` if `INT32_MAX` is defined, `long` if otherwise
					#ifndef int32_m
						#ifdef INT32_MAX
							#define int32_m int32_t
						#else
							#define int32_m long
						#endif
					#endif

					// @DOCLINE `uint32_m`: equivalent to `uint32_t` if `UINT32_MAX` is defined, `unsigned long` if otherwise
					#ifndef uint32_m
						#ifdef UINT32_MAX
							#define uint32_m uint32_t
						#else
							#define uint32_m unsigned long
						#endif
					#endif

					// @DOCLINE `int64_m`: equivalent to `int64_t` if `INT64_MAX` is defined, `long long` if otherwise
					#ifndef int64_m
						#ifdef INT64_MAX
							#define int64_m int64_t
						#else
							#define int64_m long long
						#endif
					#endif

					// @DOCLINE `uint64_m`: equivalent to `uint64_t` if `UINT64_MAX` is defined, `unsigned long long` if otherwise
					#ifndef uint64_m
						#ifdef UINT64_MAX
							#define uint64_m uint64_t
						#else
							#define uint64_m unsigned long long
						#endif
					#endif

					// @DOCLINE `MU_SIZE_MAX`: equivalent to `SIZE_MAX`
					#ifndef MU_SIZE_MAX
						#define MU_SIZE_MAX SIZE_MAX
					#endif

				#endif

				// @DOCLINE ## `stddef.h` dependencies
				#if !defined(size_m)

					#include <stddef.h>

					// @DOCLINE `size_m`: equivalent to `size_t`
					#ifndef size_m
						#define size_m size_t
					#endif

				#endif

				// @DOCLINE ## `stdbool.h` dependencies
				#if !defined(muBool)   || \
					!defined(MU_TRUE)  || \
					!defined(MU_FALSE)

					#include <stdbool.h>

					// @DOCLINE `muBool`: equivalent to `bool`
					#ifndef muBool
						#define muBool bool
					#endif

					// @DOCLINE `MU_TRUE`: equivalent to `true`
					#ifndef MU_TRUE
						#define MU_TRUE true
					#endif

					// @DOCLINE `MU_FALSE`: equivalent to `false`
					#ifndef MU_FALSE
						#define MU_FALSE false
					#endif

				#endif

			// @DOCLINE # Macros

				// @DOCLINE ## Version

					// @DOCLINE The macros `MUU_VERSION_MAJOR`, `MUU_VERSION_MINOR`, and `MUU_VERSION_PATCH` are defined to match its respective release version, following the formatting of `MAJOR.MINOR.PATCH`.

					#define MUU_VERSION_MAJOR 1
					#define MUU_VERSION_MINOR 1
					#define MUU_VERSION_PATCH 0

				// @DOCLINE ## `MUDEF`

					// @DOCLINE The `MUDEF` macro is a macro used by virtually all mu libraries, which is generally added before a header-defined variable or function. Its default value is `extern`, but can be changed to `static` by defining `MU_STATIC` before the header section of muUtility is defined. Its value can also be overwritten entirely to anything else by directly defining `MUDEF`.

					#ifndef MUDEF
						#ifdef MU_STATIC
							#define MUDEF static
						#else
							#define MUDEF extern
						#endif
					#endif

				// @DOCLINE ## Zero struct

					// @DOCLINE There are two macros, `MU_ZERO_STRUCT` and `MU_ZERO_STRUCT_CONST`, which are macro functions used to zero-out a struct's contents, with their only parameter being the struct type. The reason this needs to be defined is because the way C and C++ syntax handles an empty struct are different, and need to be adjusted for.

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

				// @DOCLINE ## Byte

					// @DOCLINE Several macros are defined to interact with the idea of a byte.

					// @DOCLINE The macro `muByte` exists as a macro for `uint8_m`, and represents a byte as a type.

					#ifndef muByte
						#define muByte uint8_m
					#endif

					// @DOCLINE There are also several macro functions used to read and write to bytes. These macros are:

					// @DOCLINE `mu_rle_uint8(b)`: Creates an expression of type `uint8_m` that reads the value stored in the 1-byte-length little-endian byte array `b`.
					#ifndef mu_rle_uint8
						#define mu_rle_uint8(b) ((uint8_m)b[0] << 0)
					#endif
					// @DOCLINE `mu_rbe_uint8(b)`: Creates an expression of type `uint8_m` that reads the value stored in the 1-byte-length big-endian byte array `b`.
					#ifndef mu_rbe_uint8
						#define mu_rbe_uint8(b) ((uint8_m)b[0] << 0)
					#endif

					// @DOCLINE `mu_rle_uint16(b)`: Creates an expression of type `uint16_m` that reads the value stored in the 2-byte-length little-endian byte array `b`.
					#ifndef mu_rle_uint16
						#define mu_rle_uint16(b) ((uint16_m)b[0] << 0 | (uint16_m)b[1] << 8)
					#endif
					// @DOCLINE `mu_rbe_uint16(b)`: Creates an expression of type `uint16_m` that reads the value stored in the 2-byte-length big-endian byte array `b`.
					#ifndef mu_rbe_uint16
						#define mu_rbe_uint16(b) ((uint16_m)b[1] << 0 | (uint16_m)b[0] << 8)
					#endif

					// @DOCLINE `mu_rle_uint32(b)`: Creates an expression of type `uint32_m` that reads the value stored in the 4-byte-length little-endian byte array `b`.
					#ifndef mu_rle_uint32
						#define mu_rle_uint32(b) ((uint32_m)b[0] << 0 | (uint32_m)b[1] << 8 | (uint32_m)b[2] << 16 | (uint32_m)b[3] << 24)
					#endif
					// @DOCLINE `mu_rbe_uint32(b)`: Creates an expression of type `uint32_m` that reads the value stored in the 4-byte-length big-endian byte array `b`.
					#ifndef mu_rbe_uint32
						#define mu_rbe_uint32(b) ((uint32_m)b[3] << 0 | (uint32_m)b[2] << 8 | (uint32_m)b[1] << 16 | (uint32_m)b[0] << 24)
					#endif

					// @DOCLINE `mu_rle_uint64(b)`: Creates an expression of type `uint64_m` that reads the value stored in the 8-byte-length little-endian byte array `b`.
					#ifndef mu_rle_uint64
						#define mu_rle_uint64(b) ((uint64_m)b[0] << 0 | (uint64_m)b[1] << 8 | (uint64_m)b[2] << 16 | (uint64_m)b[3] << 24 | (uint64_m)b[4] << 32 | (uint64_m)b[5] << 40 | (uint64_m)b[6] << 48 | (uint64_m)b[7] << 56)
					#endif
					// @DOCLINE `mu_rbe_uint64(b)`: Creates an expression of type `uint64_m` that reads the value stored in the 8-byte-length big-endian byte array `b`.
					#ifndef mu_rbe_uint64
						#define mu_rbe_uint64(b) ((uint64_m)b[7] << 0 | (uint64_m)b[6] << 8 | (uint64_m)b[5] << 16 | (uint64_m)b[4] << 24 | (uint64_m)b[3] << 32 | (uint64_m)b[2] << 40 | (uint64_m)b[1] << 48 | (uint64_m)b[0] << 56)
					#endif

					// @DOCLINE `mu_wle_uint8(b, i)`: Reads the given contents of the 1-byte-length little-endian byte array `b` into the given `uint8_m` variable `i`.
					#ifndef mu_wle_uint8
						#define mu_wle_uint8(b, i) b[0] = (uint8_m)(i >> 0);
					#endif
					// @DOCLINE `mu_wbe_uint8(b, i)`: Reads the given contents of the 1-byte-length big-endian byte array `b` into the given `uint8_m` variable `i`.
					#ifndef mu_wbe_uint8
						#define mu_wbe_uint8(b, i) b[0] = (uint8_m)(i >> 0);
					#endif

					// @DOCLINE `mu_wle_uint16(b, i)`: Reads the given contents of the 2-byte-length little-endian byte array `b` into the given `uint16_m` variable `i`.
					#ifndef mu_wle_uint16
						#define mu_wle_uint16(b, i) b[0] = (uint8_m)(i >> 0); b[1] = (uint8_m)(i >> 8);
					#endif
					// @DOCLINE `mu_wbe_uint16(b, i)`: Reads the given contents of the 2-byte-length big-endian byte array `b` into the given `uint16_m` variable `i`.
					#ifndef mu_wbe_uint16
						#define mu_wbe_uint16(b, i) b[1] = (uint8_m)(i >> 0); b[0] = (uint8_m)(i >> 8);
					#endif

					// @DOCLINE `mu_wle_uint32(b, i)`: Reads the given contents of the 4-byte-length little-endian byte array `b` into the given `uint32_m` variable `i`.
					#ifndef mu_wle_uint32
						#define mu_wle_uint32(b, i) b[0] = (uint8_m)(i >> 0); b[1] = (uint8_m)(i >> 8); b[2] = (uint8_m)(i >> 16); b[3] = (uint8_m)(i >> 24);
					#endif
					// @DOCLINE `mu_wbe_uint32(b, i)`: Reads the given contents of the 4-byte-length big-endian byte array `b` into the given `uint32_m` variable `i`.
					#ifndef mu_wbe_uint32
						#define mu_wbe_uint32(b, i) b[3] = (uint8_m)(i >> 0); b[2] = (uint8_m)(i >> 8); b[1] = (uint8_m)(i >> 16); b[0] = (uint8_m)(i >> 24);
					#endif

					// @DOCLINE `mu_wle_uint64(b, i)`: Reads the given contents of the 8-byte-length little-endian byte array `b` into the given `uint64_m` variable `i`.
					#ifndef mu_wle_uint64
						#define mu_wle_uint64(b, i) b[0] = (uint8_m)(i >> 0); b[1] = (uint8_m)(i >> 8); b[2] = (uint8_m)(i >> 16); b[3] = (uint8_m)(i >> 24); b[4] = (uint8_m)(i >> 32); b[5] = (uint8_m)(i >> 40); b[6] = (uint8_m)(i >> 48); b[7] = (uint8_m)(i >> 56);
					#endif
					// @DOCLINE `mu_wbe_uint64(b, i)`: Reads the given contents of the 8-byte-length big-endian byte array `b` into the given `uint64_m` variable `i`.
					#ifndef mu_wbe_uint64
						#define mu_wbe_uint64(b, i) b[7] = (uint8_m)(i >> 0); b[6] = (uint8_m)(i >> 8); b[5] = (uint8_m)(i >> 16); b[4] = (uint8_m)(i >> 24); b[3] = (uint8_m)(i >> 32); b[2] = (uint8_m)(i >> 40); b[1] = (uint8_m)(i >> 48); b[0] = (uint8_m)(i >> 56);
					#endif

				// @DOCLINE ## Null values

					// @DOCLINE The `MU_NULL_PTR` macro is a macro value used to represent a null pointer. It is simply 0.
					#ifndef MU_NULL_PTR
						#define MU_NULL_PTR 0
					#endif

					// @DOCLINE The `MU_NULL` macro is a macro value used to represent a null value. It is simply 0.
					#ifndef MU_NULL
						#define MU_NULL 0
					#endif

				// @DOCLINE ## Set result

					/* @DOCBEGIN The `MU_SET_RESULT(res, val)` macro is a macro function that checks if the given parameter `res` is a null pointer. If it is, it does nothing, but if it isn't, it sets its value to the given parameter `val`. This macro saves a lot of code, shrinking down what would be this:

					```c
					if (result != MU_NULL_PTR) {
					    *result = ...;
					}
					```

					into this:
					
					```c
					MU_SET_RESULT(result, ...)
					```

					@DOCEND */

					#ifndef MU_SET_RESULT
						#define MU_SET_RESULT(res, val) if(res!=MU_NULL_PTR){*res=val;}
					#endif

				// @DOCLINE ## Enum

					/* @DOCBEGIN

					The `MU_ENUM(name, ...)` macro is a macro function used to declare an enumerator. The reason why one would prefer this over the traditional way of declaring enumerators is because it actually makes it a `size_m` which can avoid errors on certain compilers (looking at you, Microsoft) in regards to treating enumerators like values. It expands like this:

					```c
					enum _##name{
					    __VA_ARGS__
					};
					typedef enum _##name _##name;
					typedef size_m name;
					```

					@DOCEND */

					#define MU_ENUM(name, ...) enum _##name{__VA_ARGS__};typedef enum _##name _##name; typedef size_m name;

				// @DOCLINE ## Operating system recognition

					/* @DOCBEGIN The macros `MU_WIN32` or `MU_UNIX` are defined (if neither were defined before) in order to allow mu libraries to easily check if they're running on a Windows or Unix system.

					`MU_WIN32` will be defined if `WIN32` or `_WIN32` are defined, one of which is usually pre-defined on Windows systems.

					`MU_UNIX` will be defined if `__unix__` is defined or both `__APPLE__` and `__MACH__` are defined.

					@DOCEND */

					#if !defined(MU_WIN32) && !defined(MU_UNIX)
						#if defined(WIN32) || defined(_WIN32)
							#define MU_WIN32
						#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
							#define MU_UNIX
						#endif
					#endif

			#ifdef __cplusplus
			}
			#endif

		#endif /* MUU_H */
		// @ATTENTION
	
	#ifdef __cplusplus
		extern "C" {
	#endif

	// @DOCLINE # C standard library dependencies

		// @DOCLINE muma has several C standard library dependencies not provided by its other library dependencies, all of which are overridable by defining them before the inclusion of its header. The following is a list of all of those dependencies.

		// @DOCLINE ## `stdlib.h` dependencies
		#if !defined(mu_malloc)  || \
			!defined(mu_free)    || \
			!defined(mu_realloc)

			#include <stdlib.h>

			// @DOCLINE `mu_malloc`: equivalent to `malloc`
			#ifndef mu_malloc
				#define mu_malloc malloc
			#endif

			// @DOCLINE `mu_free`: equivalent to `free`
			#ifndef mu_free
				#define mu_free free
			#endif

			// @DOCLINE `mu_realloc`: equivalent to `realloc`
			#ifndef mu_realloc
				#define mu_realloc realloc
			#endif

		#endif

		// @DOCLINE ## `string.h` dependencies
		#if !defined(mu_memset)  || \
			!defined(mu_memcpy)  || \
			!defined(mu_memmove)

			#include <string.h>

			// @DOCLINE `mu_memset`: equivalent to `memset`
			#ifndef mu_memset
				#define mu_memset memset
			#endif

			// @DOCLINE `mu_memcpy`: equivalent to `memcpy`
			#ifndef mu_memcpy
				#define mu_memcpy memcpy
			#endif

			// @DOCLINE `mu_memmove`: equivalent to `memmove`
			#ifndef mu_memmove
				#define mu_memmove memmove
			#endif

		#endif

	// @DOCLINE # Enums

		// @DOCLINE ## Result enum

			// @DOCLINE muma uses the result enumerator `mumaResult` to represent how a function went. It has these possible values:

			MU_ENUM(mumaResult,
				// @DOCLINE `@NLFT`: the function performed successfully.
				MUMA_SUCCESS,

				// @DOCLINE `@NLFT`: the function could not allocate the memory necessary to perform the function.
				MUMA_FAILED_TO_ALLOCATE,
				// @DOCLINE `@NLFT`: an invalid index into an array was requested, so the function could not be performed.
				MUMA_INVALID_INDEX,
				// @DOCLINE `@NLFT`: the array shifting necessary to perform the function was invalid.
				MUMA_INVALID_SHIFT_AMOUNT,
				// @DOCLINE `@NLFT`: a requested element in an array could not be located.
				MUMA_NOT_FOUND
			)

	// @DOCLINE # Macros

		// @DOCLINE ## Dynamic array

			/* @DOCBEGIN
			
			The macro `mu_dynamic_array_declaration` is a macro function that is used to create a dynamic array. What it does, more fundamentally, is declare custom struct for a dynamic array of a given type and declares several functions to use with this custom struct. Its parameters look like this:
			
			```c
			mu_dynamic_array_declaration(struct_name, type, function_name_prefix, type_comparison_func)
			```

			`struct_name` is the desired name of the dynamic array struct that will be created for the given type.

			`type` is the type that will be used to represent an element in the array.

			`function_name_prefix` is the prefix that will be used for the function names (for example, `function_name_prefix##create`).

			`type_comparison_func` is the name of a function that takes in two parameters of type `type` and returns an integer value (hopefully `muBool`) representing whether or not they're equivalent. This is used for functions that need to find a given element in an array.

			### Struct

			The dynamic array expands to do a few things. The first thing it does is create a struct for the dynamic array, following this expansion:

			```c
			struct struct_name {
				type* data;
				size_m allocated_length;
				size_m length;
			}; typedef struct struct_name struct_name;
			```

			Note that `length` is not always equal to `allocated_length`, as the dynamic array may carry extra allocated length to optimize allocation calls.

			Note that these members aren't meant to be modified by the user, only read.

			### Functions

			After the struct is defined, functions are also defined to interface with and use this struct. The following is a list of all of those functions.

			@DOCEND */

			#define mu_dynamic_array_declaration(struct_name, type, function_name_prefix, type_comparison_func) \
				\
				struct struct_name { \
					type* data; \
					size_m allocated_length; \
					size_m length; \
				}; typedef struct struct_name struct_name; \
				\
				/*@DOCBEGIN #### Creation @NEWLINE @DOCEND*/ \
				/*@DOCBEGIN The function `function_name_prefix##create` is used to create a dynamic array, defined below: @NEWLINE @DOCEND*/ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN struct_name function_name_prefix##create(mumaResult* result, size_m length) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				/*@DOCBEGIN Note that `length` being 0 is valid, and nothing will be immediately allocated. @NEWLINE @DOCEND */ \
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
				/*@DOCBEGIN #### Destruction @NEWLINE @DOCEND */ \
				/*@DOCBEGIN The function `function_name_prefix##destroy` is used to destroy a dynamic array and fully free its contents, defined below: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN struct_name function_name_prefix##destroy(mumaResult* result, struct_name s) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				/*@DOCBEGIN Note that this function is safe to call on an array with nothing allocated (of course, assuming that its members are valid). @NEWLINE @DOCEND */ \
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
				/*@DOCBEGIN #### Resize @NEWLINE @DOCEND */ \
				/*@DOCBEGIN The function `function_name_prefix##resize` is used to resize a dynamic array, defined below: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN struct_name function_name_prefix##resize(mumaResult* result, struct_name s, size_m length) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				/*@DOCBEGIN If this function performs correctly, `s.length` will be equal to `length`. @NEWLINE @DOCEND */ \
				/*@DOCBEGIN Note that `length` is allowed to be any value, including being above, below, or equal to the array's length / allocated length, as well as 0 (although, it cannot be guaranteed that this will free memory). @NEWLINE @DOCEND */ \
				/*@DOCBEGIN Note that, in general, this function is not guaranteed to allocate or free any memory, even if the length is changing a significant amount. @NEWLINE @DOCEND */ \
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
				/*@DOCBEGIN #### Left shift @NEWLINE @DOCEND */ \
				/*@DOCBEGIN The function `function_name_prefix##lshift` is used to shift a dynamic array's contents left, defined below: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN struct_name function_name_prefix##lshift(mumaResult* result, struct_name s, size_m index, size_m amount) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				/*@DOCBEGIN This function takes all of the elements of an array starting from `index` and ending at the end of the array, and shifts them left `amount` amount of times. @NEWLINE @DOCEND */ \
				/*@DOCBEGIN If this function performs correctly, `s.length` will be equal to `s.length` minus `amount`. @NEWLINE @DOCEND */ \
				/*@DOCBEGIN Note that this operation can overwrite contents of other elements within the array. @NEWLINE @DOCEND */ \
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
					mu_memmove(&s.data[index-amount], &s.data[index], sizeof(type)*(s.length-index)); \
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
				/*@DOCBEGIN #### Right shift @NEWLINE @DOCEND */ \
				/*@DOCBEGIN The function `function_name_prefix##rshift` is used to shift a dynamic array's contents right, defined below: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN struct_name function_name_prefix##rshift(mumaResult* result, struct_name s, size_m index, size_m amount) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				/*@DOCBEGIN This function takes all of the elements of an array starting from `index` and ending at the end of the array, and shifts them right `amount` amount of times. @NEWLINE @DOCEND */ \
				/*@DOCBEGIN If this function performs correctly, `s.length` will be equal to `s.length` plus `amount`. @NEWLINE @DOCEND */ \
				/*@DOCBEGIN Note that this operation cannot overwrite contents of other elements within the array. @NEWLINE @DOCEND */ \
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
					/* I have genuinely no idea why it needs to have a -1. It crashes if you don't. */\
					/* And lshift doesn't need one. Why?? */ \
					mu_memmove(&s.data[index+amount], &s.data[index], sizeof(type)*((s.length-index)-1)); \
					mu_memset(&s.data[index], 0, sizeof(type)*(amount)); \
					\
					return s; \
				} \
				\
				/*@DOCBEGIN #### Multi-insert @NEWLINE @DOCEND */ \
				/*@DOCBEGIN The function `function_name_prefix##multiinsert` is used to insert multiple elements into a dynamic array, defined below: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN struct_name function_name_prefix##multiinsert(mumaResult* result, struct_name s, size_m index, type* insert, size_m count) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				/*@DOCBEGIN This function takes `insert` (which should have `count` amount of `type` elements) and inserts it at index `index`. @NEWLINE @DOCEND */ \
				/*@DOCBEGIN If this function performs correctly, `s.length` will be equal to `s.length` plus `count`, and `s.data[index+n]` will be equal to `insert[0+n]` while `n` is less than `count` (ie, `s.data[index+0] == insert[0]`). @NEWLINE @DOCEND */ \
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
				/*@DOCBEGIN #### Insert @NEWLINE @DOCEND */ \
				/*@DOCBEGIN The function `function_name_prefix##insert` is used to insert a single element into a dynamic array, defined below: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN struct_name function_name_prefix##insert(mumaResult* result, struct_name s, size_m index, type insert) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				/*@DOCBEGIN This function just returns `function_name_prefix##multiinsert` with the following parameters: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN function_name_prefix##multiinsert(result, s, index, &insert, 1) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				struct_name function_name_prefix##insert(mumaResult* result, struct_name s, size_m index, type insert) { \
					return function_name_prefix##multiinsert(result, s, index, &insert, 1); \
				} \
				\
				/*@DOCBEGIN #### Multi-erase @NEWLINE @DOCEND */ \
				/*@DOCBEGIN The function `function_name_prefix##multierase` is used to erase multiple elements from an array, defined below: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN struct_name function_name_prefix##multierase(mumaResult* result, struct_name s, size_m index, size_m count) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				/*@DOCBEGIN This function just returns `function_name_prefix##lshift` with the following parameters: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN function_name_prefix##lshift(result, s, index+count, count) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				struct_name function_name_prefix##multierase(mumaResult* result, struct_name s, size_m index, size_m count) { \
					return function_name_prefix##lshift(result, s, index+count, count); \
				} \
				\
				/*@DOCBEGIN #### Erase @NEWLINE @DOCEND */ \
				/*@DOCBEGIN The function `function_name_prefix##erase` is used to erase an element from an array, defined below: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN struct_name function_name_prefix##erase(mumaResult* result, struct_name s, size_m index) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				/*@DOCBEGIN This function just returns `function_name_prefix##multierase` with the following parameters: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN function_name_prefix##multierase(result, s, index, 1) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				struct_name function_name_prefix##erase(mumaResult* result, struct_name s, size_m index) { \
					return function_name_prefix##multierase(result, s, index, 1); \
				} \
				/*@DOCBEGIN #### Clear @NEWLINE @DOCEND */ \
				/*@DOCBEGIN The function `function_name_prefix##clear` is used to clear all the elements from an array. It is identical to `function_name_prefix##destroy`. @NEWLINE @DOCEND */ \
				struct_name function_name_prefix##clear(mumaResult* result, struct_name s) { \
					return function_name_prefix##destroy(result, s); \
				} \
				\
				/*@DOCBEGIN #### Multi-push @NEWLINE @DOCEND */ \
				/*@DOCBEGIN The function `function_name_prefix##multipush` is used to push multiple elements on top of an array, defined below: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN struct_name function_name_prefix##multipush(mumaResult* result, struct_name s, type* push, size_m count) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
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
				/*@DOCBEGIN #### Push @NEWLINE @DOCEND */ \
				/*@DOCBEGIN The function `function_name_prefix##push` is used to push an element on top of an array, defined below: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN struct_name function_name_prefix##push(mumaResult* result, struct_name s, type push) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				/*@DOCBEGIN This function just returns `function_name_prefix##multipush` with the following parameters: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN function_name_prefix##multipush(result, s, &push, 1) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				struct_name function_name_prefix##push(mumaResult* result, struct_name s, type push) { \
					return function_name_prefix##multipush(result, s, &push, 1); \
				} \
				\
				/*@DOCBEGIN #### Multi-pop @NEWLINE @DOCEND */ \
				/*@DOCBEGIN The function `function_name_prefix##multipop` is used to pop multiple elements from the top of an array, defined below: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN struct_name function_name_prefix##multipop(mumaResult* result, struct_name s, size_m count) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				struct_name function_name_prefix##multipop(mumaResult* result, struct_name s, size_m count) { \
					return function_name_prefix##resize(result, s, s.length-count); \
				} \
				\
				/*@DOCBEGIN #### Pop @NEWLINE @DOCEND */ \
				/*@DOCBEGIN The function `function_name_prefix##pop` is used to pop an element from the top of an array, defined below: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN struct_name function_name_prefix##pop(mumaResult* result, struct_name s) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				/*@DOCBEGIN This function just returns `function_name_prefix##multipop` with the following parameters: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN function_name_prefix##multipop(result, s, 1) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				struct_name function_name_prefix##pop(mumaResult* result, struct_name s) { \
					return function_name_prefix##multipop(result, s, 1); \
				} \
				\
				/*@DOCBEGIN #### Find @NEWLINE @DOCEND */ \
				/*@DOCBEGIN The function `function_name_prefix##find` is used to find a certain element in an array, defined below: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN size_m function_name_prefix##find(mumaResult* result, struct_name s, type find) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				/*@DOCBEGIN This function scans from index 0 up until the end of the array and uses `type_comparison_func` to decide if `find` equals the element at the given index. @NEWLINE @DOCEND */ \
				/*@DOCBEGIN If a matching element is found, its index is returned. @NEWLINE @DOCEND */ \
				/*@DOCBEGIN If no matching element is found, `MU_SIZE_MAX` is returned. @NEWLINE @DOCEND */ \
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
					return MU_SIZE_MAX; \
				} \
				\
				/*@DOCBEGIN #### Find-push @NEWLINE @DOCEND */ \
				/*@DOCBEGIN The function `function_name_prefix##find_push` is used to find a certain element in an array and, if not found, push it onto the array, defined below: @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ```c @NEWLINE @DOCEND */ \
				/*@DOCBEGIN struct_name function_name_prefix##find_push(mumaResult* result, struct_name s, type find, size_m* p_index) @NEWLINE @DOCEND */ \
				/*@DOCBEGIN ``` @NEWLINE @DOCEND */ \
				/*@DOCBEGIN This function tries to find `find` in the given array `s` using `function_name_prefix##find` and, if not found, pushes `find` onto the array. In either case, if successful and `p_index` isn't 0, `p_index` is dereferenced and set to the index that contains `find`. @NEWLINE @DOCEND */ \
				struct_name function_name_prefix##find_push(mumaResult* result, struct_name s, type find, size_m* p_index) { \
					MU_SET_RESULT(result, MUMA_SUCCESS) \
					\
					mumaResult res = MUMA_SUCCESS; \
					size_m index = function_name_prefix##find(&res, s, find); \
					if (index != MU_SIZE_MAX) { \
						if (p_index != MU_NULL_PTR) { \
							*p_index = index; \
						} \
						return s; \
					} \
					if (res != MUMA_SUCCESS && res != MUMA_NOT_FOUND) { \
						if (p_index != MU_NULL_PTR) { \
							*p_index = MU_SIZE_MAX; \
						} \
						MU_SET_RESULT(result, res) \
						return s; \
					} \
					\
					s = function_name_prefix##push(&res, s, find); \
					if (res != MUMA_SUCCESS) { \
						if (p_index != MU_NULL_PTR) { \
							*p_index = MU_SIZE_MAX; \
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

		// @DOCLINE ## Version

			// @DOCLINE There are three major, minor, and patch macros respectively defined to represent the version of mum, defined as `MUMA_VERSION_MAJOR`, `MUMA_VERSION_MINOR`, and `MUMA_VERSION_PATCH`, following the formatting of `vMAJOR.MINOR.PATCH`.

			#define MUMA_VERSION_MAJOR 1
			#define MUMA_VERSION_MINOR 1
			#define MUMA_VERSION_PATCH 0

	// @DOCLINE # Functions

		// @DOCLINE ## Name function

			#ifdef MUMA_NAMES
				// @DOCLINE The function `muma_result_get_name` is used to get a `const char*` representation of a `mumaResult` value, defined below: @NLNT
				MUDEF const char* muma_result_get_name(mumaResult result);
				// @DOCLINE Note that this function is only defined if the macro `MUMA_NAMES` is defined.
			#endif

	#ifdef __cplusplus
		}
	#endif

#endif /* MUMA_H */

#ifdef MUMA_IMPLEMENTATION

	/* muUtility version 1.0.0 implementation */

		#ifndef MUU_IMPLEMENTATION
			#define MUU_IMPLEMENTATION

			#ifdef MUU_IMPLEMENTATION
	
				// ...

			#endif /* MUU_IMPLEMENTATION */
		#endif

	#ifdef __cplusplus
		extern "C" {
	#endif

	#ifdef MUMA_NAMES
		MUDEF const char* muma_result_get_name(mumaResult result) {
			switch (result) {
				default: return "MUMA_UNKNOWN"; break;
				case MUMA_SUCCESS: return "MUMA_SUCCESS"; break;
				case MUMA_FAILED_TO_ALLOCATE: return "MUMA_FAILED_TO_ALLOCATE"; break;
				case MUMA_INVALID_INDEX: return "MUMA_INVALID_INDEX"; break;
				case MUMA_INVALID_SHIFT_AMOUNT: return "MUMA_INVALID_SHIFT_AMOUNT"; break;
				case MUMA_NOT_FOUND: return "MUMA_NOT_FOUND"; break;
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

