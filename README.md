# c-allocators
[![Build Status](https://travis-ci.org/gilzoide/c-allocators.svg?branch=master)](https://travis-ci.org/gilzoide/c-allocators)

A collection of public domain single-file custom allocators for C/C++.


## Using
Copy the desired source file to your project, include it wherever necessary and
add a `#define <ALLOCATOR_NAME>_IMPLEMENTATION` before including it in exactly
**one** C/C++ file to create the implementation.


## [stack_allocator.h](stack_allocator.h)
A Stack (Bump) Allocator implementation, maintaining a memory buffer and the current allocation marker.

Allocation markers can be used to free allocator up to some point, instead of clearing the entire stack.
In particular, when allocating elements of a single type, this becomes a generic stack implementation.

There are typed macros for functions that expect `size` values, to help in readability.
There are also `FOREACH` macros for iterating stack in allocation order or reversed allocation order,
assuming that all values are of the same type.


## [double_stack_allocator.h](double_stack_allocator.h)
A Double Ended Stack (Bump) Allocator implementation, maintaining a memory buffer and current allocation
markers for both ends.

Allocation markers can be used to free allocator up to some point, instead of clearing the entire stack
on either end.
In particular, when allocating elements of a single type, this becomes a generic double ended stack
implementation, possibly with different types for each side.

There are typed macros for functions that expect `size` values, to help in readability.
There are also `FOREACH` macros for iterating stack in allocation order or reversed allocation order,
assuming that all values are of the same type.

