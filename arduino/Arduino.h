/* Copyright (C) 2013 Calvin Beck

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

#ifndef ARDUINO_H
#define ARDUINO_H

#include <stdint.h>


/* Values for pin reads and writes */
const int HIGH = 1;
const int LOW = 0;

/* Values for pin modes */
const uint8_t INPUT = 0;
const uint8_t OUTPUT = 1;
const uint8_t INPUT_PULLUP = 2;

/* Useless pinMode function! */
void pinMode(uint8_t pin, uint8_t mode);

/* Digital read and write functions */
int digitalRead(uint8_t pin);
void digitalWrite(uint8_t pin, int value);

/* Analog read and write functions */
int analogRead(uint8_t pin);
void analogWrite(uint8_t pin, int value);

/* Random functions */
void randomSeed(unsigned int);
long random(long max);
long random(long min, long max);

/* Timing functions */
void delay(unsigned long milliseconds);
unsigned long micros();
unsigned long millis();

#endif
