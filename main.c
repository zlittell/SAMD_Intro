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
#include <SAMD11D14AM.h>

void configureClocks(void)
{
    /* Configure 8MHz Clock
     * Keep all flash read values for calibration, etc
     * Set Prescaler to 3 (description 8)
     * Set OnDemand to 1 (peripheral access during sleep turns on clock)
     * Set RunStandby to 0 (clock doesn't run in sleep)
     * Enable Clock
     */
    SYSCTRL_REGS->SYSCTRL_OSC8M = 
            ((SYSCTRL_REGS->SYSCTRL_OSC8M & (SYSCTRL_OSC8M_FRANGE_Msk | SYSCTRL_OSC8M_CALIB_Msk)) |
            SYSCTRL_OSC8M_PRESC(3) | SYSCTRL_OSC8M_ONDEMAND(1) | SYSCTRL_OSC8M_RUNSTDBY(0) | 
            SYSCTRL_OSC8M_ENABLE(1));
    while(!(SYSCTRL_REGS->SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_OSC8MRDY(1))); //Wait for steady
    
    /* Configure DFLL 48MHz
     * Wait until DFLL is locked to output clock
     * Coarse lock is disabled
     * Quick Lock is Enabled
     * Chill Cycle is enabled
     * Only run clock when requested by peripheral
     * Enable USB Clock Recovery Mode
     * Locks will not be lost after waking up from sleep modes.
     * Fine calibration tracks changes in output frequency
     * DFLL Operates in open-loop mode
     * DFLL Is Enabled
     */
    SYSCTRL_REGS->SYSCTRL_DFLLCTRL = 
            (SYSCTRL_DFLLCTRL_WAITLOCK(0) | SYSCTRL_DFLLCTRL_BPLCKC(0) | 
            SYSCTRL_DFLLCTRL_QLDIS(0) | SYSCTRL_DFLLCTRL_CCDIS(0) | 
            SYSCTRL_DFLLCTRL_ONDEMAND(1) | SYSCTRL_DFLLCTRL_USBCRM(1) | 
            SYSCTRL_DFLLCTRL_LLAW(0) | SYSCTRL_DFLLCTRL_STABLE(0) | 
            SYSCTRL_DFLLCTRL_MODE(0) | SYSCTRL_DFLLCTRL_ENABLE(1));
}

void main(void)
{    
    configureClocks();
    
    PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA16;
    PORT_REGS->GROUP[0].PORT_OUTCLR = PORT_PA16;
    
    while(1)
    {
        for(uint8_t i = 0; i<255; i++)
        {
            Nop();
        }
                
        while(1)
        {
            PORT_REGS->GROUP[0].PORT_OUTTGL = PORT_PA16;
            for(uint32_t i = 0; i<2000; i++)
            {
                Nop();
            }
        }
        
        Nop();
    }
}