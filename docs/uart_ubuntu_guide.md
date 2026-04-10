# UART на Ubuntu 22.04

Налаштування CH340 · ST-Link · minicom для STM32F103 Blue Pill

---

## 1. Встановлення інструментів

```bash
sudo apt update && sudo apt install -y \
    gcc-arm-none-eabi \
    binutils-arm-none-eabi \
    make git minicom stlink-tools
```

Перевірка після встановлення:

```bash
arm-none-eabi-gcc --version   # 10.3.1 або новіше
st-flash --version            # v1.7.0 або новіше
minicom --version             # 2.8
```

---

## 2. Проблема CH340 + BRLTTY

На Ubuntu 22.04 є відома проблема: демон `brltty` (програма для шрифту Брайля)
перехоплює CH340 адаптер одразу після підключення і відключає його від системи.
Результат — `/dev/ttyUSB0` не з'являється, навіть коли `lsusb` показує пристрій.

Діагностика — підключи CH340 і перевір dmesg:

```bash
sudo dmesg | tail -20
```

Якщо бачиш такий рядок — проблема підтверджена:

```
usbfs: interface 0 claimed by ch341 while 'brltty' sets config #1
```

> ⚠ `brltty` не потрібен для розробки. Видалення не впливає на роботу системи.

Рішення:

```bash
sudo apt remove brltty
```

Після видалення перепідключи CH340 — `/dev/ttyUSB0` з'явиться одразу.

---

## 3. Права доступу до /dev/ttyUSB0

За замовчуванням `/dev/ttyUSB0` належить групі `dialout`. Звичайний користувач
не має доступу без sudo.

```bash
sudo usermod -aG dialout $USER
```

Застосувати зміни без перезавантаження:

```bash
newgrp dialout
```

> ⚠ Після `usermod` потрібно або перезайти в систему, або виконати `newgrp dialout` в поточному терміналі.

Перевірка:

```bash
ls -la /dev/ttyUSB0
# crw-rw---- 1 root dialout 188, 0 ... /dev/ttyUSB0

groups $USER   # має містити dialout
```

---

## 4. Схема підключення Blue Pill ↔ CH340

| Blue Pill   | CH340 адаптер | Примітка                     |
|-------------|---------------|------------------------------|
| PA9 (TX)    | RX            | Дані від STM32 до ПК         |
| PA10 (RX)   | TX            | Дані від ПК до STM32         |
| GND         | GND           | Спільна земля — обов'язково! |
| 3.3V або 5V | VCC           | Живлення (якщо не від USB)   |

> ⚠ TX завжди підключається до RX і навпаки. TX → RX, RX → TX.

---

## 5. Робота з minicom

### 5.1 Запуск

```bash
minicom -b 9600 -D /dev/ttyUSB0
```

Параметри:
- `-b 9600` — швидкість (baud rate), має збігатись з `uart_init()` у прошивці
- `-D /dev/ttyUSB0` — пристрій CH340

### 5.2 Local Echo — бачити що вводиш

За замовчуванням minicom **не показує** символи які ти вводиш з клавіатури
(echo вимкнений). Вводиш `on` — нічого не видно, але STM32 команду отримав.
Це нормальна поведінка UART: echo показує сам пристрій, а не термінал.

Якщо STM32 не відповідає ехом — увімкни local echo в minicom:

```
Ctrl+A, потім E     ← вмикає/вимикає local echo
```

> ⚠ Якщо STM32 сам відправляє назад отримані символи, і ти увімкнув local echo —
> кожен символ з'явиться двічі. Вибирай одне.

Запустити одразу з увімкненим echo:

```bash
minicom -b 9600 -D /dev/ttyUSB0 -e
```

### 5.3 Гарячі клавіші

| Комбінація         | Дія                                    |
|--------------------|----------------------------------------|
| `Ctrl+A`, потім `E` | Увімкнути/вимкнути local echo         |
| `Ctrl+A`, потім `X` | Вийти з minicom                       |
| `Ctrl+A`, потім `Z` | Головне меню                          |
| `Ctrl+A`, потім `O` | Меню налаштувань (cOnfigure)          |
| `Ctrl+A`, потім `W` | Увімкнути/вимкнути перенос рядків     |
| `Ctrl+A`, потім `C` | Очистити екран                        |
| `Ctrl+A`, потім `L` | Записати лог у файл                   |
| `Ctrl+A`, потім `P` | Змінити baud rate без перезапуску     |

### 5.4 Збереження налаштувань

Щоб не вводити параметри кожного разу:

```
minicom -b 9600 -D /dev/ttyUSB0
  → Ctrl+A, O               # відкрити налаштування
  → Serial port setup       # перевірити порт і швидкість
  → Save setup as dfl       # зберегти як default
```

Після збереження можна запускати просто:

```bash
minicom
```

### 5.5 Після прошивки

> ⚠ Після `st-flash` minicom треба перезапустити — інакше новий вивід не з'явиться.

```bash
# Вийти: Ctrl+A, X
# Запустити знову:
minicom -b 9600 -D /dev/ttyUSB0
```

Альтернатива без minicom:

```bash
cat /dev/ttyUSB0          # просто дивитись вивід
echo 'on' > /dev/ttyUSB0  # відправити команду
```

---

## 6. Прошивка через ST-Link

### 6.1 Підключення ST-Link v2

| ST-Link v2 | Blue Pill    | Примітка          |
|------------|--------------|-------------------|
| SWDIO      | SWDIO (IO)   | Дані              |
| SWCLK      | SWCLK (CLK)  | Тактування        |
| GND        | GND          | Земля             |
| 3.3V       | 3.3V         | Живлення (опційно)|

### 6.2 Команди st-flash

```bash
# Прошити бінарний файл
st-flash write firmware.bin 0x08000000

# Стерти flash
st-flash erase

# Інформація про мікроконтролер
st-info --probe
```

> ✓ Адреса `0x08000000` — початок Flash пам'яті STM32F103. Завжди прошивай на цю адресу.

---

## 7. Типові помилки та рішення

| Помилка | Рішення |
|---------|---------|
| `cannot open /dev/ttyUSB0: No such file` | CH340 не підключений або brltty перехопив. Видали brltty, перепідключи адаптер. |
| `/dev/ttyUSB0: Permission denied` | `sudo usermod -aG dialout $USER`, потім `newgrp dialout` |
| Нічого не виводиться в minicom | Перевір TX/RX не переплутані. Після прошивки перезапусти minicom. |
| `st-flash: LIBUSB_ERROR_ACCESS` | `sudo st-flash write ...` або додай udev rules для ST-Link. |
| `WARN: Failed to parse flash type` | Нормально для клонів Blue Pill. Прошивка все одно проходить. |
| `arm-none-eabi-gcc: command not found` | `sudo apt install gcc-arm-none-eabi` |

---

## 8. Швидка перевірка оточення

```bash
arm-none-eabi-gcc --version    # toolchain
st-flash --version             # прошивальник
minicom --version              # термінал
ls /dev/ttyUSB0                # CH340 підключений
groups | grep dialout          # права є
```

Якщо всі 5 пунктів пройшли — оточення готове до роботи.

---

*STM32F103 Blue Pill · STM32 з нуля без HAL · [github.com/pipicosim800-maker/stm32F103](https://github.com/pipicosim800-maker/stm32F103)*
