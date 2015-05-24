NAME	= blink
MCU		= atmega328p
F_CPU	= 16000000
FORMAT	= ihex
TARGET	= $(NAME)

AVRDUDE = avrdude -e -v -p$(MCU) -cusbasp -D -Uflash:w:$(NAME).hex:i -u

CSRC := $(shell find . -name '*.c')

MCU_FLAG = -mmcu=$(MCU)

DEFINES_VALUES = F_CPU=$(F_CPU)UL
DEFINES_FLAGS = $(addprefix -D,$(DEFINES_VALUES))

WARNING_FLAGS = -Wall

OPTIMIZER_FLAGS = -Os

CFLAGS_VALUES = pack-struct short-enums function-sections data-sections unsigned-char unsigned-bitfields no-strict-aliasing
CFLAGS = $(addprefix -f,$(CFLAGS_VALUES))

HEADER_CONFIG_FILES = uipconf.h config.h
HEADER_CONFIG_FLAGS = $(addprefix -include ,$(HEADER_CONFIG_FILES))

INCLUDE_PATHS = $(PWD)
INCLUDE_FLAGS = $(addprefix -I,$(INCLUDE_PATHS))

LDFLAGS = -Wl,-Map,$(PRG).map

CROSS	= avr-
SHELL	= sh
CC		= $(CROSS)gcc
OBJCOPY	= $(CROSS)objcopy
OBJDUMP	= $(CROSS)objdump
SIZE	= $(CROSS)size
NM	= $(CROSS)nm
RM	= rm -f

OBJ	= $(subst .c,.o,$(CSRC))

all: $(NAME).elf hex

%.o: %.c
	$(CC) $(MCU_FLAG) $(DEFINES_FLAGS) $(WARNING_FLAGS) $(OPTIMIZER_FLAGS) $(CFLAGS) $(INCLUDE_FLAGS) $(HEADER_CONFIG_FLAGS) -c -o $@ $^ $(LIBS)

$(NAME).elf: $(OBJ)
	$(CC) $(MCU_FLAG) $(WARNING_FLAGS) $(OPTIMIZER_FLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

text: hex bin srec

hex:	$(NAME).hex

avrdude: hex
	$(AVRDUDE)

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

clean:
	find . -name '*.o' -delete
	find . -name '*.d' -delete
	rm -rf *.elf *.hex

rebuild: clean all

.PHONY: all avrdude clean rebuild text
