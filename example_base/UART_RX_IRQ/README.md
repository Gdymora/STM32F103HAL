 **UART RX через переривання**:
- `on` → LED вмикається
- `off` → LED вимикається  
- `while(1)` не блокується — команди приходять через ISR в ring buffer

Це принципова різниця від polling версії де `main` висів і чекав.


```bash
minicom -b 9600 -D /dev/ttyUSB0
# Ctrl+A E — local echo
# Ctrl+A X — вихід
```