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

#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>

/*
  This file specifies some of the command codes for the EmulArd
  protocal.

 */


/* Commands sent from the "Arduino" to the server. */
const uint8_t SERIAL_BEGIN = 1;
const uint8_t SERIAL_WRITE = 2;
const uint8_t SERIAL_READ = 3;
const uint8_t SERIAL_AVAILABLE = 4;
const uint8_t DIGITAL_WRITE = 5;
const uint8_t DIGITAL_READ = 6;
const uint8_t ANALOG_WRITE = 7;
const uint8_t ANALOG_READ = 8;
const uint8_t PINMODE = 9;

#endif
