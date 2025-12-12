#ifndef UART_H
#define UART_H

#include "../components.h"

void get_pc_input(char* pc_input);
void int_pc_output(char* format, int number);
void string_pc_output(char* format, char* word);
void char_pc_output(char* format, char letter);

#endif