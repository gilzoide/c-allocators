include_directories("${CMAKE_SOURCE_DIR}/src" ${CRITERION_INCLUDE_DIRS})

add_executable(test-stack-allocator test_stack_allocator.c)
target_link_libraries(test-stack-allocator 
	$<TARGET_LINKER_FILE:stack-allocator>
	${CRITERION_LIBRARIES})
add_test(test-stack-allocator test-stack-allocator)

add_executable(test-double-stack-allocator test_double_stack_allocator.c)
target_link_libraries(test-double-stack-allocator 
	$<TARGET_LINKER_FILE:double-stack-allocator>
	${CRITERION_LIBRARIES})
add_test(test-double-stack-allocator test-double-stack-allocator)

