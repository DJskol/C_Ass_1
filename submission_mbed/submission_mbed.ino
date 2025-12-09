#include "mbed.h"
#include "USBSerial.h"
#include <time.h>
#include "TextLCD/TextLCD.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define WAIT_US 1000000 // multiplier to fix use of wait_us instead of wait 
#define MAX_PC_MESSAGE 256 

//Default setup from labscripts
using namespace mbed;
USBSerial pc(true); // tx, rx
SPI sw(p3, NC, p2);
TextLCD lcd(p0, p1, p12, p13, p14, p15);
BusOut bus_out(p20, p19, p18);
BusIn bus_in(p11,p10,p9,p8);
DigitalOut cs(p5);
DigitalOut myled(p21); 

char pc_input[MAX_PC_MESSAGE];
char throwaway_check;

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

void get_pc_input(char* pc_input){  
  wait_us(50000);
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
  wait_us(50000);
}

void int_pc_output(char* format, int number){
  wait_us(50000);
  pc.printf(format, number);
  throwaway_check = pc.getc();
}

void string_pc_output(char* format, char* word){
  wait_us(50000);
  pc.printf(format, word);
  throwaway_check = pc.getc();
}

void char_pc_output(char* format, char letter){
  wait_us(50000);
  pc.printf(format, letter);
  throwaway_check = pc.getc();
}

void gameplay(int* gamemode_offline,char* rand_array, int display_time){
  int display_sequence_length = 10;

  string_pc_output("%s","\n\nSequence: ");

  for(int i = 0; i < strlen(rand_array); i++){
    char_pc_output("%c", rand_array[i]);
    display_sequence_length += 1;
  } 

  wait_us(display_time * WAIT_US);
  
  string_pc_output("%s","\r");

  for (int i = 0; i < display_sequence_length; i++){
    string_pc_output("%s"," ");
  }

  string_pc_output("%s","\rGuess:");

  get_pc_input(pc_input);

  for(int i = 0; i < strlen(pc_input)-1; i++){
    // string_pc_output("%s","%d %d ", strlen(pc_input), strlen(rand_array));
    if(pc_input[i] != rand_array[i] || strlen(pc_input)-1 != strlen(rand_array)){
      *gamemode_offline = 1;
      break;
    }else if(i == strlen(pc_input)-2){
      string_pc_output("%s","\nSuccess\n");
    }
  }
}

void singleplayer(){
  int singleplayer_offline = 0;
  int display_time = get_singleplayer_display_time();
  if(!display_time)return;
  int max_digit = get_digit_range();
  int round_number = 1;

  srand((unsigned)time(NULL));

  while(!singleplayer_offline){
    char cpu_rand_array[round_number];

    for(int i = 0;i < round_number; i++){
      cpu_rand_array[i] = rand_gen(max_digit);
    }
    cpu_rand_array[round_number] = '\0';

    gameplay(&singleplayer_offline, cpu_rand_array,display_time);
    round_number += 1;
  }
  int_pc_output("\n\nResult: %d", round_number-1);
}

void multiplayer(){
  int multiplayer_offline = 0;
  int display_time = get_multiplayer_display_time();
  int round_number = 0;

  while(!multiplayer_offline){
    round_number += 1;
    char p2_rand_array[round_number];

    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("Sequence:");
    lcd.locate(0,1);

    for(int i = 0; i < round_number; i++){
      char response = key_input();
      p2_rand_array[i] = response;
      
      lcd.printf("%c",response);
    }
    p2_rand_array[round_number] = '\0';

    wait_us(WAIT_US);

    lcd.cls();

    gameplay(&multiplayer_offline, p2_rand_array,display_time);

  }
  int_pc_output("\n\nResult: %d\n", round_number-1);
}

char rand_gen(int max_digit){
  int rand_int = rand() % max_digit;
  // rand_int = rand() ;
  char rand_char;
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
    default:
      rand_int += 48;
      rand_char = (char)rand_int;
      break;
  }
  return rand_char;
}

int get_digit_range(){
  wait_us(50000);
  int max_digit = -1;

  while(max_digit == -1){
    string_pc_output("%s","\nPlease select the range of possible numbers \nMax Number (1-F): ");

    get_pc_input(pc_input);

    pc_input[strlen(pc_input)-1] = ' ';

    if(strlen(pc_input) <= 2){
      int number = atoi(pc_input);

      if(number > 0 && number < 10){
        max_digit = number + 1;
      }else{
        wait_us(50000);
        switch(toupper(pc_input[0])){
          case 'A':
            max_digit = 11;
            break;
          case 'B':
            max_digit = 12;
            break;
          case 'C':
            max_digit = 13;
            break;
          case 'D':
            max_digit = 14;
            break;
          case 'E':
            max_digit = 15;
            break;
          case 'F':
            max_digit = 16;
            break;
          default:
            string_pc_output("%s","Input Error, Please choose an option between 1-F (Hex)\n\n");
            break;
        }
      }
    }else{
      string_pc_output("%s","Too Many Inputs, Please Only 1 Option from the Menu\n\n");
    }
    // string_pc_output("%s","%d\n",max_digit);
  }
  // string_pc_output("%s","%c\n",pc_input[0]);
  return max_digit;
}

int get_singleplayer_display_time(){
  int time = -1;
  while(time == -1){
    string_pc_output("%s","\nSelect your Time Difficulty! \n1: Easy (5s)\n2: Medium (3s)\n3: Hard (1s)\n4: Back \nOption: ");
    
    get_pc_input(pc_input);
    
    if(strlen(pc_input)-1 == 1){
      switch(pc_input[0]){
        case '1':
          time = 5;
          string_pc_output("%s","\n\nLoading Easy Mode...\n");
          break;
        case '2':
          time = 3;
          string_pc_output("%s","\n\nLoading Medium Mode...\n");
          break;
        case '3':
          time = 1;
          string_pc_output("%s","\n\nLoading Hard Mode...\n");
          break;
        case '4':
          time = 0;
          break;
        default:
          string_pc_output("%s","Inputs Amount Error, Please Choose 1 Option from the Menu\n\n");
          break;
      }
    }else{
      string_pc_output("%s","Too Many Inputs, Please Only 1 Option from the Menu\n\n");
    }
  }
  return time;
}

int get_multiplayer_display_time(){
  int time;

  while(1){
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("Timer (1-9):");

    char response = key_input();
    int int_response = atoi(&response);
    lcd.printf(" ");
    if (int_response > 0 && int_response < 10){
      time = int_response;

      lcd.printf("%d", time);

      for(int i = 0; i < 3; i++){
        cs = 0;
        sw.write(0x00AA);
        sw.write(0x00AA);
        cs = 1;
        wait_us(0.4 * WAIT_US);
        cs = 0;
        sw.write(0x0000);
        sw.write(0x0000);
        cs = 1;
        wait_us(0.4 * WAIT_US);
      }

      break;
    }
  }
  lcd.cls();
  return time;
}

int main(){
  lcd.cls();

  sw.format(8,0);
  sw.frequency(1000000);
  cs = 0;
  sw.write(0x0000);
  sw.write(0x0000);
  cs = 1;
  
  int active = 1;  

  string_pc_output("%s", "\nWelcome Player(s)\n");

  while(active){
    string_pc_output("%s","\nPlease Choose Your Game Mode!\n");
    string_pc_output("%s","1: Singleplayer \n2: Multiplayer \n3: Quit \nAnswer:");

    get_pc_input(pc_input);

    if(strlen(pc_input) <= 2){
      switch(pc_input[0]){
        case '1':
          singleplayer();
          break;
        case '2':
          multiplayer();
          break;
        case '3':
          string_pc_output("%s","Goodbye!");
          active = 0;
          break;
        default:
          string_pc_output("%s","Wrong Option, Please Choose Appropriately\n\n");
          break;
      }
      
    }else{
      string_pc_output("%s","Inputs Amount Error, Please Choose 1 Option from the Menu\n\n");
    }
  }
}