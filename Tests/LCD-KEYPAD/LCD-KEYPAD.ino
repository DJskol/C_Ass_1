#include "mbed.h"
#include "USBSerial.h"
#include "TextLCD/TextLCD.cpp"
#define WAIT_US 1000000 // multiplier to fix use of wait_us instead of wait 

using namespace mbed;
USBSerial pc; // tx, rx
TextLCD lcd(p0, p1, p12, p13, p14, p15);
BusOut rows(p20, p19, p18);
BusIn cols(p11,p10,p9,p8);
char Keytable[] = { 'F', 'E', 'D', 'C', 
'3', '6', '9', 'B', 
'2', '5', '8', '0', 
'1', '4', '7', 'A'
};
char getKey(){ 
int i,j; 
char ch=' ';
for (i = 0; i <= 3; i++) { 
rows = i; 
for (j = 0; j <= 3; j++) { 
if (((cols ^ 0x00FF) & (0x0001<<j)) != 0) { 
ch = Keytable[(i * 4) + j]; 
}
} 
} 
return ch;
}
int main() { 
char key = ' '; 
while(1) {
key = getKey();
if(key != ' '){
lcd.cls();
lcd.locate(0,0);
lcd.printf("Key = %c", key);
wait_us(0.3*WAIT_US);
}
}
}