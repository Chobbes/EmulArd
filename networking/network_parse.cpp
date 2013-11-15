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

#include "network_parse.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>


/* Returns 1 for ' ', '\t', or '\n', and 0 otherwise */
static int is_whitespace(char c)
{
    switch (c) {
    case ' ':
        return 1;
    case '\t':
        return 1;
    case '\n':
        return 1;
    default:
        return 0;
    }
}

/* Returns 1 for the comment character '#', and 0 otherwise */
static int is_comment(char c)
{
    return '#' == c;
}


/* Returns 1 for the separator character ':', and 0 otherwise */
static int is_separator(char c)
{
    return ':' == c;
}


/* Returns 1 for the newline character '\n', and 0 otherwise */
static int is_newline(char c)
{
    return '\n' == c;
}


/* Returns -1 if it is not a valid digit, ['0'-'9'] */
static int char_digit_value(char c)
{
    int value = c - '0';

    if (value < 0 || value > 9) {
        return -1;
    }

    return value;
}


/* Skip the entire line, useful when a comment is encountered */
static void skip_line(FILE *ard_file)
{
    int character = fgetc(ard_file);

    while (!is_newline(character)) {
        if (feof(ard_file)) {
            return;
        }

        character = fgetc(ard_file);
    }
}


/*
  Function to seek (moves the file position indicator of the .ard file)
  past all whitespace / comments from the current position. Note that
  you have to start on whitespace, or a comment character '#' in order
  for this to actually move forward.
*/

static void skip_aesthetics(FILE *ard_file)
{
    int character = fgetc(ard_file);

    while (is_whitespace(character) || is_comment(character)) {
        if (is_comment(character)) {
            skip_line(ard_file);
            skip_aesthetics(ard_file);
            return;
        }
        else if (EOF == character) {
            return;
        }

        character = fgetc(ard_file);
    }

    if (!feof(ard_file)) {
        /* Put the non-whitespace / comment character back */
        ungetc(character, ard_file);
    }
}


static int parse_integer(FILE *ard_file)
{
    int character = fgetc(ard_file);
    int digit_value = char_digit_value(character);
    int total_value = 0;

    while (-1 != digit_value) {
        total_value *= 10;
        total_value += digit_value;

        character = fgetc(ard_file);
        digit_value = char_digit_value(character);
    }

    return total_value;
}


/* Allocates memory! */
static char *parse_identifier(FILE *ard_file)
{
    size_t max_length = 64;

    char *identifier = (char *)malloc(max_length);
    size_t length = 0;

    while (1) {
        int character = fgetc(ard_file);

        if (is_comment(character) || is_whitespace(character) || is_separator(character)) {
            if (max_length <= length) {
                max_length = length + 1;
                identifier = (char *)realloc(identifier, max_length);
            }

            identifier[length] = '\0';

            return identifier;
        }
        else {
            if (max_length <= length) {
                max_length *= 2;
                identifier = (char *)realloc(identifier, max_length);
            }

            identifier[length] = character;
            ++length;
        }
    }
}


static int parse_declaration(FILE *ard_file, ArduinoNetwork *network)
{
    skip_aesthetics(ard_file);
    char *name = parse_identifier(ard_file);

    skip_aesthetics(ard_file);
    char *path = parse_identifier(ard_file);

    network->names = (char **)realloc(network->names, network->num_arduinos + sizeof(network->names[0]));
    network->paths = (char **)realloc(network->paths, network->num_arduinos + sizeof(network->paths[0]));

    network->names[network->num_arduinos] = name;
    network->paths[network->num_arduinos] = path;

    ++network->num_arduinos;

    return 0;
}


static int arduino_lookup(char *name, ArduinoNetwork *network)
{
    for (int i = 0; i < network->num_arduinos; ++i) {
        if (0 == strcmp(name, network->names[i])) {
            return i;
        }
    }

    return -1;
}


static int parse_pin(FILE *ard_file, ArduinoNetwork *network)
{
    /* Fetch all of the fields */
    skip_aesthetics(ard_file);
    char *out_name = parse_identifier(ard_file);

    skip_aesthetics(ard_file);
    int out_pin = parse_integer(ard_file);

    skip_aesthetics(ard_file);
    char *in_name = parse_identifier(ard_file);

    skip_aesthetics(ard_file);
    int in_pin = parse_integer(ard_file);

    /* Make the new connection */
    PinConnection connection;

    connection.out_index = arduino_lookup(out_name, network);
    connection.out_pin = out_pin;

    connection.in_index = arduino_lookup(in_name, network);
    connection.in_pin = in_pin;

    /* Free the names - we don't need to keep them */
    free(out_name);
    free(in_name);

    /* Add the connection to the network */
    network->pins = (PinConnection *)realloc(network->pins, network->num_pins + sizeof(network->pins[0]));
    network->pins[network->num_pins] = connection;

    ++network->num_pins;

    return 0;
}


static int parse_serial(FILE *ard_file, ArduinoNetwork *network)
{
    /* Fetch all of the fields */
    skip_aesthetics(ard_file);
    char *out_name = parse_identifier(ard_file);

    skip_aesthetics(ard_file);
    int out_port = parse_integer(ard_file);

    skip_aesthetics(ard_file);
    char *in_name = parse_identifier(ard_file);

    skip_aesthetics(ard_file);
    int in_port = parse_integer(ard_file);

    /* Make the new connection */
    SerialConnection connection;

    connection.out_index = arduino_lookup(out_name, network);
    connection.out_port = out_port;

    connection.in_index = arduino_lookup(in_name, network);
    connection.in_port = in_port;

    /* Free the names - we don't need to keep them */
    free(out_name);
    free(in_name);

    /* Add the connection to the network */
    network->serial_ports = (SerialConnection *) realloc(network->serial_ports, network->num_serial + sizeof(network->serial_ports[0]));
    network->serial_ports[network->num_serial] = connection;

    ++network->num_serial;

    return 0;
}


static int parse_entry(FILE *ard_file, ArduinoNetwork *network)
{
    int character = fgetc(ard_file);

    switch (character) {
    case 'd':
        return parse_declaration(ard_file, network);
    case 'p':
        return parse_pin(ard_file, network);
    case 's':
        return parse_serial(ard_file, network);
    default:
        return -1;
    }

    return 0;
}


ArduinoNetwork parse_network(FILE *ard_file)
{
    ArduinoNetwork network;

    /* Initialize the network structure so we can try to fill it */
    network.names = NULL;
    network.paths = NULL;
    network.num_arduinos = 0;

    network.serial_ports = NULL;
    network.num_serial = 0;

    network.pins = NULL;
    network.num_pins = 0;

    /* First let's skip past all of the whitespace / comments */
    skip_aesthetics(ard_file);

    /* Check if we ran out of file! */
    while (!feof(ard_file)) {
        /* Should be at an entry with identifying character - d, p, or s */
        parse_entry(ard_file, &network);

        /* Now skip ahead to the next entry */
        skip_aesthetics(ard_file);
    }

    return network;
}


void free_network(ArduinoNetwork *network)
{
    /* Free all of the paths and names */
    for (int i = 0; i < network->num_arduinos; ++i) {
        free(network->names[i]);
        free(network->paths[i]);
    }

    /* Now free the arrays */
    free(network->names);
    free(network->paths);

    free(network->serial_ports);
    free(network->pins);

    network->names = NULL;
    network->paths = NULL;
    network->serial_ports = NULL;
    network->pins = NULL;

    network->num_arduinos = 0;
    network->num_serial = 0;
    network->num_pins = 0;
}
