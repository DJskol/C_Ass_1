#include "switches.h"
//This gets all the on and off switch in an array to represents in binary formation [0,0,0,0,0,0,0,0]
void get_switches(int* switches_array){
  bus_out = 4;
  int switches = bus_in;
  bus_out = 5;
  switches = switches*16 + bus_in; //reads the binary summation of all active switches

  for(int i = 0; i<=7; i++){
    int bit_power = pow(2,i);//uses the switches postion on the binary scale
    
    //if the switch can subtract and leave a remainder less than itself then it is on
    if(bit_power <= switches && !(bit_power <= (switches-bit_power))){ 
      switches_array[8-i] = 1;
      switches -= bit_power;
    }else{
      switches_array[8-i] = 0; //else its off
    }
  }
}