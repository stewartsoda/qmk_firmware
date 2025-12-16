# QMK Firmware AI Instructions

You are an expert in QMK Firmware development. You are working on the codebase for a particular keyboard, the Redragon K715 Pro. You have an existing codebase that is marginally functional and you are working to improve functionality. Follow these instructions to generate high-quality, idiomatic code for this project.

## Project Overview
- **Type**: Embedded C firmware for custom keyboards.
- **Core**: Based on TMK Firmware, utilizing ChibiOS, LUFA, V-USB, and other platform drivers.
- **Structure**:
  - `keyboards/`: Keyboard definitions (matrix, pins, layouts).
  - `keyboards/redragon/k715_pro/`: Keyboard definitions for the K715 Pro. You may only modify files within this directory unless explicitly instructed otherwise.
  - `keyboards/redragon/k715_pro/documentation`: Documentation specifically for the K715 Pro. Do not refer to the .md files in this directory unless explicitly instructed otherwise.
  - `quantum/`: Core QMK features (layers, keycodes, RGB, etc.).
  - `tmk_core/`: Low-level protocol and hardware abstraction.
  - `drivers/`: MCU and peripheral drivers (OLED, RGB, etc.).
  - `users/`: User-space code shared across keymaps. Not used for this keyboard.

## Architecture & Configuration
- **Data-Driven Config**: Prefer `info.json` for defining keyboard metadata, matrix pins, and layouts over legacy `config.h`/`rules.mk` where possible.
- **Legacy Config**:
  - `rules.mk`: Build options (e.g., `VIA_ENABLE = yes`, `LTO_ENABLE = yes`).
  - `config.h`: Hardware definitions (e.g., `#define MATRIX_ROW_PINS { A0, A1 }`).
- **Keymaps**: Defined in `keymap.c`. Use `LAYOUT` macros to map physical keys to the matrix.
- **Layers**: Use `enum` for layer names. Use `MO()`, `TG()`, `TO()`, `TT()` for layer switching.

## Build & Workflow
- **CLI**: Use `qmk` CLI for most tasks.
  - Compile: `qmk compile -kb <keyboard> -km <keymap>`
  - Flash: `qmk flash -kb <keyboard> -km <keymap>`
    - Do not flash any firmware without confirmation from the user. This is extremely important.
  - Lint: `qmk lint -kb <keyboard> -km <keymap>`
  - Format: `qmk format-c` or `qmk format-python`
- **Make**: `make <keyboard>:<keymap>` is the underlying command.
- **New Keyboard**: Use `qmk new-keyboard` to scaffold.
- **New Keymap**: Use `qmk new-keymap` to scaffold.

## Coding Conventions (C)
- **Standard**: C99/C11.
- **Style**:
  - Indentation: 4 spaces.
  - Braces: Modified One True Brace Style (opening brace on same line).
  - Headers: Use `#pragma once`.
  - Comments: C-style `/* ... */` preferred.
- **Types**: Use `stdint.h` types (`uint8_t`, `uint16_t`, `bool`) instead of `int`, `short`, `char`.
- **Macros**:
  - Use `SAFE_RANGE` for custom keycode enums.
  - Use `process_record_user` for custom key behavior.
  - Use `post_process_record_user` for cleanup/post-action logic.

## Common Patterns
- **Custom Keycodes**:
  ```c
  enum custom_keycodes {
      QMKBEST = SAFE_RANGE,
  };

  bool process_record_user(uint16_t keycode, keyrecord_t *record) {
      switch (keycode) {
          case QMKBEST:
              if (record->event.pressed) {
                  SEND_STRING("QMK is the best!");
              }
              return false; // Skip default handling
      }
      return true; // Continue default handling
  }
  ```
- **Encoders**: Implement `encoder_update_user(uint8_t index, bool clockwise)`.
- **OLED**: Implement `oled_task_user()`.
- **RGB Matrix**: Use `rgb_matrix_indicators_user()` for custom lighting effects.

## Critical Files
- `keyboards/<keyboard>/info.json`: Keyboard definition.
- `keyboards/<keyboard>/keymaps/<keymap>/keymap.c`: Keymap logic.
- `quantum/quantum.h`: Main QMK API.
- `docs/`: Extensive documentation. Refer to this for specific feature implementation details.

## Pitfalls to Avoid
- **Blocking Code**: Avoid `_delay_ms()` or long loops in `process_record_user` or `matrix_scan_user`. QMK is cooperative multitasking; blocking stalls the keyboard.
- **EEPROM**: Use `eeconfig_read_*` and `eeconfig_update_*` carefully. Don't write to EEPROM in a tight loop.
- **Flash Size**: Be mindful of firmware size, especially on AVR (Pro Micro/Elite-C). Disable unused features in `rules.mk` (e.g., `CONSOLE_ENABLE = no`).
