cmake_minimum_required (VERSION 2.6)
project (EmptyWorld)
find_library(ncurses REQUIRED)
set(CMAKE_C_FLAGS "-std=c99 -Wall -Werror -pedantic -g")
add_executable(EmptyWorld
	main.c
	colors.c
	creature.c
	creature_callbacks.c
	helpers.c
	inventory.c
	item_callbacks.c
	items.c
	map.c
	player.c
	status.c
	tiles.c
	classes.c
)
target_link_libraries(EmptyWorld ncurses m)