cmake_minimum_required(VERSION 3.6)

project(minigames)

set(CMAKE_C_STANDARD 99)

set(SOURCES src/main.c src/snake.c src/minesweeper.c src/tui.c)
set(EXECUTABLE snake)

add_executable(${EXECUTABLE} ${SOURCES})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -Werror -pedantic -lncurses")

if (MINGW)
    target_compile_definitions(${EXECUTABLE} PRIVATE __USE_MINGW_ANSI_STDIO=1)
endif ()

add_custom_target(run
    COMMAND "./${EXECUTABLE}"
    DEPENDS ${EXECUTABLE}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
