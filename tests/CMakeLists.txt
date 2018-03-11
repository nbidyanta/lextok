function(add_test_exec exec_name)
	add_executable(${exec_name} ${exec_name}.cpp)
	target_link_libraries(${exec_name} lextok)
endfunction()

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wconversion -fno-rtti -fno-exceptions")

add_test_exec(test_alphabet_match)
add_test(alphabet_match test_alphabet_match)

add_test_exec(test_lower_alphabet_match)
add_test(lower_alphabet_match test_lower_alphabet_match)

add_test_exec(test_upper_alphabet_match)
add_test(upper_alphabet_match test_upper_alphabet_match)

add_test_exec(test_decimal_digit_match)
add_test(decimal_digit_match test_decimal_digit_match)

add_test_exec(test_real_world_match)
add_test(real_world_match test_real_world_match)
