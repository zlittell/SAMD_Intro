// Config Source code for XC32 compiler.
// USER_WORD_0
#pragma config NVMCTRL_BOOTPROT = SIZE_0BYTES
#pragma config NVMCTRL_EEPROM_SIZE = SIZE_0BYTES
#pragma config BOD33USERLEVEL = 0x7 // Enter Hexadecimal value
#pragma config BOD33_EN = ENABLED
#pragma config BOD33_ACTION = RESET
#pragma config WDT_ENABLE = DISABLED
#pragma config WDT_ALWAYSON = DISABLED
#pragma config WDT_PER = CYC16384
#pragma config WDT_WINDOW_0 = SET

// USER_WORD_1
#pragma config WDT_WINDOW_1 = 0x5 // Enter Hexadecimal value
#pragma config WDT_EWOFFSET = CYC16384
#pragma config WDT_WEN = DISABLED
#pragma config BOD33_HYST = DISABLED
#pragma config NVMCTRL_REGION_LOCKS = 0xFFFF // Enter Hexadecimal value

#define CFG_TUSB_MCU OPT_MCU_SAMD11

#include <sam.h>
#include "main.h"
#include "system_configuration.h"
#include "tusb.h"

void main(void)
{    
    //Set up clock outputs
    //clock 0 should be 48mhz
    PORT->Group[0].DIRSET.reg = PORT_PA08;
    PORT->Group[0].PINCFG[8].reg |= PORT_PINCFG_PMUXEN;
    PORT->Group[0].PMUX[4].reg = PORT_PMUX_PMUXE_H;
    //clock 3 should be 32khz
    PORT->Group[0].DIRSET.reg = PORT_PA17;
    PORT->Group[0].PINCFG[17].reg |= PORT_PINCFG_PMUXEN;
    PORT->Group[0].PMUX[8].reg = PORT_PMUX_PMUXO_H;
    
    //GCLK->GENDIV.reg = GCLK_GENDIV_DIV(1) | GCLK_GENDIV_ID(0);
    //GCLK->GENCTRL.reg = GCLK_GENCTRL_OE | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC8M | GCLK_GENCTRL_ID(GCLK_CLKCTRL_GEN_GCLK0_Val);
    
    configureClocks();
    init_IO();
    init_TC2();
    
    enable_interrupts();
    
    tusb_init();
    
    PORT->Group[0].DIRSET.reg = PORT_PA16;
    PORT->Group[0].OUTCLR.reg = PORT_PA16;
    
    while(1)
    {       
        __NOP();
        
        tud_task();
    }
}