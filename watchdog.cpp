/*
*           Source code is free to use and distribute. 
*           Software originally written by Simon Ford, 
*           altered by Lerche. 
*
*           02-07-2012 - Added LPC11U24 Watchdog functions. Lerche
*           23-10-2012 - Trying to publish the library which seems VERY difficult
*/

#include "mbed.h"
#include "WatchDog.h"


WatchDog::WatchDog(float s) {
#if defined(TARGET_LPC1768) || defined(TARGET_LPC2368)
    LPC_WDT->WDCLKSEL = 0x1;                // Set CLK src to PCLK
    uint32_t clk = SystemCoreClock / 16;    // WD has a fixed /4 prescaler, PCLK default is /4 
    LPC_WDT->WDTC = s * (float)clk;         // Load WD Timer Constant with value determined by float s
    LPC_WDT->WDMOD = 0x3;                   // Enabled and Reset        
    feed();    
#elif defined(TARGET_LPC11U24)
    LPC_SYSCON->SYSAHBCLKCTRL |= 0x8000;
    LPC_SYSCON->PDRUNCFG &= 0xFFBF;
    LPC_SYSCON->WDTOSCCTRL = 0x40;
    uint32_t clk = 100000;
    LPC_WWDT->TC = s * (float)clk;
    LPC_WWDT->CLKSEL = 0x1;
    LPC_WWDT->MOD = 0x3;
    feed();
#endif
}

WatchDog_ms::WatchDog_ms(int ms) {
#if defined(TARGET_LPC1768) || defined(TARGET_LPC2368)
    LPC_WDT->WDCLKSEL = 0x1;                            // Set CLK src to PCLK
    uint32_t clk = SystemCoreClock / 16;                // WD has a fixed /4 prescaler, PCLK default is /4 
    LPC_WDT->WDTC = ((float)ms * (float)clk) /1000;     // Load WD Timer Constant with value determined by int ms    
    LPC_WDT->WDMOD = 0x3;                               // Enabled and Reset        
    feed();
#elif defined(TARGET_LPC11U24)
    LPC_SYSCON->SYSAHBCLKCTRL |= 0x8000;
    LPC_SYSCON->PDRUNCFG &= 0xFFBF;
    LPC_SYSCON->WDTOSCCTRL = 0x40;
    uint32_t clk = 100000;
    LPC_WWDT->TC = ((float)ms * (float)clk) / 1000;    
    LPC_WWDT->CLKSEL = 0x1;
    LPC_WWDT->MOD = 0x3;
    feed();
#endif    
}

WatchDog_us::WatchDog_us(int us) {
#if defined(TARGET_LPC1768) || defined(TARGET_LPC2368)
    LPC_WDT->WDCLKSEL = 0x1;                            // Set CLK src to PCLK
    uint32_t clk = SystemCoreClock / 16;                // WD has a fixed /4 prescaler, PCLK default is /4 
    LPC_WDT->WDTC = ((float)us * (float)clk) /1000000;  // Load WD Timer Constant with value determined by int us       
    LPC_WDT->WDMOD = 0x3;                               // Enabled and Reset        
    feed();
#elif defined(TARGET_LPC11U24)
    LPC_SYSCON->SYSAHBCLKCTRL |= 0x8000;
    LPC_SYSCON->PDRUNCFG &= 0xFFBF;
    LPC_SYSCON->WDTOSCCTRL = 0x40;
    uint32_t clk = 100000;
    LPC_WWDT->TC = ((float)us * (float)clk) / 1000000;    
    LPC_WWDT->CLKSEL = 0x1;
    LPC_WWDT->MOD = 0x3;
    feed();
#endif   
}

void WatchDog::feed() {
#if defined(TARGET_LPC1768) || defined(TARGET_LPC2368)
    LPC_WDT->WDFEED = 0xAA;         // Routine to feed the WatchDog
    LPC_WDT->WDFEED = 0x55;         // according to user manual 
#elif defined(TARGET_LPC11U24)
    LPC_WWDT->FEED = 0xAA;
    LPC_WWDT->FEED = 0x55;
#endif    
}
void WatchDog_ms::feed() {
#if defined(TARGET_LPC1768) || defined(TARGET_LPC2368)
    LPC_WDT->WDFEED = 0xAA;         // Routine to feed the WatchDog
    LPC_WDT->WDFEED = 0x55;         // according to user manual
#elif defined(TARGET_LPC11U24)
    LPC_WWDT->FEED = 0xAA;
    LPC_WWDT->FEED = 0x55;
#endif    
}
void WatchDog_us::feed() {
#if defined(TARGET_LPC1768) || defined(TARGET_LPC2368)
    LPC_WDT->WDFEED = 0xAA;         // Routine to feed the WatchDog
    LPC_WDT->WDFEED = 0x55;         // according to user manual
#elif defined(TARGET_LPC11U24)
    LPC_WWDT->FEED = 0xAA;
    LPC_WWDT->FEED = 0x55;
#endif    
}
