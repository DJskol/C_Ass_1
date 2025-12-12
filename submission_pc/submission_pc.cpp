#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <time.h>
#include <string>
#include "rs232.h"
#include "file_hanlder.c"

#define BUFFER_SIZE 256
#define RECONNECTION_TIMER 1000
#define TIMEOUT 10

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

using namespace std;
/*
void wait_ms(int ms){
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}
*/
void system_clear(){
    #ifdef _WIN32
        system("cls");  // Windows
    #else
        system("clear");  // Linux/macOS
    #endif
    }

typedef struct {
    int port;
    int baud_rate;
    char mode[4];
    unsigned char buffer[BUFFER_SIZE];
} Serial;

void reconnect(Serial *serial, int delay_ms) {
    puts("Disconnected. Attempting to reconnect every 1 second.");
    do {
        wait_ms(delay_ms);
        try {
            RS232_CloseComport(serial->port);
        } catch (int e) { }
    } while (RS232_OpenComport(serial->port, serial->baud_rate, serial->mode, 0));
    puts("\nReconnected successfully.\n");
}

int read_serial(Serial *serial, int timeout_ms) {
    int connection_status = 0;
    while (connection_status <= 0) {
        if (!RS232_IsConnected(serial->port) || connection_status == -1) {
            reconnect(serial, RECONNECTION_TIMER);
        }
        connection_status = RS232_PollComport(serial->port, serial->buffer, BUFFER_SIZE - 1);
        int last_connection_status = 1;
        while (connection_status && last_connection_status)
        {
            wait_ms(timeout_ms);
            last_connection_status = RS232_PollComport(serial->port, serial->buffer + connection_status, BUFFER_SIZE - 1 - connection_status);
            connection_status += last_connection_status;
        }
    }
    serial->buffer[connection_status] = 0;
    return connection_status;
}


int main(){
    //initialize vaariables
    int record;
    char pc_cache[2000000] = " "; //record storage
    char message[BUFFER_SIZE];

    Serial serial;
    serial.port = 3;
    serial.baud_rate = 9600;
    strcpy(serial.mode, "8N1");
    memset(serial.buffer, 0, BUFFER_SIZE);

    if (RS232_OpenComport(serial.port, serial.baud_rate, serial.mode, 0)) {
        char error_msg[128];
        RS232_GetLastErrorMsg(error_msg,true);
        printf("Error: %s\n", error_msg);
        return 0;
    }
    puts("Port opened, waiting for ARM Mbed...");


    RS232_flushRXTX(serial.port);

    while (1) {
        int buffer_bytes = read_serial(&serial, TIMEOUT);

        char* buffer = (char *)serial.buffer;
        //The following if else statements are commands/keywords for specific functions
        if(!strcmp(buffer, "pc_requested")){//determines if input is needed
            memset(message,'\0', sizeof(message));
            printf(" ");
            fgets(message,BUFFER_SIZE,stdin);//gets user input

            if(record){
                strcat(pc_cache, message);  //records user input if recording is active
            }

            RS232_cputs(serial.port, message);//send message
            wait_ms(50);
            RS232_cputs(serial.port, "*");//send end bit
            wait_ms(50);
            RS232_flushTX(serial.port);
        }else if(!strcmp(buffer, "system_clr")){//clears the pc terminal
            system_clear();
            wait_ms(10);
            RS232_flushRX(serial.port);
            RS232_cputs(serial.port, "*");//send end bit
        }else if(!strcmp(buffer, "system_load")){//loads previous replay saves
            replay_files();
            wait_ms(4000);
            RS232_cputs(serial.port, "*");//send end bit
            wait_ms(50);
            RS232_flushTX(serial.port);
        }else if(!strcmp(buffer, "system_save")){//save the pc_cache replay
            save_replay(pc_cache);
            RS232_flushRX(serial.port);
            RS232_cputs(serial.port, "*");//send end bit
            wait_ms(50);
            RS232_flushTX(serial.port);
        }else{
            if(!strcmp(buffer,"\n\t[-- GAME START --]\n")){//starts recording
                record = 1;
                memset(pc_cache, '\0', sizeof(pc_cache));//set the recording data to 0
            }
            if(record){
                strcat(pc_cache, buffer);//records text
            }
            if(!strcmp(buffer,"\n\t[-- GAME END --]\n")){
                record = 0;
                memset(pc_cache, '\0', sizeof(pc_cache));//set the recording data to 0
            }
            if(!strcmp(buffer, "Goodbye!")){
                wait_ms(50);
                RS232_cputs(serial.port, "*");//send end bit
                RS232_flushRX(serial.port);
                break;
            }

            printf("%s", buffer);//display message
            wait_ms(10);
            RS232_flushRX(serial.port);
            RS232_cputs(serial.port, "*");//send end bit
        }
    }

    RS232_CloseComport(serial.port);//close the serial port

    return 0;
}
