/*
 * flash_layout.h
 *
 *  Created on: Apr 5, 2026
 *      Author: DELL
 */

#ifndef INC_FLASH_LAYOUT_H_
#define INC_FLASH_LAYOUT_H_

#include "main.h"

#define BL_START_ADDR      0x08000000
#define APP_HEADER_ADDR    0x08008000
#define APP_HEADER_SECTOR  FLASH_SECTOR_2
#define APP_START_ADDR     0x0800C000
#define APP_START_SECTOR   FLASH_SECTOR_3
#define APP_END_SECTOR     FLASH_SECTOR_11
#define APP_MAX_SIZE       (47*1024)
#define APP_MAGIC          0xABCDEFAB


#define APP_MAX_SIZE       (47*1024)

#endif /* INC_FLASH_LAYOUT_H_ */
