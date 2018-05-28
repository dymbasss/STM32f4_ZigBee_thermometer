**Library for internal thermometer and I²C LCD1602**

This library is implemented in the ZigBee stack. There are two boards [STM32F407VG](http://www.st.com/en/evaluation-tools/stm32f4discovery.html). One board serves as a thermometer (zdo_start_zr.c), another as [I²C LCD1602](http://wiki.sunfounder.cc/index.php?title=I%C2%B2C_LCD1602) (zdo_start_zc.c). The signal is transmitted through [MOD-MRF24J40](https://www.olimex.com/Products/Modules/RF/MOD-MRF24J40/).
