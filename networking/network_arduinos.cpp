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

#include <Arduino.h>
#include <emulard/fakeduino.h>

#include "network_parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


pid_t launch_arduino(char *name, char *path, int *in_pipe, int *out_pipe) {
    if (-1 == pipe(in_pipe)) {
        perror("Could not create input pipe");
        exit(EXIT_FAILURE);
    }

    if (-1 == pipe(out_pipe)) {
        perror("Could not create output pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == 0) {
        /* Parent process - run the Arduino stuff */

        /* Need binary mode badly! */
        freopen(NULL, "wb", stdout);
        freopen(NULL, "rb", stdin);

        /* Set up the STDIN pipe */
        close(in_pipe[1]);  /* Close the write end */

        /* Set STDIN to the read end */
        if (-1 == dup2(in_pipe[0], STDIN_FILENO)) {
            perror("Could not set up input pipe");
            exit(EXIT_FAILURE);
        }

        /* Set up the STDOUT pipe */
        close(out_pipe[0]);  /* Close the read end */

        /* Set STDOUT to the write end */
        if (-1 == dup2(out_pipe[1], STDOUT_FILENO)) {
            perror("Could not set up output pipe");
            exit(EXIT_FAILURE);
        }

        /* Run the Arduino program */
        char *argv[] = {name, "-c", NULL};
        execvp(path, argv);

        fprintf(stderr, "Invalid Arduino program, \"%s\"\n", path);
        exit(EXIT_FAILURE);
    }

    return pid;
}


void usage(char *program_name)
{
    fprintf(stderr, "Usage: %s <input file>.ard\n", program_name);
}


int main(int argc, char *argv[])
{
    if (2 != argc) {
        fprintf(stderr, "Invalid number of arguments!\n");
        usage(argv[0]);

        return 1;
    }

    /* Open the network file and check that it is valid */
    FILE *ard_file = fopen(argv[1], "r");

    if (NULL == ard_file) {
        fprintf(stderr, "No such file: \"%s\"\n", argv[1]);
        usage(argv[0]);

        return 1;
    }

    ArduinoNetwork network = parse_network(ard_file);
    print_network(&network);

    int arduino_in[2];  /* Arduino STDIN pipe */
    int arduino_out[2];  /* Arduino STDOUT pipe */

    /* Our list of all of the Arduinos */
    ArduinoMega *arduinos[network.num_arduinos];

    for (int i = 0; i < network.num_arduinos; ++i) {
        /* Need the name and path for the Arduino process */
        char *name = network.names[i];
        char *path = network.paths[i];

        /* Launch our fake Arduino processes */
        launch_arduino(name, path, arduino_in, arduino_out);

        /* Make an entry in the giant arduino array! */
        arduinos[i] = new ArduinoMega(arduino_in[1], arduino_out[0]);
    }

    /* Can't have buffered stdout, it ruins stuff! */
    setvbuf(stdout, NULL, _IONBF, 0);

    while(1) {}

    return 0;
}
