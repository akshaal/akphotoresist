#include <avr/interrupt.h>
#include <avr/io.h>

WRITE_CFLAGS$(firmware);

// TODO: Remove it and add again based upon frequency produced by akatv
// NOTE: Sometimes it's nice to try to see which one is best to have some not low, must try some combinations
USE_REG$(tm1637__dirty, low);
USE_REG$(buzzer__play_deciseconds, low);
USE_REG$(akat_timestamp_hour_l__timestamp, low);
USE_REG$(akat_timestamp_hour_h__timestamp, low);
USE_REG$(akat_timestamp_minute_l__timestamp, low);
USE_REG$(akat_timestamp_minute_h__timestamp, low);
USE_REG$(akat_timestamp_second_l__timestamp, low);
USE_REG$(akat_timestamp_second_h__timestamp, low);
USE_REG$(akat_timestamp_decisecond__timestamp, low);
USE_REG$(tm1637__byte1, low);
USE_REG$(tm1637__byte2, low);
USE_REG$(tm1637__byte3, low);
USE_REG$(tm1637__byte4, low);
USE_REG$(state);

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
}

// Declare error led and handler for fatal errors
X_GPIO_OUTPUT$(indicator_led, B0);
X_FATAL_ERROR_HANDLER_LED$(indicator_led);

// Buzzer on PB2 (on attiny2313)
X_BUZZER$(buzzer);

// Sounds
X_BUZZER_RTTL$(startup_sounds, "d=1, o=5, b=500: c3, c4, c5")
X_BUZZER_RTTL$(start_sounds, "d=1, o=5, b=1000: e, b, a")
X_BUZZER_RTTL$(stop_sounds, "d=1, o=5, b=1000: a, b, e")
X_BUZZER_RTTL$(finish_sounds, "d=1, o=5, b=170: e, b, a, b, d6, 2b., p, e, b, a, b, e6, 2b.")
X_BUZZER_SOUNDS$(button_sound, sounds = (1 @ 1000))

// Declare variable with timestamp (contains hh, mm, ss, decis)
X_TIMESTAMP$(timestamp);

// Define display
X_TM1637$(tm1637, clk = B3, dio = B4);

// Connect display to timestamp, condition tells when we display data
X_TM1637_TIME$(tm1637_time, timestamp, tm1637, condition = state != STATE_DONE);

// Support for flashing of information on TM1637 display
X_TM1637_FLASH$(tm1637_flash, tm1637);

// Clock (timer) which controls timestamp
X_COUNTDOWN$(countdown, timestamp) {
    METHOD$(void on_finish(), inline) {
        buzzer.play(finish_sounds, NULL);
        state = STATE_DONE;
        tm1637.set_pos_1(AKAT_X_TM1637_C_D);
        tm1637.set_pos_2(AKAT_X_TM1637_C_o);
        tm1637.set_pos_3(AKAT_X_TM1637_C_n);
        tm1637.set_pos_4(AKAT_X_TM1637_C_E);
        tm1637_flash.start_pos_1();
        tm1637_flash.start_pos_2();
        tm1637_flash.start_pos_3();
        tm1637_flash.start_pos_4();
    }
}

// - - - - - - - - -  - - - - - - - Predefs handling
FUNCTION$(void set_predef1()) {
    timestamp.reset();
    predef = PREDEF1;
    timestamp.set_seconds(1, 0);
}

FUNCTION$(void set_predef2()) {
    timestamp.reset();
    predef = PREDEF2;
    timestamp.set_minutes(1, 5);
    timestamp.set_seconds(3, 0);
}

FUNCTION$(void set_predef3()) {
    timestamp.reset();
    predef = PREDEF3;
    timestamp.set_hours(0, 8);
}

FUNCTION$(void cycle_predefs()) {
    if (predef == PREDEF1) {
        set_predef2();
    } else if (predef == PREDEF2) {
        set_predef3();
    } else {
        set_predef1();
    }
}

FUNCTION$(void init_prepare_mode()) {
    state = STATE_PREPARE;
    set_predef1();
}

// - - - - - - - - -  - - - - - - - Hour indication

X_EVERY_DECISECOND$(hour_indicator) {
    u8 const on =
        (timestamp.get_hours_l() != 0 || timestamp.get_hours_h() != 0)
        && (timestamp.get_deciseconds() & AKAT_ONE);

    indicator_led.set(on);
}

// - - - - - - - - -  - - - - - - - Buttons

FUNCTION$(void init_prepare_mode_on_done_press()) {
    if (state == STATE_DONE) {
        // Order is important
        buzzer.play(button_sound, NULL);
        tm1637_flash.stop_pos_1();
        tm1637_flash.stop_pos_2();
        tm1637_flash.stop_pos_3();
        tm1637_flash.stop_pos_4();
        init_prepare_mode();
    }
}

X_BUTTON_REPEAT$(button1, D4) {
    METHOD$(void on_press()) {
        if (state == STATE_PREPARE) {
            buzzer.play(button_sound, NULL);
            // TODO: Decrement current position
        } else {
            init_prepare_mode_on_done_press();
        }
    }

    METHOD$(void on_repeat()) {
        if (state == STATE_PREPARE) {
            // TODO: Decrement current position faster
        }
    }

    METHOD$(void on_release()) {}
}

X_BUTTON_REPEAT$(button2, D5) {
    METHOD$(void on_press()) {
        if (state == STATE_PREPARE) {
            buzzer.play(button_sound, NULL);
            // TODO: Increment current position
        } else {
            init_prepare_mode_on_done_press();
        }
    }

    METHOD$(void on_repeat()) {
        if (state == STATE_PREPARE) {
            // TODO: Increment current position faster
        }
    }

    METHOD$(void on_release()) {}
}

X_BUTTON$(button3, D2) {
    // TODO: Select current position
    init_prepare_mode_on_done_press();
}

X_BUTTON_LONG$(button4, D3) {
    METHOD$(void on_press()) {
        if (state == STATE_PREPARE) {
            buzzer.play(button_sound, NULL);
            cycle_predefs();
        } else {
            init_prepare_mode_on_done_press();
        }
    }

    METHOD$(void on_long_press()) {
        if (state == STATE_PREPARE) {
            state = STATE_COUNTDOWN;
            buzzer.play(start_sounds, NULL);
            countdown.start();
        } else if (state == STATE_COUNTDOWN) {
            state = STATE_PREPARE;
            buzzer.play(stop_sounds, NULL);
            countdown.stop();
        }
    }

    METHOD$(void on_release()) {}
}

// Main
X_MAIN$() {
    init_prepare_mode();
    buzzer.play(startup_sounds, NULL);
    sei();
}
