function(add_test_exec exec_name source_name)
	add_executable(${exec_name} ${source_name})
	target_link_libraries(${exec_name} lextok)
endfunction()

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wconversion -fno-rtti -fno-exceptions")

add_test_exec(test_alphabet_match test_alphabet_match.cpp)
add_test(alphabet_match test_alphabet_match)
