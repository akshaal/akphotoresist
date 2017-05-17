AKAT_DIR=$(PWD)/$({0%/*})/../akatlib4

include ${AKAT_DIR}/src/akat.Makefile

MCU=attiny2313
SRCS=main.c

all: firmware.avr

distclean: clean

firmware.tmp.c: ${SRCS} ${AKAT_SRCS}
	${AKATPP} ${AKATPP_OPTS} ${AKAT_SRCS} ${SRCS} > "firmware.tmp.c"

firmware.avr: firmware.tmp.c
	XX=`cat firmware.cflags` && ${CC} -Os ${CFLAGS} $$XX "$<" -o $@ && \
		${OBJDUMP} -d $@ > $@.s && \
		${OBJCOPY} -j .text -j .data -O ihex firmware.avr firmware.hex && \
		hex2bin firmware.hex

clean:
	rm -f *.ii *.o *.i *.s *.a *.avr *.tmp.c *.hex *.bin
