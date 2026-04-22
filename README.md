# stm32-custom-bootloader

A custom bootloader for the STM32F429ZIT6, implementing flash validation, OTA firmware update from an embedded image array, and a persistent OTA flag mechanism.

---

## Overview

The bootloader runs first on every reset and:

1. Checks if **PA0 (user button)** is held HIGH → forces OTA immediately
2. Reads the **OTA flag** from flash → performs OTA if set
3. Validates the application (Magic Number → Reset Handler → Size → CRC32)
4. Jumps to the application if valid

---

## Hardware

| Component | Details |
|---|---|
| MCU | STM32F429ZIT6 (Nucleo-144) |
| Programmer | ST-Link (onboard) |
| UART | USART1 — 115200 baud, 8N1 |
| LEDs | PG13 (OTA fail), PG14 (validation fail) |
| OTA Force Button | PA0 — GPIO input, pull-down, active HIGH |

| Signal | Pin |
|---|---|
| UART TX | PA9 |
| UART RX | PA10 |
| OTA Force | PA0 |
| OTA Fail LED | PG13 — blinks every 250ms |
| Validation Fail LED | PG14 — blinks every 1000ms |

---

## Flash Memory Layout

```
┌─────────────────────────────────────────┐  0x08000000  (Sector 0)
│           BOOTLOADER (32 KB)            │
│  Sector 0 + Sector 1                   │
├─────────────────────────────────────────┤  0x08008000  (Sector 2)
│           APP HEADER (16 KB)            │
│  Magic · Size · CRC32                   │
├─────────────────────────────────────────┤  0x0800C000  (Sector 3)
│        APPLICATION (Sectors 3–11)       │
│  Max size: depends on sectors 3–11 ~1 MB│
│  Relocated vector table (VTOR)          │
└─────────────────────────────────────────┘
```

Defined in `flash_layout.h` (shared between bootloader and application):

```c
#define BL_START_ADDR       0x08000000
#define APP_HEADER_ADDR     0x08008000
#define APP_HEADER_SECTOR   FLASH_SECTOR_2
#define APP_START_ADDR      0x0800C000
#define APP_START_SECTOR    FLASH_SECTOR_3
#define APP_END_SECTOR      FLASH_SECTOR_11
#define APP_MAX_SIZE   (1024 * 1024)   // ~1MB safe limit
#define APP_MAGIC           0xABCDEFAB
```

---

## Bootloader Flow

```
Power On / Reset
      │
      ▼
HAL_Init → GPIO Init → USART1 Init
      │
      ▼
Print "Inside Bootloader!!"
      │
      ▼
PA0 == HIGH? (debounced)
   YES │                          NO
      ▼                            │
Print "Button Pressed - Forcing OTA!"
bl_ota_run()                       │
   PASS │   FAIL                   ▼
      │      │            flash_read_ota_flag()
      │   Print           Print "OTA Flag value: X"
      │   "OTA Failed!"           │
      │   Blink PG13     check_ota_request() == 0?
      │   @ 250ms           YES │           NO
      ▼                         ▼             ▼
Print "OTA Done! Rebooting..."  Print         Print
NVIC_SystemReset()          "OTA requested!"  "No OTA, jumping"
                                 │
                            bl_ota_run()
                        Print "OTA Flashed Successfully,
                               Jumping to app..."
                            NVIC_SystemReset()
      │
      ▼ (re-enters bootloader after reset)
bootloader_is_app_valid()
   FAIL │                        PASS
       ▼                           │
Print "Failed to Jump!!"           ▼
+ error code message        JumpToApplication()
+ header dump (CRC only)
Blink PG14 @ 1s, halt
```

---

## Validation Error Codes

| Code | Message | Meaning |
|---|---|---|
| 1 | `MAGIC ERROR!!` | `header->magic != APP_MAGIC` |
| 2 | `RESET ERROR!!` | Reset handler address out of valid range |
| 3 | `SIZE ERROR!!` | Size is zero or exceeds `APP_MAX_SIZE` |
| 4 | `CRC ERROR!!` | CRC32 mismatch — prints raw header dump |

On CRC failure, the bootloader also prints:
```
magic=0xABCDEFAB size=XXXXX crc=0xXXXXXXXX
```

---

## UART Output Examples

**Normal boot (valid app, no OTA flag):**
```
Inside Bootloader!!
OTA Flag value: 0
No OTA, jumping
```

**OTA flag triggered (first boot):**
```
Inside Bootloader!!
OTA Flag value: 3722304989
OTA requested!
Performing OTA...
OTA Flashed Successfully, Jumping to app...
```
*(After NVIC_SystemReset — second boot:)*
```
Inside Bootloader!!
OTA Flag value: 0
No OTA, jumping
```

**Button-forced OTA:**
```
Inside Bootloader!!
Button Pressed - Forcing OTA!
OTA Done! Rebooting...
```

**Validation failure — CRC mismatch:**
```
Inside Bootloader!!
OTA Flag value: 0
No OTA, jumping
Failed to Jump!!
CRC ERROR!!
magic=0xABCDEFAB size=12288 crc=0xABCD1234
```

---

## App Header

Defined in `app_header.h` (shared between both projects):

```c
typedef struct {
    uint32_t magic;
    uint32_t size;
    uint32_t crc;
} app_header_t;
```

The application places this struct at `APP_HEADER_ADDR` via a custom `.header` linker section.

---

## OTA Stream

```c
typedef struct {
    int  (*read)(uint8_t *buf, uint32_t len);
    void (*set_total_size)(uint32_t size);
} ota_stream_t;
```

`ota_read_mem()` and `ota_mem_set_total_size()` in `main.c` implement the stream from the embedded `ota_image_bin[]` array. The source can be swapped to UART, SD card, etc. by providing different function pointers.

---


---

## How to Build & Flash

### Step 1: Build Both Projects

In STM32CubeIDE, enable binary output for each project:
> Project → Properties → C/C++ Build → Settings → Tool Settings → Post-build outputs → ✅ Convert to binary

Build both in  **Debug** mode.

### Step 2: Flash Bootloader,Application

Open STM32CubeProgrammer, connect ST-Link:
```
Address : 0x08000000
File    : Bootloader/Debug/Bootloader.bin
```
```
Address : 0x0800C000
File    : Application/Debug/Application.bin
```

### Step 3: Trigger OTA
  → Force OTA manually by holding PA0 HIGH while pressing reset
- **Button**: Hold PA0 HIGH while pressing reset
- **Flag**: From running application:
  ```c
  flash_write_ota_flag(OTA_FLAG_VALUE);
  NVIC_SystemReset();
  ```

---

