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
#include <fcntl.h>
#include <sys/select.h>
#include <time.h>

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
        /* Child process - run the Arduino stuff */

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
    /* Can't have buffered stdout, it ruins stuff! */
    setvbuf(stdout, NULL, _IONBF, 0);

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

    /* Create an array of all of the Arduinos */
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

    /* Get a pseudo-tty for each Arduino */
    int tty_masters[network.num_arduinos];

    for (int i = 0; i < network.num_arduinos; ++i) {
        int master = posix_openpt(O_RDWR);  /* Create the master pty fd */

        if (-1 == master) {
            perror("Could not create pty master");
            exit(EXIT_FAILURE);
        }

        /* Set the mode and owner of the slave of our master pty */
        if (-1 == grantpt(master)) {
            perror("Could not set mode or ownership of pty");
            exit(EXIT_FAILURE);
        }

        /* Unlock the slave pty */
        if (-1 == unlockpt(master)) {
            perror("Could not get slave pty");
            exit(EXIT_FAILURE);
        }

        int flags = fcntl(master, F_GETFL);
        fcntl(master, F_SETFL, flags | O_NONBLOCK);

        /* Now we want to get the device name for the slave pty */
        char *slave_name = ptsname(master);

        if (NULL == slave_name) {
            perror("Could not get name of slave device");
            exit(EXIT_FAILURE);
        }

        /* Save in the masters array */
        tty_masters[i] = master;

        char *name = network.names[i];
        printf("%s on: %s\n", name, slave_name);
    }

    fd_set read_set;
    int max_read = tty_masters[0];

    /* Find the maximum file descriptor */
    for (int i = 0; i < network.num_arduinos; ++i) {
        if (max_read < tty_masters[i]) {
            max_read = tty_masters[i];
        }

        if (max_read < arduinos[i]->from_arduino) {
            max_read = arduinos[i]->from_arduino;
        }
    }

    max_read++;

    while (1) {
        /* Set up the read set */
        FD_ZERO(&read_set);

        for (int i = 0; i < network.num_arduinos; ++i) {
            FD_SET(tty_masters[i], &read_set);
            FD_SET(arduinos[i]->from_arduino, &read_set);
        }

        /* Wait until something happens */
        select(max_read, &read_set, NULL, NULL, NULL);

        /* TTY to Arduino */
        for (int i = 0; i < network.num_arduinos; ++i) {
            if (FD_ISSET(tty_masters[i], &read_set)) {
                char input;
                int bytes_read = read(tty_masters[i], &input, sizeof(input));

                if (-1 == bytes_read) {
                    perror("Error reading from serial");
                    exit(EXIT_FAILURE);
                }

                arduinos[i]->serial_in[0].append(input);
            }
        }

        /* Arduino doing something */
        for (int i = 0; i < network.num_arduinos; ++i) {
            if (FD_ISSET(arduinos[i]->from_arduino, &read_set)) {
                arduinos[i]->run();

                /* 4 is the number of ports on a mega */
                for (int port = 0; i < 4; ++port) {
                    if (arduinos[i]->serial_out[port].available()) {
                        char output = arduinos[i]->serial_out[port].read();

                        if (port == 0) {
                            /* Write to pseudo TTY */
                            write(tty_masters[i], &output, sizeof(output));
                        }

                        /* Find all serial connections */
                        for (int j = 0; j < network.num_serial; ++j) {
                            SerialConnection con = network.serial_ports[j];
                        
                            if (con.in_index == i && con.in_port == 0) {
                                int out = con.out_index;

                                arduinos[out]->serial_in[con.out_port].append(output);
                            }
                            else if (con.out_index == i && con.out_port == 0) {
                                int in = con.in_index;

                                arduinos[in]->serial_in[con.in_port].append(output);
                            }
                        }
                    }
                }
            }
        }

        /* Something happened, so we should try to map all of the pins */
        for (int i = 0; i < network.num_pins; ++i) {
            PinConnection con = network.pins[i];
            int pin_value = arduinos[con.out_index]->pins[con.out_pin];

            /* Write to the input pin */
            arduinos[con.in_index]->pins[con.in_pin] = pin_value;
        }
    }

    free_network(&network);
    return 0;
}
