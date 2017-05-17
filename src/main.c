#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>

WRITE_CFLAGS$(firmware);

X_GPIO_OUTPUT$(error_led, B0);

X_FULL_BUTTON$(button1, D2) {
    METHOD$(void on_press()) {
        error_led.set(1);
    }

    METHOD$(void on_release()) {
        error_led.set(0);
    }
}

// Main
X_MAIN$(cpu_freq = 8000000) {
}
