#define DOUBLE_STACK_ALLOCATOR_IMPLEMENTATION
#include "double_stack_allocator.h"

#include <criterion/criterion.h>

#define LOG_ALLOCATOR(a) \
	cr_log_info("{capacity = %d, bottom = %d, top = %d}", a.capacity, a.bottom, a.top)

Test(dsa_double_stack_allocator, initialization) {
	int size = 16;

	dsa_double_stack_allocator allocator;
	cr_assert(dsa_init_with_capacity(&allocator, size));

	cr_assert_eq(allocator.capacity, size);
	cr_assert_eq(dsa_available_memory(&allocator), size);
	cr_assert_eq(dsa_used_memory(&allocator), 0);

	dsa_release(&allocator);
	cr_assert_eq(dsa_available_memory(&allocator), 0);
	cr_assert_eq(dsa_used_memory(&allocator), 0);

	dsa_release(&allocator);
}

Test(dsa_double_stack_allocator, contiguous_memory) {
	int size = 16;

	dsa_double_stack_allocator allocator;
	cr_assert(dsa_init_with_capacity(&allocator, size));

	void *ptr1 = dsa_alloc_bottom(&allocator, 1);
	void *ptr2 = dsa_alloc_bottom(&allocator, 1);
	cr_assert_eq(ptr2, ptr1 + 1);

	ptr1 = dsa_alloc_top(&allocator, 1);
	ptr2 = dsa_alloc_top(&allocator, 1);
	cr_assert_eq(ptr1, ptr2 + 1);

	dsa_release(&allocator);
}

Test(dsa_double_stack_allocator, pop_peek) {
	int size = 16;
	size_t alloc_size = 4;

	dsa_double_stack_allocator allocator;
	cr_assert(dsa_init_with_capacity(&allocator, size));

    // Bottom
	void *first_ptr = dsa_alloc_bottom(&allocator, alloc_size);
	cr_assert_not_null(first_ptr);
	cr_assert_eq(dsa_peek_bottom(&allocator, alloc_size), first_ptr);

	void *ptr = dsa_alloc_bottom(&allocator, alloc_size);
	cr_assert_not_null(ptr);
	cr_assert_eq(dsa_peek_bottom(&allocator, alloc_size), ptr);
	cr_assert_eq(dsa_peek_bottom(&allocator, 2 * alloc_size), first_ptr);

    dsa_pop_bottom(&allocator, alloc_size);
    cr_assert_eq(dsa_peek_bottom(&allocator, alloc_size), first_ptr);

    dsa_pop_bottom(&allocator, alloc_size);
    cr_assert_null(dsa_peek_bottom(&allocator, alloc_size));

    // Top
	first_ptr = dsa_alloc_top(&allocator, alloc_size);
	cr_assert_not_null(first_ptr);
	cr_assert_eq(dsa_peek_top(&allocator, alloc_size), first_ptr);

	ptr = dsa_alloc_top(&allocator, alloc_size);
	cr_assert_not_null(ptr);
	cr_assert_eq(dsa_peek_top(&allocator, alloc_size), ptr);
	cr_assert_eq(dsa_peek_top(&allocator, 2 * alloc_size), ptr);

    dsa_pop_top(&allocator, alloc_size);
    cr_assert_eq(dsa_peek_top(&allocator, alloc_size), first_ptr);

    dsa_pop_top(&allocator, alloc_size);
    cr_assert_null(dsa_peek_top(&allocator, alloc_size));

	dsa_release(&allocator);
}

Test(dsa_double_stack_allocator, full_usage_bottom) {
	int size = 16;

	dsa_double_stack_allocator allocator;
	cr_assert(dsa_init_with_capacity(&allocator, size));

	void *ptr;
	ptr = dsa_alloc_bottom(&allocator, size);
	cr_assert_not_null(ptr);
	cr_assert_eq(dsa_available_memory(&allocator), 0);
	cr_assert_eq(dsa_used_memory(&allocator), size);

	ptr = dsa_alloc_bottom(&allocator, 1);
	cr_assert_null(ptr);
	ptr = dsa_alloc_top(&allocator, 1);
	cr_assert_null(ptr);

	dsa_clear_bottom(&allocator);
	cr_assert_eq(dsa_available_memory(&allocator), size);
	cr_assert_eq(dsa_used_memory(&allocator), 0);

	ptr = dsa_alloc_bottom(&allocator, size);
	cr_assert_not_null(ptr);
	cr_assert_eq(dsa_available_memory(&allocator), 0);
	cr_assert_eq(dsa_used_memory(&allocator), size);

	dsa_release(&allocator);
}

Test(dsa_double_stack_allocator, full_usage_top) {
	int size = 16;

	dsa_double_stack_allocator allocator;
	cr_assert(dsa_init_with_capacity(&allocator, size));

	void *ptr;
	ptr = dsa_alloc_top(&allocator, size);
	cr_assert_not_null(ptr);
	cr_assert_eq(dsa_available_memory(&allocator), 0);
	cr_assert_eq(dsa_used_memory(&allocator), size);

	ptr = dsa_alloc_top(&allocator, 1);
	cr_assert_null(ptr);
	ptr = dsa_alloc_bottom(&allocator, 1);
	cr_assert_null(ptr);

	dsa_clear_top(&allocator);
	cr_assert_eq(dsa_available_memory(&allocator), size);
	cr_assert_eq(dsa_used_memory(&allocator), 0);

	ptr = dsa_alloc_top(&allocator, size);
	cr_assert_not_null(ptr);
	cr_assert_eq(dsa_available_memory(&allocator), 0);
	cr_assert_eq(dsa_used_memory(&allocator), size);

	dsa_release(&allocator);
}

Test(dsa_double_stack_allocator, full_usage_top_bottom) {
	int size = 16;
	int half_size = 8;

	dsa_double_stack_allocator allocator;
	cr_assert(dsa_init_with_capacity(&allocator, size));

	void *ptr;
	ptr = dsa_alloc_top(&allocator, half_size);
	cr_assert_not_null(ptr);
	cr_assert_eq(dsa_available_memory(&allocator), half_size);
	cr_assert_eq(dsa_used_memory(&allocator), half_size);

	ptr = dsa_alloc_bottom(&allocator, half_size);
	cr_assert_not_null(ptr);
	cr_assert_eq(dsa_available_memory(&allocator), 0);
	cr_assert_eq(dsa_used_memory(&allocator), size);

	ptr = dsa_alloc_top(&allocator, 1);
	cr_assert_null(ptr);
	ptr = dsa_alloc_bottom(&allocator, 1);
	cr_assert_null(ptr);

	dsa_release(&allocator);
}

Test(dsa_double_stack_allocator, foreach) {
	size_t capacity = 1024;

	dsa_double_stack_allocator allocator;
	cr_assert(dsa_init_with_capacity_(&allocator, int, capacity));

    // Bottom
    int i;
    for(i = 0; i < capacity; i++) {
        *dsa_alloc_bottom_(&allocator, int) = i;
    }

    i = 0;
    DSA_FOREACH_BOTTOM(int, number, &allocator) {
        cr_assert_eq(*number, i);
        i++;
    }
    cr_assert_eq(i, capacity);

    i = capacity;
    DSA_FOREACH_BOTTOM_REVERSE(int, number, &allocator) {
        i--;
        cr_assert_eq(*number, i);
    }
    cr_assert_eq(i, 0);
    dsa_clear_bottom(&allocator);

    // Top
    for(i = 0; i < capacity; i++) {
        *dsa_alloc_top_(&allocator, int) = i;
    }

    i = 0;
    DSA_FOREACH_TOP(int, number, &allocator) {
        cr_assert_eq(*number, i);
        i++;
    }
    cr_assert_eq(i, capacity);

    i = capacity;
    DSA_FOREACH_TOP_REVERSE(int, number, &allocator) {
        i--;
        cr_assert_eq(*number, i);
        dsa_pop_top_(&allocator, int);
    }
    cr_assert_eq(i, 0);
}
