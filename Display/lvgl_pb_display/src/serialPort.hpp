// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <sys/ioctl.h> 

class SerialPort {
    public:
    SerialPort(const char *path, int baudRate) {
        openPort(path, baudRate);
    }

    void openPort(const char *path, int baudRate) {
        // Open the serial port.
        _port = open(path, O_RDWR);

        if (_port == -1) {
            printf("Error opening port %s\n", path);
            return;
        }

        // Create new termios struct, we call it 'tty' for convention
        struct termios tty;

        // Read in existing settings, and handle any error
        if(tcgetattr(_port, &tty) != 0) {
            printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
            return;
        }

        tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
        tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
        tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
        tty.c_cflag |= CS8; // 8 bits per byte (most common)
        tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
        tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

        tty.c_lflag &= ~ICANON;
        tty.c_lflag &= ~ECHO; // Disable echo
        tty.c_lflag &= ~ECHOE; // Disable erasure
        tty.c_lflag &= ~ECHONL; // Disable new-line echo
        tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
        tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

        tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
        tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
        // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
        // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

        tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
        tty.c_cc[VMIN] = 0;

        // Set in/out baud rate
        cfsetispeed(&tty, baudRate);
        cfsetospeed(&tty, baudRate);

        // Save tty settings, also checking for error
        if (tcsetattr(_port, TCSANOW, &tty) != 0) {
            printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
            return;
        }
    }

    // Returns number of bytes available in RX buffer
    int available() {
        if (_port == -1) return 0;  // return 0 if no port is open, so program doesn't freeze
        int bytes;
        ioctl(_port, FIONREAD, &bytes);
        return bytes;
    }

    // read single byte
    char readByte() {
        char byte;
        read(_port, &byte, 1);
        return byte;
    }

    // read number of bytes into destination buffer returning actually read amount
    int readBytes(uint8_t *dest, size_t len) {
        return read(_port, dest, len);
    }

    // write bytes, return amount written
    int writeBytes(uint8_t *buf, size_t len) {
        return write(_port, buf, len);
    }

    void closePort() {
        close(_port);
    }

    private:
    int _port = -1;
};