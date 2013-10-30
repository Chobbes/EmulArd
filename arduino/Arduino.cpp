b/* Copyright (C) 2013 Calvin Beck

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#include "Arduino.h"
#include "commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>


/*
  Implementation of the Arduino functions.
 */

void pinMode(uint8_t pin, uint8_t mode) {
    /* Send PINMODE command identifier */
    ARDUINO_COMMAND(PINMODE);

    /* Send the pin argument, and then the mode */
    ARDUINO_SEND(pin);
    ARDUINO_SEND(mode);
}


int digitalRead(uint8_t pin) {
    /* Send DIGITAL_READ command */
    ARDUINO_COMMAND(DIGITAL_READ);

    /* Send the pin number argument */
    ARDUINO_SEND(pin);

    /* Receive the integer result */
    int total_read = 0;
    int value = 0;
    char *int_buff = (char *) &value;

    while (total_read < sizeof(value)) {
        char int_part;
        ssize_t bytes_read = read(STDIN_FILENO, int_buff, sizeof(int_part) - total_read);

        if (bytes_read > 0) {
            int_buff += bytes_read;
            total_read += bytes_read;
        }
    }

    return value;
}


void digitalWrite(uint8_t pin, int value) {
    /* Send DIGITAL_WRITE command */
    ARDUINO_COMMAND(DIGITAL_WRITE);

    /* Write our integer */
    ARDUINO_SEND(value);
}


int analogRead(uint8_t pin) {
    /* Send ANALOG_READ command */
    ARDUINO_COMMAND(ANALOG_READ);

    /* Send the pin number argument */
    ARDUINO_SEND(PIN);

    /* Receive the integer result */
    int total_read = 0;
    int value = 0;
    char *int_buff = (char *) &value;

    while (total_read < sizeof(value)) {
        char int_part;
        ssize_t bytes_read = read(STDIN_FILENO, int_buff, sizeof(int_part) - total_read);

        if (bytes_read > 0) {
            int_buff += bytes_read;
            total_read += bytes_read;
        }
    }

    return value;
}


void analogWrite(uint8_t pin, int value) {
    /* Send ANALOG_WRITE command */
    ARDUINO_COMMAND(ANALOG_WRITE);

    /* Write our integer */
    ARDUINO_SEND(value);
}


void randomSeed(unsigned int seed) {
    srand(seed);
}


long random(long max) {
    return rand() % max;
}


long random(long min, long max) {
    return (rand() % (max - min)) + min;
}


void delay(unsigned long milliseconds) {
    usleep(milliseconds * 1000);
}


unsigned long micros() {
    return (clock() / CLOCKS_PER_SEC) * 1000000;
}


unsigned long millis() {
    return (clock() / CLOCKS_PER_SEC) * 1000;
}
