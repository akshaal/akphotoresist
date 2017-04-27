#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>

AKAT_DECLARE(/* cpu_frequency = */              1000000,
             /* tasks = */                      8,
             /* dispatcher_idle_code = */       ,
             /* dispatcher_overflow_code = */   )

AKAT_DEFINE_OUTPUT_PIN(error_led, B, 0)
AKAT_DEFINE_INPUT_PIN_WITH_INTERNAL_PULLUP(key1, D, 2)

__ATTR_NORETURN__
void main () {
    akat_init ();

    error_led.set(0);
    error_led.enable_output_mode();

    key1.enable_input_with_internal_pullup();

    while(1) {
        error_led.set(!key1.read());
    }

    akat_dispatcher_loop ();
}
