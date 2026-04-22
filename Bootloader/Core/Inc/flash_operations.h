/*
 * flash_operations.h
 *
 *  Created on: Apr 8, 2026
 *      Author: DELL
 */

#ifndef INC_FLASH_OPERATIONS_H_
#define INC_FLASH_OPERATIONS_H_


#include <stdint.h>

uint32_t flash_read_ota_flag(void);
uint32_t flash_erase_app(void);
uint32_t flash_erase_header(void);
void flash_write_word(uint32_t addr, uint32_t data);

#endif /* INC_FLASH_OPERATIONS_H_ */
