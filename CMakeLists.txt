cmake_minimum_required (VERSION 2.6)
project (EmptyWorld)
find_library(ncurses REQUIRED)
set(CMAKE_C_FLAGS "-std=c99 -Wall -Werror -pedantic -g")
add_executable(EmptyWorld
        main.c
        bodies.c
        colors.c
        creatures.c
        display.c
        helpers.c
        items.c
        lists.c
        maps.c
        tiles.c
        trees.c
)
target_link_libraries(EmptyWorld ncurses m)
