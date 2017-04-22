AKAT_DIR=$(PWD)/$({0%/*})/../akatlib

include ${AKAT_DIR}/src/akat.Makefile

MCU=attiny2313
SRCS=main.cpp

all: firmware.avr

distclean: clean

firmware.tmp.cpp: ${SRCS} ${AKAT_SRCS}
	cat ${AKAT_SRCS} ${SRCS} > "firmware.tmp.cpp"

firmware.avr: firmware.tmp.cpp
	${CXX} ${CXXFLAGS} -Os "$<" -DAKAT_DEBUG_OFF -save-temps -o $@
	${OBJDUMP} -d $@ > $@.s

clean:
	rm -f *.ii *.o *.i *.s *.a *.avr *.tmp.cpp
