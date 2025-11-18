# K715 Pro Debug Notes

## Hardware Information

### Chip Information

#### Main MCU

STM32F103RCT6 - STM High-density performance line Cortex-M3 microcontroller - [Datasheet link](datasheets/stm32f103rc_datasheet.pdf)

* 256KB flash
* 48KB SRAM
* 5 USARTs
* 4 general-purpose 16-bit timers, 2 advanced-control PWM timers, 2 basic timers
* 3 ADCs, 2 DACs
* USB, CAN support
* LQFP64 package

📝 **Note:** The MCU configured by default for tihs keyboard's bootloader[^1] in QMK is STM32F103xB, which is a lower-spec chip with less peripherals and more importantly, less flash (64 kB) and SRAM (20 kB). A better option is the STM32F103xE, which is the same as the STM32F103xC, but has more flash (512 kB) and SRAM (64 kB). The following lines in `board.h` and the custom linker script `ld/STM32F103xC_stm32duino.ld` referenced in `rules.mk` correct this issue.
[^1]: (stm32duino)

```h
//board.h
#undef STM32F103xB
#define STM32F103xE
```

```make
#rules.mk
MCU_LDSCRIPT = STM32F103xC_stm32duino
```

The `STM32F103xC` linker `ld` script is essentially the same as the `STM32F103xB`, save for defining expanded flash and RAM memory ranges.

#### Bluetooth Processor

FR5082DM Bluetooth SoC - [Datasheet link](datasheets/FR5082_datasheet.pdf)

* Integrated RF, CODEC, PMU, Baseband, Cortex-M3 CPU, and audio DSP
* 1 MB flash for Cortex M3
* QSPI interface with STM MCU

#### LED Drivers

IS31FL3723 12x16 dots matrix LED driver (2 are placed on the PCB) - [Datasheet link](datasheets/IS31FL3723_datasheet.pdf)

* 2 on the board
* Can drive up to 64 RGB LEDs each
* 1 MHz I2C interface
* Appears pin-compatible with IS31FL3733, which driver QMK uses for this chip.

#### Flash Chip

FT24C512A 64 kB two-wire serial EEPROM - [Datasheet link](datasheets/FMD_24C512A_datasheet.pdf)

* 1 MHz I2C interface
* Typically 1,000,000 program-erase cycles

Note: This isn't configured in the base Redragon QMK source. I can't get it to respond with an I2C scan of the I2C bus that the LED drivers use, either. There is a chance that it is on I2C2, which would require custom code to communicate with.

## Stock Firmware Investigation

### Battery Calculations

I found the floating point code at [this github link.](https://github.com/MrMatch246/rAIversingEvaluation/blob/ebac6d2aa76a4db68263c19291353b50db1140dc/gpt-3.5-turbo/p2im_maj_full_v1_1/extraction/Robot/Robot_original.c#L1159)

Descriptions of the functions are [in the GCC documentation.](https://gcc.gnu.org/onlinedocs/gccint/Soft-float-library-routines.html)

The get_battery_voltage function at 0x08005d90 does the following:

1. Read the ADC1 V<sub>REFINT</sub> voltage
2. Read the V<sub>BATT</sub> voltage (from pin C0, I think)
3. Convert the V<sub>BATT</sub> voltage to a float V<sub>BATT<sub>float</sub></sub>
4. Multiply V<sub>BATT<sub>float</sub></sub> by 1.2
5. Convert the V<sub>REFINT</sub> to a float V<sub>REFINT<sub>float</sub></sub>
6. Divide V<sub>BATT<sub>float</sub></sub> by V<sub>REFINT<sub>float</sub></sub>
7. Multiply the result of that division by 1000
8. Add the result of that multiplication to itself
9. Convert the result of that addition back to an int
10. Mask off the upper 16 bits so only 16 bits remain

### Pinout Listing for STM32F103RCT6

|   Pin Name    |   QMK function    |   Alternate Functions |   Notes   |
|---------------|-------------------|-----------------------|-----------|
| PA15 | COL15 | JTDI | Gets configured as PAL_MODE_OUTPUT_PUSHPULL in board_init() in btsys.c, disabling JTAG/SWD in normal operation |
|PA14|COL14|JTCK-SWCLK|Gets configured as PAL_MODE_OUTPUT_PUSHPULL in board_init() in btsys.c, disabling JTAG/SWD in normal operation|
|PA13|COL13|JTSM-SWDIO|Gets configured as PAL_MODE_OUTPUT_PUSHPULL in board_init() in btsys.c, disabling JTAG/SWD in normal operation|



### Output from HID Console

* Pressing the DEBUG key:

```text
DEBUG: disabled.
DEBUG: enabled.
```

* Pressing the "speed up RGB" key:

```text
rgb matrix set speed [EEPROM]: 255
[01045892]led_blink_start(1045892):led 2, 12, (250,250), 3, 0
rgb matrix set speed [EEPROM]: 255
[01046165]led_blink_start(1046165):led 2, 12, (250,250), 3, 0
rgb matrix set speed [EEPROM]: 255
[01046614]led_blink_start(1046614):led 2, 12, (250,250), 3, 0
[01048159]led_set(1048159):stop led 2, 12, 0
```

* Holding down the knob button, then turning:

```text
[01171758]led_blink_start(1171758):led 2, 11, (300,300), 2, 0
[01171758]encoder switch,mode=1
[01173007]led_set(1173007):stop led 2, 11, 0
[01175073]clockwise=1,layer=0
[01175073]led_blink_start(1175073):led 2, 12, (250,250), 3, 0
[01176608]led_set(1176608):stop led 2, 12, 0
[01179548]clockwise=1,layer=0
[01179548]led_blink_start(1179548):led 2, 12, (250,250), 3, 0
[01181057]led_set(1181057):stop led 2, 12, 0
```

* A bunch of presses while in that mode:

```text
[01228928]encoder short press,mode=1
rgb matrix mode [EEPROM]: 11
[01232098]encoder short press,mode=1
rgb matrix mode [EEPROM]: 12
[01235493]encoder short press,mode=1
rgb matrix mode [EEPROM]: 13
[01235756]encoder short press,mode=1
rgb matrix mode [EEPROM]: 1
[01236020]encoder short press,mode=1
rgb matrix mode [EEPROM]: 2
[01236448]encoder short press,mode=1
rgb matrix mode [EEPROM]: 3
[01236742]encoder short press,mode=1
rgb matrix mode [EEPROM]: 4
[01236986]encoder short press,mode=1
rgb matrix mode [EEPROM]: 5
[01237906]encoder short press,mode=1
rgb matrix mode [EEPROM]: 6
[01238250]encoder short press,mode=1
rgb matrix mode [EEPROM]: 7
[01239024]encoder short press,mode=1
rgb matrix mode [EEPROM]: 8
[01239664]encoder short press,mode=1
rgb matrix mode [EEPROM]: 9
[01240684]encoder short press,mode=1
rgb matrix mode [EEPROM]: 10
```

* Holding down the button again to get out of that mode:

```text
[01293466]led_blink_start(1293466):led 2, 11, (300,300), 2, 0
[01293466]encoder switch,mode=0
[01294710]led_set(1294710):stop led 2, 11, 0
```

* Holding down FN+3 to switch BT modes:

```text
[01293466]led_blink_start(1293466):led 2, 11, (300,300), 2, 0
[01293466]encoder switch,mode=0
[01294710]led_set(1294710):stop led 2, 11, 0
[01352042]set bt_chn:3
[01352042]led_blink_start(1352042):led 0, 4, (250,250), 120, 88
[01352042]send pair...
[01352237]spi rd 0x51-dump 12 Bytes
01 00 00 00 00 FF 00 00 00 00 00 FF
[01352237]mark bt disconnected
[01352238]disconnect:1411042,1352238,1,0
[01352238]bt pair again
[01352239]send pair...
```

* Tap FN+1 to go back to paired with this PC:

```text
[01381235]set bt_chn:1
[01381235]led_blink_start(1381235):led 0, 1, (500,500), 60, 88
[01381236]send reconn...
[01381338]spi rd 0x51-dump 12 Bytes
01 02 00 00 00 FF 00 00 00 00 00 FF
[01381339]bt connected:0x01,chn:1
[01381512]spi rd 0x51-dump 12 Bytes
01 02 00 00 00 FF 00 00 00 00 00 FF
[01381513]bt connected:0x01,chn:1
```

* Encoder things - turn one clockwise then one counterclockwise:

```text
[01432518]clockwise=1,layer=0
[01432518]BLE extra rpt,size=3,id=4
[01432518]rpt_extra-dump 3 Bytes
04 E9 00
[01432529]BLE extra rpt,size=3,id=4
[01432529]rpt_extra-dump 3 Bytes
04 00 00
[01434498]clockwise=0,layer=0
[01434498]BLE extra rpt,size=3,id=4
[01434499]rpt_extra-dump 3 Bytes
04 EA 00
[01434509]BLE extra rpt,size=3,id=4
[01434510]rpt_extra-dump 3 Bytes
04 00 00
```

* Switch to BT #2 where other computer is connected, pairing failed, try pairing again, then switch back to BT #1:

```text
[01541128]set bt_chn:2
[01541128]led_blink_start(1541128):led 0, 2, (250,250), 120, 88
[01541129]send pair...
[01541310]spi rd 0x51-dump 12 Bytes
01 00 00 00 00 FF 00 00 00 00 00 FF
[01541310]mark bt disconnected
[01541310]disconnect:1600129,1541310,1,0
[01541311]bt pair again
[01541312]send pair...
[01601157]led_set(1601157):stop led 0, 2, 88
[01601491]spi rd 0x51-dump 12 Bytes
01 00 00 00 00 FF 00 00 00 00 00 FF
[01601492]mark bt disconnected
[01601492]disconnect:1600129,1601492,1,0
[01601492]bt pair/reconn timeout!
[01615298]led_blink_start(1615298):led 0, 2, (250,250), 120, 88
[01615298]send pair...
[01615352]spi rd 0x51-dump 12 Bytes
01 01 00 00 00 FF 00 00 00 00 00 FF
[01615352]bt discover:0x01,chn:2,p:1
[01615353]mark bt disconnected
[01631022]spi rd 0x51-dump 12 Bytes
01 02 00 00 00 FF 00 00 00 00 00 FF
[01631023]bt connected:0x01,chn:2
[01631023]update connected:0x03,chn:2,b:0x01
[01637465]spi rd 0x51-dump 12 Bytes
01 02 00 00 00 FF 00 00 00 00 00 FF
[01637465]bt connected:0x03,chn:2
[01681469]set bt_chn:1
[01681469]led_blink_start(1681469):led 0, 1, (500,500), 60, 88
[01681470]send reconn...
[01681661]spi rd 0x51-dump 12 Bytes
01 00 00 00 00 FF 00 00 00 00 00 FF
[01681662]mark bt disconnected
[01681662]disconnect:1740470,1681662,0,1
[01681662]bt reconn again
[01681663]send reconn...
[01681960]spi rd 0x51-dump 12 Bytes
01 02 00 00 00 FF 00 00 00 00 00 FF
[01681960]bt connected:0x03,chn:1
```

* Mac/Win switch, switch from Win to Mac and back:

```text
[01728967]now OS:MAC
[01731186]now OS:WIN
```

* FN+SPACE to toggle backlight:

```text
[01968745]RGB_TOG flag:0x5->0x0
rgb matrix set speed [EEPROM]: 0
[01971070]RGB_TOG flag:0x0->0x5
rgb matrix set speed [EEPROM]: 5
```

* FN+L_ARROW and FN+R_ARROW to speed up and slow down RGB:

```text
rgb matrix set speed [EEPROM]: 192
rgb matrix set speed [EEPROM]: 255
[02068939]led_blink_start(2068939):led 2, 12, (250,250), 3, 0
[02070458]led_set(2070458):stop led 2, 12, 0
```

* Here I unplugged to run off battery for a while, I'm trying to capture a battery reading so I need to draw it down a bit... Then I plugged it back in:

```text
[03059146]ADC:716, VBATT:4160, 4.160, 1-1
[03067146]send bt batt_level:100
```

* Unplugged and replugged more quickly:

```text
Device disconnected.
Waiting for new device:....
Listening:
[03111162]ADC:716, VBATT:4160, 4.160, 1-1
```

* Hit CAPS a couple of times:

```text
[03153071]spi rd 0x51-dump 12 Bytes
03 02 00 00 00 FF 00 00 00 00 00 FF
[03153071]bt connected:0x03,chn:1
[03154615]spi rd 0x51-dump 12 Bytes
01 02 00 00 00 FF 00 00 00 00 00 FF
[03154616]bt connected:0x03,chn:1
[03160632]spi rd 0x51-dump 12 Bytes
03 02 00 00 00 FF 00 00 00 00 00 FF
[03160632]bt connected:0x03,chn:1
[03161425]spi rd 0x51-dump 12 Bytes
01 02 00 00 00 FF 00 00 00 00 00 FF
[03161426]bt connected:0x03,chn:1
```

* Battery indicator light is red under the "M" indicator (which is blue, probably the Mode indicator), and the "A" indicator, which is blank, but goes red when I press CAPS and capslock is on
