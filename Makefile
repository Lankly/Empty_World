SOURCES = main.o map.o tiles.o helpers.o colors.o status.o items.o item_callbacks.o inventory.o player.o creature.o creature_callbacks.o
DEPS = $(SOURCES:.o=.d)
TARGET = empty_world

all: $(TARGET)

-include $(DEPS)

$(TARGET): $(SOURCES)
	gcc $(SOURCES) -lncurses -o $(TARGET)

$(SOURCES): %.o: %.c
	gcc -std=c99 -Wall -Werror -MMD -g -o $@ -c $<
