/*
 * bl_jump.c
 *
 *  Created on: Apr 5, 2026
 *      Author: DELL
 */

#include "main.h"
#include "flash_layout.h"
#include "bl_jump.h"
#include "app_header.h"
#include "crc32.h"

extern UART_HandleTypeDef huart1;

#define APP_MAGIC 0xABCDEFAB

typedef void (*pFunction)(void);
//function which allows bootloader to jump to the application
void JumpToApplication(void)
{
    uint32_t appStack;
    uint32_t appResetHandler;
    pFunction appEntry;

    /* Read application stack pointer */ //stack pointer->located at begin of app flash
    //first 4 bytes of app flash contains initial msb value
    //msb must be set before starting executing
    appStack = *(volatile uint32_t*)APP_START_ADDR;
    //strting
    /* Read reset handler address */
    appResetHandler = *(volatile uint32_t*)(APP_START_ADDR + 4);
    //this addr acts as entry point to app
    appEntry = (pFunction)appResetHandler;

    HAL_UART_DeInit(&huart1);
    HAL_RCC_DeInit();
      HAL_DeInit();

    /* Disable interrupts */
    __disable_irq();  //disabling all active interrupts

    /* Stop SysTick */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;


   // SCB->VTOR = APP_START_ADDR;   // move this here

    /* Set main stack pointer */
        __set_MSP(appStack);

       // __enable_irq();               // keep this here

    /* Jump to application reset handler */
    appEntry();
}

int bootloader_is_app_valid(void)
{
    uint32_t HDR_ADDR = APP_HEADER_ADDR;
    const app_header_t *app_hdr = (const app_header_t *)HDR_ADDR;

    /* 1. Magic */
    if (app_hdr->magic != APP_MAGIC)
        return 1;

    /* 2. Reset handler sanity */
    uint32_t reset_handler = *(uint32_t *)(APP_START_ADDR + 4);
    if ((reset_handler & 0xFF000000) != 0x08000000)
        return 2;

    /* 3. Size sanity */
    if (app_hdr->size == 0 || app_hdr->size > APP_MAX_SIZE)
        return 3;

    /* 4. CRC check */
    uint32_t calc_crc =
        crc32((const uint8_t *)APP_START_ADDR, app_hdr->size);



    char dbg[60];
    sprintf(dbg, "calc_crc=0x%08lX expected=0x%08lX\r\n", calc_crc, app_hdr->crc);
    HAL_UART_Transmit(&huart1, (uint8_t *)dbg, strlen(dbg), 100);

    if (calc_crc != app_hdr->crc)
        return 4;

    return 0;   // VALID
}
