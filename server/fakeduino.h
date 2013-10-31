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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "commands.h"


/*
  Need a circular buffer for the serial ports.
 */
class SerialBuffer {
 private:
    uint8_t serial_buffer[64];

    size_t start;
    size_t end;

 public:
    SerialBuffer() {
        start = 0;
        end = 0;
    }

    int available() {
        return abs(start - end);
    }

    int append(uint8_t value) {
        if (this->available() < sizeof(serial_buffer)) {
            serial_buffer[end] = value;
            end = (end + 1) % sizeof(serial_buffer);

            return 0;
        }
        else {
            return -1;
        }
    }

    int peek() {
        if (!this->available()) {
            return -1;
        }

        return serial_buffer[start];
    }

    int read() {
        if (!this->available()) {
            return -1;
        }

        uint8_t value = serial_buffer[start];
        start = (start + 1) % sizeof(serial_buffer);

        return value;
    }
};


/*
  All Arduinos have digital pins, modes for the digital pins, and
  analog pins.

  Each Arduino also has at least one serial port.
 */

class ArduinoMega {
 public:
    /* Pins - analog and digital are in the same array */
    int pins[70];
    uint8_t pin_modes[70];

    /* Serial buffers for the different ports */
    SerialBuffer serial_buffer;
    unsigned long serial_baud;
    
    SerialBuffer serial1_buffer;
    unsigned long serial1_baud;

    SerialBuffer serial2_buffer;
    unsigned long serial2_baud;

    SerialBuffer serial3_buffer;
    unsigned long serial3_baud;

    /* Pipes for talking to the Arduino process */
    int to_arduino;
    int from_arduino;

    ArduinoMega(int to, int from) {
        this->to_arduino = to;
        this->from_arduino = from;
    }

    /* May be none-blocking */
    void run() {
        /* Read command for dispatching */
        uint8_t command = receive_char(from_arduino);

        if (command) {
            /* Perform the appropriate action for the command */
            switch (command) {
            case SERIAL_BEGIN:
                this->serial_begin();
                break;
            case SERIAL_WRITE:
                this->serial_write();
                break;
            case DIGITAL_WRITE:
                this->digital_write();
                break;
            case DIGITAL_READ:
                this->digital_read();
                break;
            case ANALOG_WRITE:
                this->analog_write();
                break;
            case ANALOG_READ:
                this->analog_read();
                break;
            case PIN_MODE:
                this->pin_mode();
                break;
            default:
                break;
            }
        }
    }

    void serial_begin() {
        unsigned int port = receive_int(from_arduino);
        unsigned long baud_rate = receive_long(from_arduino);

        printf("Port: %u  --  Baud: %lu\n", port, baud_rate);

        switch (port) {
        case 0:
            serial_baud = baud_rate;
            break;
        case 1:
            serial1_baud = baud_rate;
            break;
        case 2:
            serial2_baud = baud_rate;
            break;
        case 3:
            serial3_baud = baud_rate;
            break;
        }
    }

    void serial_write() {
        unsigned int port = receive_int(from_arduino);
        char value = receive_char(from_arduino);

        switch (port) {
        case 0:
            serial_buffer.append(value);
            break;
        case 1:
            serial1_buffer.append(value);
            break;
        case 2:
            serial2_buffer.append(value);
            break;
        case 3:
            serial3_buffer.append(value);
            break;
        }
    }

    void digital_write() {
        uint8_t pin = receive_char(from_arduino);
        uint8_t value = receive_char(from_arduino);

        if (pin >= sizeof(pins) / sizeof(pins[0])) {
            return;
        }

        if (pin >= 54) {
            pins[pin] = value ? 1023 : 0;
        }
        else {
            pins[pin] = value ? 1 : 0;
        }
    }

    void digital_read() {
        uint8_t pin = receive_char(from_arduino);
        int value = 0;

        if (pin < sizeof(pins) / sizeof(pins[0])) {
            value = pins[pin] ? 1 : 0;
        }

        FD_SEND(to_arduino, value);
    }

    void analog_write() {
        uint8_t pin = receive_char(from_arduino);
        int value = receive_int(from_arduino);

        if (pin >= 0 && pin < 16) {
            pins[pin + 54] = value;
        }
    }

    void analog_read() {
        uint8_t pin = receive_char(from_arduino);
        int value = 0;

        if (pin >= 0 && pin < 16) {
            value = pins[pin + 54];
        }

        FD_SEND(to_arduino, value);
    }

    void pin_mode() {
        uint8_t pin = receive_char(from_arduino);
        uint8_t mode = receive_char(from_arduino);

        if (pin < sizeof(pin_modes) / sizeof(pin_modes[0])) {
            pin_modes[pin] = mode;
        }
    }
};
