# 📚 Індекс конспектів — «STM32 без магії»

Всі конспекти і шпаргалки серії. Файли в папці `docs/`.

---

## 🚀 Початок роботи

| Файл | Що всередині |
|------|-------------|
| [STM32_FirstSteps.md](docs/STM32_FirstSteps.md) | Встановлення тулчейну, udev правила, перший LED, Makefile |
| [STM32F103_dovidnyk.md](docs/STM32F103_dovidnyk.md) | Повний довідник: піни, адреси, регістри, таблиця векторів |

---

## 🔌 Периферія

| Файл | Що всередині |
|------|-------------|
| [Структура_регістрів_CRL_та_CRH.md](docs/Структура_регістрів_CRL_та_CRH.md) | GPIO режими, CRL/CRH, BSRR vs ODR, карта пам'яті |
| [RCC_MAP_REGISTER.md](docs/RCC_MAP_REGISTER.md) | Карта регістрів RCC, тактування периферії |
| [EXTI_notes.md](docs/EXTI_notes.md) | Переривання EXTI — повний конспект з питаннями для статті |
| [Переривання_EXTI_на_STM32F103.md](docs/Переривання_EXTI_на_STM32F103.md) | Шпаргалка EXTI — шлях сигналу, ISR, граблі |
| [PWM.md](docs/PWM.md) | TIM2 PWM — які піни можуть, AF режим, ARR/CCR |
| [SPI___що_це_таке_фізично.md](docs/SPI___що_це_таке_фізично.md) | SPI протокол, регістри SPI1, Nokia 5110 підключення |
| [I2C_MCU6050.md](docs/I2C_MCU6050.md) | I2C протокол, MPU6050, repeated start, ACK/NACK |

---

## 🛠 Інструменти

| Файл | Що всередині |
|------|-------------|
| [DEBUG.md](docs/DEBUG.md) | GDB + st-util, читання таблиці векторів, діагностика |
| [ELF_FILE.md](docs/ELF_FILE.md) | objdump, nm, readelf — що всередині ELF файлу |

---

## 📝 Статті на DOU

| # | Назва | Статус |
|---|-------|--------|
| 1 | STM32 з нуля без HAL: GPIO, регістри, переривання | ✅ опубліковано |
| 2 | Arduino з нуля: пишемо свій HAL для STM32 | ✅ опубліковано |
| 3 | Переривання на STM32: EXTI, UART, TIM2 | 🔄 в роботі |

---

## 🗂 Структура репо

```
├── example_base/          ← голі bare-metal приклади
│   ├── BLINK_stm32/
│   ├── GPIO_stm32/
│   ├── RUNNING_FIRE_stm32/
│   ├── UART_stm32/
│   ├── BUTTON_EXTI_stm32/
│   ├── UART_RX_IRQ/
│   ├── SYS_TICK_BLINK/
│   ├── TIM2_BLINK/
│   ├── TIM2_PWM_BLINK/
│   ├── SPI_NOKIA5110/
│   ├── I2C_MPU6050/
│   └── OLED_PULSE/
│
├── HAL_stm32/             ← свій мінімальний HAL
│   ├── hal/
│   │   ├── hal.h/.c
│   │   ├── hal_gpio.h/.c
│   │   ├── hal_systick.h/.c
│   │   ├── hal_uart.h/.c
│   │   ├── hal_exti.h/.c
│   │   ├── hal_tim.h/.c
│   │   ├── hal_spi.h/.c
│   │   ├── hal_i2c.h/.c
│   │   └── hal_adc.h/.c
│   └── src/
│       ├── startup.c
│       └── main.c
│
├── docs/                  ← конспекти і шпаргалки
├── INDEX.md               ← цей файл
└── README.md
```

---

*Платформа: Blue Pill STM32F103C8T6 · ST-Link v2 · Linux · arm-none-eabi-gcc*
*Серія: «STM32 без магії» на DOU.ua*
