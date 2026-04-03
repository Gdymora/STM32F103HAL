TARGET  = hal_blink
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS  = -mcpu=cortex-m3 -mthumb -O0 -g \
          -Wall -ffunction-sections -fdata-sections \
          -Ihal

LDFLAGS = -T ld/stm32f103.ld \
          -mcpu=cortex-m3 -mthumb \
          -Wl,--gc-sections \
          -nostdlib

SRC = src/startup.c \
      src/main.c \
      hal/hal.c \
      hal/hal_gpio.c \
      hal/hal_systick.c \
      hal/hal_uart.c \
      hal/hal_exti.c \
      hal/hal_tim.c  \
      hal/hal_spi.c  \
      hal/hal_i2c.c  \
      hal/hal_adc.c  \
      hal/hal_ds1307.c

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
	arm-none-eabi-size $(TARGET).elf

flash:
	st-flash write $(TARGET).bin 0x08000000

clean:
	rm -f $(TARGET).elf $(TARGET).bin