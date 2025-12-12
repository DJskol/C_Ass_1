#include "led.h"
#define WAIT_US_FLASH 100000 //default flash time
//setup and turns the lights RED GREEN
void led_setup(){
    sw.format(8,0);
    sw.frequency(1000000);
    cs = 0;             //sets the chip select to the switches (enter write operation)
    sw.write(0x00FF);   //controls the first 4 lights
    sw.write(0x00FF);   //controls the last 4 lights
    cs = 1;             //enter read mode
}
//flashes red to simulate rejected or wrong inputs
void denied_flash(){
    for(int i = 0; i < 2; i++){
        cs = 0;
        sw.write(0x0055);
        sw.write(0x0055);
        cs = 1;
        wait_us(WAIT_US_FLASH);
        cs = 0;
        sw.write(0x0000);
        sw.write(0x0000);
        cs = 1;
        wait_us(WAIT_US_FLASH);
    }
}
//flashes red to simulate accepted or corrects inputs
void accepted_flash(){
    for(int i = 0; i < 2; i++){
        cs = 0;
        sw.write(0x00AA);
        sw.write(0x00AA);
        cs = 1;
        wait_us(WAIT_US_FLASH);
        cs = 0;
        sw.write(0x0000);
        sw.write(0x0000);
        cs = 1;
        wait_us(WAIT_US_FLASH);
    }
}