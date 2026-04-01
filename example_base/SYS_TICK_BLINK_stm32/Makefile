TARGET = sys_tick_blink

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS  = -mcpu=cortex-m3 -mthumb -O0 -g \
          -Wall -ffunction-sections -fdata-sections

LDFLAGS = -T ld/stm32f103.ld \
          -mcpu=cortex-m3 -mthumb \
          -Wl,--gc-sections \
          -nostdlib

SRC = src/startup.c src/main.c

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
	$(OBJCOPY) -O ihex   $(TARGET).elf $(TARGET).hex
	arm-none-eabi-size $(TARGET).elf

flash:
	st-flash write $(TARGET).bin 0x08000000

clean:
	rm -f $(TARGET).elf $(TARGET).bin $(TARGET).hex