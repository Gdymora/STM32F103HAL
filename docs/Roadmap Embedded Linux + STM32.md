# Embedded Linux + STM32
## Навчальний курс · 6 місяців
### Персональний роадмап: від GPIO до Kernel Driver

---

## Загальна структура курсу

Курс розрахований на 6 місяців самостійного навчання з фокусом на практичні задачі.
Кожен місяць — окремий блок з чіткою темою, конкретними задачами і результатом
у вигляді працюючого проекту або статті на DOU.

**Ключовий принцип:** STM32 присутній в кожному місяці в новій ролі.
Місяць 1 — вчимо. Місяць 2 — STM32 як периферія Linux.
Місяць 3 — Linux керує STM32. Місяць 4 — пишемо драйвер для нього.

| Місяць | Блок | Основна тема | Платформа |
|--------|------|--------------|-----------|
| 1 | C та STM32 периферія | GPIO/UART/I2C/Timer bare-metal + свій HAL | STM32 Blue Pill |
| 2 | Buildroot + STM32 міст | BR2_PACKAGE, U-Boot, STM32 шле дані в Linux | Luckfox + Blue Pill |
| 3 | Device Tree + GStreamer | DTS модифікація, GStreamer, Linux керує STM32 | Luckfox + Blue Pill |
| 4 | Kernel modules | character device driver, HC-SR04, STM32 через SPI | Luckfox / Pi |
| 5 | FreeRTOS + Python | FreeRTOS на STM32, Python автотести, OpenCV | STM32 + Pi |
| 6 | Фінальний проект | OpenWrt або TDOA мікрофони | TP-Link / Luckfox |

---

## Місяць 1 — C та STM32 периферія

**Мета:** Впевнено писати bare-metal C для STM32, покрити всю базову периферію,
написати свій мінімальний HAL як в Arduino. Підготуватись до місяця 2 де STM32
стає периферією для Linux.

**Статус:** частково зроблено (SOS morse, GPIO bare-metal, перша стаття).

| Тиждень | Тема | Практична задача | Результат | Стаття? |
|---------|------|-----------------|-----------|---------|
| Тиж 1 | GPIO + переривання | Стаття 1: GPIO bare-metal — регістри, BSRR, EXTI | Стаття DOU | 📝 DOU |
| Тиж 2 | Свій HAL: GPIO | Пишемо pinMode/digitalWrite/digitalRead як в Arduino | HAL gpio.c/.h | 📝 DOU |
| Тиж 3 | SysTick + UART + HAL | delay_ms через SysTick. UART TX/RX. Додаємо в HAL | HAL uart.c/.h | 📝 DOU |
| Тиж 4 | I2C + Timer/PWM + HAL | BMP180 або MPU6050. ШІМ через TIM2. HAL повний | HAL i2c + timer | 📝 DOU |

**Результат місяця:**
- Власний HAL: GPIO + UART + I2C + Timer — можна використовувати в проектах
- 3-4 статті DOU про STM32 bare-metal
- STM32 відповідає на UART команди — готовий до місяця 2

---

## Місяць 2 — Buildroot глибоко + STM32 як периферія Linux

**Мета:** Розуміти Buildroot зсередини, вміти додавати власні пакети.
**Ключове доповнення:** STM32 підключений до Luckfox через UART — перший міст між bare-metal і Linux.

| Тиждень | Тема | Практична задача | Результат | Стаття? |
|---------|------|-----------------|-----------|---------|
| Тиж 1 | BR2_PACKAGE — Go сервер | Оформити Go HTTP сервер як Buildroot пакет. Автозапуск через init.d | package/go-dashboard/ | 📝 DOU |
| Тиж 2 | STM32 шле дані в Linux | STM32 збирає BMP180 → UART → Luckfox читає /dev/ttyS* → Python парсить | «Перший міст» | 📝 DOU |
| Тиж 3 | U-Boot basics | U-Boot для Luckfox. bootcmd, bootargs. Змінити kernel args | Кастомний boot message | ні |
| Тиж 4 | Boot sequence глибоко | Схема: ROM → SPL → U-Boot → kernel → init → userspace | Схема + кастомний init.d | 📝 DOU |

**Результат місяця:**
- STM32 і Linux спілкуються через UART — реальна embedded архітектура
- Вміє додавати пакети у Buildroot правильно
- Розуміє повний boot sequence

---

## Місяць 3 — Device Tree + GStreamer + Linux керує STM32

**Мета:** Вміти читати і модифікувати DTS. GStreamer замість ffmpeg.
**Ключове доповнення:** Linux через UART дає команди STM32 — «Luckfox як мозок, STM32 як м'язи».

| Тиждень | Тема | Практична задача | Результат | Стаття? |
|---------|------|-----------------|-----------|---------|
| Тиж 1 | Device Tree теорія | Читаємо DTS Luckfox. compatible, reg, interrupts | Анотований .dts | ні |
| Тиж 2 | DTS overlay + Linux керує STM32 | UART overlay. Linux шле команди → STM32 керує сервоприводом | DTS + STM32 слухає Linux | 📝 DOU |
| Тиж 3 | GStreamer basics | Тестові пайплайни на Pi 3B+. JPEG з USB камери | Перший pipeline | ні |
| Тиж 4 | GStreamer MJPEG стрімінг | v4l2src → jpegenc → tcpserversink. Замінюємо ffmpeg | GStreamer замість ffmpeg | 📝 DOU |

**Результат місяця:**
- Linux керує STM32 через UART — повноцінна система мозок+м'язи
- Вміє читати та модифікувати Device Tree
- GStreamer pipeline замість ffmpeg

---

## Місяць 4 — Linux Kernel Modules

**Мета:** Написати kernel module, реалізувати character device driver.
Це те що відрізняє embedded junior від middle.

| Тиждень | Тема | Практична задача | Результат | Стаття? |
|---------|------|-----------------|-----------|---------|
| Тиж 1 | Hello World kernel module | init/exit, printk, module_param. insmod на Luckfox | hello.ko на реальній платі | ні |
| Тиж 2 | Kernel module + GPIO | Керуємо GPIO через kernel API. LED з ядра | led_driver.ko | 📝 DOU |
| Тиж 3 | Character device driver | /dev/mydev з file_operations: open/read/write/release | /dev/mydev з userspace | ні |
| Тиж 4 | HC-SR04 character driver | trigger GPIO + echo interrupt + відстань через read() | /dev/hcsr04 → cat показує см | 📝 DOU |

**Бонус:** kernel module що читає дані з STM32 через SPI → /dev/stm32sensor

**Результат місяця:**
- Реальний kernel driver задеплоєний на залізо
- Розуміє interrupt handling, file_operations, kobject
- Покриває вимогу вакансій: «інтеграція драйверів та модулів ядра»

---

## Місяць 5 — FreeRTOS + Python автоматизація

**Мета:** RTOS досвід для вакансії дрони/STM32. Python для автоматизації тестування.

| Тиждень | Тема | Практична задача | Результат | Стаття? |
|---------|------|-----------------|-----------|---------|
| Тиж 1 | FreeRTOS basics | Два таски: LED + кнопка. Scheduler, priority, vTaskDelay | FreeRTOS + 2 tasks | ні |
| Тиж 2 | FreeRTOS queues + semaphores | UART таск читає команди, LED виконує. Mutex для UART | UART через Queue | 📝 DOU |
| Тиж 3 | Python serial + pytest | Підключається до STM32 по UART. Автоматичні тести прошивки | pytest suite для STM32 | ні |
| Тиж 4 | Python OpenCV + Luckfox | MJPEG стрім з Luckfox → детекція руху → запис по тригеру | Motion detection на Pi | 📝 DOU |

**Результат місяця:**
- FreeRTOS досвід — покриває вакансію БпЛА
- Python автотести embedded пристроїв — унікально для DOU

---

## Місяць 6 — Фінальний проект

### Варіант A — OpenWrt на TP-Link WR340G (MIPS)

Унікально для резюме: реальний продакшн девайс, інша архітектура (MIPS), flash dumping.

| Тиждень | Задача | Результат | Стаття? |
|---------|--------|-----------|---------|
| Тиж 1 | CH341B + SOIC8: дамп flash. binwalk розпаковка | Повний дамп + squashfs | 📝 DOU |
| Тиж 2 | OpenWrt image builder для ath79. Прошивка UART + TFTP | OpenWrt на роутері | ні |
| Тиж 3 | Власний opkg пакет: C демон моніторить трафік → syslog | opkg install my-monitor | ні |
| Тиж 4 | Фінальна стаття + оновлене резюме | «Зламав і покращив роутер» | 📝 DOU |

### Варіант B — Мікрофонна трилатерація (TDOA)

Luckfox + 3 мікрофони (CZN-15E + MAX9814) + scipy для локалізації джерела звуку.

- Тиж 1: USB audio dongle на Luckfox, запис через ALSA
- Тиж 2: Синхронний запис з 3 мікрофонів, TDOA cross-correlation на Python
- Тиж 3: Визначення напрямку на джерело звуку (300-1200 Hz)
- Тиж 4: Стаття + відео демо

---

## Матриця навичок: до і після курсу

| Навичка | Зараз | Через 6м | Вакансія |
|---------|-------|----------|----------|
| Buildroot / custom Linux image | ✅ | ✅✅ | Вак 1 (основна), Вак 2 |
| Device Tree | ❌ | ✅ | Вак 1 (обов'язк), Вак 2 |
| U-Boot / boot sequence | ❌ | ✅ | Вак 1, Вак 2 |
| Linux kernel modules | ❌ | ✅ | Вак 1 (основна) |
| Character device driver (C) | ❌ | ✅ | Вак 1 (основна) |
| GStreamer / V4L2 | ❌ | ✅ | Вак 2 (ключова) |
| Cross-compilation / toolchains | ✅ | ✅✅ | Всі вакансії |
| C системне програмування | 🟡 | ✅ | Всі вакансії |
| STM32 bare-metal | 🟡 | ✅ | Вак 3 |
| Свій HAL (GPIO/UART/I2C/Timer) | ❌ | ✅ | Вак 3 |
| STM32 ↔ Linux інтеграція | ❌ | ✅ | Вак 1, Вак 3 |
| FreeRTOS | ❌ | ✅ | Вак 3 (основна) |
| Python автоматизація | 🟡 | ✅ | Вак 1, Вак 2 |
| OpenWrt | ❌ | ✅ | Вак 2 (перевага) |
| UART / I2C / SPI / GPIO | ✅ | ✅✅ | Вак 3, Вак 2 |
| Git / Docker / CI | ✅ | ✅✅ | Всі вакансії |
| C++ (базово) | ❌ | 🟡 | Вак 2, Вак 3 (потім) |

*Легенда: ✅ є базово · ✅✅ впевнено · 🟡 частково · ❌ немає*

---

## План статей DOU · 12 статей за 6 місяців

| # | Місяць | Назва статті | Чому цікава читачам |
|---|--------|-------------|---------------------|
| 1 | 1 | STM32 з нуля без HAL: GPIO, регістри, переривання | Альтернатива для тих хто зав'яз у HAL |
| 2 | 1 | Arduino з нуля: пишемо свій HAL для STM32 | Унікально — розуміння через побудову |
| 3 | 1 | SysTick + UART на STM32: час і комунікація | Практика без бібліотек |
| 4 | 2 | STM32 + Linux: перший міст через UART | Як з'єднати bare-metal і embedded Linux |
| 5 | 2 | BR2_PACKAGE: додаємо Go сервер у Buildroot | Майже немає укр. матеріалу |
| 6 | 2 | Boot sequence від нуля: ROM → U-Boot → шел | Базова стаття якої не вистачає |
| 7 | 3 | Luckfox як мозок, STM32 як м'язи | Реальна embedded архітектура |
| 8 | 3 | GStreamer vs ffmpeg: стрімінг з Luckfox | Практичне порівняння з кодом |
| 9 | 4 | Linux kernel driver: від hello world до /dev/hcsr04 | Найбільш технічна — зберемо фахівців |
| 10 | 5 | FreeRTOS на STM32: перші кроки і підводне каміння | Для тих хто переходить з bare-metal |
| 11 | 5 | Python автотести для embedded: pytest + STM32 UART | Унікально для DOU embedded аудиторії |
| 12 | 6 | Я зламав свій роутер і тепер він кращий | TP-Link + OpenWrt — clickbait але по суті |

---

## Ресурси по темах

### STM32 bare-metal
- **RM0008** — STM32F103 Reference Manual (головний довідник)
- **Joseph Yiu** — Ядро Cortex-M3. Повне керівництво
- **Магда Ю.С.** — Програмування C/C++ для мікроконтролерів ARM

### C та Linux системне програмування
- **The Linux Programming Interface** — Michael Kerrisk (біблія)
- **Beej's Guide to C Programming** — безкоштовно онлайн

### Buildroot
- buildroot.org/docs.html — офіційний manual
- Bootlin Buildroot training (PDF, безкоштовно)
- github.com/LuckfoxTECH/luckfox-pico — Luckfox SDK

### U-Boot
- docs.u-boot.org
- Bootlin U-Boot training slides (безкоштовно)

### Device Tree
- devicetree.org/specifications
- Bootlin Device Tree для Linux (slides + lab)

### Linux Kernel / Drivers
- **Linux Device Drivers 3rd ed.** — lwn.net/Kernel/LDD3/ (безкоштовно)
- The Linux Kernel Module Programming Guide — tldp.org
- kernelnewbies.org

### GStreamer
- gstreamer.freedesktop.org/documentation
- kernel.org/doc/html/latest/userspace-api/media/v4l/v4l2.html

### FreeRTOS
- FreeRTOS.org — документація + приклади
- **Mastering the FreeRTOS Real Time Kernel** — безкоштовна книга від автора

---
 

*Вдачі! Головне — не зупинятись після місяця 2 😄*