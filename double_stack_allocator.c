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

#include "double_stack_allocator.h"

#include <stdlib.h>
#include <string.h>

int dsa_init(double_stack_allocator *memory) {
	return dsa_init_with_size(memory, DSA_MEMORY_INITIAL_SIZE);
}

int dsa_init_with_size(double_stack_allocator *memory, int size) {
	memory->buffer = malloc(size);
	int malloc_success = memory->buffer != NULL;
	memory->capacity = malloc_success * size;
	memory->top = (malloc_success * size) - 1;
	memory->bottom = 0;
	return malloc_success;
}

void dsa_release(double_stack_allocator *memory) {
	free(memory->buffer);
	// zero out buffer, capacity and bottom, set top to -1
	*memory = (double_stack_allocator){
		.top = -1,
	};
}

void *dsa_alloc_top(double_stack_allocator *memory, int size) {
	if(memory->top - size < memory->bottom) return NULL;
	memory->top -= size;
	void *ptr = memory->buffer + memory->top + 1;
	return ptr;
}

void *dsa_alloc_bottom(double_stack_allocator *memory, int size) {
	if(memory->bottom + size > memory->top) return NULL;
	void *ptr = memory->buffer + memory->bottom;
	memory->bottom += size;
	return ptr;
}

int dsa_available_memory(double_stack_allocator *memory) {
	return memory->top + 1 - memory->bottom;
}

int dsa_used_memory(double_stack_allocator *memory) {
	return memory->bottom + (memory->capacity - (memory->top + 1));
}

int dsa_get_top_marker(double_stack_allocator *memory) {
	return memory->top;
}
int dsa_get_bottom_marker(double_stack_allocator *memory) {
	return memory->bottom;
}

void dsa_free_top(double_stack_allocator *memory) {
	memory->top = memory->capacity - 1;
}

void dsa_free_bottom(double_stack_allocator *memory) {
	memory->bottom = 0;
}

void dsa_free_top_marker(double_stack_allocator *memory, int marker) {
	if(marker > memory->top && marker < memory->capacity) {
		memory->top = marker;
	}
}

void dsa_free_bottom_marker(double_stack_allocator *memory, int marker) {
	if(marker < memory->bottom && marker >= 0) {
		memory->bottom = marker;
	}
}

