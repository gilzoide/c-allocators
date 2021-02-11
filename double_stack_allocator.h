/**
 * double_stack_allocator.h -- Double Stack Allocator (also called Bump Allocator) implementation
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

#ifndef DSA_DEF
    #ifdef DSA_STATIC
        #define DSA_DEF static
    #else
        #define DSA_DEF extern
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Custom memory manager: a double stack allocator.
 *
 * The bottom marker is increased, so pushed memory blocks have increasing addresses.
 * The top marker is decreased, so pushed memory blocks have decreasing addresses.
 *
 */
typedef struct dsa_double_stack_allocator {
    void *buffer;  ///< Memory buffer used
    size_t capacity;  ///< Capacity of memory buffer
    size_t bottom;  ///< Bottok mark, moved when allocating from the bottom
    size_t top;  ///< Top mark, moved when allocating from the top
} dsa_double_stack_allocator;

/// Helper macro to construct Double Stack Allocators from already allocated buffer
#define DSA_NEW(buffer, capacity) \
    ((dsa_double_stack_allocator){ buffer, capacity, 0, capacity })

/**
 * Create a new Double Stack Allocator from buffer and capacity.
 *
 * @param buffer    Allocated buffer
 * @param capacity  Size in bytes of the buffer
 *
 * @return Double Stack Allocator
 */
DSA_DEF dsa_double_stack_allocator dsa_new(void *buffer, size_t capacity);
/**
 * Create a new Double Stack Allocator from capacity.
 *
 * Uses DSA_MALLOC to allocate the buffer.
 *
 * @param capacity  Size in bytes of the buffer
 *
 * @return Double Stack Allocator
 */
DSA_DEF dsa_double_stack_allocator dsa_new_with_capacity(size_t capacity);

/**
 * Initializes a Double Stack Allocator with a memory size.
 *
 * Upon failure, allocator will have a capacity of 0.
 *
 * @param memory    The Double Stack Allocator.
 * @param capacity  Size in bytes of the memory block to be allocated.
 *
 * @return Non-zero if memory was allocated successfully.
 * @return 0 otherwise.
 */
DSA_DEF int dsa_init_with_capacity(dsa_double_stack_allocator *memory, size_t capacity);
/**
 * Release the memory associated with a Double Stack Allocator.
 *
 * This also zeroes out all fields in Allocator.
 *
 * It is safe to call this on a zero-initialized or previously released
 * allocator.
 *
 * @param memory The Double Stack Allocator.
 */
DSA_DEF void dsa_release(dsa_double_stack_allocator *memory);

/**
 * Allocates a sized chunk of memory from top of Double Stack Allocator.
 *
 * @param memory The Double Stack Allocator.
 * @param size   Size in bytes of the allocation.
 *
 * @return Allocated block memory on success.
 * @return NULL if not enought memory is available.
 */
DSA_DEF void *dsa_alloc_top(dsa_double_stack_allocator *memory, size_t size);
/**
 * Allocates a sized chunk of memory from bottom of Double Stack Allocator.
 *
 * @param memory The Double Stack Allocator.
 * @param size   Size in bytes of the allocation.
 *
 * @return Allocated block memory on success.
 * @return NULL if not enought memory is available.
 */
DSA_DEF void *dsa_alloc_bottom(dsa_double_stack_allocator *memory, size_t size);

/**
 * Free all used memory from top of Double Stack Allocator, making it available
 * for allocation once more.
 *
 * After calling this, all top markers previously got become invalid.
 *
 * To actually reclaim the used memory for the OS, use #dsa_release instead.
 *
 * @param memory The Double Stack Allocator.
 */
DSA_DEF void dsa_clear_top(dsa_double_stack_allocator *memory);
/**
 * Free all used memory from bottom of Double Stack Allocator, making it
 * available for allocation once more.
 *
 * After calling this, all bottom markers previously got become invalid.
 *
 * To actually reclaim the used memory for the OS, use #dsa_release instead.
 *
 * @param memory The Double Stack Allocator.
 */
DSA_DEF void dsa_clear_bottom(dsa_double_stack_allocator *memory);

/**
 * Get a marker for the current top allocation state.
 *
 * The result can be used for freeing a Double Stack Allocator back to this state.
 *
 * @param memory The Double Stack Allocator.
 *
 * @return Marker for the current allocation state.
 */
DSA_DEF size_t dsa_get_top_marker(dsa_double_stack_allocator *memory);
/**
 * Get a marker for the current bottom allocation state.
 *
 * The result can be used for freeing a Double Stack Allocator back to this state.
 *
 * @param memory The Double Stack Allocator.
 *
 * @return Marker for the current allocation state.
 */
DSA_DEF size_t dsa_get_bottom_marker(dsa_double_stack_allocator *memory);

/**
 * Free the used memory from Double Stack Allocator top up until `marker`,
 * making it available for allocation once more.
 *
 * Memory is only freed if `marker` points to allocated memory, so invalid
 * markers are ignored.
 *
 * After calling this, markers lesser than `marker` become invalid.
 *
 * To actually reclaim the used memory for the OS, use #dsa_release instead.
 *
 * @param memory The Double Stack Allocator.
 * @param marker A marker to a top allocation state.
 */
DSA_DEF void dsa_clear_top_marker(dsa_double_stack_allocator *memory, size_t marker);
/**
 * Free the used memory from Double Stack Allocator bottom up until `marker`,
 * making it available for allocation once more.
 *
 * Memory is only freed if `marker` points to allocated memory, so invalid
 * markers are ignored.
 *
 * After calling this, markers greater than `marker` become invalid.
 *
 * To actually reclaim the used memory for the OS, use #dsa_release instead.
 *
 * @param memory The Double Stack Allocator.
 * @param marker A marker to a bottom allocation state.
 */
DSA_DEF void dsa_clear_bottom_marker(dsa_double_stack_allocator *memory, size_t marker);

/**
 * Retrieve a pointer to the last `size` bytes allocated from top.
 *
 * @param memory The Double Stack Allocator.
 * @param size   Number of bytes to peek.
 *
 * @return Pointer to the allocated memory, if at least `size` bytes are
 *         allocated from top.
 * @return NULL otherwise.
 */
DSA_DEF void *dsa_peek_top(dsa_double_stack_allocator *memory, size_t size);
/**
 * Retrieve a pointer to the last `size` bytes allocated from bottom.
 *
 * @param memory The Double Stack Allocator.
 * @param size   Number of bytes to peek.
 *
 * @return Pointer to the allocated memory, if at least `size` bytes are
 *         allocated from bottom.
 * @return NULL otherwise.
 */
DSA_DEF void *dsa_peek_bottom(dsa_double_stack_allocator *memory, size_t size);

/**
 * Get the quantity of free memory available in a Double Stack Allocator.
 *
 * @param memory The Double Stack Allocator.
 *
 * @return Size in bytes of the free memory block in allocator.
 */
DSA_DEF size_t dsa_available_memory(dsa_double_stack_allocator *memory);
/**
 * Get the quantity of used memory in a Double Stack Allocator.
 *
 * @param memory The Double Stack Allocator.
 *
 * @return Size in bytes of the memory block used in allocator.
 */
DSA_DEF size_t dsa_used_memory(dsa_double_stack_allocator *memory);

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

DSA_DEF dsa_double_stack_allocator dsa_new(void *buffer, size_t capacity) {
    return DSA_NEW(buffer, capacity);
}

DSA_DEF dsa_double_stack_allocator dsa_new_with_capacity(size_t capacity) {
    dsa_double_stack_allocator double_stack_allocator;
    dsa_init_with_capacity(&double_stack_allocator, capacity);
    return double_stack_allocator;
}

DSA_DEF int dsa_init_with_capacity(dsa_double_stack_allocator *memory, size_t capacity) {
    memory->buffer = DSA_MALLOC(capacity);
    int malloc_success = memory->buffer != NULL;
    capacity = malloc_success * capacity;
    memory->capacity = capacity;
    memory->top = capacity;
    memory->bottom = 0;
    return malloc_success;
}

DSA_DEF void dsa_release(dsa_double_stack_allocator *memory) {
    DSA_FREE(memory->buffer);
    *memory = (dsa_double_stack_allocator){};
}

DSA_DEF void *dsa_alloc_top(dsa_double_stack_allocator *memory, size_t size) {
    if(memory->top < memory->bottom + size) return NULL;
    memory->top -= size;
    void *ptr = ((uint8_t *) memory->buffer) + memory->top;
    return ptr;
}

DSA_DEF void *dsa_alloc_bottom(dsa_double_stack_allocator *memory, size_t size) {
    if(memory->bottom + size > memory->top) return NULL;
    void *ptr = ((uint8_t *) memory->buffer) + memory->bottom;
    memory->bottom += size;
    return ptr;
}

DSA_DEF void dsa_clear_top(dsa_double_stack_allocator *memory) {
    memory->top = memory->capacity;
}

DSA_DEF void dsa_clear_bottom(dsa_double_stack_allocator *memory) {
    memory->bottom = 0;
}

DSA_DEF size_t dsa_get_top_marker(dsa_double_stack_allocator *memory) {
    return memory->top;
}

DSA_DEF size_t dsa_get_bottom_marker(dsa_double_stack_allocator *memory) {
    return memory->bottom;
}

DSA_DEF void dsa_clear_top_marker(dsa_double_stack_allocator *memory, size_t marker) {
    if(marker > memory->top && marker <= memory->capacity) {
        memory->top = marker;
    }
}

DSA_DEF void dsa_clear_bottom_marker(dsa_double_stack_allocator *memory, size_t marker) {
    if(marker < memory->bottom) {
        memory->bottom = marker;
    }
}

DSA_DEF void *dsa_peek_top(dsa_double_stack_allocator *memory, size_t size) {
    if(memory->capacity - memory->top < size) return NULL;
    return ((uint8_t *) memory->buffer) + memory->top;
}

DSA_DEF void *dsa_peek_bottom(dsa_double_stack_allocator *memory, size_t size) {
    if(memory->bottom < size) return NULL;
    return ((uint8_t *) memory->buffer) + memory->bottom - size;
}

DSA_DEF size_t dsa_available_memory(dsa_double_stack_allocator *memory) {
    return memory->top - memory->bottom;
}

DSA_DEF size_t dsa_used_memory(dsa_double_stack_allocator *memory) {
    return memory->bottom + (memory->capacity - memory->top);
}

#endif

