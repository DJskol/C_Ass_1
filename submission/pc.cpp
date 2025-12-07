/**************************************************
File: pc.cpp
Purpose: Simple demo that tests UART connection with Mbed.
Functionality:
    Opens a serial port and waits for Mbed to send a message.
    When a message is received, it sends a response to Mbed.
    Continues receiving/sending messages until "exit" is received from Mbed.
Commands to compile and run (only if compilation was successful):
    Linux:
        g++ pc.cpp rs232.c -o pc && ./pc
    Windows powershell:
        g++ pc.cpp rs232.c -o pc.exe; if ($?) { .\pc.exe }
    Windows command prompt:
        g++ pc.cpp rs232.c -o pc.exe && .\pc.exe
Exit the program by pressing Ctrl-C (may require resetting the board on Linux to take effect)
**************************************************/
#include <stdlib.h>
#include <stdio.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include "rs232.h"

#define BUFFER_SIZE 256

// This strucure will help supplying serial port data to functions
// easily by passing single "Serial *" argument instead of many args.
typedef struct {
    int port;
    int baud_rate;
    char mode[4];
    unsigned char buffer[BUFFER_SIZE];
} Serial;

void wait_ms(int ms);
int read_serial(Serial *serial, int timeout_ms=10);
void reconnect(Serial *serial, int delay_ms=1000);

int main() {
    // Initialize serial port data
    Serial serial;
    serial.port = 4;
    // serial.port = RS232_GetPortnr("ttyACM0");
    serial.baud_rate = 9600;
    strcpy(serial.mode, "8N1");
    memset(serial.buffer, 0, BUFFER_SIZE);

    if (RS232_OpenComport(serial.port, serial.baud_rate, serial.mode, 0)) {
        char error_msg[128];
        RS232_GetLastErrorMsg(error_msg, /* clear_error*/ true);
        printf("Error: %s\n", error_msg);
        return 0;
    }
    puts("Port opened, waiting for ARM Mbed...");

    // receive messages from Mbed until "exit" is received
    while (strcmp((char*)serial.buffer, "exit")) {
        int n = read_serial(&serial);
        printf("Received: %d bytes\n", n);
        printf("Mbed: %s", (char *)serial.buffer);

        // send response to Mbed
        char hello_msg[] = "Hello Mbed!!!\n";
        RS232_cputs(serial.port, hello_msg);
        printf("Sending: %s\n", hello_msg);
    }

    printf("\"exit\" received. Closing port.\n");
    // close and quit
    RS232_CloseComport(serial.port); // close the port
    return 0;
}


void reconnect(Serial *serial, int delay_ms) {
    puts("Disconnected. Attempting to reconnect every 1 second.");
    do {
        wait_ms(delay_ms);
        // on Linux, without closing the port, it wouldn't reconnect
        try {
            RS232_CloseComport(serial->port);
        } catch (int e) { }
    } while (RS232_OpenComport(serial->port, serial->baud_rate, serial->mode, 0));
    puts("Reconnected successfully.\n");
}

// read_serial waits until data is available on the serial port and allows
// additional "timeout_ms" milliseconds for more data to arrive after the
// last byte received.
//     (this fixed the issue in the past where a single message from Pico was
//     delivered as 2 separate messages due to unexpected delay shortly after
//     turning the board on)
int read_serial(Serial *serial, int timeout_ms) {
    int n = 0;
    while (n <= 0) {
        if (!RS232_IsConnected(serial->port) || n == -1) {
            reconnect(serial);
        }
        n = RS232_PollComport(serial->port, serial->buffer, BUFFER_SIZE - 1);
        int last_n = 1;
        while (n && last_n)
        {
            wait_ms(timeout_ms);
            last_n = RS232_PollComport(serial->port, serial->buffer + n, BUFFER_SIZE - 1 - n);
            n += last_n;
        }
    }
    serial->buffer[n] = 0;
    return n;
}


void wait_ms(int ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}
