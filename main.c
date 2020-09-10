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


#include <xc.h>
//#include <SAMD11D14AM.h>
#include "main.h"

void configureClocks(void)
{
    /* 
     * Configure 8MHz Clock
     */
    SYSCTRL_REGS->SYSCTRL_OSC8M |=
            (SYSCTRL_OSC8M_PRESC(3) | SYSCTRL_OSC8M_ONDEMAND(1) | SYSCTRL_OSC8M_RUNSTDBY(0) | 
            SYSCTRL_OSC8M_ENABLE(1));
    while(!(SYSCTRL_REGS->SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_OSC8MRDY(1))); //Wait for steady
    
    GCLK_REGS->GCLK_GENDIV = (GCLK_GENDIV_DIV(0) | GCLK_GENDIV_ID(1));
    GCLK_REGS->GCLK_GENCTRL = (GCLK_GENCTRL_GENEN(1) | GCLK_GENCTRL_SRC_OSC8M | GCLK_GENCTRL_ID(1));
    
    /* 
     * Configure DFLL 48MHz
     */
    SYSCTRL_REGS->SYSCTRL_DFLLCTRL = 
            (SYSCTRL_DFLLCTRL_WAITLOCK(0) | SYSCTRL_DFLLCTRL_BPLCKC(0) | 
            SYSCTRL_DFLLCTRL_QLDIS(0) | SYSCTRL_DFLLCTRL_CCDIS(0) | 
            SYSCTRL_DFLLCTRL_ONDEMAND(0) | SYSCTRL_DFLLCTRL_USBCRM(1) | 
            SYSCTRL_DFLLCTRL_LLAW(0) | SYSCTRL_DFLLCTRL_STABLE(0) | 
            SYSCTRL_DFLLCTRL_MODE(0) | SYSCTRL_DFLLCTRL_ENABLE(1));
    
    GCLK_REGS->GCLK_GENCTRL = (GCLK_GENCTRL_GENEN(1) | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(2));
    
    /*
     * Set Clock Gen 0 to be 48MHz
     * Clock Gen 0 Clocks the CPU
     */
    GCLK_REGS->GCLK_GENDIV = (GCLK_GENDIV_DIV(0) | GCLK_GENDIV_ID(0));
    GCLK_REGS->GCLK_GENCTRL = (GCLK_GENCTRL_GENEN(1) | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(0));
    
    /*
     * Configure 32.768KHZ internal oscillator on GCLK Channel 3 
     */
    SYSCTRL_REGS->SYSCTRL_OSC32K |= (SYSCTRL_OSC32K_ENABLE(1) | SYSCTRL_OSC32K_EN32K(1)) ;
    GCLK_REGS->GCLK_GENDIV = (GCLK_GENDIV_DIV(0) | GCLK_GENDIV_ID(3));
    GCLK_REGS->GCLK_GENCTRL = (GCLK_GENCTRL_GENEN(1) | GCLK_GENCTRL_SRC_OSC32K | GCLK_GENCTRL_ID(3));
}

void init_TC2(void)
{    
    //Enable TC2 Bus Clock
    PM_REGS->PM_APBCMASK |= PM_APBCMASK_TC2(1);
    
    // Set up Clock Gen
    GCLK_REGS->GCLK_CLKCTRL = 
            (GCLK_CLKCTRL_WRTLOCK(0) | GCLK_CLKCTRL_CLKEN(1) | 
            GCLK_CLKCTRL_GEN_GCLK3 | GCLK_CLKCTRL_ID_TC1_TC2);
        
    TC2_REGS->COUNT16.TC_INTENSET = (TC_INTENSET_ERR(1) | TC_INTENSET_MC0(1));

    //This should be ~10mS interrupts
    TC2_REGS->COUNT16.TC_CC[0] = 328;
    
    TC2_REGS->COUNT16.TC_CTRLA = 
            (TC_CTRLA_PRESCSYNC_PRESC | TC_CTRLA_RUNSTDBY(0) | 
            TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_WAVEGEN_NFRQ | 
            TC_CTRLA_MODE_COUNT16 | TC_CTRLA_ENABLE(1) | 
            TC_CTRLA_SWRST(0));
}

void main(void)
{    
    configureClocks();
    init_TC2();
    enable_interrupts();
    
    PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA16;
    PORT_REGS->GROUP[0].PORT_OUTCLR = PORT_PA16;
    
    while(1)
    {        
        Nop();
    }
}