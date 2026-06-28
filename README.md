# STM32 TFT Display Project

This project drives an ILI9341-based TFT LCD from an STM32F446RE Nucleo board using STM32 HAL and SPI1. It includes a small display layer for drawing menus, text, icons, full-screen color images, and bring-up tests for validating the LCD wiring.

## Hardware Used

- MCU board: STM32F446RE Nucleo
- Display: ILI9341 TFT LCD, 240 x 320 pixels
- Interface: 4-wire SPI-style display control
- Toolchain: STM32CubeIDE
- HAL family: STM32F4xx HAL

## Project Structure

```text
Core/
  Inc/
    main.h                  GPIO labels and project-wide declarations
  Src/
    main.c                  Application entry point and bring-up tests
    stm32f4xx_hal_msp.c     SPI1 and USART2 pin alternate-function setup

LCD/
  Display/
    display.c               Higher-level display screens and image drawing
    display.h               Display function declarations
  ILI9341/
    ILI9341_Driver.c        Low-level ILI9341 driver
    ILI9341_Driver.h        Low-level drawing API
  Icons/
    *_icon*.h               RGB565 icon arrays
  Images/
    bmw.h                   Portrait BMW image data
    bmw_320x240.h           Landscape BMW image data
    spiderman.h             Spider-Man image data
    captain.h, ironman.h    Other image assets
```

## Board Connections

The TFT is connected to the STM32F446RE Nucleo through SPI1 plus three control pins. The most important connections are `SCK`, `MOSI`, `CS`, `DC`, `RESET`, `3.3V`, and `GND`.

### Power

| TFT Pin | Connect To | Notes |
|---|---|---|
| `VCC` | Nucleo `3V3` | Use 3.3 V logic power for the display module. |
| `GND` | Nucleo `GND` | The TFT and STM32 must share common ground. |
| `LED`, `BL`, or `BLK` | `3V3` or module backlight supply | Depends on your TFT module. Some modules have onboard current limiting; others need a current-limited backlight supply. |

### SPI Data Lines

| TFT Pin | STM32 Pin | Nucleo Header | Function |
|---|---:|---|---|
| `SCK`, `CLK`, or `SCL` | `PA5` | `D13` | SPI1 clock |
| `MOSI`, `SDA`, `SDI`, or `DIN` | `PA7` | `D11` | SPI data from STM32 to TFT |
| `MISO`, `SDO`, or `DO` | `PA6` | `D12` | Optional readback line; many ILI9341 display-only examples do not use it |

### TFT Control Lines

| TFT Pin | STM32 Pin | Firmware Name | Purpose |
|---|---:|---|---|
| `CS`, `LCD_CS`, or `T_CS` | `PA4` | `CS_Pin` | Selects the TFT before commands/data are sent |
| `DC`, `RS`, or `A0` | `PC4` | `DC_Pin` | Chooses command mode or pixel/data mode |
| `RESET`, `RST`, or `RES` | `PC5` | `RESET_Pin` | Resets the ILI9341 controller |

### Quick Wiring Checklist

```text
TFT VCC    -> Nucleo 3V3
TFT GND    -> Nucleo GND
TFT SCK    -> PA5  / D13
TFT MOSI   -> PA7  / D11
TFT MISO   -> PA6  / D12  optional
TFT CS     -> PA4
TFT DC     -> PC4
TFT RESET  -> PC5
TFT LED/BL -> 3V3 or suitable backlight supply
```

Important notes:

- All grounds must be common.
- The ILI9341 logic pins should be driven at 3.3 V.
- `PA6 / SPI1_MISO` is configured by CubeMX, but the current drawing code only writes to the display.
- `PC4` and `PC5` are listed by STM32 port/pin name because they may be easiest to access from the Nucleo Morpho headers.

## Project Photo

![TFT display output](docs/images/tft_output.jpg)

## Pin Configuration in Firmware

The pins are named in `Core/Inc/main.h`:

```c
#define CS_Pin GPIO_PIN_4
#define CS_GPIO_Port GPIOA

#define DC_Pin GPIO_PIN_4
#define DC_GPIO_Port GPIOC

#define RESET_Pin GPIO_PIN_5
#define RESET_GPIO_Port GPIOC
```

SPI1 alternate-function pins are configured in `Core/Src/stm32f4xx_hal_msp.c`:

```c
PA5 -> SPI1_SCK
PA6 -> SPI1_MISO
PA7 -> SPI1_MOSI
```

SPI1 is initialized as:

- Master mode
- 8-bit data
- CPOL low
- CPHA first edge
- Software NSS
- Prescaler 16
- MSB first

## Display Driver Overview

The low-level ILI9341 driver lives in `LCD/ILI9341/ILI9341_Driver.c`.

Useful functions include:

```c
ILI9341_Init();
ILI9341_Set_Rotation(rotation);
ILI9341_Fill_Screen(color);
ILI9341_Draw_Pixel(x, y, color);
ILI9341_Draw_String(x, y, color, background, text, size);
ILI9341_Draw_Empty_Rectangle(color, x1, y1, x2, y2);
ILI9341_Draw_Circle(x0, y0, radius, color, fill);
```

The higher-level display screens live in `LCD/Display/display.c`:

```c
Display_Menu();
Display_Text();
Display_Picture();
Display_Color_Picture();
Display_BMW_Picture();
Display_Back_Icon(x, y);
```

## Display Rotation

The ILI9341 driver supports four rotations:

| Rotation | Screen Size Used by Driver | Typical Orientation |
|---:|---|---|
| `0` | 240 x 320 | Portrait |
| `1` | 320 x 240 | Landscape |
| `2` | 240 x 320 | Portrait flipped |
| `3` | 320 x 240 | Landscape flipped |

For the landscape BMW image, `Display_BMW_Picture()` uses rotation `3` and draws a 320 x 240 image:

```c
ILI9341_Set_Rotation(3);
```

The `bmw_320x240.h` image stores each RGB565 pixel as two array entries, so `Display_BMW_Picture()` combines the high and low bytes before drawing:

```c
uint32_t index = 2 * ((320 * y) + x);
uint16_t pixels = (bmw_landscape[index] << 8) | bmw_landscape[index + 1];
ILI9341_Draw_Pixel(x, y, pixels);
```

## Bring-Up Test Modes

`Core/Src/main.c` contains a compile-time switch:

```c
#define DISPLAY_BRINGUP_TEST 2
```

Available modes:

| Value | Mode |
|---:|---|
| `0` | Normal LCD demo menu |
| `1` | Visible heartbeat/reset test |
| `2` | Minimal LCD test area for display experiments |
| `3` | Bit-banged SPI LCD color cycle |
| `4` | Hardware SPI LCD color cycle |

Mode `2` is currently used for experimenting with the display functions. Example calls you can enable there:

```c
Display_BMW_Picture();
Display_Text();
Display_Menu();
Display_Back_Icon(0, 200);
```

## Image and Icon Data

The project stores images and icons as C arrays:

- Full-screen images are in `LCD/Images/`
- UI icons are in `LCD/Icons/`
- Icons are included through `icons_included.h`

Avoid including icon headers such as `back_icon.h` directly in multiple `.c` files because those headers define the actual arrays. Including the same definition in more than one compilation unit causes linker errors such as:

```text
multiple definition of `back_icon_40x40'
```

Instead, keep icon drawing inside `display.c` or expose wrapper functions such as:

```c
Display_Back_Icon(0, 200);
```

## Building

Open the project in STM32CubeIDE and build the `Debug` configuration.

Typical build command generated by CubeIDE:

```text
make -j8 all
```

The output firmware is generated under:

```text
Debug/tft_display.elf
```

## Flashing and Running

1. Connect the STM32F446RE Nucleo board to your PC over USB.
2. Connect the TFT display using the wiring table above.
3. Open the project in STM32CubeIDE.
4. Select the `Debug` configuration.
5. Build the project.
6. Flash or debug the firmware from STM32CubeIDE.

## Troubleshooting

If the display stays white or black:

- Check 3.3 V and GND first.
- Confirm `CS`, `DC`, and `RESET` are connected to PA4, PC4, and PC5.
- Confirm SCK and MOSI are connected to PA5 and PA7.
- Try `DISPLAY_BRINGUP_TEST == 4` for a simple hardware-SPI color-cycle test.
- Try `DISPLAY_BRINGUP_TEST == 1` to verify reset pin activity.

If the image is shifted, clipped, or rotated:

- Check the selected `ILI9341_Set_Rotation()` value.
- Make sure the image dimensions match the rotation:
  - 240 x 320 for portrait
  - 320 x 240 for landscape

If the linker reports multiple definitions for an icon or image:

- Do not include that asset header in multiple `.c` files.
- Prefer drawing assets from `display.c`, or convert asset headers to `extern` declarations plus one `.c` definition file.

## Current Status

The project can initialize the ILI9341 display, draw primitive shapes and text, render menu icons, and draw full-screen RGB565 images in portrait or landscape orientation.
