/*
 * Copyright (c) 2019 Gil Barbosa Reis
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef __STACK_ALLOCATOR_H__
#define __STACK_ALLOCATOR_H__

/**
 * A static stack allocator.
 *
 * The bottom marker is increased, so pushed memory blocks have increasing addresses.
 */
typedef struct stack_allocator {
	void *buffer;  ///< Memory buffer used.
	int capacity;  ///< Capacity of memory buffer.
	int marker;  ///< Top mark. Points to the next available memory block.
} stack_allocator;

/**
 * Initializes a Stack Allocator with a memory size.
 *
 * Upon failure, allocator will have a capacity of 0.
 *
 * @param memory The Stack Allocator.
 * @param size   Size in bytes of the memory block to be allocated.
 *
 * @return Non-zero if memory was allocated successfully.
 * @return 0 otherwise.
 */
int sa_init_with_size(stack_allocator *memory, unsigned int size);
/**
 * Release the memory associated with a Stack Allocator.
 *
 * This also zeroes out all fields in Allocator.
 *
 * It is safe to call this on a zero-initialized or previously released
 * allocator.
 *
 * @param memory The Stack Allocator.
 */
void sa_release(stack_allocator *memory);

/**
 * Allocates a sized chunk of memory from Stack Allocator.
 *
 * @param memory The Stack Allocator.
 * @param size   Size in bytes of the allocation.
 *
 * @return Allocated block memory on success.
 * @return NULL if not enought memory is available.
 */
void *sa_alloc(stack_allocator *memory, unsigned int size);
/**
 * Free all used memory from Stack Allocator, making it available for 
 * allocation once more.
 *
 * After calling this, all markers previously got become invalid.
 *
 * To actually reclaim the used memory for the OS, use #sa_release instead.
 *
 * @param memory The Stack Allocator.
 */
void sa_free(stack_allocator *memory);
/**
 * Retrieve a pointer to the top `size` bytes allocated.
 *
 * @param memory The Stack Allocator.
 * @param size   Number of bytes to peek.
 *
 * @return Pointer to the allocated memory, if at least `size` bytes are allocated.
 * @return NULL otherwise.
 */
void *sa_peek(stack_allocator *memory, unsigned int size);

/**
 * Get a marker for the current allocation state.
 *
 * The result can be used for freeing a Stack Allocator back to this state.
 *
 * @param memory The Stack Allocator.
 *
 * @return Marker for the current allocation state.
 */
int sa_get_marker(stack_allocator *memory);
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
 * @param memory The Stack Allocator.
 * @param marker A marker to an allocation state.
 */
void sa_free_marker(stack_allocator *memory, int marker);

/**
 * Get the quantity of free memory available in a Stack Allocator.
 *
 * @param memory The Stack Allocator.
 *
 * @return Size in bytes of the free memory block in allocator.
 */
int sa_available_memory(stack_allocator *memory);
/**
 * Get the quantity of used memory in a Stack Allocator.
 *
 * @param memory The Stack Allocator.
 *
 * @return Size in bytes of the memory block used in allocator.
 */
int sa_used_memory(stack_allocator *memory);

#endif

