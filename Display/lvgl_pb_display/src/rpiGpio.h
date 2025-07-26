#pragma once

// Vibe coded, because there appears to be no simple single-header C++ gpio library?
// wiringpi is deprecated and pigpio _always_ needs root, even for the simple gpio calls 

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>
#include <string>
#include <stdexcept>
#include <vector>
#include <chrono>
#include <thread>

namespace RpiGpio {

class GpioPin {
private:
    int chip_fd;
    int line_fd;
    int pin_number;
    bool is_output;
    bool is_claimed;

public:
    enum Direction {
        INPUT = 0,
        OUTPUT = 1
    };

    enum Pull {
        NONE = 0,
        UP = 1,
        DOWN = 2
    };

    // Constructor
    GpioPin(int pin, Direction dir = INPUT, const std::string& consumer = "RpiGpio") 
        : chip_fd(-1), line_fd(-1), pin_number(pin), is_output(dir == OUTPUT), is_claimed(false) {
        
        // Open GPIO chip (usually gpiochip0 on Raspberry Pi)
        chip_fd = open("/dev/gpiochip0", O_RDONLY);
        if (chip_fd < 0) {
            // throw std::runtime_error("Failed to open /dev/gpiochip0. Make sure you're in the 'gpio' group.");

            // Just print this error for now, for easier simulation on non-raspi targets
            printf("Failed to open /dev/gpiochip0. Make sure you're in the 'gpio' group.");
            return;
        }

        // Prepare line request
        struct gpiohandle_request req;
        req.lineoffsets[0] = pin_number;
        req.lines = 1;
        req.flags = (dir == OUTPUT) ? GPIOHANDLE_REQUEST_OUTPUT : GPIOHANDLE_REQUEST_INPUT;
        
        // Copy consumer string
        strncpy(req.consumer_label, consumer.c_str(), sizeof(req.consumer_label) - 1);
        req.consumer_label[sizeof(req.consumer_label) - 1] = '\0';

        // Default values for output pins
        if (dir == OUTPUT) {
            req.default_values[0] = 0;
        }

        // Request the line
        if (ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &req) < 0) {
            close(chip_fd);
            throw std::runtime_error("Failed to request GPIO line " + std::to_string(pin));
        }

        line_fd = req.fd;
        is_claimed = true;
    }

    // Destructor
    ~GpioPin() {
        if (line_fd >= 0) {
            close(line_fd);
        }
        if (chip_fd >= 0) {
            close(chip_fd);
        }
    }

    // Delete copy constructor and assignment operator
    GpioPin(const GpioPin&) = delete;
    GpioPin& operator=(const GpioPin&) = delete;

    // Move constructor
    GpioPin(GpioPin&& other) noexcept 
        : chip_fd(other.chip_fd), line_fd(other.line_fd), 
          pin_number(other.pin_number), is_output(other.is_output), 
          is_claimed(other.is_claimed) {
        other.chip_fd = -1;
        other.line_fd = -1;
        other.is_claimed = false;
    }

    // Move assignment operator
    GpioPin& operator=(GpioPin&& other) noexcept {
        if (this != &other) {
            // Clean up current resources
            if (line_fd >= 0) close(line_fd);
            if (chip_fd >= 0) close(chip_fd);

            // Move resources
            chip_fd = other.chip_fd;
            line_fd = other.line_fd;
            pin_number = other.pin_number;
            is_output = other.is_output;
            is_claimed = other.is_claimed;

            // Reset other
            other.chip_fd = -1;
            other.line_fd = -1;
            other.is_claimed = false;
        }
        return *this;
    }

    // Write to output pin
    void write(bool value) {
        if (!is_output) {
            throw std::runtime_error("Cannot write to input pin");
        }

        struct gpiohandle_data data;
        data.values[0] = value ? 1 : 0;

        if (ioctl(line_fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
            throw std::runtime_error("Failed to write to GPIO pin " + std::to_string(pin_number));
        }
    }

    // Read from pin
    bool read() {
        struct gpiohandle_data data;

        if (ioctl(line_fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0) {
            throw std::runtime_error("Failed to read from GPIO pin " + std::to_string(pin_number));
        }

        return data.values[0] != 0;
    }

    // Convenience operators
    GpioPin& operator=(bool value) {
        write(value);
        return *this;
    }

    operator bool() {
        return read();
    }

    // Get pin number
    int getPin() const {
        return pin_number;
    }

    // Check if pin is configured as output
    bool isOutput() const {
        return is_output;
    }
};

// Utility class for PWM-like functionality using software PWM
class SoftPwm {
private:
    GpioPin pin;
    std::thread pwm_thread;
    bool running;
    double duty_cycle; // 0.0 to 1.0
    int frequency_hz;

    void pwmLoop() {
        auto period = std::chrono::microseconds(1000000 / frequency_hz);
        
        while (running) {
            if (duty_cycle > 0.0) {
                pin.write(true);
                auto high_time = std::chrono::duration_cast<std::chrono::microseconds>(
                    period * duty_cycle);
                std::this_thread::sleep_for(high_time);
            }
            
            if (duty_cycle < 1.0 && running) {
                pin.write(false);
                auto low_time = std::chrono::duration_cast<std::chrono::microseconds>(
                    period * (1.0 - duty_cycle));
                std::this_thread::sleep_for(low_time);
            }
        }
        pin.write(false);
    }

public:
    SoftPwm(int pin_num, int freq_hz = 1000) 
        : pin(pin_num, GpioPin::OUTPUT), running(false), duty_cycle(0.0), frequency_hz(freq_hz) {
    }

    ~SoftPwm() {
        stop();
    }

    void start(double duty = 0.5) {
        if (running) {
            stop();
        }
        
        duty_cycle = std::max(0.0, std::min(1.0, duty));
        running = true;
        pwm_thread = std::thread(&SoftPwm::pwmLoop, this);
    }

    void stop() {
        if (running) {
            running = false;
            if (pwm_thread.joinable()) {
                pwm_thread.join();
            }
        }
    }

    void setDutyCycle(double duty) {
        duty_cycle = std::max(0.0, std::min(1.0, duty));
    }

    void setFrequency(int freq_hz) {
        bool was_running = running;
        if (was_running) {
            stop();
        }
        frequency_hz = freq_hz;
        if (was_running) {
            start(duty_cycle);
        }
    }

    double getDutyCycle() const {
        return duty_cycle;
    }

    int getFrequency() const {
        return frequency_hz;
    }
};

// Utility functions
namespace Utils {
    // Delay functions
    inline void delayMs(int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    inline void delayUs(int microseconds) {
        std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
    }

    // Check if running on Raspberry Pi
    inline bool isRaspberryPi() {
        int fd = open("/dev/gpiochip0", O_RDONLY);
        if (fd >= 0) {
            close(fd);
            return true;
        }
        return false;
    }

    // Get GPIO chip info
    inline std::string getChipInfo() {
        int fd = open("/dev/gpiochip0", O_RDONLY);
        if (fd < 0) {
            return "GPIO chip not available";
        }

        struct gpiochip_info info;
        if (ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &info) < 0) {
            close(fd);
            return "Failed to get chip info";
        }

        close(fd);
        return std::string(info.name) + " - " + std::string(info.label) + 
               " (" + std::to_string(info.lines) + " lines)";
    }
}

} // namespace RpiGpio

/*
USAGE EXAMPLE:

#include "RpiGpio.hpp"
#include <iostream>

int main() {
    try {
        // Create output pin (LED on pin 18)
        RpiGpio::GpioPin led(18, RpiGpio::GpioPin::OUTPUT);
        
        // Create input pin (button on pin 24)
        RpiGpio::GpioPin button(24, RpiGpio::GpioPin::INPUT);
        
        // Blink LED
        for (int i = 0; i < 10; i++) {
            led = true;
            RpiGpio::Utils::delayMs(500);
            led = false;
            RpiGpio::Utils::delayMs(500);
        }
        
        // Read button and control LED
        while (true) {
            if (button.read()) {
                led.write(true);
            } else {
                led.write(false);
            }
            RpiGpio::Utils::delayMs(50);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

COMPILATION:
g++ -std=c++11 -pthread your_program.cpp -o your_program

SETUP (run once):
sudo usermod -a -G gpio $USER
# Then log out and back in

FEATURES:
- Single header file
- No root privileges required (after gpio group setup)
- RAII-based resource management
- Move semantics support
- Software PWM capability
- Utility functions for delays
- Exception-based error handling
- Modern C++ interface

*/