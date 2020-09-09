
#include <xc.h>

void TC2_Handler()
{
    //If TC2 OverFlow
    if(TC2_REGS->COUNT16.TC_INTFLAG & TC_INTFLAG_MC0(1))
    {
        //Clear interrupt
        TC2_REGS->COUNT16.TC_INTFLAG = TC_INTFLAG_MC0(1);
        
        TC2_REGS->COUNT16.TC_COUNT = 0;
        
        //Toggle LED
        PORT_REGS->GROUP[0].PORT_OUTTGL = PORT_PA16;
    }
}

void enable_interrupts(void)
{
    NVIC_EnableIRQ(TC2_IRQn);
}