#include "double_stack_allocator.h"

#include <criterion/criterion.h>

Test(double_stack_allocator, initialization) {
	int size = 16;

	double_stack_allocator allocator;
	cr_assert(dsa_init_with_size(&allocator, size));

	cr_assert_eq(allocator.capacity, size);
	cr_assert_eq(dsa_available_memory(&allocator), size);
	cr_assert_eq(dsa_used_memory(&allocator), 0);

	dsa_release(&allocator);
	cr_assert_eq(dsa_available_memory(&allocator), 0);
	cr_assert_eq(dsa_used_memory(&allocator), 0);

	dsa_release(&allocator);
}

Test(double_stack_allocator, full_usage) {
	int size = 16;

	double_stack_allocator allocator;
	cr_assert(dsa_init_with_size(&allocator, size));

	void *ptr;
	ptr = dsa_alloc_bottom(&allocator, size);
	cr_assert_not_null(ptr);
	cr_assert_eq(dsa_available_memory(&allocator), 0);
	cr_assert_eq(dsa_used_memory(&allocator), size);

	dsa_free_bottom(&allocator);
	cr_assert_eq(dsa_available_memory(&allocator), size);
	cr_assert_eq(dsa_used_memory(&allocator), 0);

	ptr = dsa_alloc_bottom(&allocator, size);
	cr_assert_not_null(ptr);
	cr_assert_eq(dsa_available_memory(&allocator), 0);
	cr_assert_eq(dsa_used_memory(&allocator), size);

	dsa_release(&allocator);
}

