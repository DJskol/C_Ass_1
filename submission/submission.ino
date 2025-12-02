#include "mbed.h"
#include "USBSerial.h"
#include <time.h>
#include "TextLCD/TextLCD.cpp"
#include <stdlib.h>
#include <ctype.h>

#define WAIT_US 1000000 // multiplier to fix use of wait_us instead of wait 

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

char getKey(){ 
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
    key = getKey();
    if(key != ' '){
      // pc.printf("Key = %c\r\n", key);
      wait_us(0.3*WAIT_US);
      return key;
    }
  }
}

int getswitches(){
  bus_out = 4;
  int switches = bus_in;
  bus_out = 5;
  switches = switches*16 + bus_in;

  return switches;
}

char rand_gen(int max_num){
  int rand_int = rand()%max_num;
  char rand_char = (char)rand_int;

  pc.printf("%d %c", rand_int, rand_char);
  if (rand_int > 9){
    switch(rand_int){
      case 10:
        rand_char = 'A';
        break;
      case 11:
        rand_char = 'B';
        break;
      case 12:
        rand_char = 'C';
        break;
      case 13:
        rand_char = 'D';
        break;
      case 14:
        rand_char = 'E';
        break;
      case 15:
        rand_char = 'F';
        break;
    }
  }
  return rand_char;
}

int round_manager(int time, int max_num, char mode){
  //universal result
  int result = 0;
  int randInt = rand(); //throwaway Random Int
  pc.printf("%c ", rand_gen(max_num));
  //singleplayer or multiplayer
  if(mode == 's'){
    int round_len = 0;
    
    while(1){
      char displayInt[round_len+1];
      for(int i = 0; i<round_len; i++){
        displayInt[i] = rand_gen(max_num);
      }

      lcd.cls();
      lcd.locate(0,0);
      lcd.printf("%s", displayInt);
      for (int i = 0; i<strlen(displayInt); i++){
        pc.printf("%c", displayInt[i]);
      }
      wait_us(time*WAIT_US);
      lcd.cls();
      lcd.locate(0,0);
      lcd.printf("Answer:");
      pc.printf("\nPlease Enter your Answer: ");
      lcd.locate(0,1);
      
      for(int i = 0; i<strlen(displayInt); i++){
        char checker_char = displayInt[i];
        char response = key_input();

        
        if (response == checker_char){
          
          lcd.printf("%c", response);
          pc.printf("%c", response);
          
          wait_us(0.5*WAIT_US);
          
          round_len += 1;
          
          if(i == strlen(displayInt) - 1){
            result += 1;
          }

        }else{
          lcd.cls();
          lcd.locate(0,0);
          lcd.printf("Game Over :(");
          
          lcd.locate(0,1);
          lcd.printf("Result: %d", result);

          wait_us(5*WAIT_US);

          lcd.cls();

          return result;
        }
      }
    }

  }
}

int range_selection(){
  int range = -1;

  while(range == -1){
    pc.printf("\nPlease select the range of possible numbers \nMax Number (0-F): ");
    char response = key_input();
    pc.printf("%c\n", response);
      
    if(isdigit(response) &&  response != '0'){
      range = response - '0' ;
      range += 1;
      // pc.printf("%d %c",range, response);
      break;
    }else{
      switch(response){
        case 'A':
          range = 11;
          break;
        case 'B':
          range = 12;
          break;
        case 'C':
          range = 13;
          break;
        case 'D':
          range = 14;
          break;
        case 'E':
          range = 15;
          break;
        case 'F':
          range = 16;
          break;
        default:
          pc.printf("\nError, Please choose a Valid Value\n");
          break;
      }
    }
  }
  return range;
}

int difficulty_selection(){
  int time;
  int answered = 0;
  while(!answered){
    pc.printf("\nSelect your Time Difficulty! \n1: Easy \n2: Medium \n3: Hard \n4: Back \nOption: ");

    char response = key_input();
    pc.printf("%c\n", response);
    switch(response){
      case '1':
        pc.printf("\nLoading Easy Mode...\n");
        time = 7;
        answered = 1;
        break;
      case '2':
        pc.printf("\nLoading Medium Mode...\n");
        time = 5;
        answered = 1;
        break;
      case '3':
        pc.printf("\nLoading Hard Mode...\n");
        time = 2;
        answered = 1;
        break;
      case '4':
        game_start();
        answered = 1;
        break;
      default:
        pc.printf("\nError, Please choose from the Menu\n");
        break;
    }
    return time;
  }
}

char game_start(){
  char mode;
  while(1){
    pc.printf("\nPlease choose your gamemode!:\n1: Singleplayer\n2: Multiplayer \n3: Quit \nOption: ");

    char response = key_input();
    pc.printf("%c\n", response);
    if (response == '1'){
      pc.printf("\n[  --  Singleplay Mode  --  ]\n");
      mode = 's';
      break;
    }else if (response == '2'){
      mode = 'm';
      pc.printf("\n[  --  Multiplayer Mode  --  ]\n");
      break;
    }else if(response == '3'){
      mode = 'q';
      break;
    }else{
      pc.printf("\nError, Please choose from the Menu\n");
      delay(2500);
    }
  }
  return mode;
}

int main(){
  lcd.cls();

  sw.format(8,0);
  sw.frequency(1000000);
  cs = 0;
  sw.write(0x0080);
  sw.write(0x0000);
  cs = 1;

  srand((unsigned)time(NULL));
  
  pc.printf("Welcome Player(s)\n");
  while(1){
    char mode = game_start();

    if (mode == 'q'){
      pc.printf("\nGoodBye!\n");
      break;
    }

    int time = difficulty_selection();
    int range = range_selection();

    int result = round_manager(time, range, mode);

    // pc.printf("%d ", time);
    // pc.printf("%d", range);
    // pc.printf("%d, result);
  }
}