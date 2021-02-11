/**
 * double_stack_allocator.h -- Double Ended Stack (Bump) Allocator implementation
 *
 * Project URL: https://github.com/gilzoide/c-allocators
 *
 * Do this:
 *    #define DOUBLE_STACK_ALLOCATOR_IMPLEMENTATION
 * before you include this file in *one* C or C++ file to create the implementation.
 *
 * i.e.:
 *   #include ...
 *   #include ...
 *   #define DOUBLE_STACK_ALLOCATOR_IMPLEMENTATION
 *   #include "double_stack_allocator.h"
 *
 * Optionally provide the following defines with your own implementations:
 *
 * DSA_MALLOC(size)  - your own malloc function (default: malloc(size))
 * DSA_FREE(p)       - your own free function (default: free(p))
 * DSA_STATIC        - if defined and DSA_DECL is not defined, functions will be declared `static` instead of `extern`
 * DSA_DECL          - function declaration prefix (default: `extern` or `static` depending on DSA_STATIC)
 */
#ifndef DOUBLE_STACK_ALLOCATOR_H
#define DOUBLE_STACK_ALLOCATOR_H

#include <stdlib.h>

#ifndef DSA_DECL
    #ifdef DSA_STATIC
        #define DSA_DECL static
    #else
        #define DSA_DECL extern
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// Custom memory manager: a Double Stack Allocator.
/// 
/// Memory blocks pushed from bottom have increasing addresses.
/// Memory blocks pushed from top have decreasing addresses.
/// In particular, when allocating elements of a single type,
/// this becomes a double ended stack implementation.
typedef struct dsa_double_stack_allocator {
    void *buffer;     ///< Memory buffer used
    size_t capacity;  ///< Capacity of memory buffer
    size_t bottom;    ///< Bottom mark, moved when allocating from the bottom
    size_t top;       ///< Top mark, moved when allocating from the top
} dsa_double_stack_allocator;

/// Helper macro to construct Double Stack Allocators from already allocated buffer
#define DSA_NEW(buffer, capacity) \
    ((dsa_double_stack_allocator){ (buffer), (capacity), 0, (capacity) })
/// Typed version of DSA_NEW
#define DSA_NEW_(buffer, type, capacity) \
    DSA_NEW((buffer), sizeof(type) * (capacity))

/// Helper macro for iterating a Double Stack Allocator from bottom,
/// assuming all elements are of the same type.
///
/// Elements will be traversed in insertion order.
/// `identifier` will be a pointer for `type` elements.
#define DSA_FOREACH_BOTTOM(type, identifier, memory) \
    for(type *identifier = (type *) (memory)->buffer; identifier <= dsa_peek_bottom_((memory), type); identifier++)

/// Helper macro for iterating a Double Stack Allocator in reverse from bottom,
/// assuming all elements are of the same type.
///
/// Elements will be traversed in reverse of insertion order.
/// `identifier` will be a pointer for `type` elements.
#define DSA_FOREACH_BOTTOM_REVERSE(type, identifier, memory) \
    for(type *identifier = dsa_peek_bottom_((memory), type); identifier >= (type *) (memory)->buffer; identifier--)

/// Helper macro for iterating a Double Stack Allocator from top,
/// assuming all elements are of the same type.
///
/// Elements will be traversed in insertion order.
/// `identifier` will be a pointer for `type` elements.
#define DSA_FOREACH_TOP(type, identifier, memory) \
    for(type *identifier = (type *) ((memory)->buffer + (memory)->capacity - sizeof(type)); identifier >= dsa_peek_top_((memory), type); identifier--)

/// Helper macro for iterating a Double Stack Allocator in reverse from top,
/// assuming all elements are of the same type.
///
/// Elements will be traversed in reverse of insertion order.
/// `identifier` will be a pointer for `type` elements.
#define DSA_FOREACH_TOP_REVERSE(type, identifier, memory) \
    for(type *identifier = dsa_peek_top_((memory), type); identifier <= (type *) ((memory)->buffer + (memory)->capacity - sizeof(type)); identifier++)

/// Create a new Double Stack Allocator from buffer and capacity.
DSA_DECL dsa_double_stack_allocator dsa_new(void *buffer, size_t capacity);
/// Typed version of dsa_new
#define dsa_new_(buffer, type, capacity) \
    dsa_new((buffer), sizeof(type) * (capacity))

/// Create a new Double Stack Allocator from capacity.
/// 
/// Uses DSA_MALLOC to allocate the buffer.
DSA_DECL dsa_double_stack_allocator dsa_new_with_capacity(size_t capacity);
/// Typed version of dsa_new_with_capacity
#define dsa_new_with_capacity_(type, capacity) \
    dsa_new_with_capacity(sizeof(type) * (capacity))

/// Initializes a Double Stack Allocator with a memory size.
/// 
/// Upon failure, allocator will have a capacity of 0.
/// 
/// @return Non-zero if memory was allocated successfully.
/// @return 0 otherwise.
DSA_DECL int dsa_init_with_capacity(dsa_double_stack_allocator *memory, size_t capacity);
/// Typed version of dsa_init_with_capacity
#define dsa_init_with_capacity_(memory, type, capacity) \
    dsa_init_with_capacity((memory), sizeof(type) * (capacity))

/// Release the memory associated with a Double Stack Allocator with DSA_FREE.
/// 
/// This also zeroes out all fields in Allocator.
/// 
/// With the default DSA_FREE definition, it is safe to call this on a
/// zero-initialized or previously released allocator.
DSA_DECL void dsa_release(dsa_double_stack_allocator *memory);

/// Allocates a sized chunk of memory from top of Double Stack Allocator.
/// 
/// @return Allocated block memory on success.
/// @return NULL if not enought memory is available.
DSA_DECL void *dsa_alloc_top(dsa_double_stack_allocator *memory, size_t size);
/// Typed version of dsa_alloc_top
#define dsa_alloc_top_(memory, type) \
    ((type *) dsa_alloc_top((memory), sizeof(type)))

/// Allocates a sized chunk of memory from bottom of Double Stack Allocator.
/// 
/// @return Allocated block memory on success.
/// @return NULL if not enought memory is available.
DSA_DECL void *dsa_alloc_bottom(dsa_double_stack_allocator *memory, size_t size);
/// Typed version of dsa_alloc_bottom
#define dsa_alloc_bottom_(memory, type) \
    ((type *) dsa_alloc_bottom((memory), sizeof(type)))

/// Free all used memory from top of Double Stack Allocator, making it available
/// for allocation once more.
/// 
/// After calling this, all top markers previously got become invalid.
/// 
/// To actually reclaim the used memory for the OS, use #dsa_release instead.
DSA_DECL void dsa_clear_top(dsa_double_stack_allocator *memory);

/// Free all used memory from bottom of Double Stack Allocator, making it
/// available for allocation once more.
/// 
/// After calling this, all bottom markers previously got become invalid.
/// 
/// To actually reclaim the used memory for the OS, use #dsa_release instead.
DSA_DECL void dsa_clear_bottom(dsa_double_stack_allocator *memory);

/// Get a marker for the current top allocation state.
/// 
/// The result can be used for freeing a Double Stack Allocator back to this state.
DSA_DECL size_t dsa_get_top_marker(dsa_double_stack_allocator *memory);

/// Get a marker for the current bottom allocation state.
/// 
/// The result can be used for freeing a Double Stack Allocator back to this state.
DSA_DECL size_t dsa_get_bottom_marker(dsa_double_stack_allocator *memory);

/// Free the used memory from Double Stack Allocator top up until `marker`,
/// making it available for allocation once more.
/// 
/// Memory is only freed if `marker` points to allocated memory, so invalid
/// markers are ignored.
/// 
/// After calling this, markers lesser than `marker` become invalid.
/// 
/// To actually reclaim the used memory for the OS, use #dsa_release instead.
DSA_DECL void dsa_clear_top_marker(dsa_double_stack_allocator *memory, size_t marker);

/// Free the used memory from Double Stack Allocator bottom up until `marker`,
/// making it available for allocation once more.
/// 
/// Memory is only freed if `marker` points to allocated memory, so invalid
/// markers are ignored.
/// 
/// After calling this, markers greater than `marker` become invalid.
/// 
/// To actually reclaim the used memory for the OS, use #dsa_release instead.
DSA_DECL void dsa_clear_bottom_marker(dsa_double_stack_allocator *memory, size_t marker);

/// Retrieve a pointer to the last `size` bytes allocated from top.
/// 
/// @return Pointer to the allocated memory, if at least `size` bytes are
///         allocated from top.
/// @return NULL otherwise.
DSA_DECL void *dsa_peek_top(dsa_double_stack_allocator *memory, size_t size);
/// Typed version of dsa_peek_top
#define dsa_peek_top_(memory, type) \
    ((type *) dsa_peek_top((memory), sizeof(type)))

/// Retrieve a pointer to the last `size` bytes allocated from bottom.
/// 
/// @return Pointer to the allocated memory, if at least `size` bytes are
///         allocated from bottom.
/// @return NULL otherwise.
DSA_DECL void *dsa_peek_bottom(dsa_double_stack_allocator *memory, size_t size);
/// Typed version of dsa_peek_bottom
#define dsa_peek_bottom_(memory, type) \
    ((type *) dsa_peek_bottom((memory), sizeof(type)))

/// Get the quantity of free memory available in a Double Stack Allocator
DSA_DECL size_t dsa_available_memory(dsa_double_stack_allocator *memory);

/// Get the quantity of used memory in a Double Stack Allocator
DSA_DECL size_t dsa_used_memory(dsa_double_stack_allocator *memory);

#ifdef __cplusplus
}
#endif

#endif  // __DOUBLE_STACK_ALLOCATOR_H__

///////////////////////////////////////////////////////////////////////////////

#ifdef DOUBLE_STACK_ALLOCATOR_IMPLEMENTATION

#include <stdint.h>

#ifndef DSA_MALLOC
    #define DSA_MALLOC(size) malloc(size)
#endif
#ifndef DSA_FREE
    #define DSA_FREE(size) free(size)
#endif

DSA_DECL dsa_double_stack_allocator dsa_new(void *buffer, size_t capacity) {
    return DSA_NEW(buffer, capacity);
}

DSA_DECL dsa_double_stack_allocator dsa_new_with_capacity(size_t capacity) {
    dsa_double_stack_allocator double_stack_allocator;
    dsa_init_with_capacity(&double_stack_allocator, capacity);
    return double_stack_allocator;
}

DSA_DECL int dsa_init_with_capacity(dsa_double_stack_allocator *memory, size_t capacity) {
    memory->buffer = DSA_MALLOC(capacity);
    int malloc_success = memory->buffer != NULL;
    capacity = malloc_success * capacity;
    memory->capacity = capacity;
    memory->top = capacity;
    memory->bottom = 0;
    return malloc_success;
}

DSA_DECL void dsa_release(dsa_double_stack_allocator *memory) {
    DSA_FREE(memory->buffer);
    *memory = (dsa_double_stack_allocator){};
}

DSA_DECL void *dsa_alloc_top(dsa_double_stack_allocator *memory, size_t size) {
    if(memory->top < memory->bottom + size) return NULL;
    memory->top -= size;
    void *ptr = ((uint8_t *) memory->buffer) + memory->top;
    return ptr;
}

DSA_DECL void *dsa_alloc_bottom(dsa_double_stack_allocator *memory, size_t size) {
    if(memory->bottom + size > memory->top) return NULL;
    void *ptr = ((uint8_t *) memory->buffer) + memory->bottom;
    memory->bottom += size;
    return ptr;
}

DSA_DECL void dsa_clear_top(dsa_double_stack_allocator *memory) {
    memory->top = memory->capacity;
}

DSA_DECL void dsa_clear_bottom(dsa_double_stack_allocator *memory) {
    memory->bottom = 0;
}

DSA_DECL size_t dsa_get_top_marker(dsa_double_stack_allocator *memory) {
    return memory->top;
}

DSA_DECL size_t dsa_get_bottom_marker(dsa_double_stack_allocator *memory) {
    return memory->bottom;
}

DSA_DECL void dsa_clear_top_marker(dsa_double_stack_allocator *memory, size_t marker) {
    if(marker > memory->top && marker <= memory->capacity) {
        memory->top = marker;
    }
}

DSA_DECL void dsa_clear_bottom_marker(dsa_double_stack_allocator *memory, size_t marker) {
    if(marker < memory->bottom) {
        memory->bottom = marker;
    }
}

DSA_DECL void *dsa_peek_top(dsa_double_stack_allocator *memory, size_t size) {
    if(memory->capacity - memory->top < size) return NULL;
    return ((uint8_t *) memory->buffer) + memory->top;
}

DSA_DECL void *dsa_peek_bottom(dsa_double_stack_allocator *memory, size_t size) {
    if(memory->bottom < size) return NULL;
    return ((uint8_t *) memory->buffer) + memory->bottom - size;
}

DSA_DECL size_t dsa_available_memory(dsa_double_stack_allocator *memory) {
    return memory->top - memory->bottom;
}

DSA_DECL size_t dsa_used_memory(dsa_double_stack_allocator *memory) {
    return memory->bottom + (memory->capacity - memory->top);
}

#endif  // DOUBLE_STACK_ALLOCATOR_IMPLEMENTATION
