cmake_minimum_required(VERSION 2.5)
project(judger C)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

set(BASE_CMAKE_C_FLAGS "-Wall -Werror -O3 -std=c99")
set(CMAKE_C_FLAGS "${BASE_CMAKE_C_FLAGS}")


# make judger lib
file(GLOB SOURCE "src/*.c" "src/rules/*.c")
add_library(judger SHARED ${SOURCE})
target_link_libraries(judger pthread seccomp)


install(DIRECTORY output/ DESTINATION /usr/lib/judger)