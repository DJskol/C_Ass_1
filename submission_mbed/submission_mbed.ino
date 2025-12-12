#include "mbed.h"
#include "Components/TextLCD/TextLCD.c"
#include "Components/Sounds/sounds.c"
#include "Components/Keypad/keypad.c"
#include "Components/Switches/switches.c"
#include "Components/UART/uart.c"
#include "Components/Led/led.c"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define WAIT_US 1000000 
#define MAX_PC_MESSAGE 256 

using namespace mbed;

int keypad_state; 
int switches_array[8];
char pc_input[MAX_PC_MESSAGE];
//positive feedback
void positive_feedback(){
  play_accepted();
  accepted_flash();
}
//negative feedback
void negative_feedback(){
  play_wrong();
  denied_flash();
}
//Determines the input medium each time its called
void menu_query(){
  get_switches(switches_array);
  keypad_state = switches_array[8];
  myled = keypad_state;
  //based on the last switch (Right side), input will be taken from the pc or keypad
  if(keypad_state == 1){
    char response = get_key();
    pc_input[0] = response;
    char_pc_output("%c\n",response);
  }else{
    get_pc_input(pc_input);
  }
}
//Funtion is clled for pc inputs exceeding specific ranges
void amount_option_output(){
  negative_feedback();
  string_pc_output("%s","Too Many Inputs, Please Choose 1 Option from the Menu\n\n");
  lcd_full_display("- Wrong Answer -", " ", 2);
}
//Function called for slecting non existing options
void wrong_option_output(){
  negative_feedback();
  string_pc_output("%s","Wrong Option, Please Choose Appropriately\n\n");
  lcd_full_display("- Wrong Option -", " ", 2);
}
//Gameplay code for the Number Memory Game
void gameplay(int* gamemode_offline,char* rand_array, int display_time, int* round_number){
  string_pc_output("%s","\nSequence: ");
  lcd_full_display("Sequence: ", " ", 0);

  int display_sequence_length = 10; //length of "Sequence: "
  //displays each number and character
  for(int i = 0; i < strlen(rand_array); i++){
    char_pc_output("%c", rand_array[i]);
    lcd_2ndline_display(&rand_array[i],i);
    display_sequence_length += 1;
  } 

  wait_us(display_time * WAIT_US); //wait time to memorize
  //clears the sequence
  string_pc_output("%s","\r");
  for (int i = 0; i < display_sequence_length; i++){
    string_pc_output("%s"," ");
    lcd.printf(" ");
  }
  string_pc_output("%s","\rGuess:");
  lcd_full_display("Guess:", " ", 0);
  //gets the pc answer
  get_pc_input(pc_input);
  lcd_2ndline_display(pc_input,0);
  //validates answer
  for(int i = 0; i < strlen(pc_input)-1 ||strlen(pc_input)-1 <= 0; i++){
    //if answer is wrong based on length or value
    if(pc_input[i] != rand_array[i] || strlen(pc_input)-1 != strlen(rand_array)){
      negative_feedback();
      string_pc_output("%s","Wrong!\n");
      lcd_full_display("[  - Wrong! -  ]"," ",1);
      //print result
      int_pc_output("\n\nScore: %d", *round_number-1);
      lcd_full_display("Score: "," ",0);
      char result_char = (char)(48 + (*round_number-1));
      lcd_1stline_display(&result_char, 7);
      wait_us(3*WAIT_US);
      //post game menu
      while(1){
        string_pc_output("%s","\nPlay again? (Y/N): ");
        get_pc_input(pc_input);
        //Asks to restart the game with the same settings
        if(strlen(pc_input)-1<=1){
          if(pc_input[0] == 'Y' || pc_input[0] == 'y'){
            *round_number = 0;    //resets the game round start again
            string_pc_output("%s","\n\t[-- RESTART! --]\n");
            lcd_full_display("[-  RESTART!  -]"," ",0.5);
            positive_feedback();
            break;           
          }else if(pc_input[0] == 'N' || pc_input[0] == 'n'){
            while(1){
              string_pc_output("%s","\nSave Game Replay? (Y/N): ");
              lcd_full_display("Replay? (Y/N): "," ",0);
              get_pc_input(pc_input);
              lcd_1stline_display(pc_input,15);
              positive_feedback();
              //validates the input
              if(strlen(pc_input)-1<=1){
                if(pc_input[0] == 'Y' || pc_input[0] == 'y'){
                  string_pc_output("%s","system_save");     //Commands to save game replay in pc
                  lcd_full_display("[-=- SAVING -=-]"," ",1);
                  positive_feedback();
                  break;
                }else if(pc_input[0] == 'N' || pc_input[0] == 'n'){
                  positive_feedback();
                  break;
                }else{
                  wrong_option_output();
                }
              }else{
                amount_option_output();
              }
            }
            *gamemode_offline = 1;
            string_pc_output("%s","\n\t[-- GAME END --]\n");  //Ends game and stops recording
            lcd_full_display("[-  GAME END  -]", " ", 2);
            break;
          }else{
            wrong_option_output();
          }
        }else{
          amount_option_output();
        }
          lcd.cls();
      }
      break;
    }else if(i == strlen(pc_input)-2){
      play_correct();
      string_pc_output("%s","Correct!\n");
      lcd_full_display("[ - Correct! - ]", " ", 2); //correct input, next round
    }
  }
}
//Singleplayer game initializer
void singleplayer(){
  int singleplayer_offline = 0;
  int display_time = get_singleplayer_display_time();
  if(!display_time)return; //Option to go back to menu
  int max_digit = get_digit_range();
  int round_number = 1;
  //start game and recording
  string_pc_output("%s","\n\t[-- GAME START --]\n");
  lcd_full_display("[- GAME START -]", " ", 1);
  //begin singleplayer loop
  while(!singleplayer_offline){
    char cpu_rand_array[round_number];

    for(int i = 0;i < round_number; i++){
      cpu_rand_array[i] = rand_gen(max_digit);
    }
    cpu_rand_array[round_number] = '\0';

    gameplay(&singleplayer_offline, cpu_rand_array,display_time,&round_number);
    round_number += 1;  //round increase
  }
}
//Multiplayer game initializer
void multiplayer(){
  //Displays which player you are
  string_pc_output("%s","\n   -=- P1 -=-   \n");
  lcd_full_display("   -=- P2 -=-   ", " ", 2);

  string_pc_output("%s","\n\t[-- GAME START --]\n");
  lcd_full_display("[- GAME START -]", " ", 0.5);
  string_pc_output("%s","waiting for P2...\n");
  
  int multiplayer_offline = 0;
  int display_time = get_multiplayer_display_time(); //gets time from p2
  int round_number = 0;

  //start multiplayer loop
  while(!multiplayer_offline){
    round_number += 1;
    char p2_rand_array[round_number]; 

    string_pc_output("%s","\nwaiting for P2...\n");
    lcd_full_display("Sequence:", " ", 0);
    //generates random sequence that as long as the round number
    for(int i = 0; i < round_number; i++){
      char response = get_key();
      p2_rand_array[i] = response;
      lcd_2ndline_display(&response,i);
      play_accepted(); //positive audio and visual feedback
      accepted_flash();
    }
    p2_rand_array[round_number] = '\0'; // end case to prevent corruption bug

    wait_us(WAIT_US);
    lcd.cls();

    gameplay(&multiplayer_offline, p2_rand_array,display_time,&round_number); //loop
  }
}
//Creates the random number
char rand_gen(int max_digit){
  int rand_int = rand() % max_digit;
  rand_int = rand() % max_digit; //rand_int is called twice to actually obtain random numbers
  char rand_char;
  //convert to character
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
//gets the Maximum possible digit in the sequence
int get_digit_range(){
  int max_digit = -1;

  while(max_digit == -1){
    //menu code
    string_pc_output("%s","\nPlease select the range of possible numbers \nMax Number (1-F): ");
    lcd_full_display("Max Num (1-F): ", " ", 0);

    menu_query();

    if(strlen(pc_input) <= 2){
      lcd_1stline_display(pc_input, 15);

      int number = atoi(pc_input);

      if(number > 0 && number < 10){
        max_digit = number + 1;
      }else{
        switch(pc_input[0]){
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
            wrong_option_output();
            break;
        }
      }
    }else{
      amount_option_output();
    }
    // string_pc_output("%s","%d\n",max_digit);
  }
  positive_feedback();
  lcd.cls();
  // string_pc_output("%s","%c\n",pc_input[0]);
  return max_digit;
}
//gets the time allowed to view the sequence for singleplayer
int get_singleplayer_display_time(){
  int time = -1;
  //start of loop
  while(time == -1){
    //lcd and pc display menu
    string_pc_output("%s","\nSelect your Time Difficulty! \n1: Easy (5s)\n2: Medium (3s)\n3: Hard (1s)\n4: Back \nOption: ");
    lcd_full_display(" -=-  Time  -=- ","1:E 2:M 3:H 4:<<",0);
    menu_query();
    //mode chosen based on input
    if(strlen(pc_input)-1 <= 2){
      switch(pc_input[0]){
        case '1':
          time = 5;
          string_pc_output("%s","\nLoading Easy Mode...\n");
          lcd_full_display(" - Easy Mode! - "," ",2);
          break;
        case '2':
          time = 3;
          string_pc_output("%s","\nLoading Medium Mode...\n");
          lcd_full_display(" -Medium Mode- "," ",2);
          break;
        case '3':
          time = 1;
          string_pc_output("%s","\nLoading Hard Mode...\n");
          lcd_full_display(" - Hard Mode! - "," ",2);
          break;
        case '4':
          time = 0;
          break;
        default:
          wrong_option_output();
          break;
      }
    }else{
      amount_option_output();
    }
  }
  positive_feedback();
  lcd.cls();
  return time;
}
//gets the time allowed to view the sequence for multiplayer
int get_multiplayer_display_time(){
  int time;
  int int_response;
  //begin loop
  while(1){
    lcd_full_display("Time Set(1-9): ", " ", 0);

    char response = get_key();
    int_response = atoi(&response); //converts input to integer

    lcd_1stline_display(&response, 13);

    if (int_response > 0 && int_response < 10){
      time = int_response;
      break;
    }else{
      lcd_full_display("- Wrong Option -", " ", 0);
      negative_feedback();
    }
  }
  int_pc_output("\nTime Set: %d", int_response);
  positive_feedback();
  lcd.cls();
  return time;
}
//start of everyhting
int main(){
  //initializes unique requirements
  int active = 1; 
  srand((unsigned)time(NULL));
  // Game Startup + Intro Song
  led_setup();
  string_pc_output("%s", "system_clr");
  string_pc_output("%s", "Welcome Player(s)\n");
  lcd_full_display("Welcome Players!", " ", 1.5);
  play_rickroll();
  //Start Menu Loop
  while(active){
    string_pc_output("%s", "system_clr");
    string_pc_output("%s","Please Choose Your Game Mode!\n");
    string_pc_output("%s","1: Singleplayer \n2: Multiplayer \n3: Replay \n4: Quit \nAnswer:");
    lcd_full_display("[-=-  Menu  -=-]", "1>S 2>M 3>R 4>X", 0);
    //get Menu Option
    menu_query();
    //selects mode based on input
    if(strlen(pc_input) <= 2){
      switch(pc_input[0]){
        case '1':
          string_pc_output("%s","\n\n\t[ --- SinglePlayer --- ]\n\n");
          lcd_full_display("-=- Loading: -=-","[ SinglePlayer ]",2);
          play_accepted(); 
          accepted_flash();
          singleplayer();
          break;
        case '2':
          string_pc_output("%s","\n\n\t[ --- Multiplayer --- ]\n\n");
          lcd_full_display("-=- Loading: -=-","[ Multiplayer! ]",2);
          positive_feedback();
          multiplayer();
          break;
        case '3':
          string_pc_output("%s","system_load");
          lcd_full_display("[-- REPLAYS --]", " ", 0);
          break;
        case '4':
          string_pc_output("%s","Goodbye!");
          lcd_full_display("[=- Goodbye! -=]"," ",2);
          positive_feedback();
          active = 0;
          break;
        default:
          wrong_option_output();
          break;
      }
    }else{
      amount_option_output();
    }
  }
  lcd.cls();
  return 0;
}