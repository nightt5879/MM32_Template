////////////////////////////////////////////////////////////////////////////////
/// @file     SYSTEM_MM32.C
/// @author   AE TEAM
/// @brief    THIS FILE PROVIDES ALL THE SYSTEM FUNCTIONS.
////////////////////////////////////////////////////////////////////////////////
/// @attention
///
/// THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
/// CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
/// TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
/// CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
/// HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
/// CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
///
/// <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////

// Define to prevent recursive inclusion
#define _SYSTEM_MM32_C_

// Files includes

/// @addtogroup CMSIS
/// @{

#include "mm32_device.h"


/// @}



/// @}


/// Uncomment the line corresponding to the desired System clock (SYSCLK)
/// frequency (after reset the HSI is used as SYSCLK source)
///
/// IMPORTANT NOTE:
/// ==============
/// 1. After each device reset the HSI is used as System clock source.
///
/// 2. Please make sure that the selected System clock doesn't exceed your device's
/// maximum frequency.
///
/// 3. If none of the define below is enabled, the HSI is used as System clock
/// source.
///
/// 4. The System clock configuration functions provided within this file assume that:
/// - For Low, Medium and High density Value line devices an external 8MHz
/// crystal is used to drive the System clock.
/// - For Low, Medium and High density devices an external 8MHz crystal is
/// used to drive the System clock.
/// - For Connectivity line devices an external 25MHz crystal is used to drive
/// the System clock.
/// If you are using different crystal you have to adapt those functions accordingly.

//#define SYSCLK_HSE_XXMHz  24000000
//#define SYSCLK_HSE_XXMHz  36000000
//#define SYSCLK_HSE_XXMHz  48000000
//#define SYSCLK_HSE_XXMHz  72000000

#if defined(SYSCLK_HSE_XXMHz)
#if defined(HSE_VALUE) && (!(HSE_VALUE == 8000000))
#warning redefine HSE_VALUE in reg_common.h Line 43 and ignore this warning
#endif
#endif


//#define SYSCLK_HSI_XXMHz  24000000
//#define SYSCLK_HSI_XXMHz  36000000
//#define SYSCLK_HSI_XXMHz  48000000
#define SYSCLK_HSI_XXMHz  72000000

/// Uncomment the following line if you need to relocate your vector Table in
/// Internal SRAM.
///#define VECT_TAB_SRAM
#define VECT_TAB_OFFSET  0x0
/// Vector Table base offset field.
/// This value must be a multiple of 0x200.


/// @}




///////////////////////////////////////////////////////////////
///Clock Definitions
///////////////////////////////////////////////////////////////
#if defined SYSCLK_HSE_XXMHz
u32 SystemCoreClock         = SYSCLK_HSE_XXMHz;
#elif defined SYSCLK_HSI_XXMHz
u32 SystemCoreClock         = SYSCLK_HSI_XXMHz;
#else //HSI Selected as System Clock source 
u32 SystemCoreClock         = HSI_VALUE;
#endif

__I u8 AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

/// @}


static void SetSysClock(void);

#if defined SYSCLK_HSE_XXMHz
static void SetSysClockToXX_HSE(void);

#elif defined SYSCLK_HSI_XXMHz
static void SetSysClockToXX_HSI(void);
#endif

#ifdef DATA_IN_ExtSRAM
static void SystemInit_ExtMemCtl(void);
#endif //DATA_IN_ExtSRAM 


/// @}



/// @brief  Setup the microcontroller system
///         Initialize the Embedded Flash Interface, the PLL and update the
///         SystemCoreClock variable.
/// @note   This function should be used only after reset.
/// @param  None
/// @retval None

void SystemInit (void)
{
    //Reset the RCC clock configuration to the default reset state(for debug purpose)
    //Set HSION bit
    RCC->CR |= (u32)0x00000001;

    //Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits
    RCC->CFGR &= (u32)0xF8FFC00C;

    //Reset HSEON, CSSON and PLLON bits
    RCC->CR &= (u32)0xFEF6FFFF;

    //Reset HSEBYP bit
    RCC->CR &= (u32)0xFFFBFFFF;

    //Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits
    RCC->CFGR &= (u32)0xFF3CFFFF;
    RCC->CR &= (u32)0x008FFFFF;

    //Disable all interrupts and clear pending bits
    RCC->CIR = 0x009F0000;

    //Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers
    //Configure the Flash Latency cycles and enable prefetch buffer
    SetSysClock();
}



/// @brief  use to return the pllm&plln.
/// @param  pllclkSourceFrq : PLL source clock frquency;
///         pllclkFrq : Target PLL clock frquency;
///         plln : PLL factor PLLN
///         pllm : PLL factor PLLM
/// @retval amount of error

u32 AutoCalPllFactor(u32 pllclkSourceFrq, u32 pllclkFrq, u8* plln, u8* pllm)
{
    u32 n, m;
    u32 tempFrq;
    u32 minDiff = pllclkFrq;
    u8  flag = 0;
    for(m = 0; m < 4 ; m++) {
        for(n = 0; n < 64 ; n++) {
            tempFrq =  pllclkSourceFrq * (n + 1) / (m + 1);
            tempFrq = (tempFrq >  pllclkFrq) ? (tempFrq - pllclkFrq) : (pllclkFrq - tempFrq) ;

            if(minDiff > tempFrq) {
                minDiff =  tempFrq;
                *plln = n;
                *pllm = m;
            }
            if(minDiff == 0) {
                flag = 1;
                break;
            }
        }
        if(flag != 0) {
            break;
        }
    }
    return  minDiff;
}


/// @brief  Configures the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers.
/// @param  None
/// @retval None

static void SetSysClock(void)
{
#ifdef SYSCLK_HSE_XXMHz
    SetSysClockToXX_HSE();
#elif defined SYSCLK_HSI_XXMHz
    SetSysClockToXX_HSI();
#endif

    //If none of the define above is enabled, the HSI is used as System clock
    //source (default after reset)
}


#ifdef SYSCLK_HSE_XXMHz
/// @brief  Sets System clock frequency to XXMHz and configure HCLK, PCLK2
///         and PCLK1 prescalers.
/// @note   This function should be used only after reset.
/// @param  None
/// @retval None
static void SetSysClockToXX_HSE(void)
{
    __IO u32 temp, tn, tm;
    __IO u32 StartUpCounter = 0, HSEStatus = 0;

    u8 plln, pllm;
	uint32_t tmpreg = 0, i = 0;

    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));
    //PLL SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------
    //Enable HSE
    RCC->CR |= ((u32)RCC_CR_HSEON);

    for (i = 0; i < 1000; i++)
    {
        __ASM("nop");
    }

    //Wait till HSE is ready and if Time out is reached exit
    while(1) {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        if(HSEStatus != 0)
            break;
        StartUpCounter++;
        if(StartUpCounter >= (10 * HSE_STARTUP_TIMEOUT))
            return;
    }

    if ((RCC->CR & RCC_CR_HSERDY) == RESET) {
        //If HSE fails to start-up, the application will have wrong clock
        //configuration. User can add here some code to deal with this error
        HSEStatus = (u32)0x00;
        return;
    }

    HSEStatus = (u32)0x01;

    for (i = 0; i < 1000; i++)
    {
        __ASM("nop");
    }

    SystemCoreClock         = SYSCLK_HSE_XXMHz;
    //Enable Prefetch Buffer
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    //Flash 0 wait state ,bit0~2
    FLASH->ACR &= ~FLASH_ACR_LATENCY;

    temp = (SystemCoreClock - 1) / 24000000;

    tmpreg= (temp & FLASH_ACR_LATENCY);
	FLASH->ACR |= tmpreg;
    RCC->CFGR &= (~RCC_CFGR_HPRE) & ( ~RCC_CFGR_PPRE1) & (~RCC_CFGR_PPRE2);

    //HCLK = AHB = FCLK = SYSCLK divided by 4
    RCC->CFGR |= (u32)RCC_CFGR_HPRE_DIV4;

    //PCLK2 = APB2 = HCLK divided by 1, APB2 is high APB CLK
    RCC->CFGR |= (u32)RCC_CFGR_PPRE2_DIV1;

    if(SystemCoreClock > 72000000) {
        //PCLK1 = APB1 = HCLK divided by 4, APB1 is low APB CLK
        RCC->CFGR |= (u32)RCC_CFGR_PPRE1_DIV4;
    }
    else if(SystemCoreClock > 36000000) {
        //PCLK1 = APB1 = HCLK divided by 2, APB1 is low APB CLK
        RCC->CFGR |= (u32)RCC_CFGR_PPRE1_DIV2;
    }

    AutoCalPllFactor(HSE_VALUE, SystemCoreClock, &plln, &pllm);

    RCC->PLLCFGR &= ~((u32 ) RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLXTPRE);
    RCC->PLLCFGR |= (u32 ) RCC_PLLCFGR_PLLSRC  ;

    tm = (((u32)pllm) & 0x07);
    tn = (((u32)plln) & 0x7F);

    RCC->APB1ENR |= RCC_APB1ENR_PWR;
    RCC->PLLCFGR &= (u32)((~RCC_PLLCFGR_PLL_DN) & (~RCC_PLLCFGR_PLL_DP));
    tmpreg = (tm << RCC_PLLCFGR_PLL_DP_Pos);
    tmpreg |= (tn << RCC_PLLCFGR_PLL_DN_Pos) ;
    RCC->PLLCFGR |= tmpreg;
    //Enable PLL
    RCC->CR |= RCC_CR_PLLON;
    //Wait till PLL is ready
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
	{
        __ASM ("nop") ;//__NOP();
    }
    //Select PLL as system clock source
    RCC->CFGR &= (u32)((u32)~(RCC_CFGR_SW));
    RCC->CFGR |= (u32)RCC_CFGR_SW_PLL;

    //Wait till PLL is used as system clock source
    while ((RCC->CFGR & (u32)RCC_CFGR_SWS) != (u32)RCC_CFGR_SWS_PLL)
	{
        __ASM ("nop") ;//__NOP();
    }

    for (i = 0; i < 1000; i++)
    {
        __ASM("nop");
    }

    // set HCLK = AHB = FCLK = SYSCLK divided by 2
    RCC->CFGR &= (~(RCC_CFGR_HPRE_0));

    for (i = 0; i < 1000; i++)
    {
        __ASM("nop");
    }

    // set HCLK = AHB = FCLK = SYSCLK divided by 1
    RCC->CFGR &= (~(RCC_CFGR_HPRE_3));

    for (i = 0; i < 1000; i++)
    {
        __ASM("nop");
    }
}


#elif defined SYSCLK_HSI_XXMHz

static void SetSysClockToXX_HSI(void)
{
    __IO u32 temp, tn, tm;
    u8 plln, pllm;
	uint32_t tmpreg = 0, i = 0;

    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));

    SystemCoreClock         = SYSCLK_HSI_XXMHz;
    //Enable Prefetch Buffer
    FLASH->ACR |= FLASH_ACR_PRFTBE;
    //Flash 0 wait state ,bit0~2
    FLASH->ACR &= ~FLASH_ACR_LATENCY;

    temp = (SystemCoreClock - 1) / 24000000;
	tmpreg = (temp & FLASH_ACR_LATENCY);

	FLASH->ACR |= tmpreg;

    RCC->CFGR &= (~RCC_CFGR_HPRE) & ( ~RCC_CFGR_PPRE1) & (~RCC_CFGR_PPRE2);
    //HCLK = AHB = FCLK = SYSCLK divided by 4
    RCC->CFGR |= (u32)RCC_CFGR_HPRE_DIV4;

    //PCLK2 = APB2 = HCLK divided by 1, APB2 is high APB CLK
    RCC->CFGR |= (u32)RCC_CFGR_PPRE2_DIV1;

    if(SystemCoreClock > 72000000) {
        //PCLK1 = APB1 = HCLK divided by 4, APB1 is low APB CLK
        RCC->CFGR |= (u32)RCC_CFGR_PPRE1_DIV4;
    }
    else if(SystemCoreClock > 36000000)
	{
        //PCLK1 = APB1 = HCLK divided by 2, APB1 is low APB CLK
        RCC->CFGR |= (u32)RCC_CFGR_PPRE1_DIV2;
    }

    AutoCalPllFactor(HSI_VALUE_PLL_ON, SystemCoreClock, &plln, &pllm);

    RCC->PLLCFGR &= ~((u32 ) RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLXTPRE) ;
    RCC->PLLCFGR &= ~((u32 ) RCC_PLLCFGR_PLLSRC);

    tm = (((u32)pllm) & 0x07);
    tn = (((u32)plln) & 0x7F);

    RCC->APB1ENR |= RCC_APB1ENR_PWR;
    RCC->PLLCFGR &= (u32)((~RCC_PLLCFGR_PLL_DN) & (~RCC_PLLCFGR_PLL_DP));
    tmpreg = (tm << RCC_PLLCFGR_PLL_DP_Pos);
    tmpreg |= (tn << RCC_PLLCFGR_PLL_DN_Pos) ;
    RCC->PLLCFGR |= tmpreg;
    //Enable PLL
    RCC->CR |= RCC_CR_PLLON;
    //Wait till PLL is ready
    while((RCC->CR & RCC_CR_PLLRDY) == 0) {
        __ASM ("nop") ;//__NOP();
    }
    //Select PLL as system clock source
    RCC->CFGR &= (u32)((u32)~(RCC_CFGR_SW));
    RCC->CFGR |= (u32)RCC_CFGR_SW_PLL;

    //Wait till PLL is used as system clock source
    while ((RCC->CFGR & (u32)RCC_CFGR_SWS) != (u32)RCC_CFGR_SWS_PLL)
	{
        __ASM ("nop") ;//__NOP();
    }

    for (i = 0; i < 1000; i++)
    {
        __ASM("nop");
    }

    // set HCLK = AHB = FCLK = SYSCLK divided by 2
    RCC->CFGR &= (~(RCC_CFGR_HPRE_0));
    for (i = 0; i < 1000; i++)
    {
        __ASM("nop");
    }

    // set HCLK = AHB = FCLK = SYSCLK divided by 1
    RCC->CFGR &= (~(RCC_CFGR_HPRE_3));

    for (i = 0; i < 1000; i++)
    {
        __ASM("nop");
    }
}

#endif



/// @}



/// @}



/// @}



