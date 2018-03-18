set(CMAKE_CXX_FLAGS "-Wall -Werror -Wconversion -fno-rtti -fno-exceptions")

add_executable(example1 example1.cpp)
target_link_libraries(example1 lextok)
