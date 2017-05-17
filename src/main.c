#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>

WRITE_CFLAGS$(firmware);

// Declare error led
X_GPIO_OUTPUT$(error_led, B0);

// Declare variable with timestamp (contains hh, mm, ss, decis)
X_TIMESTAMP$(timestamp);

X_FULL_BUTTON$(button1, D2) {
    METHOD$(void on_press()) {
        error_led.set(1);
    }

    METHOD$(void on_release()) {
        error_led.set(0);
    }
}


X_CLOCK$(c, timestamp);

X_TIMESTAMP_CALLBACKS$(timestamp) {
    METHOD$(void on_new_second_l(), inline) {
        error_led.set(timestamp.get_seconds_l() & 1);
    }
}

// Main
X_MAIN$(cpu_freq = 1000000) {
    timestamp.reset();
    c.start();
    sei();
}
