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

#ifndef __DOUBLE_STACK_ALLOCATOR_H__
#define __DOUBLE_STACK_ALLOCATOR_H__

#define DSA_MEMORY_INITIAL_SIZE 4096

/**
 * Custom memory manager: a double stack allocator.
 *
 * The bottom marker is increased, so pushed memory blocks have increasing addresses.
 * The top marker is decreased, so pushed memory blocks have decreasing addresses.
 *
 */
typedef struct double_stack_allocator {
	void *buffer;  ///< Memory buffer used
	int capacity;  ///< Capacity of memory buffer
	int top;  ///< Top mark, moved when allocating from the top
	int bottom;  ///< Bottok mark, moved when allocating from the bottom
} double_stack_allocator;

int dsa_init(double_stack_allocator *memory);
int dsa_init_with_size(double_stack_allocator *memory, unsigned int size);
void dsa_release(double_stack_allocator *memory);

void *dsa_alloc_top(double_stack_allocator *memory, unsigned int size);
void *dsa_alloc_bottom(double_stack_allocator *memory, unsigned int size);

int dsa_available_memory(double_stack_allocator *memory);
int dsa_used_memory(double_stack_allocator *memory);

int dsa_get_top_marker(double_stack_allocator *memory);
int dsa_get_bottom_marker(double_stack_allocator *memory);

void dsa_free_top(double_stack_allocator *memory);
void dsa_free_bottom(double_stack_allocator *memory);

void dsa_free_top_marker(double_stack_allocator *memory, int marker);
void dsa_free_bottom_marker(double_stack_allocator *memory, int marker);

#endif

