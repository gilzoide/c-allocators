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

#include "stack_allocator.h"

#include <stdlib.h>

int sa_init_with_size(stack_allocator *memory, unsigned int size) {
	memory->buffer = malloc(size);
	int malloc_success = memory->buffer != NULL;
	memory->capacity = malloc_success * size;
	memory->marker = 0;
	return malloc_success;
}

void sa_release(stack_allocator *memory) {
	free(memory->buffer);
	*memory = (stack_allocator){};
}

void *sa_alloc(stack_allocator *memory, unsigned int size) {
	if(memory->marker + size > memory->capacity) return NULL;
	void *ptr = memory->buffer + memory->marker;
	memory->marker += size;
	return ptr;
}

void sa_free(stack_allocator *memory) {
	memory->marker = 0;
}

void *sa_peek(stack_allocator *memory, unsigned int size) {
	if(size == 0 || memory->marker < size) return NULL;
	return memory->buffer + memory->marker - size;
}

int sa_get_marker(stack_allocator *memory) {
	return memory->marker;
}

void sa_free_marker(stack_allocator *memory, int marker) {
	if(marker < memory->marker && marker >= 0) {
		memory->marker = marker;
	}
}

int sa_available_memory(stack_allocator *memory) {
	return memory->capacity - memory->marker;
}

int sa_used_memory(stack_allocator *memory) {
	return memory->marker;
}

