#include "stack_allocator.h"

#include <criterion/criterion.h>

Test(stack_allocator, initialization) {
	int capacity = 16;

	stack_allocator allocator;
	cr_assert(sa_init_with_size(&allocator, capacity));

	cr_assert_eq(allocator.capacity, capacity);
	cr_assert_eq(sa_available_memory(&allocator), capacity);
	cr_assert_eq(sa_used_memory(&allocator), 0);

	sa_release(&allocator);
	cr_assert_eq(sa_available_memory(&allocator), 0);
	cr_assert_eq(sa_used_memory(&allocator), 0);

	sa_release(&allocator);
}

Test(stack_allocator, empty) {
	stack_allocator allocator;
	cr_assert(sa_init_with_size(&allocator, 0));

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

Test(stack_allocator, full_usage) {
	int capacity = 16;

	stack_allocator allocator;
	cr_assert(sa_init_with_size(&allocator, capacity));

	void *ptr;
	ptr = sa_alloc(&allocator, capacity);
	cr_assert_not_null(ptr);
	cr_assert_eq(sa_available_memory(&allocator), 0);
	cr_assert_eq(sa_used_memory(&allocator), capacity);

	ptr = sa_alloc(&allocator, 1);
	cr_assert_null(ptr);
	ptr = sa_alloc(&allocator, 0);
	cr_assert_not_null(ptr);

	sa_free(&allocator);
	cr_assert_eq(sa_available_memory(&allocator), capacity);
	cr_assert_eq(sa_used_memory(&allocator), 0);

	sa_release(&allocator);
}

Test(stack_allocator, partial_usage) {
	int capacity = 16;
	int alloc_size = 4;

	stack_allocator allocator;
	cr_assert(sa_init_with_size(&allocator, capacity));

	void *ptr;
	ptr = sa_alloc(&allocator, alloc_size);
	cr_assert_not_null(ptr);
	cr_assert_eq(sa_available_memory(&allocator), capacity - alloc_size);
	cr_assert_eq(sa_used_memory(&allocator), alloc_size);

	int marker = sa_get_marker(&allocator);

	ptr = sa_alloc(&allocator, capacity - alloc_size);
	cr_assert_not_null(ptr);
	cr_assert_eq(sa_available_memory(&allocator), 0);
	cr_assert_eq(sa_used_memory(&allocator), capacity);

	ptr = sa_alloc(&allocator, 1);
	cr_assert_null(ptr);
	ptr = sa_alloc(&allocator, 0);
	cr_assert_not_null(ptr);

	sa_free_marker(&allocator, marker);
	cr_assert_eq(sa_available_memory(&allocator), capacity - alloc_size);
	cr_assert_eq(sa_used_memory(&allocator), alloc_size);

	sa_free(&allocator);
	cr_assert_eq(sa_available_memory(&allocator), capacity);
	cr_assert_eq(sa_used_memory(&allocator), 0);

	sa_release(&allocator);
}

Test(stack_allocator, peek) {
	int capacity = 16;
	int alloc_size = 4;

	stack_allocator allocator;
	cr_assert(sa_init_with_size(&allocator, capacity));

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
