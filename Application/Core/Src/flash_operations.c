/*
 * flash_operations.c
 *
 *  Created on: Apr 8, 2026
 *      Author: DELL
 */

#include "main.h"
#include "flash_operations.h"

void flash_read_page(uint32_t page_start_addr, uint32_t *buffer)
{
    uint32_t *flash_ptr = (uint32_t *)page_start_addr;

    for (uint32_t i = 0; i < 5; i++)
    {
        buffer[i] = flash_ptr[i];
    }
}

/* For STM32F429 -> sector erase */
void flash_erase_sector(uint32_t sector)
{
    FLASH_EraseInitTypeDef erase;
    uint32_t error = 0;

    erase.TypeErase    = FLASH_TYPEERASE_SECTORS;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    erase.Sector       = sector;
    erase.NbSectors    = 1;

    HAL_FLASHEx_Erase(&erase, &error);
}

void flash_write_page(uint32_t page_start_addr, uint32_t *buffer)
{
    uint32_t addr = page_start_addr;

    for (uint32_t i = 0; i < 5; i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, buffer[i]);
        addr += 4;
    }
}


