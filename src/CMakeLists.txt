add_library(stack-allocator stack_allocator.c)
set_target_properties(stack-allocator PROPERTIES
	VERSION "${PROJECT_VERSION}"
	SOVERSION "${PROJECT_SOVERSION}")

add_library(double-stack-allocator double_stack_allocator.c)
set_target_properties(double-stack-allocator PROPERTIES
	VERSION "${PROJECT_VERSION}"
	SOVERSION "${PROJECT_SOVERSION}")
