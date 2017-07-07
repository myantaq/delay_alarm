PROG=tiny13_delay_alarm
OBJS=${PROG}.o

#MCU=attiny2313

MCU=attiny13a
F_CPU=600000UL # internal 4.8MHz clock & CKDIV8 fuse bit enabled

#MCU=atmega88
#F_CPU=1000000UL # internal 8MHz clock & CKDIV8 fuse bit enabled
#--------

#MCU=attiny10
#MCU=attiny13a
#MCU=attiny2313
#MCU=atmega88V
#MCU=atmega164P
#MCU=atmega168P
#MCU=atmega328P


ELF=$(PROG).elf
IHEX=$(PROG).hex

CC=avr-gcc
CFLAGS=-g -Wall -O2 -mmcu=$(MCU) -DF_CPU=$(F_CPU)
LDFLAGS=-g -Wall -O2 -mmcu=$(MCU)
OBJCOPY=avr-objcopy

.SUFFIXES:
.SUFFIXES: .c .o .elf .hex

all: $(IHEX)

$(IHEX): $(ELF)

$(ELF): $(OBJS)


write_hex:
	hidspx -d6 tiny13_wait_buzzer.hex


write_fuse:
	hidspx -d6 -fl0x29 -fh0x1f


.elf.hex:
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
	@echo ""
	@avr-size $@
	@echo ""

.o.elf:
	$(CC) $(LDFLAGS) -o $@ $<

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	-$(RM) $(ELF) $(IHEX) $(OBJS)

.PHONY: all clean
