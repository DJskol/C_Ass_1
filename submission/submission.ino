#include "mbed.h"
#include "USBSerial.h"
#include <time.h>
#include "TextLCD/TextLCD.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define WAIT_US 1000000 // multiplier to fix use of wait_us instead of wait 
#define MAX_PC_MESSAGE 512 

//Default setup from labscripts
using namespace mbed;
USBSerial pc; // tx, rx
SPI sw(p3, NC, p2);
TextLCD lcd(p0, p1, p12, p13, p14, p15);
BusOut bus_out(p20, p19, p18);
BusIn bus_in(p11,p10,p9,p8);
DigitalOut cs(p5);
DigitalOut myled(p21); 

char pc_input[MAX_PC_MESSAGE];
int input_message_length = 0;

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

void clear_serial_monitor(){
  for (int i = 0; i < 100; i++) {
    pc.printf("\n");                //Clear Terminal
  }
}

void get_pc_input(char* pc_input, int* input_message_length){  
  int i = 0;
  int message_length = 0;

  pc_input[i] = pc.getc();
  for(i = 1; i < MAX_PC_MESSAGE && pc.available(); i++){
    pc_input[i] = pc.getc();
    message_length += 1;
  }

  *input_message_length =  message_length;
}

void singleplayer(){
  int singleplayer_offline = 0;
  int display_time = get_singleplayer_display_time();
  int max_digit = get_digit_range();
  int round_number = 0;

  srand((unsigned)time(NULL));

  while(!singleplayer_offline){
    round_number += 1;
    char rand_array[round_number];

    for(int i = 0;i < round_number; i++){
      rand_array[i] = rand_gen(max_digit);
    }

    pc.printf("\n\nSequence: ");
    for(int i = 0; i < sizeof(rand_array); i++){
      pc.printf("%c", rand_array[i]);
    } 

    wait_us(display_time * WAIT_US);

    clear_serial_monitor();

    get_pc_input(pc_input,&input_message_length);

    for(int i = 0; i < input_message_length; i++){
      pc.printf("%c", pc_input[i]);
      if(pc_input[i] != rand_array[i] || input_message_length != sizeof(rand_array)){
        singleplayer_offline = 1;
        break;
      }else if(i == input_message_length - 1){
        pc.printf("\nSuccess\n");
      }
    }
  }
  pc.printf("\nResult: %d", round_number);
}

void multiplayer(){
  int multiplayer_offline = 0;
  int display_time = get_multiplayer_display_time();
  int round_number = 0;

  while(!multiplayer_offline){
    round_number += 1;
    char rand_array[round_number];

    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("Sequence:");
    lcd.locate(0,1);

    for(int i = 0; i < round_number; i++){
      char response = key_input();
      rand_array[i] = response;
      
      lcd.printf("%c",response);
    }

    lcd.cls();

    pc.printf("\n\nSequence: ");
    for(int i = 0; i < sizeof(rand_array); i++){
      pc.printf("%c", rand_array[i]);
    } 

    wait_us(display_time * WAIT_US);

    clear_serial_monitor();

    get_pc_input(pc_input,&input_message_length);

    for(int i = 0; i < input_message_length; i++){
      pc.printf("%c", pc_input[i]);
      if(pc_input[i] != rand_array[i] || input_message_length != sizeof(rand_array)){
        multiplayer_offline = 1;
        break;
      }else if(i == input_message_length - 1){
        pc.printf("\nSuccess\n");
      }
    }

  }
  pc.printf("\nResult: %d", round_number);
}

char rand_gen(int max_digit){
  int rand_int = rand();
  rand_int = rand() % max_digit;
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
  // pc.printf("%c\n", rand_char);
  // pc.printf("\n%d %c\n", rand_int,rand_char);
  return rand_char;
}

int get_digit_range(){
  int max_digit = -1;

  while(max_digit == -1){
    pc.printf("\nPlease select the range of possible numbers \nMax Number (1-F): ");

    get_pc_input(pc_input,&input_message_length);

    if(input_message_length == 1){
      int number = atoi(pc_input);

      if(number > 0 && number < 9){
        max_digit = number + 1;
      }else{
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
            pc.printf("Input Error, Please choose an option between 1-F (Hex)\n\n");
            break;
        }
      }
    }else{
      pc.printf("Too Many Inputs, Please Only 1 Option from the Menu\n\n");
    }
    // pc.printf("%d\n",max_digit);
  }
  pc.printf("%c\n",pc_input[0]);
  return max_digit;
}

int get_singleplayer_display_time(){
  int time = -1;
  while(time == -1){
    pc.printf("\nSelect your Time Difficulty! \n1: Easy (7s)\n2: Medium (5s)\n3: Hard (2s)\n4: Back \nOption: ");
    
    get_pc_input(pc_input,&input_message_length);
    
    if(input_message_length == 1){
      switch(pc_input[0]){
        case '1':
          time = 7;
          pc.printf("%d\n", time);
          pc.printf("\nLoading Easy Mode...\n");
          break;
        case '2':
          time = 5;
          pc.printf("%d\n", time);
          pc.printf("\nLoading Medium Mode...\n");
          break;
        case '3':
          time = 2;
          pc.printf("%d\n", time);
          pc.printf("\nLoading Hard Mode...\n");
          break;
        case '4':
          time = 0;
          break;
        default:
          pc.printf("Inputs Amount Error, Please Choose 1 Option from the Menu\n\n");
          break;
      }
    }else{
      pc.printf("Too Many Inputs, Please Only 1 Option from the Menu\n\n");
    }
    // pc.printf("\nTime: %d\n",time);
    return time;
  }
}

int get_multiplayer_display_time(){
  int time;

  while(1){
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("Timer (1-9): ");
    lcd.locate(0,1);

    char response = key_input();
    int int_response = atoi(&response);
    if (int_response > 0 && int_response < 10){
      for(int i = 0; i < 3; i++){
        cs = 0;
        sw.write(0x0055);
        sw.write(0x0055);
        cs = 1;
        wait_us(0.4 * WAIT_US);
        cs = 0;
        sw.write(0x0000);
        sw.write(0x0000);
        cs = 1;
        wait_us(0.4 * WAIT_US);
      }

      time = int_response;
      break;
    }
  }
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

  clear_serial_monitor();
  
  int active = 1;  

  while(active){
    pc.printf("\nWelcome Player(s)\n");
    pc.printf("\nPlease Choose Your Game Mode!\n");
    pc.printf("1: Singleplayer \n2: Multiplayer \n3: Quit \nAnswer: ");

    get_pc_input(pc_input,&input_message_length);

    if(input_message_length == 1){

      switch(pc_input[0]){
        case '1':
          pc.printf("%c\n",pc_input[0]);
          singleplayer();
          break;
        case '2':
          pc.printf("%c\n",pc_input[0]);
          multiplayer();
          break;
        case '3':
          pc.printf("%c\n",pc_input[0]);
          pc.printf("Goodbye!\n\n");
          active = 0;
          break;
        default:
          pc.printf("%c\n\n",pc_input[0]);
          pc.printf("Wrong Option, Please Choose Appropriately\n\n");
          break;
      }
      
    }else{
      pc.printf("Inputs Amount Error, Please Choose 1 Option from the Menu\n\n");
    }
  }
}