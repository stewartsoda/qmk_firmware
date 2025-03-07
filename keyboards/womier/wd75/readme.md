# WD75
[![WD75](https://s21.ax1x.com/2025/03/07/pEtZIoD.png)](https://imgse.com/i/pEtZIoD)
* Keyboard Maintainer: [jonylee@hfd](https://github.com/jonylee1986)
* Hardware Supported: WOMIER WD75
* Hardware Availability: [WOMIER WD75](https://womierkeyboard.com/)

Make example for this keyboard (after setting up your build environment):

    make womier/wd75:default

Flashing example for this keyboard:

    make womier/wd75:default:flash
    
See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 2 ways:

* **Bootmagic reset**: Hold down the Hold down the top left key (commonly programmed as *Esc*) and plug in the keyboard
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
