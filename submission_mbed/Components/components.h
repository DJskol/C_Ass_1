#ifndef COMPONENTS_H
#define COMPONENTS_H
//Handles all the peripherals of the mbed board
#include "mbed.h"
#include "USBSerial.h"

using namespace mbed;

USBSerial pc;
//output and input pins of each component add on
DigitalOut cs(p5);
SPI sw(p3, NC, p2);
PwmOut buzzer(p27);
BusOut bus_out(p20, p19, p18);
BusIn bus_in(p11,p10,p9,p8);
TextLCD lcd(p0, p1, p12, p13, p14, p15);
DigitalOut myled(p21);


#endif