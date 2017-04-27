#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>

AKAT_DECLARE(/* cpu_frequency = */              1000000,
             /* tasks = */                      8,
             /* dispatcher_idle_code = */       ,
             /* dispatcher_overflow_code = */   )

AKAT_DEFINE_OUT_PIN(error_led, B, 0)

__ATTR_NORETURN__
void main () {
    akat_init ();

    error_led.set(0);
    error_led.enable_output_mode();

    while(1) {
        error_led.set(0);
        akat_delay_ms(200);
        akat_delay_ms(200);
        akat_delay_ms(200);
        akat_delay_ms(200);
        akat_delay_ms(200);
        error_led.set(1);
        akat_delay_ms(200);
        akat_delay_ms(200);
        akat_delay_ms(200);
        akat_delay_ms(200);
        akat_delay_ms(200);
    }

    akat_dispatcher_loop ();
}
