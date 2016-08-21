cmake_minimum_required(VERSION 2.5)
project(judger C)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

set(BASE_CMAKE_C_FLAGS "-Wall -Werror -O3 -std=c99")
set(CMAKE_C_FLAGS "${BASE_CMAKE_C_FLAGS}")


# make judger lib
file(GLOB SOURCE "src/*.c")
add_library(judger SHARED ${SOURCE})
target_link_libraries(judger pthread dl)


# make language seccomp rules
file(GLOB RULE_SOURCE src/rules/*)
foreach(lang_rule_path ${RULE_SOURCE})
        file(RELATIVE_PATH lang_name ${CMAKE_CURRENT_SOURCE_DIR}/src/rules ${lang_rule_path})
        add_library("rule_${lang_name}" SHARED "${lang_rule_path}/rule.c")
        target_link_libraries("rule_${lang_name}" seccomp)
endforeach(lang_rule_path src/rules)

install(DIRECTORY output/ DESTINATION /usr/lib/judger)