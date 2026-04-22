/*
 * flash_operations.c
 *
 *  Created on: Apr 8, 2026
 *      Author: DELL
 */

#ifndef INC_FLASH_OPERATIONS_C_
#define INC_FLASH_OPERATIONS_C_

#include <stdint.h>

void flash_read_page(uint32_t page_start_addr, uint32_t *buffer);
void flash_erase_sector(uint32_t sector);
void flash_write_page(uint32_t page_start_addr, uint32_t *buffer);



#endif /* INC_FLASH_OPERATIONS_C_ */
