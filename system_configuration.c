#include <sam.h>

void configureClocks(void)
{
    NVMCTRL->CTRLB.reg |=  NVMCTRL_CTRLB_RWS(0);
    
    //_pm_init
    PM->CPUSEL.reg = PM_CPUSEL_CPUDIV_DIV1;
    PM->APBASEL.reg = PM_APBASEL_APBADIV_DIV1;
    PM->APBBSEL.reg = PM_APBBSEL_APBBDIV_DIV1;
    PM->APBCSEL.reg = PM_APBCSEL_APBCDIV_DIV1;
    
    //_sysctrl_init_sources
    SYSCTRL->OSC8M.reg = ((SYSCTRL->OSC8M.reg & SYSCTRL_OSC8M_FRANGE_Msk) | 
            (SYSCTRL->OSC8M.reg & SYSCTRL_OSC8M_CALIB_Msk) | 
            SYSCTRL_OSC8M_PRESC_0 | SYSCTRL_OSC8M_ENABLE);
    SYSCTRL->OSC32K.reg |= SYSCTRL_OSC32K_ENABLE;
    //SYSCTRL->OSCULP32K.reg |= ((SYSCTRL->OSCULP32K.reg & SYSCTRL_OSCULP32K_CALIB_Msk) Not sure why it does this?
    while((SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_OSC8MRDY)==0);
    SYSCTRL->OSC8M.reg |= SYSCTRL_OSC8M_ONDEMAND;
    
    //_sysctrl init referenced
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
    while((SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0);
    SYSCTRL->DFLLMUL.reg = (SYSCTRL_DFLLMUL_CSTEP(1) | SYSCTRL_DFLLMUL_FSTEP(1) | SYSCTRL_DFLLMUL_MUL(48000));
    uint32_t CoarseCALRead =  ((FUSES_DFLL48M_COARSE_CAL_Msk & (*((uint32_t *)FUSES_DFLL48M_COARSE_CAL_ADDR))) >> FUSES_DFLL48M_COARSE_CAL_Pos);
    SYSCTRL->DFLLVAL.reg =  SYSCTRL_DFLLVAL_COARSE(((CoarseCALRead) == 0x3F) ? 0x1F : (CoarseCALRead)) | SYSCTRL_DFLLVAL_FINE(512);
    SYSCTRL->DFLLCTRL.reg = (SYSCTRL_DFLLCTRL_CCDIS | SYSCTRL_DFLLCTRL_USBCRM | SYSCTRL_DFLLCTRL_MODE | SYSCTRL_DFLLCTRL_ENABLE);
    while((SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0);
    while(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
    SYSCTRL->OSC32K.reg &= (~SYSCTRL_OSC32K_ENABLE);
    
    //_gclk_init_generators last init
    GCLK->GENDIV.reg = (GCLK_GENDIV_DIV(1) | GCLK_GENDIV_ID(0));
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_OE | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC8M | GCLK_GENCTRL_ID(0));
    GCLK->GENDIV.reg = (GCLK_GENDIV_DIV(1) | GCLK_GENDIV_ID(1));
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_OE | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(1));
    
    //init pins
    PM->APBBMASK.reg |= PM_APBBMASK_USB;
    PM->AHBMASK.reg |= PM_AHBMASK_USB;
    GCLK->CLKCTRL.reg = (USB_GCLK_ID | GCLK_CLKCTRL_GEN_GCLK1 | GCLK_CLKCTRL_CLKEN);
    
    // USB Pin Init
    PORT->Group[0].DIRSET.reg = PORT_PA24;
    PORT->Group[0].OUTCLR.reg = PORT_PA24;
    PORT->Group[0].PINCFG[24].reg = PORT_PINCFG_PMUXEN; //mux enabled no pull up/down

    PORT->Group[0].DIRSET.reg = PORT_PA25;
    PORT->Group[0].OUTCLR.reg = PORT_PA25;
    PORT->Group[0].PINCFG[25].reg = PORT_PINCFG_PMUXEN; //mux enabled no pull up/down

    //Pins 24/25 is group 12 and USB is function G
    PORT->Group[0].PMUX[12].reg = (PORT_PMUX_PMUXO_G | PORT_PMUX_PMUXE_G);   
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
    //TC2->COUNT16.CC[0].reg = 328;
    
    TC2->COUNT16.CC[0].reg = 3000;
    
    TC2->COUNT16.CTRLA.reg = 
            (TC_CTRLA_PRESCSYNC_PRESC | 
            TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_WAVEGEN_NFRQ | 
            TC_CTRLA_MODE_COUNT16 | TC_CTRLA_ENABLE);
}

void init_IO(void)
{
    
}
