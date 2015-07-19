NAME	= blink
MCU		= atmega328p
F_CPU	= 16000000
FORMAT	= ihex
TARGET	= $(NAME)

AVRDUDE = avrdude -e -v -p$(MCU) -cusbasp -D -Uflash:w:$(NAME).hex:i -u

CSRC := $(shell find . -name '*.c')

MCU_FLAG = -mmcu=$(MCU)

DEFINE_VALUES = F_CPU=$(F_CPU)UL
DEFINE_FLAGS = $(addprefix -D,$(DEFINE_VALUES))

WARNING_FLAGS = -Wall

OPTIMIZER_FLAGS = -Os

CFLAGS_VALUES = pack-struct short-enums function-sections data-sections unsigned-char unsigned-bitfields no-strict-aliasing
CFLAGS = $(addprefix -f,$(CFLAGS_VALUES))

#HEADER_CONFIG_FILES = uipconf.h config.h
#HEADER_CONFIG_FLAGS = $(addprefix -include ,$(HEADER_CONFIG_FILES))

INCLUDE_PATHS = $(PWD)
INCLUDE_FLAGS = $(addprefix -I,$(INCLUDE_PATHS))

LDFLAGS = -Wl,--gc-sections,-Map,$(PRG).map

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
	$(CC) $(MCU_FLAG) $(DEFINE_FLAGS) $(WARNING_FLAGS) $(OPTIMIZER_FLAGS) $(CFLAGS) $(INCLUDE_FLAGS) $(HEADER_CONFIG_FLAGS) -c -o $@ $^ $(LIBS)

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

size: $(NAME).elf
	$(SIZE) -A $(NAME).elf

-include $(subst .c,.d,$(CSRC))

%.d: %.c
	$(create-dep)

define create-dep
	$(CC) -M $(CFLAGS) $(INCLUDE_FLAGS) $(DEFINE_FLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(BUILD_DIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
endef

.PHONY: all avrdude clean rebuild text size
