#include "uart.h"

#define WAIT_US_UART 50000

char throwaway_check;

void get_pc_input(char* pc_input){  
  wait_us(WAIT_US_UART);
  pc.printf("pc_requested");
  int message_index = 0;

  while(1){
    char recieved_message = pc.getc();
    if(recieved_message == '*'){
      pc_input[message_index] = '\0';
      break;
    }
    pc_input[message_index] = recieved_message;
    ++message_index;  
  }
  wait_us(WAIT_US_UART);
}

void int_pc_output(char* format, int number){
  wait_us(WAIT_US_UART);
  pc.printf(format, number);
  throwaway_check = pc.getc();
}

void string_pc_output(char* format, char* word){
  wait_us(WAIT_US_UART);
  pc.printf(format, word);
  throwaway_check = pc.getc();
}

void char_pc_output(char* format, char letter){
  wait_us(WAIT_US_UART);
  pc.printf(format, letter);
  throwaway_check = pc.getc();
}
