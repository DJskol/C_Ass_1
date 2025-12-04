#include "mbed.h"
#include "USBSerial.h"
#include <time.h>
#include "TextLCD/TextLCD.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define WAIT_US 1000000 // multiplier to fix use of wait_us instead of wait 
//Default setup from labscripts
using namespace mbed;
USBSerial pc; // tx, rx
SPI sw(p3, NC, p2);
TextLCD lcd(p0, p1, p12, p13, p14, p15);
BusOut bus_out(p20, p19, p18);
BusIn bus_in(p11,p10,p9,p8);
DigitalOut cs(p5);
DigitalOut myled(p21); 

char Keytable[] = { 'F', 'E', 'D', 'C', 
'3', '6', '9', 'B', 
'2', '5', '8', '0', 
'1', '4', '7', 'A'
};

char get_Key(){ 
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

char key_input(){
  char key = ' '; 
  while(1) {
    key = get_Key();
    if(key != ' '){
      // pc.printf("Key = %c\r\n", key);
      wait_us(0.3*WAIT_US);
      return key;
    }
  }
}

int get_switches(){
  bus_out = 4;
  int switches = bus_in;
  bus_out = 5;
  switches = switches*16 + bus_in;

  return switches;
}

//Custom code
//Processing functions


int main(){
  lcd.cls();

  sw.format(8,0);
  sw.frequency(1000000);
  cs = 0;
  sw.write(0x0000);
  sw.write(0x0000);
  cs = 1;

  srand((unsigned)time(NULL));

  int active = 1;
  
  while(active){
    char raw_menu_choice = '0';
    pc.printf("\nWelcome Player(s)\n");
    
    pc.printf("\nPlease Choose Your Game Mode!\n");
    pc.printf("1: Singleplayer \n2: Multiplayer \n3:Quit \nAnswer: ");

    while(1){
      raw_menu_choice = pc.getc();
      if(raw_menu_choice != '0'){
        break;
      }
    }

    switch(raw_menu_choice){
      case '1':
        pc.printf("%d\n\n",raw_menu_choice);
        break;
      case '2':
        pc.printf("%d\n\n",raw_menu_choice);
        break;
      case '3':
        pc.printf("Goodbye!\n\n");
        active = 0;
        break;
      default:
        pc.printf("Wrong Option, Please Choose Approprately\n\n");
        break;
    }
    
  }
}