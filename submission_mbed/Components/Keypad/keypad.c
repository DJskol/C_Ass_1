#include "keypad.h"
//defines the debounce time
#define DEBOUNCE_US 1000000 
//the keytable for the mbed
char Keytable[] = { 'F', 'E', 'D', 'C', 
                    '3', '6', '9', 'B', 
                    '2', '5', '8', '0', 
                    '1', '4', '7', 'A'
                    };
//the function below gets the pressed key and returns it
char keypad_input(){ 
  int i,j; 
  char ch=' ';
  for (i = 0; i <= 3; i++) { 
    bus_out = i;  
    for (j = 0; j <= 3; j++) { 
        if (((bus_in ^ 0x00FF) & (0x0001<<j)) != 0) { 
        ch = Keytable[(i * 4) + j]; 
        }
      } 
  } 
  return ch;
}
//loops until an actual key is pressed;
char get_key(){
  char key = ' '; 
  while(1) {
    key = keypad_input();
    if(key != ' '){
      wait_us(0.3*DEBOUNCE_US);//accounts for bounce
      return key;
    }
  }
}
