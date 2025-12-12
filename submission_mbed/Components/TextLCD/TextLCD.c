#include "TextLCD.cpp" //includes the LCD file
#include "../components.h"//includes the ports file


#define WAIT_US 1000000

//This function clears the lcd and prints new text for a certain amount of time
void lcd_full_display(char* line1_text, char* line2_text, float time){ 
    lcd.cls();  //clear lcd
    lcd.locate(0,0);
    lcd.printf(line1_text);//print Top Line
    lcd.locate(0,1);
    lcd.printf(line2_text);//print Bottom Line
    wait_us(WAIT_US * time);
}
//This function prints new text on the top line at a specified column
void lcd_1stline_display(char* line1_text, int line_index){
    lcd.locate(line_index,0); //goes to specific column
    lcd.printf(line1_text);
}
//This function prints new text on the bottom line at a specified column
void lcd_2ndline_display(char* line2_text, int line_index){
    lcd.locate(line_index,1);
    lcd.printf(line2_text);
}