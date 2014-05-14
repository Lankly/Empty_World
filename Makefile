SOURCES = main.o map.o tiles.o helpers.o
TARGET = empty_world

all: $(TARGET)

$(TARGET): $(SOURCES)
	gcc $(SOURCES) -lncurses -o $(TARGET)

$(SOURCES): %.o: %.c
	gcc $< -c -o $@ -std=c99 -Wall -Werror