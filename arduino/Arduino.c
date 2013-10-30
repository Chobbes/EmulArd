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
    write(STDOUT_FILENO, &PINMODE, sizeof(PINMODE));

    /* Send the pin argument, and then the mode */
    write(STDOUT_FILENO, &pin, sizeof(pin));
    write(STDOUT_FILENO, &mode, sizeof(mode));
}


int digitalRead(uint8_t pin) {
    /* Send DIGITAL_READ command */
    write(STDOUT_FILENO, &DIGITAL_READ, sizeof(DIGITAL_READ));

    /* Send the pin number argument */
    write(STDOUT_FILENO, &pin, sizeof(pin));

    /* Receive the integer result */
    int total_read = 0;
    int our_int = 0;

    while (total_read < sizeof(our_int)) {
        char int_part;
        ssize_t bytes_read = read(STDIN_FILENO, &int_part, sizeof(int_part));

        if (1 == bytes_read) {
            /* Stuff the byte we read into our integer */
            our_int << sizeof(int_part);
            our_int |= int_part;

            total_read++;
        }
    }

    return our_int;
}


void digitalWrite(uint8_t pin, int value) {
    /* Send DIGITAL_WRITE command */
    write(STDOUT_FILENO, &DIGITAL_WRITE, sizeof(DIGITAL_WRITE));

    /* Write our integer */
    int shift_amount = (sizeof(value) - sizeof(int_part));

    while (total_sent < sizeof(value)) {
        char int_part = value >> shift_amount;

        write(STDOUT_FILENO, &int_part, sizeof(int_part));

        shift_amount -= sizeof(int_part);
        total_sent++;
    }
}


int analogRead(uint8_t pin) {
    /* Send ANALOG_READ command */
    write(STDOUT_FILENO, &ANALOG_READ, sizeof(ANALOG_READ));

    /* Send the pin number argument */
    write(STDOUT_FILENO, &pin, sizeof(pin));

    /* Receive the integer result */
    int total_read = 0;
    int our_int = 0;

    while (total_read < sizeof(our_int)) {
        char int_part;
        ssize_t bytes_read = read(STDIN_FILENO, &int_part, sizeof(int_part));

        if (1 == bytes_read) {
            /* Stuff the byte we read into our integer */
            our_int << sizeof(int_part);
            our_int |= int_part;

            total_read++;
        }
    }

    return our_int;
}


void analogWrite(uint8_t pin, int value) {
    /* Send DIGITAL_WRITE command */
    write(STDOUT_FILENO, &DIGITAL_WRITE, sizeof(DIGITAL_WRITE));

    /* Write our integer */
    int shift_amount = (sizeof(value) - sizeof(int_part));

    while (total_sent < sizeof(value)) {
        char int_part = value >> shift_amount;

        write(STDOUT_FILENO, &int_part, sizeof(int_part));

        shift_amount -= sizeof(int_part);
        total_sent++;
    }
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
