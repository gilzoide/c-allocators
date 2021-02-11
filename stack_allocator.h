#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include <stdlib.h>

#ifndef SA_DEF
    #ifdef SA_STATIC
        #define SA_DEF static
    #else
        #define SA_DEF extern
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A static stack allocator.
 *
 * The bottom marker is increased, so pushed memory blocks have increasing addresses.
 */
typedef struct sa_stack_allocator {
    void *buffer;  ///< Memory buffer used.
    size_t capacity;  ///< Capacity of memory buffer.
    size_t marker;  ///< Top mark. Points to the next available memory block.
} sa_stack_allocator;

/// Helper macro to construct Stack Allocators from already allocated buffer
#define SA_NEW(buffer, capacity) \
    ((sa_stack_allocator){ buffer, capacity, 0 })


/**
 * Create a new Stack Allocator from buffer and capacity.
 *
 * @param buffer    Allocated buffer
 * @param capacity  Size in bytes of the buffer
 *
 * @return Stack Allocator
 */
SA_DEF sa_stack_allocator sa_new(void *buffer, size_t capacity);
/**
 * Create a new Stack Allocator from capacity.
 *
 * Uses SA_MALLOC to allocate the buffer.
 *
 * @param capacity  Size in bytes of the buffer
 *
 * @return Stack Allocator
 */
SA_DEF sa_stack_allocator sa_new_with_capacity(size_t capacity);

/**
 * Initializes a Stack Allocator with a memory size.
 *
 * Upon failure, allocator will have a capacity of 0.
 *
 * @param memory  The Stack Allocator.
 * @param size    Size in bytes of the memory block to be allocated.
 *
 * @return Non-zero if memory was allocated successfully.
 * @return 0 otherwise.
 */
SA_DEF int sa_init_with_capacity(sa_stack_allocator *memory, size_t size);
/**
 * Release the memory associated with a Stack Allocator with SA_FREE.
 *
 * This also zeroes out all fields in Allocator.
 *
 * With the default SA_FREE definition, it is safe to call this on a
 * zero-initialized or previously released allocator.
 *
 * @param memory  The Stack Allocator.
 */
SA_DEF void sa_release(sa_stack_allocator *memory);

/**
 * Allocates a sized chunk of memory from Stack Allocator.
 *
 * @param memory  The Stack Allocator.
 * @param size    Size in bytes of the allocation.
 *
 * @return Allocated block memory on success.
 * @return NULL if not enought memory is available.
 */
SA_DEF void *sa_alloc(sa_stack_allocator *memory, size_t size);
/**
 * Free all used memory from Stack Allocator, making it available for 
 * allocation once more.
 *
 * After calling this, all markers previously got become invalid.
 *
 * To actually reclaim the used memory for the OS, use #sa_release instead.
 *
 * @param memory  The Stack Allocator.
 */
SA_DEF void sa_clear(sa_stack_allocator *memory);

/**
 * Get a marker for the current allocation state.
 *
 * The result can be used for freeing a Stack Allocator back to this state.
 *
 * @param memory  The Stack Allocator.
 *
 * @return Marker for the current allocation state.
 */
SA_DEF size_t sa_get_marker(sa_stack_allocator *memory);
/**
 * Free the used memory from Stack Allocator up until `marker`, making it
 * available for allocation once more.
 *
 * Memory is only freed if `marker` points to allocated memory, so invalid
 * markers are ignored.
 *
 * After calling this, markers greater than `marker` become invalid.
 *
 * To actually reclaim the used memory for the OS, use #sa_release instead.
 *
 * @param memory  The Stack Allocator.
 * @param marker  A marker to an allocation state.
 */
SA_DEF void sa_clear_marker(sa_stack_allocator *memory, size_t marker);

/**
 * Retrieve a pointer to the top `size` bytes allocated.
 *
 * @param memory  The Stack Allocator.
 * @param size    Number of bytes to peek.
 *
 * @return Pointer to the allocated memory, if at least `size` bytes are allocated.
 * @return NULL otherwise.
 */
SA_DEF void *sa_peek(sa_stack_allocator *memory, size_t size);

/**
 * Get the quantity of free memory available in a Stack Allocator.
 *
 * @param memory  The Stack Allocator.
 *
 * @return Size in bytes of the free memory block in allocator.
 */
SA_DEF size_t sa_available_memory(sa_stack_allocator *memory);
/**
 * Get the quantity of used memory in a Stack Allocator.
 *
 * @param memory  The Stack Allocator.
 *
 * @return Size in bytes of the memory block used in allocator.
 */
SA_DEF size_t sa_used_memory(sa_stack_allocator *memory);

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

SA_DEF sa_stack_allocator sa_new(void *buffer, size_t capacity) {
    return SA_NEW(buffer, capacity);
}

SA_DEF sa_stack_allocator sa_new_with_capacity(size_t capacity) {
    sa_stack_allocator stack_allocator;
    sa_init_with_capacity(&stack_allocator, capacity);
    return stack_allocator;
}

SA_DEF int sa_init_with_capacity(sa_stack_allocator *memory, size_t capacity) {
    memory->buffer = SA_MALLOC(capacity);
    int malloc_success = memory->buffer != NULL;
    memory->capacity = malloc_success * capacity;
    memory->marker = 0;
    return malloc_success;
}

SA_DEF void sa_release(sa_stack_allocator *memory) {
    SA_FREE(memory->buffer);
    *memory = (sa_stack_allocator){};
}

SA_DEF void *sa_alloc(sa_stack_allocator *memory, size_t size) {
    if(memory->marker + size > memory->capacity) return NULL;
    void *ptr = ((uint8_t *) memory->buffer) + memory->marker;
    memory->marker += size;
    return ptr;
}

SA_DEF void sa_clear(sa_stack_allocator *memory) {
    memory->marker = 0;
}

SA_DEF size_t sa_get_marker(sa_stack_allocator *memory) {
    return memory->marker;
}

SA_DEF void sa_clear_marker(sa_stack_allocator *memory, size_t marker) {
    if(marker < memory->marker) {
        memory->marker = marker;
    }
}

SA_DEF void *sa_peek(sa_stack_allocator *memory, size_t size) {
    if(memory->marker < size) return NULL;
    return ((uint8_t *) memory->buffer) + memory->marker - size;
}

SA_DEF size_t sa_available_memory(sa_stack_allocator *memory) {
    return memory->capacity - memory->marker;
}

SA_DEF size_t sa_used_memory(sa_stack_allocator *memory) {
    return memory->marker;
}

#endif
