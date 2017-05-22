#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>

WRITE_CFLAGS$(firmware);

X_CPU$(cpu_freq = 1061658);

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
X_BUZZER$(buzzer);

// Key press
X_BUZZER_SOUNDS$(
    button_press_sounds,
    sounds = (
        1 @ 433,
        2 @ 1000,
        1 @ 1500))

// Declare variable with timestamp (contains hh, mm, ss, decis)
X_TIMESTAMP$(timestamp);

// Define display
X_TM1637$(tm1637, clk = B3, dio = B4);

// Connect display to timestamp
X_TM1637_TIME$(tm1637_time, timestamp, tm1637, condition = 1);

// Support for flashing
X_TM1637_FLASH$(tm1637_flash, tm1637);

X_BUTTON_REPEAT$(button1, D2) {
    METHOD$(void on_press()) {
        error_led.set(1);
        buzzer.play(button_press_sounds, NULL);
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
X_MAIN$() {
    timestamp.reset();
    timestamp.set_hours(0, 1);
    timestamp.set_minutes(0, 2);
    c.start();
    tm1637_flash.start_pos_1();
    sei();
}
