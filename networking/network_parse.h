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

#ifndef NETWORK_PARSE_H
#define NETWORK_PARSE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


typedef struct PinConnection {
    size_t out_index;
    size_t in_index;

    uint8_t out_pin;
    uint8_t in_pin;
} PinConnection;


typedef struct SerialConnection {
    size_t out_index;
    size_t in_index;

    int out_port;
    int in_port;
} SerialConnection;


typedef struct ArduinoNetwork {
    char **names;  /* Names of the Arduinos corresponding to the given index */
    char **paths;  /* Path to the executable for an Arduino at a given index */
    size_t num_arduinos;

    SerialConnection *serial_ports;
    size_t num_serial;

    PinConnection *pins;
    size_t num_pins;
} ArduinoNetwork;


/* Returned struct contains malloc'd memory */
ArduinoNetwork parse_network(FILE *ard_file);
void free_network(ArduinoNetwork *network);

#endif
