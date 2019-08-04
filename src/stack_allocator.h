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

#define SA_MEMORY_INITIAL_SIZE 4096

/**
 * Custom memory manager: a stack allocator.
 *
 * The marker is increased, so pushed memory blocks have increasing addresses.
 *
 */
typedef struct stack_allocator {
	void *buffer;  ///< Memory buffer used
	int capacity;  ///< Capacity of memory buffer
	int marker;  ///< Top mark, moved when allocating
} stack_allocator;

int sa_init(stack_allocator *memory);
int sa_init_with_size(stack_allocator *memory, unsigned int size);
void sa_release(stack_allocator *memory);

void *sa_alloc(stack_allocator *memory, unsigned int size);

int sa_available_memory(stack_allocator *memory);
int sa_used_memory(stack_allocator *memory);

int sa_get_marker(stack_allocator *memory);

void sa_free(stack_allocator *memory);

void sa_free_marker(stack_allocator *memory, int marker);

#endif

