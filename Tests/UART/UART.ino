#include "mbed.h"
#include "USBSerial.h"
#include "TextLCD/TextLCD.cpp"
TextLCD lcd(p0, p1, p12, p13, p14, p15);
USBSerial pc(true);
char str1 [] = "Hi host PC!!!\n";
char str2[256];
int main() {
lcd.cls();
pc.printf("%s",str1);
char throwaway = pc.getc();
while(1){
  wait_us(50000);
  pc.printf("pc_requested");
  int message_index = 0;

  while(1){
    char recieved_message = pc.getc();
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("%c",recieved_message);
    if(recieved_message == '*'){
      lcd.printf("Stopped");
      str2[message_index] = '\0';
      break;
    }
    lcd.printf("%d", message_index);
    str2[message_index] = recieved_message;
    ++message_index;  
  }
  wait_us(50000);
  pc.printf("Done\n");
  throwaway = pc.getc();
  // gets(str2);
  lcd.locate(0,0);
  lcd.printf("%s", str1);
  
  // pc.gets(str2,256);
  lcd.locate(0,1);
  lcd.printf("%s", str2);
  lcd.cls();
  lcd.printf("%d %d", message_index, strlen(str2));
  // for(int i = 0; i < strlen(str2); i++){
  //   str2[] = 
  // }
  if(!strcmp(str2, "Cool")){
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("%s", str2);
    pc.printf("Goodbye!");
    throwaway = pc.getc();
    break;
  }
  }
}