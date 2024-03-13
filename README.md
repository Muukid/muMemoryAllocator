# muMemoryAllocator
muMemoryAllocator (muma) is a public domain simple C header file that allows for easy and optimized dynamic allocation. To use it, simply download the `muMemoryAllocator.h` file, add it to your include path, and include it like so:

```
#include MUMA_IMPLEMENTATION
#include "muMemoryAllocator.h"
```

Note that *not* defining `MUMA_IMPLEMENTATION` will define everything needed to interact with the library, but will not give an implementation for the functions, meaning that you can include it across multiple files without redefinition errors as long as you define `MUMA_IMPLEMENTATION` once in your main file.

# Licensing
muma is licensed under public domain or MIT, whichever you prefer. More information is provided in the accompanying file `license.md` and at the bottom of `muMemoryAllocator.h`.

# General information

## C/C++ standard compliancy
muma is C99 and C++11 compliant.

## Thread safety
muma is doesn't define or access any globally defined variables, but has no built-in system for locking a dynamic array with mutexes.

## Error checking
Most functions in muma take in a result pointer, `mumaResult* result`, as their first parameter. This is used to indicate whether or not the function executed successfully or not. `MU_NULL_PTR` can be passed in to ignore this, but if a valid pointer is passed in, it will dereference it with a `mumaResult` enum value indicating how the function executed.

# Functions

## Convert `mumaResult` to `const char*`
The function `muma_result_get_name` is used to get a string representation of a `mumaResult` enum value, defined below:

```
MUDEF const char* muma_result_get_name(mumaResult result);
```

Note that this function is only declared if `MUMA_NAMES` is defined before including the header file.

Note that this function returns `MUMA_UNKNOWN` if the enum value doesn't match any defined `mumaResult` enum value.

## Dynamic array creation and destruction
The function `mu_dynamic_array_create` is used to create a valid `muDynamicArray` struct, defined below:

```
MUDEF muDynamicArray mu_dynamic_array_create(mumaResult* result, size_m type_size, size_m length);
```

Note that this function can be used to initialize dynamic arrays with a length of 0, and no memory will be allocated until necessary.

Note that all memory allocated by this function will automatically be set to all 0's.

The function `mu_dynamic_array_destroy` is used to destroy a valid `muDynamicArray` struct and free its allocated memory, defined below:

```
MUDEF muDynamicArray mu_dynamic_array_destroy(mumaResult* result, muDynamicArray da);
```

Note that *not* calling this function on an allocated `muDynamicArray` struct before the program ends will most likely lead to memory leaks.

## Getting and setting values

The functions `mu_dynamic_array_multiset` and `mu_dynamic_array_set` are used to set values within a given dynamic array at certain indexes, defined below:

```
MUDEF muDynamicArray mu_dynamic_array_multiset(mumaResult* result, muDynamicArray da, void* data, size_m count, size_m index);

MUDEF muDynamicArray mu_dynamic_array_set(mumaResult* result, muDynamicArray da, void* data, size_m index);
```

The function `mu_dynamic_array_get` is used to get a position in a given dynamic array by index, defined below:

```
MUDEF void* mu_dynamic_array_get(mumaResult* result, muDynamicArray da, size_m index);
```

Note that if this function fails, the pointer returned will be `MU_NULL_PTR`.

Note that a quicker (but less safe) way of getting and setting values within a dynamic array is using the `muda_i` macro, defined in the **Macros** section.

## Resizing
The function `mu_dynamic_array_resize` is used to resize a given dynamic array based on length, defined below:

```
MUDEF muDynamicArray mu_dynamic_array_resize(mumaResult* result, muDynamicArray da, size_m length);
```

Note that all memory added or removed from the length of the array will be automatically set to 0 by this function. This means that sizing down an array and then back up will not restore the memory originally lost when sizing down.

## Shifting
The functions `mu_dynamic_array_lshift` and `mu_dynamic_array_rshift` are used to shift an array's contents left or right respectively at an index, defined below:

```
MUDEF muDynamicArray mu_dynamic_array_lshift(mumaResult* result, muDynamicArray da, size_m index, size_m amount);

MUDEF muDynamicArray mu_dynamic_array_rshift(mumaResult* result, muDynamicArray da, size_m index, size_m amount);
```

Note that these functions automatically handle the resizing necessary to handle the operations specified.

## Inserting and erasing
The functions `mu_dynamic_array_multiinsert` and `mu_dynamic_array_insert` are used to insert values into a given dynamic array, defined below:

```
MUDEF muDynamicArray mu_dynamic_array_multiinsert(mumaResult* result, muDynamicArray da, void* insert, size_m count, size_m index);

MUDEF muDynamicArray mu_dynamic_array_insert(mumaResult* result, muDynamicArray da, void* insert, size_m index);
```

The functions `mu_dynamic_array_multierase` and `mu_dynamic_array_erase` are used to erase values from a given dynamic array, defined below:

```
MUDEF muDynamicArray mu_dynamic_array_multierase(mumaResult* result, muDynamicArray da, size_m index, size_m count);

MUDEF muDynamicArray mu_dynamic_array_erase(mumaResult* result, muDynamicArray da, size_m index);
```

## Clear
The function `mu_dynamic_array_clear` is used to clear all the contents from a given dynamic array, defined below:

```
MUDEF muDynamicArray mu_dynamic_array_clear(mumaResult* result, muDynamicArray da);
```

## Pushing and popping
The functions `mu_dynamic_array_multipush` and `mu_dynamic_array_push` are used to push items on top of a given dynamic array, defined below:

```
MUDEF muDynamicArray mu_dynamic_array_multipush(mumaResult* result, muDynamicArray da, void* push, size_m count);

MUDEF muDynamicArray mu_dynamic_array_push(mumaResult* result, muDynamicArray da, void* push);
```

The functions `mu_dynamic_array_multipop` and `mu_dynamic_array_pop` are used to pop items off the top of a given dynamic array, defined below:

```
MUDEF muDynamicArray mu_dynamic_array_multipop(mumaResult* result, muDynamicArray da, size_m count);

MUDEF muDynamicArray mu_dynamic_array_pop(mumaResult* result, muDynamicArray da);
```

## Find
The functions `mu_dynamic_array_find` and `mu_dynamic_array_find_add` are used to find the first occurance of a given element in a given dynamic array, defined below:

```
MUDEF size_m mu_dynamic_array_find(mumaResult* result, muDynamicArray da, void* find);

MUDEF size_m mu_dynamic_array_find_add(mumaResult* result, muDynamicArray da, void* find);
```

`mu_dynamic_array_find` attempts to find an instance of `find` in the given dynamic array `da`. If none could be found, `MU_NONE` will be returned and `result` will be set to `MUMA_NOT_FOUND`.

`mu_dynamic_array_find_add` does the same thing as `mu_dynamic_array_find`, but if it fails to find one, it will attempt to push an instance of `find` on top of the dynamic array and return that one.

# Structs

## `muDynamicArray`
The struct `muDynamicArray` is used to hold information about a dynamic array across functions, defined below:

```
struct muDynamicArray {
	void* data;
	size_m type_size;
	size_m length;
	size_m allocated_length;
};
typedef struct muDynamicArray muDynamicArray;
```

Note that the members of this struct are not meant to be manually modified by the user, only read from, and if they are modified, functions will most likely not work properly with it.

# Enums

## `mumaResult`
The enum `mumaResult` is used to represent a functions' success in executing, defined below:

```
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
```

# Macros

## Secure warnings
muma automatically defines `_CRT_SECURE_NO_WARNINGS` to avoid errors with calling non-safe versions of functions. To disable this functionality, define `MU_SECURE_WARNINGS` before any inclusion of the file.

## Version
muma automatically defines `MUMA_VERSION_MAJOR`, `MUMA_VERSION_MINOR`, and `MUMA_VERSION_PATCH`. These versions match the releases on the GitHub page (formatting major.minor.patch) and can be used to check if the correct version of muma is running.

## Static/Extern defines
Defining `MU_STATIC` before including `muMemoryAllocator.h` will set all the definitions of the functions to `static`. Otherwise, it defaults to `extern`. Additionally, you can define `MUDEF` directly to have full control over its definition.

## Null pointer
muma defines `MU_NULL_PTR` as a handle for a null pointer. Really, it's just `0`.

## None
muma defines `MU_NONE` as a representation for nothing, being equivalent to MU_SIZE_MAX.

## Short indexing
Using `mu_dynamic_array_get` and `mu_dynamic_array_set` to get and set values from a dynamic array respectively can be annoying to type. So, the macro function `muda_i` can be used to quickly get an actual index position in a dynamic array, defined below:

```
#define muda_i(arr, type, index)
```

`arr` is a valid `muDynamicArray` struct, `type` is the type you would like to read the `muDynamicArray` struct data as an array of, and `index` is the index you would like to access. Note that this macro is *not* safe, and can very easily be used to reach an invalid index and cause the program to crash.

Note that since this returns an actual index position, this can be used to both get and set values.

## Names
If the macro `MUMA_NAMES` is defined below inclusion of the header file, the function `muma_result_get_name` will be defined.

## C standard library dependencies
muma relies on several C standard library functions, types, and defines. The list of types can be found below:

`size_m`: equivalent to `size_t`, uses `stddef.h` for definition.

`mu_malloc`: equivalent to `malloc`, uses `stdlib.h` for definition.

`mu_free`: equivalent to `free`, uses `stdlib.h` for definition.

`mu_realloc`: equivalent to `realloc`, uses `stdlib.h` for definition.

`mu_memset`: equivalent to `memset`, uses `string.h` for definition.

`mu_memcpy`: equivalent to `memcpy`, uses `string.h` for definition.

`MU_SIZE_MAX`: equivalent to `SIZE_MAX`, uses `stdint.h` for definition.

Note that all of these macros can be overwritten by defining them before including `muMemoryAllocator.h`.

Note that overwriting all macros relating to a C standard library header file prevents it from being automatically included.
