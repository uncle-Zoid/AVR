# AVR
Libraries for DS18B20, Dalas CRC, OWI, 7seg written in c++.

Main loop run Thermometer and sends data via UART.


**Protocol**
| HEAD | size | command | data |
| --- | --- | --- | ---|
|0x1A|Bytes||optional data

| Command | Value | Data | Description |
| :--      | :--:    | :---   | -- |
| SEND_POWER_MODE |1 | <ul><li>0 = no sensor</li><li>1 = parasitic supply</li><li>2 = external supply</li></ul> | |
| SEND_ROM | 2 | 8 B od sensor ROM |
| SEND_SCRATCHPAD | 3 | 9 B od scratchpad
| SEND_TEMPERATURE | 4 | -- | send single shot temperature via UART <br>Stops automatical measure.
| SET_AUTOMATIC_MEASURE | 5 | -- | start automatical measure
| SET_SENSOR_PARAMS | 6 | Th, Tl, config<br>to EEPROM | sets sensor registers
| SET_MEASURE_PERIOD | 7 | 2 B | time in milliseconds

**Scratchpad**

| Byte | Description |
| -- | -- |
| TEMPERATURE_LSB | lower byte of temperature |
| TEMPERATURE_MSB | higher byte of temperature |
| ThRegister | alarmu high value|
| TlRegister | alarmu low value |
| ConfigRegister | sensor resolution |
| Reserved | contains 0xFF |
| Reserved | |
| Reserved |contains 0x10 |
| Crc | Dallas CRC 8

**Sensor Resolution**

| Resolition | Code |
| -- | -- |
| 12B | 0x7F
| 11B | 0x5F
| 10B | 0x3F
|  9B | 0x1F