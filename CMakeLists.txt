cmake_minimum_required(VERSION 3.6)

project(snake)

set(CMAKE_C_STANDARD 99)

# Executable sources
set(SOURCES main.c)

# Test sources
# file(GLOB TESTS_GLOB "tests/test*.c")
# foreach (file_path ${TESTS_GLOB})
    # message("Found test: " ${file_path})
# endforeach ()
# set(TEST_SOURCES minesweeper.c
        # tests/cut_main_runner.c
        # tests/utils_for_tests.c
        # ${TESTS_GLOB})

# Targets
set(EXECUTABLE snake)
# set(TESTS_EXECUTABLE minesweeper-tests)

# Executable
add_executable(${EXECUTABLE} ${SOURCES})

# Tests
# add_definitions(-DCUT)
# add_executable(${TESTS_EXECUTABLE} ${TEST_SOURCES})


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -pedantic -lncurses")

if (MINGW)
    target_compile_definitions(${EXECUTABLE} PRIVATE __USE_MINGW_ANSI_STDIO=1)
endif ()

add_custom_target(run
    COMMAND "./${EXECUTABLE}"
    DEPENDS ${EXECUTABLE}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
