#include "hal_ds1307.h"
#include "hal_i2c.h"

// ─── BCD конвертація ─────────────────────────────────────────
// DS1307 зберігає всі значення в BCD форматі
// BCD: 0x49 = 4*10 + 9 = 49
// Наприклад: 23 години → записуємо 0x23, читаємо 0x23 → 23

static uint8_t bcd_to_dec(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static uint8_t dec_to_bcd(uint8_t dec) {
    return ((dec / 10) << 4) | (dec % 10);
}

// ─── Публічні функції ─────────────────────────────────────────

void ds1307_init(void) {
    // Читаємо регістр секунд
    uint8_t sec = i2c1_read(DS1307_ADDR, DS1307_REG_SEC);

    // Біт 7 (CH — Clock Halt) = 1 означає що годинник зупинений
    // Скидаємо його щоб годинник пішов
    if (sec & 0x80) {
        sec &= ~0x80;  // CH = 0
        i2c1_write(DS1307_ADDR, DS1307_REG_SEC, sec);
    }

    // Вимикаємо SQW виход (не потрібен)
    i2c1_write(DS1307_ADDR, DS1307_REG_CTRL, 0x00);
}

void ds1307_set(ds1307_time_t *t) {
    // Записуємо всі 7 регістрів одним burst write
    // CH=0 (біт 7 секунд) — годинник запущено
    uint8_t buf[7];
    buf[0] = dec_to_bcd(t->sec)   & 0x7F;  // CH=0
    buf[1] = dec_to_bcd(t->min);
    buf[2] = dec_to_bcd(t->hour)  & 0x3F;  // 24-год режим (біт 6=0)
    buf[3] = dec_to_bcd(t->dow);
    buf[4] = dec_to_bcd(t->date);
    buf[5] = dec_to_bcd(t->month);
    buf[6] = dec_to_bcd(t->year);

    // Записуємо починаючи з регістру 0x00
    // Використовуємо burst write через i2c напряму
    // (i2c1_write записує один байт, нам треба 7)
    // Тому звертаємось до I2C напряму:
    extern void i2c1_write_buf(uint8_t addr, uint8_t reg,
                                uint8_t *data, uint8_t len);
    i2c1_write_buf(DS1307_ADDR, DS1307_REG_SEC, buf, 7);
}

void ds1307_get(ds1307_time_t *t) {
    uint8_t buf[7];
    i2c1_read_buf(DS1307_ADDR, DS1307_REG_SEC, buf, 7);

    t->sec   = bcd_to_dec(buf[0] & 0x7F);  // маскуємо CH біт
    t->min   = bcd_to_dec(buf[1]);
    t->hour  = bcd_to_dec(buf[2] & 0x3F);  // маскуємо 12/24 біт
    t->dow   = bcd_to_dec(buf[3]);
    t->date  = bcd_to_dec(buf[4]);
    t->month = bcd_to_dec(buf[5]);
    t->year  = bcd_to_dec(buf[6]);
}
