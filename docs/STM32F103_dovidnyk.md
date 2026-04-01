# STM32F103C8T6 — Dovidnyk serii "STM32 bez mahii"

> Tsentralnyi dokument serii. Uroky posylaiutsia siudy zamist povtorennia odnoho y toho zh.
> Dzherelo: RM0008 (Reference Manual), STM32F103C8T6 datasheet.

---

## Rozdil 1 — Piny Blue Pill

### Naivazhlivishi piny

| Pin  | Funktsiia     | Prymitka                        |
|------|---------------|---------------------------------|
| PC13 | Vbudovanyi LED| Invertovanyi: LOW = horyt       |
| PA9  | USART1_TX     | Urok 3                          |
| PA10 | USART1_RX     | Urok 3                          |
| PA2  | USART2_TX     | alternatyvnyi UART              |
| PA3  | USART2_RX     | alternatyvnyi UART              |
| PB6  | I2C1_SCL      | Urok 4                          |
| PB7  | I2C1_SDA      | Urok 4                          |
| PA5  | SPI1_SCK      |                                 |
| PA6  | SPI1_MISO     |                                 |
| PA7  | SPI1_MOSI     |                                 |
| SWDIO | SWD data    | ST-Link pidkliuchennia          |
| SWDCLK| SWD clock  | ST-Link pidkliuchennia          |

### GPIO rezhymy (bity CNF + MODE v CRL/CRH)

```
MODE[1:0]   Shcho oznachaie
00          Input
01          Output 10 MHz
10          Output 2 MHz   <- vykorystovuiemo dlia LED
11          Output 50 MHz

CNF[1:0] dlia OUTPUT:
00          Push-pull       <- dlia LED
01          Open-drain
10          Alt. push-pull  <- dlia UART TX, SPI
11          Alt. open-drain <- dlia I2C SDA, SCL

CNF[1:0] dlia INPUT:
00          Analog
01          Floating        <- za zamovchuvannyam
10          Pull-up/down
```

### Rehistry GPIO (zmishchennia vid bazovoi adresy)

| Zmishchennia | Rehistr | Shcho robyt          |
|--------------|---------|----------------------|
| 0x00         | CRL     | nalashtuvannnia piniv 0-7  |
| 0x04         | CRH     | nalashtuvannnia piniv 8-15 |
| 0x08         | IDR     | chytannia vkhodiv    |
| 0x0C         | ODR     | zapys vykhodiv       |
| 0x10         | BSRR    | atomarnyi set/reset  |
| 0x14         | BRR     | atomarnyi reset      |

---

## Rozdil 2 — Tablytsia vektoriv pereryvann

### Systemni vektory Cortex-M3 (pershi 16)

| #  | Zmishchennia | Nazva           | Koly                        |
|----|--------------|-----------------|---------------------------  |
| 0  | 0x00         | Initial SP      | pochatkova adresa steku     |
| 1  | 0x04         | Reset           | pislia uvimknennia/reset    |
| 2  | 0x08         | NMI             | nevidiiemne pereryvannnia   |
| 3  | 0x0C         | HardFault       | krytychna pomylka           |
| 11 | 0x2C         | SVCall          | systemnyi vyklyk            |
| 14 | 0x38         | PendSV          | FreeRTOS context switch     |
| 15 | 0x3C         | SysTick         | systemnyi taiimer <- Urok 2 |

### Pereryvannnia peryferii STM32F103

| IRQ# | Vektor# | Zmishchennia | Nazva    | Urok   |
|------|---------|--------------|----------|--------|
| 5    | 21      | 0x54         | EXTI0    | --     |
| 17   | 33      | 0x84         | USART1   | Urok 3 |
| 18   | 34      | 0x88         | USART2   | Urok 3 |
| 21   | 37      | 0x94         | I2C1_EV  | Urok 4 |
| 22   | 38      | 0x98         | I2C1_ER  | Urok 4 |
| 25   | 41      | 0xA4         | SPI1     | --     |
| 28   | 44      | 0xB0         | TIM2     | --     |
| 29   | 45      | 0xB4         | TIM3     | --     |

> Povna tablytsia: RM0008, hlava 10, tablytsia 63

---

## Rozdil 3 — Karta pamiati i bazovi adresy

### Zahalna memory map Cortex-M3

```
0x08000000 - 0x0800FFFF   Flash (64 KB dlia C8T6)
0x20000000 - 0x20004FFF   SRAM (20 KB)
0x40000000 - 0x400237FF   APB1 peryferiya
0x40010000 - 0x40013FFF   APB2 peryferiya
0xE000E010                SysTick
0xE000E100                NVIC
```

### Bazovi adresy peryfeiii

| Peryferiya | Adresa     | Shyna | Bit v RCC         |
|------------|------------|-------|-------------------|
| TIM2       | 0x40000000 | APB1  | RCC_APB1ENR[0]    |
| USART2     | 0x40004400 | APB1  | RCC_APB1ENR[17]   |
| USART3     | 0x40004800 | APB1  | RCC_APB1ENR[18]   |
| I2C1       | 0x40005400 | APB1  | RCC_APB1ENR[21]   |
| I2C2       | 0x40005800 | APB1  | RCC_APB1ENR[22]   |
| GPIOA      | 0x40010800 | APB2  | RCC_APB2ENR[2]    |
| GPIOB      | 0x40010C00 | APB2  | RCC_APB2ENR[3]    |
| GPIOC      | 0x40011000 | APB2  | RCC_APB2ENR[4]    |
| SPI1       | 0x40013000 | APB2  | RCC_APB2ENR[12]   |
| USART1     | 0x40013800 | APB2  | RCC_APB2ENR[14]   |
| TIM1       | 0x40012C00 | APB2  | RCC_APB2ENR[11]   |

### Systemni rehistry Cortex-M3

| Rehistr      | Adresa     | Shcho robyt                  |
|--------------|------------|------------------------------|
| SYSTICK_CTRL | 0xE000E010 | uvimknennia SysTick <- Urok 2|
| SYSTICK_LOAD | 0xE000E014 | znachennia perezavantazhennia |
| SYSTICK_VAL  | 0xE000E018 | potochne znachennia          |
| NVIC_ISER0   | 0xE000E100 | uvimknennia pereryvann 0-31  |
| NVIC_ISER1   | 0xE000E104 | uvimknennia pereryvann 32-63 |

---

## Rozdil 4 — RCC: taktuvannia

### Shcho take RCC

RCC (Reset and Clock Control) — blok yakyi keruie taktuvanniiam vsiei peryfeiii.
Persh nizh vykorystovuvaty bud-yaku peryferiiiu — treba uvimknuty yii taktuvannia
cherez RCC. Inakshe zapys v rehistry peryfeiii prosto ne spratsiuie.

### Bazova adresa RCC

```c
#define RCC_BASE     0x40021000
#define RCC_APB2ENR  (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR  (*(volatile uint32_t *)(RCC_BASE + 0x1C))
```

### Taktuvannia pislia reset

```
Pislia reset STM32F103 pratsiuie na:
  HSI = 8 MHz (RC-henerator, vbudovanyi)
  SYSCLK = 8 MHz
  APB1 = 8 MHz
  APB2 = 8 MHz

Maksymalna chastota (z PLL):
  SYSCLK = 72 MHz
  APB1 = 36 MHz (maksymum dlia APB1)
  APB2 = 72 MHz
```

> U nashii serii urokiv pratsiuiemo na 8 MHz (bez nalashtuvannnia PLL).

### Rozrakhunok SYSTICK_LOAD

```
SYSTICK_LOAD = (SYSCLK / potribna_chastota) - 1

Dlia 1 ms pry 8 MHz:
  SYSTICK_LOAD = (8_000_000 / 1000) - 1 = 7999

Dlia 1 ms pry 72 MHz:
  SYSTICK_LOAD = (72_000_000 / 1000) - 1 = 71999
```

---

## Rozdil 5 — Korysni makrosy (kopiui v proekt)

```c
// === RCC ================================================
#define RCC_BASE        0x40021000UL
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x1C))

#define RCC_IOPAEN      (1 << 2)
#define RCC_IOPBEN      (1 << 3)
#define RCC_IOPCEN      (1 << 4)   // <- nash GPIOC
#define RCC_SPI1EN      (1 << 12)
#define RCC_USART1EN    (1 << 14)  // <- Urok 3
#define RCC_I2C1EN      (1 << 21)  // <- Urok 4
#define RCC_USART2EN    (1 << 17)

// === GPIOC ==============================================
#define GPIOC_BASE      0x40011000UL
#define GPIOC_CRL       (*(volatile uint32_t *)(GPIOC_BASE + 0x00))
#define GPIOC_CRH       (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_IDR       (*(volatile uint32_t *)(GPIOC_BASE + 0x08))
#define GPIOC_ODR       (*(volatile uint32_t *)(GPIOC_BASE + 0x0C))
#define GPIOC_BSRR      (*(volatile uint32_t *)(GPIOC_BASE + 0x10))

// === GPIOA ==============================================
#define GPIOA_BASE      0x40010800UL
#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_CRH       (*(volatile uint32_t *)(GPIOA_BASE + 0x04))
#define GPIOA_IDR       (*(volatile uint32_t *)(GPIOA_BASE + 0x08))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))
#define GPIOA_BSRR      (*(volatile uint32_t *)(GPIOA_BASE + 0x10))

// === GPIOB ==============================================
#define GPIOB_BASE      0x40010C00UL
#define GPIOB_CRL       (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_CRH       (*(volatile uint32_t *)(GPIOB_BASE + 0x04))
#define GPIOB_IDR       (*(volatile uint32_t *)(GPIOB_BASE + 0x08))
#define GPIOB_ODR       (*(volatile uint32_t *)(GPIOB_BASE + 0x0C))
#define GPIOB_BSRR      (*(volatile uint32_t *)(GPIOB_BASE + 0x10))

// === USART1 (PA9=TX, PA10=RX) ===========================
#define USART1_BASE     0x40013800UL
#define USART1_SR       (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART1_DR       (*(volatile uint32_t *)(USART1_BASE + 0x04))
#define USART1_BRR      (*(volatile uint32_t *)(USART1_BASE + 0x08))
#define USART1_CR1      (*(volatile uint32_t *)(USART1_BASE + 0x0C))
// USART1_SR bits
#define USART_SR_TXE    (1 << 7)   // TX buffer empty
#define USART_SR_TC     (1 << 6)   // transmission complete
#define USART_SR_RXNE   (1 << 5)   // RX not empty
// USART1_CR1 bits
#define USART_CR1_UE    (1 << 13)  // USART enable
#define USART_CR1_TE    (1 << 3)   // transmitter enable
#define USART_CR1_RE    (1 << 2)   // receiver enable

// === I2C1 (PB6=SCL, PB7=SDA) ============================
#define I2C1_BASE       0x40005400UL
#define I2C1_CR1        (*(volatile uint32_t *)(I2C1_BASE + 0x00))
#define I2C1_CR2        (*(volatile uint32_t *)(I2C1_BASE + 0x04))
#define I2C1_DR         (*(volatile uint32_t *)(I2C1_BASE + 0x10))
#define I2C1_SR1        (*(volatile uint32_t *)(I2C1_BASE + 0x14))
#define I2C1_SR2        (*(volatile uint32_t *)(I2C1_BASE + 0x18))
#define I2C1_CCR        (*(volatile uint32_t *)(I2C1_BASE + 0x1C))
#define I2C1_TRISE      (*(volatile uint32_t *)(I2C1_BASE + 0x20))

// === SysTick ============================================
#define SYSTICK_BASE    0xE000E010UL
#define SYSTICK_CTRL    (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define SYSTICK_LOAD    (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define SYSTICK_VAL     (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))
// SYSTICK_CTRL bits
#define SYSTICK_ENABLE  (1 << 0)
#define SYSTICK_TICKINT (1 << 1)   // enable interrupt
#define SYSTICK_CLKSRC  (1 << 2)   // use processor clock
```

---

## Rozdil 6 — BOOT0/BOOT1 i rezhymy zavantazhennia

| BOOT1 | BOOT0 | Rezhym        | Koly vykorystovuvaty             |
|-------|-------|---------------|----------------------------------|
| x     | 0     | Flash         | normalna robota                  |
| 0     | 1     | System memory | proshyvka cherez UART bez ST-Link|
| 1     | 1     | SRAM          | vidlahodzhennia bez zapysu       |

> Na Blue Pill ie dva dzhampery BOOT0 i BOOT1.
> Zvychaino obydva v polozheni 0 — prohrama startuie z Flash.

---

## Rozdil 7 — Kharakterystyky chipa

| Parametr          | Znachennia              |
|-------------------|-------------------------|
| Yadro             | ARM Cortex-M3           |
| Arkhitektura      | ARMv7-M                 |
| Taktova chastota  | do 72 MHz               |
| Flash             | 64 KB                   |
| SRAM              | 20 KB                   |
| GPIO              | 37 piniv                |
| USART             | 3 (USART1/2/3)          |
| SPI               | 2 (SPI1/2)              |
| I2C               | 2 (I2C1/2)              |
| ADC               | 2 x 12-bit              |
| Napruha zhyvlennia| 2.0 - 3.6 V             |
| Korpus            | LQFP48                  |
| Chip ID           | 0x0410                  |

---

## Navihatsiia po serii

| Urok   | Tema                          | Rozdily dovidnyka |
|--------|-------------------------------|-------------------|
| Urok 0 | Shcho tse take? HAL, vektory  | 3, 2              |
| Urok 1 | Pershyi bit — mihaiemo LED    | 1, 3, 4, 5        |
| Urok 2 | SysTick i pereryvannnia       | 2, 3, 4           |
| Urok 3 | UART — tekst u terminal       | 1, 2, 3, 5        |
| Urok 4 | I2C — pidkliuchuiemo sensor   | 1, 2, 3, 5        |
| Urok 5 | Mini-proekt — vse razom       | vsi rozdily       |

---
*STM32F103C8T6 / Blue Pill / arm-none-eabi-gcc 10.3 / Linux*
*Dzherela: RM0008 rev 21, STM32F103C8 datasheet rev 17*
