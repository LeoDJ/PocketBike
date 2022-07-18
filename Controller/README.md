# Controller

The controller is a small board containing a Pi Pico which sits inbetween the different data buses (VESC UART, display UART, BMS UART) and I/O (WS2812 LED Strip, buttons / other future inputs).

Critical signals like throttle and brake are directly connected to the VESC, they can be read out via UART/CAN.