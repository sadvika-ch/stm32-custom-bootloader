/*
 * app_ota.c
 *
 *  Created on: Apr 8, 2026
 *      Author: DELL
 */

#include "flash_layout.h"
#include "main.h"
#include "app_ota.h"
#include "flash_operations.h"
extern UART_HandleTypeDef huart1;

#define OTA_FLAG_START    1

uint32_t flash_buffer[5];

void enable_ota_request(void)
{
    HAL_StatusTypeDef status;

    status = HAL_FLASH_Unlock();
    if(status != HAL_OK){
        HAL_UART_Transmit(&huart1, (uint8_t *)"Unlock FAIL\r\n", 13, 100);
        return;
    }

    flash_read_page(APP_HEADER_ADDR, flash_buffer);
    flash_buffer[0] = OTA_FLAG_START;

    flash_erase_sector(APP_HEADER_SECTOR);

//    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, APP_HEADER_ADDR, flash_buffer[0]);
//    if(status != HAL_OK){
//        HAL_UART_Transmit(&huart1, (uint8_t *)"Write FAIL\r\n", 12, 100);
//        HAL_FLASH_Lock();
//        return;
//    }

    /* Write updated header back */
      flash_write_page(APP_HEADER_ADDR, flash_buffer);


    HAL_UART_Transmit(&huart1, (uint8_t *)"OTA flag set!\r\n", 15, 100);

    HAL_FLASH_Lock();
    NVIC_SystemReset();
}
//
//void enable_ota_request(void)
//{
//    HAL_FLASH_Unlock();
//
//    /* Read existing header */
//    flash_read_page(APP_HEADER_ADDR, flash_buffer);
//
//    /* Set OTA flag */
//    flash_buffer[0] = OTA_FLAG_START;
//
//    /* Erase header sector (Sector 2 for your board) */
//    flash_erase_sector(APP_HEADER_SECTOR);
//
//    /* Write updated header back */
//    flash_write_page(APP_HEADER_ADDR, flash_buffer);
//
//    HAL_FLASH_Lock();
//
//    /* Restart system so bootloader can read flag */
//    NVIC_SystemReset();
//}


