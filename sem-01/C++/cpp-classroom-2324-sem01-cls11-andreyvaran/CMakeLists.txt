cmake_minimum_required (VERSION 3.8)

project(clw-11)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX") # CMake already contains W3 in its flags
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra-semi -O1 -g -fsanitize=address -fno-omit-frame-pointer")
endif()

add_executable(task-0-test-any-all src/task_0_any_all/test.cpp)
add_executable(task-0-test-for-each src/task_0_for_each/test.cpp)
add_executable(task-1-test src/task_1_logger/test.cpp)
add_executable(task-2-test src/task_2_event/test.cpp)
add_executable(task-3-test-apply src/task_3_tuple_utils/test_apply.cpp)
add_executable(task-3-test-at-many src/task_3_tuple_utils/test_at_many.cpp)
add_executable(task-3-test-transform-elements src/task_3_tuple_utils/test_transform_elements.cpp)
add_executable(task-4-test src/task_4_join/test.cpp)