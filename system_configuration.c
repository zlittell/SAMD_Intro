#include <sam.h>

void configureClocks(void)
{
    NVMCTRL->CTRLB.reg |= NVMCTRL_CTRLB_RWS(2);
    //__pm_init())
    PM->CPUSEL.reg = PM_CPUSEL_CPUDIV(PM_CPUSEL_CPUDIV_DIV1);
    PM->APBASEL.reg = PM_APBASEL_APBADIV(PM_APBASEL_APBADIV_DIV1);
    PM->APBBSEL.reg = PM_APBBSEL_APBBDIV(PM_APBBSEL_APBBDIV_DIV1);
    PM->APBCSEL.reg = PM_APBCSEL_APBCDIV(PM_APBCSEL_APBCDIV_DIV1);
    
    //SYSCTRL Clock Configure
    //OSC8M
    SYSCTRL->OSC8M.reg = 
            ((SYSCTRL->OSC8M.reg & SYSCTRL_OSC8M_FRANGE_Msk) | 
            (SYSCTRL->OSC8M.reg & SYSCTRL_OSC8M_CALIB_Msk) | 
            SYSCTRL_OSC8M_PRESC_3 | SYSCTRL_OSC8M_ENABLE);
    while(!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_OSC8MRDY)); //Wait for steady
    
    //OSC32K (could use always on low power clock instead but has less accuracy)
    SYSCTRL->OSC32K.reg |= (SYSCTRL_OSC32K_ENABLE | SYSCTRL_OSC32K_EN32K);
    
    //GCLK Configure
    //1)set up reference GCLKs
    //48MHz on Gen0
    GCLK->GENDIV.reg = GCLK_GENDIV_DIV(1) | GCLK_GENDIV_ID(0);
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(0));
    //32KHz on Gen3
    GCLK->GENDIV.reg = (GCLK_GENDIV_DIV(0) | GCLK_GENDIV_ID(3));
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC32K | GCLK_GENCTRL_ID(3));
    
    //DFLL enabled in SYSCTRL
    SYSCTRL->DFLLCTRL.reg = 
            (SYSCTRL_DFLLCTRL_CCDIS | SYSCTRL_DFLLCTRL_ONDEMAND | 
            SYSCTRL_DFLLCTRL_USBCRM | SYSCTRL_DFLLCTRL_MODE | 
            SYSCTRL_DFLLCTRL_ONDEMAND | SYSCTRL_DFLLCTRL_ENABLE);
    SYSCTRL->DFLLMUL.reg = 
            (SYSCTRL_DFLLMUL_CSTEP(1) | SYSCTRL_DFLLMUL_FSTEP(1) | 
            SYSCTRL_DFLLMUL_MUL(48000));
    //Use DFLLVAL from TINYUSB source. Not sure why but hey whatever.
    //DS says these are readonly when in closed loop mode???????
    SYSCTRL->DFLLVAL.reg = 
            (SYSCTRL_DFLLVAL_COARSE(0xA) | SYSCTRL_DFLLVAL_FINE(0x200));
    while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY));


    //USB INIT MOVE THIS EVENTUALLY
    PM->APBBMASK.reg |= PM_APBBMASK_USB;
    PM->AHBMASK.reg |= PM_AHBMASK_USB;
    GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_USB);
    
     // USB Pin Init
    PORT->Group[0].DIRSET.reg = PORT_PA24;
    PORT->Group[0].OUTCLR.reg = PORT_PA24;
    PORT->Group[0].PINCFG[24].reg = 0x01; //mux enabled no pull up/down
    
    PORT->Group[0].DIRSET.reg = PORT_PA25;
    PORT->Group[0].OUTCLR.reg = PORT_PA25;
    PORT->Group[0].PINCFG[25].reg = 0x01; //mux enabled no pull up/down
    
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
