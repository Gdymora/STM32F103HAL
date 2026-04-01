# STM32 Mini HAL

Мінімальний HAL для STM32F103 (Blue Pill) написаний з нуля без CubeMX і ST HAL.

**Платформа:** Blue Pill (STM32F103C8T6) · ST-Link v2 · Linux · arm-none-eabi-gcc

---

## Структура проекту

```
stm32_hal/
├── hal/
│   ├── hal.h           ← головний include
│   ├── hal.c           ← hal_init()
│   ├── hal_gpio.h
│   ├── hal_gpio.c      ← GPIO: init, write, read, toggle
│   ├── hal_systick.h
│   ├── hal_systick.c   ← SysTick: delay_ms
│   ├── hal_uart.h
│   └── hal_uart.c      ← UART: init, putc, puts, printf, getc, gets
├── src/
│   ├── startup.c       ← таблиця векторів + Reset_Handler
│   └── main.c
├── ld/
│   └── stm32f103.ld    ← лінкер скрипт
└── Makefile
```

---

## Збірка і прошивка

```bash
make          # збірка
make flash    # прошивка через ST-Link
make clean    # очистити
```

---

## HAL API

### hal.h — ініціалізація

```c
#include "hal/hal.h"

hal_init();   // ініціалізує SysTick на 8MHz HSI
```

**Константи:**
```c
#define CPU_HZ  8000000UL   // Blue Pill = 8MHz HSI за замовчуванням
```

---

### hal_gpio — GPIO

#### Піни

Всі піни визначені як макроси. Формат: `PIN_P{порт}{номер}`

```c
// Порт A
PIN_PA0, PIN_PA1, PIN_PA2, PIN_PA3, PIN_PA4,
PIN_PA5, PIN_PA6, PIN_PA7, PIN_PA8, PIN_PA9, PIN_PA10

// Порт B
PIN_PB0, PIN_PB1, PIN_PB6, PIN_PB7

// Порт C
PIN_PC13, PIN_PC14, PIN_PC15
```

> Потрібного піна немає в списку? Додай в `hal_gpio.h`:
> ```c
> #define PIN_PB5  PACK_PIN(GPIOB_BASE, 5)
> ```

#### Режими

```c
INPUT           // вхід floating
INPUT_PULLUP    // вхід з підтяжкою до HIGH
OUTPUT          // вихід push-pull 2MHz
OUTPUT_FAST     // вихід push-pull 50MHz
```

#### Функції

```c
void    gpio_init(uint32_t pin, uint8_t mode);
void    gpio_write(uint32_t pin, uint8_t state);   // HIGH або LOW
void    gpio_toggle(uint32_t pin);
uint8_t gpio_read(uint32_t pin);                   // повертає 0 або 1
```

#### Приклади

```c
// LED на PC13 (Blue Pill — інверсна логіка: LOW = світиться)
gpio_init(PIN_PC13, OUTPUT);
gpio_write(PIN_PC13, LOW);    // увімкнути
gpio_write(PIN_PC13, HIGH);   // вимкнути
gpio_toggle(PIN_PC13);        // перемкнути

// Кнопка на PA1 між піном і GND
gpio_init(PIN_PA1, INPUT_PULLUP);
if (!gpio_read(PIN_PA1)) {
    // кнопка натиснута
}
```

> **Важливо:** `gpio_init` автоматично вмикає тактування потрібного порту через RCC.
> Окремо писати `RCC_APB2ENR |= ...` не потрібно.

---

### hal_systick — затримки

```c
void systick_init(uint32_t cpu_hz);   // викликається з hal_init()
void delay_ms(uint32_t ms);           // затримка в мілісекундах
```

#### Приклад

```c
delay_ms(500);    // пауза 500мс
```

> **Як працює:** SysTick — вбудований таймер Cortex-M3. Рахує вниз від `cpu_hz/1000`
> до нуля — це рівно 1мс. `delay_ms` викликає це `ms` разів.
> Не використовує переривань — polling через `COUNTFLAG`.

---

### hal_uart — UART

**Підключення CH340/USB-TTL:**
```
Blue Pill PA9  (TX) → RX адаптера
Blue Pill PA10 (RX) → TX адаптера
Blue Pill GND       → GND адаптера
```

#### Функції

```c
void uart_init(uint32_t baud);              // ініціалізація USART1
void uart_putc(char c);                     // відправити символ
void uart_puts(const char *s);              // відправити рядок
void uart_printf(const char *fmt, ...);     // форматований вивід
char uart_getc(void);                       // отримати символ (блокуючий)
void uart_gets(char *buf, uint8_t len);     // отримати рядок до \r або \n
```

#### Формати uart_printf

```c
%d    // int32_t
%s    // рядок
%x    // uint32_t в hex (з префіксом 0x)
%c    // символ
%%    // символ %
```

#### Приклади

```c
uart_init(9600);

uart_puts("STM32 HAL ready\r\n");
uart_printf("temp=%d, addr=%x\r\n", 25, 0x40010800);

// читання команд
char buf[16];
uart_gets(buf, 16);
if (buf[0] == 'o' && buf[1] == 'n') {
    // команда "on"
}
```

#### Термінал

```bash
minicom -b 9600 -D /dev/ttyUSB0
# CTRL-A E — local echo
# CTRL-A X — вихід
```

---

## Відомі особливості

**Blue Pill LED інверсний:**
Вбудований світлодіод на PC13 підключений анодом до 3.3V — тому `LOW` = світиться, `HIGH` = не світиться.

```c
gpio_write(PIN_PC13, LOW);   // LED увімкнути
gpio_write(PIN_PC13, HIGH);  // LED вимкнути
```

**Pull-up для кнопки:**
З `INPUT_PULLUP` логіка інверсна — натиснуто = `0`, відпущено = `1`. Тому перевірка через `!gpio_read()`.

**CPU частота:**
Blue Pill стартує на внутрішньому HSI 8MHz без додаткового налаштування RCC. Якщо змінюєш частоту через PLL — оновлюй `CPU_HZ` в `hal.h`.

---

## Що далі

- [ ] UART RX через переривання (зараз polling)
- [ ] `uart_printf` — додати `%u`, `%f`
- [ ] ESP32 AT команди через UART
- [ ] I2C HAL
- [ ] Timer/PWM HAL
- [ ] SPI HAL
