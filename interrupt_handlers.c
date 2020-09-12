
#include <xc.h>

void TC2_Handler()
{
    //If TC2 OverFlow
    if(TC2->COUNT16.INTFLAG.reg & TC_INTFLAG_MC0)
    {
        //Clear interrupt
        TC2->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0;
        
        TC2->COUNT16.COUNT.reg = 0;
        
        //Toggle LED
        PORT->Group[0].OUTTGL.reg = PORT_PA16;
    }
}

void enable_interrupts(void)
{
    NVIC_EnableIRQ(TC2_IRQn);
    NVIC_EnableIRQ(USB_IRQn);
}