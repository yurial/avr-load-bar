DEVICE	 = atmega8

#CLOCK	 = 1000000
#-U lfuse:w:0x62:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m

CLOCK    = 8000000
#-U lfuse:w:0xe2:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m

WARNING_FLAGS = -Wall -Wundef -Wundef
COMMON_FLAGS = -O3 -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -I. $(WARNING_FLAGS) -gdwarf-2 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -MD -MP 
CFLAGS = $(COMMON_FLAGS) -Wstrict-prototypes
CXXFLAGS = -std=c++11 $(COMMON_FLAGS)
CC = avr-gcc
CXX = avr-g++
AVRDUDE = avrdude -v -C/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf -p$(DEVICE) -c usbasp

OBJECTS	= main.o

all:	main.hex

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

clean:
	rm -f main.hex main.elf $(OBJECTS)

erase:
	$(AVRDUDE) -e

%.s: %.cpp
	$(CC) $(CXXFLAGS) -g -S $< -o $@

main.elf: $(OBJECTS)
	$(CC) $(COMMON_FLAGS) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=$(DEVICE) main.elf
