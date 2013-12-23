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

#ifndef NETWORK_UTILITIES_H
#define NETWORK_UTILITIES_H

#include "network_parse.h"
#include <emulard/fakeduino.h>


/*
  Function to write a .dot file containing network information.

  Arguments:
  path: Path for the file that we want to write to.
  name: Name of the graph in output .dot file.
  network: Arduino network that we want a graph for.
  arduinos: An array of all of the Arduinos in the network.
  node_print: NULL, or a function for how to print an Arduino
              node in the dot file.

  For digital pins black is a LOW value, and red is a HIGH
  value. Serial connections are blue edges, so it's easy to
  differentiate between them and the digital pins.
 */

void write_graph(const char *path, const char *name, ArduinoNetwork *network, ArduinoMega **arduinos, void (*node_print)(FILE*, ArduinoNetwork*, ArduinoMega*, int));


#endif
