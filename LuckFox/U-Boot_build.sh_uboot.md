## Що робимо і навіщо

Ми модифікуємо U-Boot — це bootloader який запускається першим на Luckfox Pico Pro, ще до Linux. Три патчі, кожен окремий концепт для статті.

---

### Патч 1: `CONFIG_BOOTDELAY=3`

**Що:** змінюємо одну цифру в defconfig.

**Навіщо:** зараз `bootdelay=0` — U-Boot стартує і **одразу** передає керування Linux без паузи. Через це я мусив робити трюк з Ctrl+C щоб зловити prompt. Після патчу буде 3 секунди на натискання будь-якої клавіші — як у звичайному PC BIOS.

**Для статті:** показує що defconfig — це не магія, а звичайний текстовий файл з `KEY=value`. Одна цифра змінює поведінку всього завантаження.

---

### Патч 2: `checkboard()`

**Що:** додаємо функцію в board-специфічний C файл.

**Навіщо:** `checkboard()` — це `__weak` хук в U-Boot. Тобто в `board_info.c` вже є порожня версія з атрибутом `weak`, і якщо ти визначаєш свою — компілятор бере твою. U-Boot викликає її автоматично при старті після ініціалізації залізa. Результат — твій рядок з'являється в консолі при кожному boot.

**Для статті:** пояснює механізм `__weak` функцій — важливий паттерн в embedded C. Також показує де в дереві коду живе board-специфічна логіка (`board/rockchip/evb_rv1106/`).

---

### Патч 3: команда `mystatus`

**Що:** новий файл `cmd/mystatus.c` + один рядок в `Makefile`.

**Навіщо:** U-Boot має власний shell з командами (`printenv`, `md`, `version`...). Кожна команда — це окремий `.c` файл з макросом `U_BOOT_CMD()`. Ми додаємо свою команду яка друкує інфо про систему і читає `gd->ram_size` — глобальну структуру даних U-Boot де зберігається стан системи.

**Для статті:** показує архітектуру команд U-Boot. Читач розуміє що U-Boot — це не чорна скринька, а звичайний C проект куди можна додати свій код за 20 рядків.

---

### Після збірки

`./build.sh uboot` перекомпілює U-Boot і покладе новий `uboot.img` в `output/image/`. Прошиваємо **тільки його** через `upgrade_tool` — не весь `update.img`. Це важливо: повний reflash займає хвилини, тільки uboot — секунди.

---

три патчі по порядку.

---

## Патч 1: bootdelay в defconfig

```bash
sed -i 's/CONFIG_BOOTDELAY=0/CONFIG_BOOTDELAY=3/' \
  ~/luckfox-pico/sysdrv/source/uboot/u-boot/configs/luckfox_rv1106_uboot_defconfig

# Перевірка
grep BOOTDELAY ~/luckfox-pico/sysdrv/source/uboot/u-boot/configs/luckfox_rv1106_uboot_defconfig
```

---

## Патч 2: checkboard() в evb_rv1106.c

```bash
cat >> ~/luckfox-pico/sysdrv/source/uboot/u-boot/board/rockchip/evb_rv1106/evb_rv1106.c << 'EOF'

int checkboard(void)
{
	printf("Board: Luckfox Pico Pro (custom build by Alex)\n");
	printf("Series: STM32 з нуля без HAL — article 6\n");
	return 0;
}
EOF
```

---

## Патч 3: нова команда mystatus

```bash
cat > ~/luckfox-pico/sysdrv/source/uboot/u-boot/cmd/mystatus.c << 'EOF'
/*
 * mystatus - custom command for Luckfox Pico Pro
 * Part of "STM32 з нуля без HAL" article series
 */
#include <common.h>
#include <command.h>

static int do_mystatus(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	DECLARE_GLOBAL_DATA_PTR;

	printf("\n=== Luckfox Pico Pro — System Status ===\n");
	printf("U-Boot version : 2017.09 (custom patch)\n");
	printf("CPU            : RV1103/RV1106 (Cortex-A7)\n");
	printf("DRAM           : %lu MiB\n", gd->ram_size / 1024 / 1024);
	printf("Boot device    : SPI NAND\n");
	printf("Author         : Alex (STM32 без HAL series)\n");
	printf("=========================================\n\n");
	return 0;
}

U_BOOT_CMD(
	mystatus, 1, 0, do_mystatus,
	"show Luckfox system status",
	"  — prints CPU, RAM, boot device info"
);
EOF
```

Тепер реєструємо команду в Makefile:

```bash
# Додаємо після рядка з demo.o
sed -i 's/obj-\$(CONFIG_CMD_DEMO) += demo.o/obj-\$(CONFIG_CMD_DEMO) += demo.o\nobj-y += mystatus.o/' \
  ~/luckfox-pico/sysdrv/source/uboot/u-boot/cmd/Makefile

# Перевірка
grep mystatus ~/luckfox-pico/sysdrv/source/uboot/u-boot/cmd/Makefile
```

---

## Збірка

```bash
cd ~/luckfox-pico
./build.sh uboot 2>&1 | tail -30
```

Запускати по порядку . Особливо важливий вивід `./build.sh uboot` — там побачимо чи є помилки компіляції.\