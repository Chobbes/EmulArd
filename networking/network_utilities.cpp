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

#include "network_utilities.h"


void write_graph(const char *path, const char *name, ArduinoNetwork *network, ArduinoMega **arduinos, void (*node_print)(FILE*, ArduinoNetwork*, ArduinoMega*, int))
{
    FILE *file = fopen(path, "w");

    fprintf(file, "digraph %s {\n", name);

    /* Declare the nodes */
    for (int i = 0; i < network->num_arduinos; ++i) {
        if (NULL == node_print) {
            fprintf(file, "    %s;\n", network->names[i]);
        }
        else {
            node_print(file, network, arduinos[i], i);
        }
    }

    fprintf(file, "\n");

    /* Create all of the one way digital connections */
    for (int i = 0; i < network->num_pins; ++i) {
        PinConnection con = network->pins[i];

        ArduinoMega *out_arduino = arduinos[con.out_index];
        char *out_name = network->names[con.out_index];

        ArduinoMega *in_arduino = arduinos[con.in_index];
        char *in_name = network->names[con.in_index];

        if (out_arduino->pins[con.out_pin]) {
            /* HIGH value on pin */
            fprintf(file, "    %s -> %s", in_name, out_name);
            fprintf(file, " [label=\" %d->%d\"", con.out_pin, con.in_pin);
            fprintf(file, " color=red];\n");
        }
        else {
            /* LOW value on pin */
            fprintf(file, "    %s -> %s", in_name, out_name);
            fprintf(file, " [label=\" %d->%d\"", con.out_pin, con.in_pin);
            fprintf(file, " color=black];\n");
        }
    }

    fprintf(file, "\n");

    /* Create all of the serial connections */
    for (int i = 0; i < network->num_serial; ++i) {

    }

    fprintf(file, "\n}\n");
    fclose(file);
}
