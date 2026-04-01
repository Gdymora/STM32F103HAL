# STM32 First Steps on Linux
## Blue Pill (STM32F103C8T6) + ST-Link v2 — з нуля до мигаючого LED

---

## 1. Перевірити що Linux бачить ST-Link

```bash
lsusb | grep -i stm
# Bus 003 Device 007: ID 0483:3748 STMicroelectronics ST-LINK/V2
```

---

## 2. udev правило (щоб без sudo)

```bash
sudo nano /etc/udev/rules.d/49-stlinkv2.rules
```

Вміст:
```
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3748", MODE="0666", GROUP="plugdev"
```

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger

# перевірити групу
groups | grep plugdev

# якщо немає — додати себе
sudo usermod -aG plugdev $USER
newgrp plugdev
```

---

## 3. Встановити тулчейн

```bash
sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi stlink-tools

# перевірити
arm-none-eabi-gcc --version
st-info --probe
```

Очікуваний вивід `st-info --probe`:
```
Found 1 stlink programmers
  version:    V2J34S7
  flash:      65536 (pagesize: 1024)   ← 64 КБ
  sram:       20480                     ← 20 КБ
  chipid:     0x0410                    ← F1xx Medium-density
```

---

## 4. Структура проекту (без HAL, без CubeMX)

```bash
mkdir -p ~/STM32/stm32_blink/{src,ld}
cd ~/STM32/stm32_blink
```

```
stm32_blink/
├── src/
│   ├── startup.c   ← таблиця векторів + Reset_Handler
│   └── main.c      ← наш код
├── ld/
│   └── stm32f103.ld  ← memory map
└── Makefile
```

---

## 5. ld/stm32f103.ld — memory map чіпа

```ld
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS
{
    .text :
    {
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
    } > FLASH

    .data :
    {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > SRAM AT > FLASH

    _sidata = LOADADDR(.data);

    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > SRAM

    _estack = ORIGIN(SRAM) + LENGTH(SRAM);
}
```

> `0x08000000` — початок Flash, звідси Cortex-M3 стартує після reset  
> `0x20000000` — початок SRAM, тут живуть змінні і стек

---

## 6. src/startup.c — таблиця векторів і boot sequence

```c
#include <stdint.h>

extern uint32_t _estack;
extern uint32_t _sdata, _edata, _sidata;
extern uint32_t _sbss, _ebss;
extern int main(void);

void Reset_Handler(void) {
    // копіюємо .data з Flash в SRAM
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) *dst++ = *src++;

    // обнуляємо .bss
    dst = &_sbss;
    while (dst < &_ebss) *dst++ = 0;

    main();
    while (1);
}

void Default_Handler(void) { while (1); }

#define WEAK_ALIAS __attribute__((weak, alias("Default_Handler")))

void NMI_Handler(void)        WEAK_ALIAS;
void HardFault_Handler(void)  WEAK_ALIAS;
void SVC_Handler(void)        WEAK_ALIAS;
void PendSV_Handler(void)     WEAK_ALIAS;
void SysTick_Handler(void)    WEAK_ALIAS;

__attribute__((section(".vectors")))
uint32_t vectors[] = {
    (uint32_t)&_estack,           // 0: top of stack
    (uint32_t)Reset_Handler,      // 1: reset
    (uint32_t)NMI_Handler,        // 2: NMI
    (uint32_t)HardFault_Handler,  // 3: hard fault
    0, 0, 0, 0, 0, 0, 0,         // 4-10: reserved
    (uint32_t)SVC_Handler,        // 11: SVCall
    0, 0,                         // 12-13: reserved
    (uint32_t)PendSV_Handler,     // 14: PendSV
    (uint32_t)SysTick_Handler,    // 15: SysTick
};
```

---

## 7. src/main.c — мигаємо PC13 через регістри

```c
#include <stdint.h>

// RCC
#define RCC_BASE        0x40021000
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB2ENR_IOPCEN  (1 << 4)

// GPIOC
#define GPIOC_BASE      0x40011000
#define GPIOC_CRH       (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_ODR       (*(volatile uint32_t *)(GPIOC_BASE + 0x0C))
#define GPIO_PIN_13     (1 << 13)

void delay(volatile uint32_t n) { while (n--); }

int main(void) {
    // 1. тактування порту C
    RCC_APB2ENR |= RCC_APB2ENR_IOPCEN;

    // 2. PC13 — output push-pull 2MHz
    // CRH керує пінами 8-15, PC13 → біти [23:20]
    // 0b0010 = output push-pull 2MHz
    GPIOC_CRH &= ~(0xF << 20);
    GPIOC_CRH |=  (0x2 << 20);

    while (1) {
        GPIOC_ODR ^= GPIO_PIN_13;  // toggle
        delay(500000);
    }
    return 0;
}
```

> ⚠️ PC13 на Blue Pill **інвертований** — LOW = LED горить

---

## 8. Makefile

```makefile
TARGET = blink

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
```

> ⚠️ Відступи в Makefile — це **Tab**, не пробіли

---

## 9. Збірка і прошивка

```bash
make          # збірка
make flash    # прошивка через ST-Link
make clean    # очистити
make && make flash # збірати і прошити
```

Очікуваний результат `make`:
```
   text    data     bss     dec     hex filename
    272       0       0     272     110 blink.elf
```

Очікуваний результат `make flash`:
```
Flash written and verified! jolly good!
```

---

## Що читати далі

**Джозеф Ю — «Ядро Cortex-M3 компании ARM. Полное руководство»**

| Глави | Тема | Зв'язок з нашим кодом |
|-------|------|-----------------------|
| 2–4   | Архітектура, регістри, Thumb-2 | Чому `-mthumb` у CFLAGS |
| 5–6   | Memory map, адресний простір | `0x08000000`, `0x20000000` в ld |
| 7–8   | Exceptions, NVIC | Таблиця векторів у startup.c |
| 9     | Boot sequence | Reset_Handler, копіювання .data |
| 12    | Debug — SWD/JTAG | ST-Link, st-info --probe |
| 13    | Bit-banding | Атомарний доступ до GPIO |

---

*STM32F103C8T6 (Blue Pill) + ST-Link v2 + Linux + arm-none-eabi-gcc 10.3*
