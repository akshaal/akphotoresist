#include <avr/interrupt.h>
#include <avr/io.h>

X_CPU$(cpu_freq = 1061658);

// 50 shades of state and such
#define STATE_PREPARE       0
#define STATE_COUNTDOWN     AKAT_ONE
#define STATE_DONE          2
#define PREDEF1       0
#define PREDEF2       AKAT_ONE
#define PREDEF3       2

typedef u8 state_t;
typedef u8 predef_t;

GLOBAL$() {
    STATIC_VAR$(state_t state, initial = STATE_PREPARE);
    STATIC_VAR$(predef_t predef, initial = PREDEF1);
    STATIC_VAR$(u8 select_first);
}

// Watchdog
X_WATCHDOG$(8s);

// Switch
X_GPIO_OUTPUT$(switch_pin, D1);

// Declare error led and handler for fatal errors
X_GPIO_OUTPUT$(indicator_led, B0);
X_FATAL_ERROR_HANDLER_LED$(indicator_led);

// Buzzer on PB2 (on attiny2313), because 0C0A is there
X_BUZZER$(buzzer, no_callbacks);

// Declare variable with timestamp (contains hh, mm, ss, decis)
X_TIMESTAMP$(timestamp);

// Define display
X_TM1637$(tm1637, clk = B3, dio = B4);

// Connect display to timestamp, condition tells when we display timestamp
X_TM1637_TIME$(tm1637_time, timestamp, tm1637, condition = state != STATE_DONE);

// Support for flashing of information on TM1637 display
X_TM1637_FLASH$(tm1637_flash, tm1637);

// - - - - - - - - -  - - - - - - - Sounds

// Sounds
X_BUZZER_RTTL$(startup_sounds, "d=1, o=5, b=500: c3, c4, c5")
X_BUZZER_RTTL$(start_sounds, "d=1, o=5, b=1000: e, b, a")
X_BUZZER_RTTL$(stop_sounds, "d=1, o=5, b=1000: a, b, e")
X_BUZZER_RTTL$(finish_sounds, "d=1, o=5, b=170: e, b, a, b, d6, 2b., p, e, b, a, b, e6, 2b.")
X_BUZZER_SOUNDS$(button_sound, sounds = (1 @ 1000))

FUNCTION$(void play_button_sound(), no_inline) {
    buzzer.play(button_sound);
}

// - - - - - - - - -  - - - - - - - Unused pins

// For safety reasons we set them to read with pull up
X_UNUSED_PIN$(B1);
X_UNUSED_PIN$(B5); // MOSI
X_UNUSED_PIN$(B6); // MISO
X_UNUSED_PIN$(B7); // SCL
X_UNUSED_PIN$(D0);
X_UNUSED_PIN$(D6);

// - - - - - - - - -  - - - - - - - Countdown

// Clock (timer) which controls timestamp
X_COUNTDOWN$(countdown, timestamp) {
    METHOD$(void on_finish(), inline) {
        switch_pin.set(0);
        buzzer.play(finish_sounds);
        state = STATE_DONE;
        tm1637.set_pos_1(AKAT_X_TM1637_C_D);
        tm1637.set_pos_2(AKAT_X_TM1637_C_o);
        tm1637.set_pos_3(AKAT_X_TM1637_C_n);
        tm1637.set_pos_4(AKAT_X_TM1637_C_E);
        tm1637_flash.start_all();
    }
}

// - - - - - - - - -  - - - - - - - Selection

FUNCTION$(void stop_selection_flashing()) {
    if (select_first) {
        tm1637_flash.stop_pos_1();
        tm1637_flash.stop_pos_2();
    } else {
        tm1637_flash.stop_pos_3();
        tm1637_flash.stop_pos_4();
    }
}

FUNCTION$(void start_selection_flashing()) {
    if (select_first) {
        tm1637_flash.start_pos_1();
        tm1637_flash.start_pos_2();
    } else {
        tm1637_flash.start_pos_3();
        tm1637_flash.start_pos_4();
    }
}

FUNCTION$(void cycle_selection_flashing()) {
    stop_selection_flashing();
    select_first = ~select_first;
    start_selection_flashing();
}

// - - - - - - - - -  - - - - - - - Predefs handling
FUNCTION$(void set_predef1()) {
    timestamp.reset();
    predef = PREDEF1;
    timestamp.set_bcd_seconds(AKAT_BCD(1, 0));
}

FUNCTION$(void set_predef2()) {
    timestamp.reset();
    predef = PREDEF2;
    timestamp.set_bcd_minutes(AKAT_BCD(1, 5));
    timestamp.set_bcd_seconds(AKAT_BCD(3, 0));
}

FUNCTION$(void set_predef3()) {
    timestamp.reset();
    predef = PREDEF3;
    timestamp.set_bcd_hours(AKAT_BCD(0, 8));
}

FUNCTION$(void cycle_predefs()) {
    stop_selection_flashing();
    if (predef == PREDEF1) {
        set_predef2();
    } else if (predef == PREDEF2) {
        set_predef3();
    } else {
        set_predef1();
    }
    start_selection_flashing();
}

FUNCTION$(void init_prepare_mode()) {
    state = STATE_PREPARE;
    select_first = 0;
    set_predef1();
    start_selection_flashing();
}

// - - - - - - - - -  - - - - - - - Hour indication

X_EVERY_DECISECOND$(hour_indicator) {
    STATIC_VAR$(u8 blink);

    u8 on = timestamp.has_hours() & blink;
    blink = ~blink;

    indicator_led.set(on);
}

// - - - - - - - - -  - - - - - - - Timestamp modifications
FUNCTION$(void decrement_selection_position()) {
    stop_selection_flashing();

    if (timestamp.has_hours()) {
        if (select_first) {
            timestamp.dec_hours();
            if (!timestamp.has_hours()) {
                timestamp.set_bcd_hours(AKAT_BCD(2, 3)); // Skip '0'
            }
        } else {
            timestamp.dec_minutes(0);
        }
    } else {
        if (timestamp.has_minutes()) {
            if (select_first) {
                timestamp.dec_minutes(0);
                if (!timestamp.has_minutes()) {
                    timestamp.set_bcd_minutes(AKAT_BCD(5, 9)); // Skip '0'
                }
            } else {
                timestamp.dec_seconds(0);
            }
        } else {
            if (select_first) {
                timestamp.dec_seconds(0);
                if (!timestamp.has_seconds()) {
                    timestamp.set_bcd_seconds(AKAT_BCD(5, 9)); // Skip '0'
                }
            } else {
                timestamp.dec_deciseconds(0);
            }
        }
    }

    start_selection_flashing();
}

FUNCTION$(void increment_selection_position()) {
    stop_selection_flashing();

    if (timestamp.has_hours()) {
        if (select_first) {
            timestamp.inc_hours();
            if (!timestamp.has_hours()) {
                timestamp.set_bcd_hours(AKAT_BCD(0, 1)); // Skip '0'
            }
        } else {
            timestamp.inc_minutes(0);
        }
    } else {
        if (timestamp.has_minutes()) {
            if (select_first) {
                timestamp.inc_minutes(0);
                if (!timestamp.has_minutes()) {
                    timestamp.set_bcd_minutes(AKAT_BCD(0, 1)); // Skip '0'
                }
            } else {
                timestamp.inc_seconds(0);
            }
        } else {
            if (select_first) {
                timestamp.inc_seconds(0);
                if (!timestamp.has_seconds()) {
                    timestamp.set_bcd_seconds(AKAT_BCD(0, 1)); // Skip '0'
                }
            } else {
                timestamp.inc_deciseconds(0);
            }
        }
    }

    start_selection_flashing();
}


// - - - - - - - - -  - - - - - - - Buttons

FUNCTION$(void init_prepare_mode_on_done_press()) {
    if (state == STATE_DONE) {
        // Order is important
        play_button_sound();
        tm1637_flash.stop_all();
        init_prepare_mode();
    }
}

X_BUTTON_REPEAT$(button1, D4) {
    METHOD$(void on_press()) {
        if (state == STATE_PREPARE) {
            play_button_sound();
            decrement_selection_position();
        } else {
            init_prepare_mode_on_done_press();
        }
    }

    METHOD$(void on_repeat()) {
        if (state == STATE_PREPARE) {
            decrement_selection_position();
        }
    }

    METHOD$(void on_release()) {}
}

X_BUTTON_REPEAT$(button2, D5) {
    METHOD$(void on_press()) {
        if (state == STATE_PREPARE) {
            play_button_sound();
            increment_selection_position();
        } else {
            init_prepare_mode_on_done_press();
        }
    }

    METHOD$(void on_repeat()) {
        if (state == STATE_PREPARE) {
            increment_selection_position();
        }
    }

    METHOD$(void on_release()) {}
}

X_BUTTON$(button3, D2) {
    if (state == STATE_PREPARE) {
        play_button_sound();
        cycle_selection_flashing();
    } else {
        init_prepare_mode_on_done_press();
    }
}

X_BUTTON_LONG$(button4, D3) {
    METHOD$(void on_press()) {
        if (state == STATE_PREPARE) {
            play_button_sound();
            cycle_predefs();
        } else {
            init_prepare_mode_on_done_press();
        }
    }

    METHOD$(void on_long_press()) {
        if (state == STATE_PREPARE) {
            switch_pin.set(1);
            stop_selection_flashing();
            state = STATE_COUNTDOWN;
            buzzer.play(start_sounds);
            countdown.start();
        } else if (state == STATE_COUNTDOWN) {
            switch_pin.set(0);
            start_selection_flashing();
            state = STATE_PREPARE;
            buzzer.play(stop_sounds);
            countdown.stop();
        }
    }

    METHOD$(void on_release()) {}
}

// Main
X_MAIN$() {
    switch_pin.set(0);
    init_prepare_mode();
    buzzer.play(startup_sounds);
    sei();
}
