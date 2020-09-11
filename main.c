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


//#include <xc.h>
#include <sam.h>
#include <xc.h>
#include "main.h"

void configureClocks(void)
{
    /* 
     * Configure 8MHz Clock
     */
    
    SYSCTRL->OSC8M.reg |= 
            (SYSCTRL_OSC8M_PRESC(3) | SYSCTRL_OSC8M_ONDEMAND |
            SYSCTRL_OSC8M_ENABLE);
    while(!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_OSC8MRDY)); //Wait for steady
    
    GCLK->GENDIV.reg = (GCLK_GENDIV_DIV(0) | GCLK_GENDIV_ID(1));
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC8M | GCLK_GENCTRL_ID(1));
    
    /* 
     * Configure DFLL 48MHz
     */
    SYSCTRL->DFLLCTRL.reg = 
            (SYSCTRL_DFLLCTRL_USBCRM |  SYSCTRL_DFLLCTRL_ENABLE);
    
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(2));
    
    /*
     * Set Clock Gen 0 to be 48MHz
     * Clock Gen 0 Clocks the CPU
     */
    GCLK->GENDIV.reg = (GCLK_GENDIV_DIV(0) | GCLK_GENDIV_ID(0));
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(0));
    
    /*
     * Configure 32.768KHZ internal oscillator on GCLK Channel 3 
     */
    SYSCTRL->OSC32K.reg |= (SYSCTRL_OSC32K_ENABLE | SYSCTRL_OSC32K_EN32K);
    GCLK->GENDIV.reg = (GCLK_GENDIV_DIV(0) | GCLK_GENDIV_ID(3));
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC32K | GCLK_GENCTRL_ID(3));
}

void init_TC2(void)
{    
    //Enable TC2 Bus Clock
    PM->APBCMASK.reg |= PM_APBCMASK_TC2;
    
    // Set up Clock Gen
    GCLK->CLKCTRL.reg = 
            (GCLK_CLKCTRL_CLKEN | 
            GCLK_CLKCTRL_GEN_GCLK3 | GCLK_CLKCTRL_ID_TC1_TC2);
        
    TC2->COUNT16.INTENSET.reg = (TC_INTENSET_ERR | TC_INTENSET_MC0);

    //This should be ~10mS interrupts
    TC2->COUNT16.CC[0].reg = 328;
    
    TC2->COUNT16.CTRLA.reg = 
            (TC_CTRLA_PRESCSYNC_PRESC | 
            TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_WAVEGEN_NFRQ | 
            TC_CTRLA_MODE_COUNT16 | TC_CTRLA_ENABLE);
}

void main(void)
{    
    configureClocks();
    init_TC2();
    enable_interrupts();
    
    PORT->Group[0].DIRSET.reg = PORT_PA16;
    PORT->Group[0].OUTCLR.reg = PORT_PA16;
    
    while(1)
    {        
        _nop();
    }
}