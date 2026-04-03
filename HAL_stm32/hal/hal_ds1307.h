#ifndef HAL_DS1307_H
#define HAL_DS1307_H

#include <stdint.h>

// DS1307 I2C адреса
#define DS1307_ADDR  0x68

// Регістри (з datasheet Table 2)
#define DS1307_REG_SEC   0x00  // секунди + CH біт
#define DS1307_REG_MIN   0x01  // хвилини
#define DS1307_REG_HOUR  0x02  // години
#define DS1307_REG_DOW   0x03  // день тижня (1=Пн ... 7=Нд)
#define DS1307_REG_DATE  0x04  // число місяця
#define DS1307_REG_MON   0x05  // місяць
#define DS1307_REG_YEAR  0x06  // рік (00-99, тобто 2000-2099)
#define DS1307_REG_CTRL  0x07  // control register

// Структура часу
typedef struct {
    uint8_t sec;   // 0-59
    uint8_t min;   // 0-59
    uint8_t hour;  // 0-23
    uint8_t dow;   // 1-7 (1=Пн, 7=Нд)
    uint8_t date;  // 1-31
    uint8_t month; // 1-12
    uint8_t year;  // 0-99 (2000-2099)
} ds1307_time_t;

// Ініціалізація — запускає годинник (скидає CH біт)
void ds1307_init(void);

// Встановити час і дату
void ds1307_set(ds1307_time_t *t);

// Читати час і дату
void ds1307_get(ds1307_time_t *t);

#endif
