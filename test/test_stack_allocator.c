#define STACK_ALLOCATOR_IMPLEMENTATION
#include "stack_allocator.h"

#include <criterion/criterion.h>

Test(sa_stack_allocator, initialization) {
	size_t capacity = 16;

	sa_stack_allocator allocator;
	cr_assert(sa_init_with_capacity(&allocator, capacity));

	cr_assert_eq(allocator.capacity, capacity);
	cr_assert_eq(sa_available_memory(&allocator), capacity);
	cr_assert_eq(sa_used_memory(&allocator), 0);

	sa_release(&allocator);
	cr_assert_eq(sa_available_memory(&allocator), 0);
	cr_assert_eq(sa_used_memory(&allocator), 0);

	sa_release(&allocator);
}

Test(sa_stack_allocator, empty) {
	sa_stack_allocator allocator;
	cr_assert(sa_init_with_capacity(&allocator, 0));

	void *ptr;
	ptr = sa_alloc(&allocator, 1);
	cr_assert_null(ptr);
	ptr = sa_alloc(&allocator, 0);
	cr_assert_not_null(ptr);

	sa_release(&allocator);
	cr_assert_eq(sa_available_memory(&allocator), 0);
	cr_assert_eq(sa_used_memory(&allocator), 0);

	sa_release(&allocator);
}

Test(sa_stack_allocator, full_usage) {
	size_t capacity = 16;

	sa_stack_allocator allocator;
	cr_assert(sa_init_with_capacity(&allocator, capacity));

	void *ptr;
	ptr = sa_alloc(&allocator, capacity);
	cr_assert_not_null(ptr);
	cr_assert_eq(sa_available_memory(&allocator), 0);
	cr_assert_eq(sa_used_memory(&allocator), capacity);

	ptr = sa_alloc(&allocator, 1);
	cr_assert_null(ptr);
	ptr = sa_alloc(&allocator, 0);
	cr_assert_not_null(ptr);

	sa_clear(&allocator);
	cr_assert_eq(sa_available_memory(&allocator), capacity);
	cr_assert_eq(sa_used_memory(&allocator), 0);

	sa_release(&allocator);
}

Test(sa_stack_allocator, partial_usage) {
	size_t capacity = 16;
	size_t alloc_size = 4;

	sa_stack_allocator allocator;
	cr_assert(sa_init_with_capacity(&allocator, capacity));

	void *ptr;
	ptr = sa_alloc(&allocator, alloc_size);
	cr_assert_not_null(ptr);
	cr_assert_eq(sa_available_memory(&allocator), capacity - alloc_size);
	cr_assert_eq(sa_used_memory(&allocator), alloc_size);

	size_t marker = sa_get_marker(&allocator);

	ptr = sa_alloc(&allocator, capacity - alloc_size);
	cr_assert_not_null(ptr);
	cr_assert_eq(sa_available_memory(&allocator), 0);
	cr_assert_eq(sa_used_memory(&allocator), capacity);

	ptr = sa_alloc(&allocator, 1);
	cr_assert_null(ptr);
	ptr = sa_alloc(&allocator, 0);
	cr_assert_not_null(ptr);

	sa_clear_marker(&allocator, marker);
	cr_assert_eq(sa_available_memory(&allocator), capacity - alloc_size);
	cr_assert_eq(sa_used_memory(&allocator), alloc_size);

	sa_clear(&allocator);
	cr_assert_eq(sa_available_memory(&allocator), capacity);
	cr_assert_eq(sa_used_memory(&allocator), 0);

	sa_release(&allocator);
}

Test(sa_stack_allocator, pop) {
	size_t capacity = 16;
	size_t alloc_size = 4;

	sa_stack_allocator allocator;
	cr_assert(sa_init_with_capacity(&allocator, capacity));

	void *first_ptr = sa_alloc(&allocator, alloc_size);
	cr_assert_not_null(first_ptr);
	cr_assert_eq(sa_peek(&allocator, alloc_size), first_ptr);

	void *ptr = sa_alloc(&allocator, alloc_size);
	cr_assert_not_null(ptr);
	cr_assert_eq(sa_peek(&allocator, alloc_size), ptr);
	cr_assert_eq(sa_peek(&allocator, 2 * alloc_size), first_ptr);

    sa_pop(&allocator, alloc_size);
    cr_assert_eq(sa_peek(&allocator, alloc_size), first_ptr);

    sa_pop(&allocator, alloc_size);
    cr_assert_null(sa_peek(&allocator, alloc_size));

	sa_release(&allocator);
}

Test(sa_stack_allocator, peek) {
	size_t capacity = 16;
	size_t alloc_size = 4;

	sa_stack_allocator allocator;
	cr_assert(sa_init_with_capacity(&allocator, capacity));

	void *first_ptr = sa_alloc(&allocator, alloc_size);
	cr_assert_not_null(first_ptr);
	cr_assert_eq(sa_peek(&allocator, alloc_size), first_ptr);

	void *ptr = sa_alloc(&allocator, alloc_size);
	cr_assert_not_null(ptr);
	cr_assert_eq(sa_peek(&allocator, alloc_size), ptr);
	cr_assert_eq(sa_peek(&allocator, 2 * alloc_size), first_ptr);

	cr_assert_null(sa_peek(&allocator, capacity));

	sa_release(&allocator);
}

Test(sa_stack_allocator, foreach) {
	size_t capacity = 16;

	sa_stack_allocator allocator;
	cr_assert(sa_init_with_capacity_(&allocator, int, capacity));

    int i;
    for(i = 0; i < capacity; i++) {
        *sa_alloc_(&allocator, int) = i;
    }

    i = 0;
    SA_FOREACH(int, number, &allocator) {
        cr_assert_eq(*number, i);
        i++;
    }
    cr_assert_eq(i, capacity);

    i = capacity;
    SA_FOREACH_REVERSE(int, number, &allocator) {
        i--;
        cr_assert_eq(*number, i);
    }
    cr_assert_eq(i, 0);
}
