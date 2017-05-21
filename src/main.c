#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>

WRITE_CFLAGS$(firmware);

// Declare error led
X_GPIO_OUTPUT$(error_led, B0);

// Declare error handler
X_FATAL_ERROR_HANDLER$() {
    // TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (code > 0) {
        error_led.set(1);
    }

    while(1) {};
}

// Buzzer on PB2 (on attiny2313)
X_BUZZER$(buzzer, min_freq = 1000, max_freq = 15000);

// Declare variable with timestamp (contains hh, mm, ss, decis)
X_TIMESTAMP$(timestamp);

X_BUTTON_REPEAT$(button1, D2) {
    METHOD$(void on_press()) {
        error_led.set(1);
    }

    METHOD$(void on_repeat()) {
        error_led.set(timestamp.get_deciseconds() & 1);
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
    buzzer.set_freq(11000);
    sei();
}
