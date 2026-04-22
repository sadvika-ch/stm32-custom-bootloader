/*
 * flash_layout.h
 *
 *  Created on: Apr 5, 2026
 *      Author: DELL
 */

#ifndef INC_FLASH_LAYOUT_H_
#define INC_FLASH_LAYOUT_H_

#define BL_START_ADDR      0x08000000  // Sector 0,1 → Bootloader  (32KB)
#define APP_HEADER_ADDR    0x08008000  // Sector 2   → OTA Flag + Header (16KB)
#define APP_HEADER_SECTOR  FLASH_SECTOR_2
#define APP_START_ADDR     0x0800C000  // Sector 3+  → Application

#define APP_MAX_SIZE       (47*1024)

#endif /* INC_FLASH_LAYOUT_H_ */
