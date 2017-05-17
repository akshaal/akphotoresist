#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>

WRITE_CFLAGS$(firmware);

X_GPIO_OUTPUT$(error_led, B0);

X_BUTTON$(button1, D2) {
}

// Main
X_MAIN$(cpu_freq = 8000000) {
}
