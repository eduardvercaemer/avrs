CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f

MCU = atmega328p
F_CPU = 8000000

AVRDUDE_MCU = $(MCU)
AVRDUDE_PROGRAMMER = usbasp
AVRDUDE_SPEED = 1MHz

TARGET = firmware
SRC = main.c
OBJ = $(SRC:.c=.o)
FORMAT = ihex

OPTLEVEL = s

MSG_LINKING = Linking:
MSG_COMPILING = Compiling:
MSG_FLASH = Preparing HEX file:
MSG_EEP = Preparing EEPROM file:

CDEFS =
CFLAGS = -DF_CPU=$(F_CPU)UL
CFLAGS += -O$(OPTLEVEL)
CFLAGS += -mmcu=$(MCU)
CFLAGS += -std=gnu99
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -Wa,-adhlns=$(<:.c=.lst)
CFLAGS += -gdwarf

LDFLAGS = -Wl,--gc-sections
LDFLAGS += -Wl,--print-gc-sections

AVRDUDE_FLAGS = -p $(AVRDUDE_MCU)
AVRDUDE_FLAGS += -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += -B $(AVRDUDE_SPEED)

all: $(TARGET).elf $(TARGET).hex $(TARGET).eep size

.SECONDARY: $(TARGET).elf
.PRECIOUS: $(OBJ)

%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -j .text -j .data $< $@

%.eep: %.elf
	@echo
	@echo $(MSG_EEP) $@
	$(OBJCOPY) -O $(FORMAT) -j .eeprom --change-section-lma .eeprom=0 $< $@

%.elf: $(OBJ)
	@echo
	@echo $(MSG_LINKING) $@
	$(CC) -mmcu=$(MCU) $(LDFLAGS) $^ --output $(@F)

%.o : %.c
	@echo $(MSG_COMPILING) $<
	$(CC) $(CFLAGS) -c $< -o $(@F)

size: $(TARGET).elf
	@echo
	$(SIZE) -C --mcu=$(AVRDUDE_MCU) $(TARGET).elf

analyze: $(TARGET).elf
	$(OBJDUMP) -m avr -D $< | less

flash: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(TARGET).hex

eeprom: $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U eeprom:w:$(TARGET).eep

release: flash eeprom

clean:
	$(REMOVE) $(TARGET).hex $(TARGET).eep $(TARGET).elf $(OBJ) $(LST) *~
