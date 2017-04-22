#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>

AKAT_DECLARE(/* cpu_frequency = */              8000000,
             /* tasks = */                      8,
             /* dispatcher_idle_code = */       ,
             /* dispatcher_overflow_code = */   )

__ATTR_NORETURN__
void main () {
    akat_init ();

    akat_dispatcher_loop ();
}
