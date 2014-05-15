SOURCES = main.o map.o tiles.o helpers.o colors.o status.o
DEPS = $(SOURCES:.o=.d)
TARGET = empty_world

all: $(TARGET)

-include $(DEPS)

$(TARGET): $(SOURCES)
	gcc $(SOURCES) -lncurses -o $(TARGET)

$(SOURCES): %.o: %.c
	gcc -std=c99 -Wall -Werror -MMD -o $@ -c $<
