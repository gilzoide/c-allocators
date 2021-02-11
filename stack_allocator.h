/**
 * stack_allocator.h -- Stack (Bump) Allocator implementation
 *
 * Project URL: https://github.com/gilzoide/c-allocators
 *
 * Do this:
 *    #define STACK_ALLOCATOR_IMPLEMENTATION
 * before you include this file in *one* C or C++ file to create the implementation.
 *
 * i.e.:
 *   #include ...
 *   #include ...
 *   #define STACK_ALLOCATOR_IMPLEMENTATION
 *   #include "stack_allocator.h"
 *
 * Optionally provide the following defines with your own implementations:
 *
 * SA_MALLOC(size)  - your own malloc function (default: malloc(size))
 * SA_FREE(p)       - your own free function (default: free(p))
 * SA_STATIC        - if defined and SA_DECL is not defined, functions will be declared `static` instead of `extern`
 * SA_DECL          - function declaration prefix (default: `extern` or `static` depending on SA_STATIC)
 */

#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include <stdlib.h>

#ifndef SA_DECL
    #ifdef SA_STATIC
        #define SA_DECL static
    #else
        #define SA_DECL extern
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// A static stack allocator.
/// 
/// Memory blocks pushed have increasing addresses.
/// In particular, when allocating elements of a single type,
/// this becomes a stack implementation.
typedef struct sa_stack_allocator {
    void *buffer;     ///< Memory buffer used.
    size_t capacity;  ///< Capacity of memory buffer.
    size_t marker;    ///< Marker that points to the next available memory block.
} sa_stack_allocator;

/// Helper macro to construct Stack Allocators from already allocated buffer
#define SA_NEW(buffer, capacity) \
    ((sa_stack_allocator){ (buffer), (capacity), 0 })
/// Typed version of SA_NEW
#define SA_NEW_(buffer, type, capacity) \
    SA_NEW((buffer), sizeof(type) * (capacity))

/// Helper macro for iterating a Stack Allocator, assuming all elements are
/// of the same type.
///
/// Elements will be traversed in insertion order.
/// `identifier` will be a pointer for `type` elements.
#define SA_FOREACH(type, identifier, memory) \
    for(type *identifier = (type *) (memory)->buffer; identifier <= sa_peek_((memory), type); identifier++)

/// Helper macro for iterating a Stack Allocator in reverse, assuming all
/// elements are of the same type.
///
/// Elements will be traversed in reverse of insertion order.
/// `identifier` will be a pointer for `type` elements.
#define SA_FOREACH_REVERSE(type, identifier, memory) \
    for(type *identifier = sa_peek_((memory), type); identifier >= (type *) (memory)->buffer; identifier--)

/// Create a new Stack Allocator from buffer and capacity.
/// 
/// @param buffer    Allocated buffer
/// @param capacity  Size in bytes of the buffer
/// 
/// @return Stack Allocator
SA_DECL sa_stack_allocator sa_new(void *buffer, size_t capacity);
/// Typed version of sa_new
#define sa_new_(buffer, type, capacity) \
    sa_new((buffer), sizeof(type) * (capacity))

/// Create a new Stack Allocator from capacity.
/// 
/// Uses SA_MALLOC to allocate the buffer.
SA_DECL sa_stack_allocator sa_new_with_capacity(size_t capacity);
/// Typed version of sa_new_with_capacity
#define sa_new_with_capacity_(type, capacity) \
    sa_new_with_capacity(sizeof(type) * (capacity))

/// Initializes a Stack Allocator with a memory size.
/// 
/// Upon failure, allocator will have a capacity of 0.
/// 
/// @return Non-zero if memory was allocated successfully.
/// @return 0 otherwise.
SA_DECL int sa_init_with_capacity(sa_stack_allocator *memory, size_t capacity);
/// Typed version of sa_init_with_capacity
#define sa_init_with_capacity_(memory, type, capacity) \
    sa_init_with_capacity((memory), sizeof(type) * (capacity))

/// Release the memory associated with a Stack Allocator with SA_FREE.
/// 
/// This also zeroes out all fields in Allocator.
/// 
/// With the default SA_FREE definition, it is safe to call this on a
/// zero-initialized or previously released allocator.
SA_DECL void sa_release(sa_stack_allocator *memory);

/// Allocates a sized chunk of memory from Stack Allocator.
/// 
/// @return Allocated block memory on success.
/// @return NULL if not enought memory is available.
SA_DECL void *sa_alloc(sa_stack_allocator *memory, size_t size);
/// Typed version of sa_alloc
#define sa_alloc_(memory, type) \
    ((type *) sa_alloc((memory), sizeof(type)))

/// Free all used memory from Stack Allocator, making it available for 
/// allocation once more.
/// 
/// After calling this, all markers previously got become invalid.
/// 
/// To actually reclaim the used memory for the OS, use #sa_release instead.
SA_DECL void sa_clear(sa_stack_allocator *memory);

/// Get a marker for the current allocation state.
/// 
/// The result can be used for freeing a Stack Allocator back to this state.
SA_DECL size_t sa_get_marker(sa_stack_allocator *memory);

/// Free the used memory from Stack Allocator up until `marker`, making it
/// available for allocation once more.
/// 
/// Memory is only freed if `marker` points to allocated memory, so invalid
/// markers are ignored.
/// 
/// After calling this, markers greater than `marker` become invalid.
/// 
/// To actually reclaim the used memory for the OS, use #sa_release instead.
SA_DECL void sa_clear_marker(sa_stack_allocator *memory, size_t marker);

/// Free the last `size` bytes from Stack Allocator.
///
/// It's safe to pop more bytes than there are allocated.
SA_DECL void sa_pop(sa_stack_allocator *memory, size_t size);
/// Typed version of sa_pop
#define sa_pop_(memory, type) \
    sa_pop((memory), sizeof(type))

/// Retrieve a pointer to the top `size` bytes allocated.
/// 
/// @return Pointer to the allocated memory, if at least `size` bytes are allocated.
/// @return NULL otherwise.
SA_DECL void *sa_peek(sa_stack_allocator *memory, size_t size);
/// Typed version of sa_peek
#define sa_peek_(memory, type) \
    ((type *) sa_peek((memory), sizeof(type)))

/// Get the quantity of free memory available in a Stack Allocator.
SA_DECL size_t sa_available_memory(sa_stack_allocator *memory);

/// Get the quantity of used memory in a Stack Allocator.
SA_DECL size_t sa_used_memory(sa_stack_allocator *memory);

#ifdef __cplusplus
}
#endif

#endif  // __STACK_ALLOCATOR_H__

///////////////////////////////////////////////////////////////////////////////

#ifdef STACK_ALLOCATOR_IMPLEMENTATION

#include <stdint.h>

#ifndef SA_MALLOC
    #define SA_MALLOC(size) malloc(size)
#endif
#ifndef SA_FREE
    #define SA_FREE(size) free(size)
#endif

SA_DECL sa_stack_allocator sa_new(void *buffer, size_t capacity) {
    return SA_NEW(buffer, capacity);
}

SA_DECL sa_stack_allocator sa_new_with_capacity(size_t capacity) {
    sa_stack_allocator stack_allocator;
    sa_init_with_capacity(&stack_allocator, capacity);
    return stack_allocator;
}

SA_DECL int sa_init_with_capacity(sa_stack_allocator *memory, size_t capacity) {
    memory->buffer = SA_MALLOC(capacity);
    int malloc_success = memory->buffer != NULL;
    memory->capacity = malloc_success * capacity;
    memory->marker = 0;
    return malloc_success;
}

SA_DECL void sa_release(sa_stack_allocator *memory) {
    SA_FREE(memory->buffer);
    *memory = (sa_stack_allocator){};
}

SA_DECL void *sa_alloc(sa_stack_allocator *memory, size_t size) {
    if(memory->marker + size > memory->capacity) return NULL;
    void *ptr = ((uint8_t *) memory->buffer) + memory->marker;
    memory->marker += size;
    return ptr;
}

SA_DECL void sa_clear(sa_stack_allocator *memory) {
    memory->marker = 0;
}

SA_DECL size_t sa_get_marker(sa_stack_allocator *memory) {
    return memory->marker;
}

SA_DECL void sa_clear_marker(sa_stack_allocator *memory, size_t marker) {
    if(marker < memory->marker) {
        memory->marker = marker;
    }
}

SA_DECL void sa_pop(sa_stack_allocator *memory, size_t size) {
    if(size > memory->marker) {
        memory->marker = 0;
    }
    else {
        memory->marker -= size;
    }
}

SA_DECL void *sa_peek(sa_stack_allocator *memory, size_t size) {
    if(memory->marker < size) return NULL;
    return ((uint8_t *) memory->buffer) + memory->marker - size;
}

SA_DECL size_t sa_available_memory(sa_stack_allocator *memory) {
    return memory->capacity - memory->marker;
}

SA_DECL size_t sa_used_memory(sa_stack_allocator *memory) {
    return memory->marker;
}

#endif  // STACK_ALLOCATOR_IMPLEMENTATION
