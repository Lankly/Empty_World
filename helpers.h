#include <stdbool.h>

#ifndef HELPERS_H
#define HELPERS_H

void* Calloc(int items, int size);

char* str_lowercase(char* str);
bool str_is_num(char* str);

int get_coord(int x,int y,int width);

void quit(const char* error_msg);

#endif
