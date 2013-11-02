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
#include "fakeduino.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/select.h>


void setup();
void loop();

/*
  Main to act as a server for a single arduino.
 */


int main(int argc, char *argv[]) {
    int arduino_in[2];  /* Arduino STDIN pipe */
    int arduino_out[2];  /* Arduino STDOUT pipe */

    if (-1 == pipe(arduino_in)) {
        perror("Could not create input pipe");
        exit(EXIT_FAILURE);
    }

    if (-1 == pipe(arduino_out)) {
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
        close(arduino_in[1]);  /* Close the write end */

        /* Set STDIN to the read end */
        if (-1 == dup2(arduino_in[0], STDIN_FILENO)) {
            perror("Could not set up input pipe");
            exit(EXIT_FAILURE);
        }

        /* Set up the STDOUT pipe */
        close(arduino_out[0]);  /* Close the read end */

        /* Set STDOUT to the write end */
        if (-1 == dup2(arduino_out[1], STDOUT_FILENO)) {
            perror("Could not set up output pipe");
            exit(EXIT_FAILURE);
        }

        /* Run the Arduino stuff */
        setup();

        while (1) {
            loop();
        }
    }

    /* Can't have buffered stdout, it ruins stuff! */
    setvbuf(stdout, NULL, _IONBF, 0);

    /* Child process - set up Arduino server */
    ArduinoMega mega(arduino_in[1], arduino_out[0]);
    int prev_light = 0;

    /* Set up a PTTY so we can connect to our Arduino! */
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

    /* Now we want to get the device name for the slave pty */
    char *slave_name = ptsname(master);

    if (NULL == slave_name) {
        perror("Could not get name of slave device");
        exit(EXIT_FAILURE);
    }

    printf("Arduino on: %s\n", slave_name);

    fd_set read_set;
    int max_read = 1 + (master > mega.from_arduino ? master : mega.from_arduino);

    while (1) {
        /* Set up the read set */
        FD_ZERO(&read_set);

        FD_SET(master, &read_set);
        FD_SET(mega.from_arduino, &read_set);

        /* Wait until something happens */
        select(max_read, &read_set, NULL, NULL, NULL);

        if (FD_ISSET(master, &read_set)) {
            char input;
            int bytes_read = read(master, &input, sizeof(input));

            if (-1 == bytes_read) {
                perror("Error reading from serial");
                exit(EXIT_FAILURE);
            }

            mega.serial_in.append(input);
        }

        if (FD_ISSET(mega.from_arduino, &read_set)) {
            mega.run();

            if (mega.serial_out.available()) {
                char output = mega.serial_out.read();
                write(master, &output, sizeof(output));
            }
        }
    }

    return 0;
}
    
