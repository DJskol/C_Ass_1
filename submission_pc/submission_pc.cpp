#include <stdlib.h>
#include <stdio.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include "rs232.h"

#define BUFFER_SIZE 256
#define RECONNECTION_TIMER 1000

void wait_ms(int ms){
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
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
    puts("Reconnected successfully.\n");
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

    char pc_input[BUFFER_SIZE];

    while (strcmp((char*)serial.buffer, "exit")) {
        printf("%s\n", (char *)serial.buffer);
        scanf("%s", pc_input);
        RS232_cputs(serial.port, pc_input);
    }

    printf("\"exit\" received. Closing port.\n");

    RS232_CloseComport(serial.port);

    return 0;
}
