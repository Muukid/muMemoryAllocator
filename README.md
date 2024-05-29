
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


# C standard library dependencies

muma has several C standard library dependencies not provided by its other library dependencies, all of which are overridable by defining them before the inclusion of its header. The following is a list of all of those dependencies.

## `stdlib.h` dependencies

`mu_malloc`: equivalent to `malloc`

`mu_free`: equivalent to `free`

`mu_realloc`: equivalent to `realloc`

## `string.h` dependencies

`mu_memset`: equivalent to `memset`

`mu_memcpy`: equivalent to `memcpy`

`mu_memmove`: equivalent to `memmove`

# Enums

## Result enum

muma uses the result enumerator `mumaResult` to represent how a function went. It has these possible values:

`MUMA_SUCCESS`: the function performed successfully.

`MUMA_FAILED_TO_ALLOCATE`: the function could not allocate the memory necessary to perform the function.

`MUMA_INVALID_INDEX`: an invalid index into an array was requested, so the function could not be performed.

`MUMA_INVALID_SHIFT_AMOUNT`: the array shifting necessary to perform the function was invalid.

`MUMA_NOT_FOUND`: a requested element in an array could not be located.

# Macros

## Dynamic array
			
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

#### Creation 

The function `function_name_prefix##create` is used to create a dynamic array, defined below: 

```c 

struct_name function_name_prefix##create(mumaResult* result, size_m length) 

``` 

Note that `length` being 0 is valid, and nothing will be immediately allocated. 

#### Destruction 

The function `function_name_prefix##destroy` is used to destroy a dynamic array and fully free its contents, defined below: 

```c 

struct_name function_name_prefix##destroy(mumaResult* result, struct_name s) 

``` 

Note that this function is safe to call on an array with nothing allocated (of course, assuming that its members are valid). 

#### Resize 

The function `function_name_prefix##resize` is used to resize a dynamic array, defined below: 

```c 

struct_name function_name_prefix##resize(mumaResult* result, struct_name s, size_m length) 

``` 

If this function performs correctly, `s.length` will be equal to `length`. 

Note that `length` is allowed to be any value, including being above, below, or equal to the array's length / allocated length, as well as 0 (although, it cannot be guaranteed that this will free memory). 

Note that, in general, this function is not guaranteed to allocate or free any memory, even if the length is changing a significant amount. 

#### Left shift 

The function `function_name_prefix##lshift` is used to shift a dynamic array's contents left, defined below: 

```c 

struct_name function_name_prefix##lshift(mumaResult* result, struct_name s, size_m index, size_m amount) 

``` 

This function takes all of the elements of an array starting from `index` and ending at the end of the array, and shifts them left `amount` amount of times. 

If this function performs correctly, `s.length` will be equal to `s.length` minus `amount`. 

Note that this operation can overwrite contents of other elements within the array. 

#### Right shift 

The function `function_name_prefix##rshift` is used to shift a dynamic array's contents right, defined below: 

```c 

struct_name function_name_prefix##rshift(mumaResult* result, struct_name s, size_m index, size_m amount) 

``` 

This function takes all of the elements of an array starting from `index` and ending at the end of the array, and shifts them right `amount` amount of times. 

If this function performs correctly, `s.length` will be equal to `s.length` plus `amount`. 

Note that this operation cannot overwrite contents of other elements within the array. 

#### Multi-insert 

The function `function_name_prefix##multiinsert` is used to insert multiple elements into a dynamic array, defined below: 

```c 

struct_name function_name_prefix##multiinsert(mumaResult* result, struct_name s, size_m index, type* insert, size_m count) 

``` 

This function takes `insert` (which should have `count` amount of `type` elements) and inserts it at index `index`. 

If this function performs correctly, `s.length` will be equal to `s.length` plus `count`, and `s.data[index+n]` will be equal to `insert[0+n]` while `n` is less than `count` (ie, `s.data[index+0] == insert[0]`). 

#### Insert 

The function `function_name_prefix##insert` is used to insert a single element into a dynamic array, defined below: 

```c 

struct_name function_name_prefix##insert(mumaResult* result, struct_name s, size_m index, type insert) 

``` 

This function just returns `function_name_prefix##multiinsert` with the following parameters: 

```c 

function_name_prefix##multiinsert(result, s, index, &insert, 1) 

``` 

#### Multi-erase 

The function `function_name_prefix##multierase` is used to erase multiple elements from an array, defined below: 

```c 

struct_name function_name_prefix##multierase(mumaResult* result, struct_name s, size_m index, size_m count) 

``` 

This function just returns `function_name_prefix##lshift` with the following parameters: 

```c 

function_name_prefix##lshift(result, s, index+count, count) 

``` 

#### Erase 

The function `function_name_prefix##erase` is used to erase an element from an array, defined below: 

```c 

struct_name function_name_prefix##erase(mumaResult* result, struct_name s, size_m index) 

``` 

This function just returns `function_name_prefix##multierase` with the following parameters: 

```c 

function_name_prefix##multierase(result, s, index, 1) 

``` 

#### Clear 

The function `function_name_prefix##clear` is used to clear all the elements from an array. It is identical to `function_name_prefix##destroy`. 

#### Multi-push 

The function `function_name_prefix##multipush` is used to push multiple elements on top of an array, defined below: 

```c 

struct_name function_name_prefix##multipush(mumaResult* result, struct_name s, type* push, size_m count) 

``` 

#### Push 

The function `function_name_prefix##push` is used to push an element on top of an array, defined below: 

```c 

struct_name function_name_prefix##push(mumaResult* result, struct_name s, type push) 

``` 

This function just returns `function_name_prefix##multipush` with the following parameters: 

```c 

function_name_prefix##multipush(result, s, &push, 1) 

``` 

#### Multi-pop 

The function `function_name_prefix##multipop` is used to pop multiple elements from the top of an array, defined below: 

```c 

struct_name function_name_prefix##multipop(mumaResult* result, struct_name s, size_m count) 

``` 

#### Pop 

The function `function_name_prefix##pop` is used to pop an element from the top of an array, defined below: 

```c 

struct_name function_name_prefix##pop(mumaResult* result, struct_name s) 

``` 

This function just returns `function_name_prefix##multipop` with the following parameters: 

```c 

function_name_prefix##multipop(result, s, 1) 

``` 

#### Find 

The function `function_name_prefix##find` is used to find a certain element in an array, defined below: 

```c 

size_m function_name_prefix##find(mumaResult* result, struct_name s, type find) 

``` 

This function scans from index 0 up until the end of the array and uses `type_comparison_func` to decide if `find` equals the element at the given index. 

If a matching element is found, its index is returned. 

If no matching element is found, `MU_SIZE_MAX` is returned. 

#### Find-push 

The function `function_name_prefix##find_push` is used to find a certain element in an array and, if not found, push it onto the array, defined below: 

```c 

struct_name function_name_prefix##find_push(mumaResult* result, struct_name s, type find, size_m* p_index) 

``` 

This function tries to find `find` in the given array `s` using `function_name_prefix##find` and, if not found, pushes `find` onto the array. In either case, if successful and `p_index` isn't 0, `p_index` is dereferenced and set to the index that contains `find`. 


## Version

There are three major, minor, and patch macros respectively defined to represent the version of mum, defined as `MUMA_VERSION_MAJOR`, `MUMA_VERSION_MINOR`, and `MUMA_VERSION_PATCH`, following the formatting of `vMAJOR.MINOR.PATCH`.

# Functions

## Name function

The function `muma_result_get_name` is used to get a `const char*` representation of a `mumaResult` value, defined below: 

```c
MUDEF const char* muma_result_get_name(mumaResult result);
```


Note that this function is only defined if the macro `MUMA_NAMES` is defined.
