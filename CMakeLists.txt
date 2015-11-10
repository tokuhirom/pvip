cmake_minimum_required(VERSION 2.6)
project(pvip)

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

SET(CMAKE_C_FLAGS_RELEASE "-W -O2 -g -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare")
SET(CMAKE_C_FLAGS_DEBUG   "-W -Wextra -Wno-unused-function -Wno-sign-compare -g -Werror")

SET(serial "0.0.1")
SET(soserial "1")

add_executable(greg 3rd/greg/greg.c 3rd/greg/tree.c 3rd/greg/compile.c)
set_target_properties(greg PROPERTIES COMPILE_FLAGS "-g")

add_library(libpvip STATIC src/pvip_node.c src/pvip_string.c src/gen.pvip.y.c src/gen.node.c)
set_target_properties(libpvip PROPERTIES OUTPUT_NAME "pvip")

add_custom_command(
    OUTPUT src/gen.pvip.y.c
    COMMAND ./greg -o src/gen.pvip.y.c src/pvip.y
    DEPENDS greg
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
add_custom_command(
    OUTPUT src/gen.node.c
    COMMAND perl build/node.pl
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )

# t/c_level.t
add_executable(c_level_test src/c_level_test.c)
target_link_libraries(c_level_test libpvip)
set_target_properties(c_level_test
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "t/"
    OUTPUT_NAME "c_level.t")

# pvip.exe
add_executable(pvip_exe src/commander.c src/main.c)
target_link_libraries(pvip_exe libpvip)
set_target_properties(pvip_exe
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "./"
    OUTPUT_NAME "pvip")

# `make test ARGS="-V"` to run test cases in verbose mode
add_test(NAME pvip-prove COMMAND prove)
add_custom_target(pvip-prove DEPENDS c_level_test pvip_exe)

