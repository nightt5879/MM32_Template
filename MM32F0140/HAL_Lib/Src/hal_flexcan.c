////////////////////////////////////////////////////////////////////////////////
/// @file     hal_flexcan.c
/// @author   AE TEAM
/// @brief    THIS FILE PROVIDES ALL THE CAN FIRMWARE FUNCTIONS.
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
#define __HAL_FLEXCAN_C

// Files includes
#include "hal_flexcan.h"
#include "hal_rcc.h"

#include "string.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup CAN_HAL
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup CAN_Exported_Functions
/// @{

// Typedef for interrupt handler.
typedef void (*flexcan_isr_t)(Flex_CAN_TypeDef* flex_can, flexcan_handle_t* handle);



#if (defined(FLEXCAN_HAS_ERRATA_9595) && FLEXCAN_HAS_ERRATA_9595)
void FLEXCAN_EnterFreezeMode(Flex_CAN_TypeDef* flex_can)
{
    u32 u32TimeoutCount = 0U;
    u32 u32TempMCR      = 0U;
    u32 u32TempIMASK1   = 0U;
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    u32 u32TempIMASK2 = 0U;
#endif

    // Step1: set FRZ enable in MCR.
    flex_can->MCR |= CAN_MCR_FRZ_MASK;

    // Step2: to check if MDIS bit set in MCR. if yes, clear it.
    if (0U != (flex_can->MCR & CAN_MCR_MDIS_MASK)) {
        flex_can->MCR &= ~CAN_MCR_MDIS_MASK;
    }

    // Step3: polling LPMACK.
    u32TimeoutCount = (u32)FLEXCAN_WAIT_TIMEOUT;
    while ((0U == (flex_can->MCR & CAN_MCR_LPMACK_MASK)) && (u32TimeoutCount > 0U)) {
        u32TimeoutCount--;
    }

    // Step4: to check FLTCONF in ESR1 register
    if (0U == (flex_can->ESR1 & CAN_ESR1_FLTCONF_BUSOFF)) {
        // Step5B: Set Halt bits.
        flex_can->MCR |= CAN_MCR_HALT_MASK;

        // Step6B: Poll the MCR register until the Freeze Acknowledge (FRZACK) bit is set, timeout need more than 178
        // CAN bit length, so 20 multiply timeout is enough.
        u32TimeoutCount = (u32)FLEXCAN_WAIT_TIMEOUT * 20U;
        while ((0U == (flex_can->MCR & CAN_MCR_FRZACK_MASK)) && (u32TimeoutCount > 0U)) {
            u32TimeoutCount--;
        }
    }
    else {
        // backup MCR and IMASK register. Errata document not descript it, but we need backup for step 8A and 9A.
        u32TempMCR    = flex_can->MCR;
        u32TempIMASK1 = flex_can->IMASK1;
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        u32TempIMASK2 = flex_can->IMASK2;
#endif

        // Step5A: Set the Soft Reset bit ((SOFTRST) in the MCR.
        flex_can->MCR |= CAN_MCR_SOFTRST_MASK;

        // Step6A: Poll the MCR register until the Soft Reset (SOFTRST) bit is cleared.
        u32TimeoutCount = (u32)FLEXCAN_WAIT_TIMEOUT;
        while ((CAN_MCR_SOFTRST_MASK == (flex_can->MCR & CAN_MCR_SOFTRST_MASK)) && (u32TimeoutCount > 0U)) {
            u32TimeoutCount--;
        }

        // Step7A: Poll the MCR register until the Freeze Acknowledge (FRZACK) bit is set.
        u32TimeoutCount = (u32)FLEXCAN_WAIT_TIMEOUT;
        while ((0U == (flex_can->MCR & CAN_MCR_FRZACK_MASK)) && (u32TimeoutCount > 0U)) {
            u32TimeoutCount--;
        }

        // Step8A: reconfig MCR.
        flex_can->MCR = u32TempMCR;

        // Step9A: reconfig IMASK.
        flex_can->IMASK1 = u32TempIMASK1;
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        flex_can->IMASK2 = u32TempIMASK2;
#endif
    }
}
#elif (defined(FLEXCAN_HAS_ERRATA_8341) && FLEXCAN_HAS_ERRATA_8341)
void FLEXCAN_EnterFreezeMode(Flex_CAN_TypeDef* flex_can)
{
    u32 u32TimeoutCount = 0U;
    u32 u32TempMCR      = 0U;
    u32 u32TempIMASK1   = 0U;
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    u32 u32TempIMASK2   = 0U;
#endif

    // Step1: set FRZ and HALT bit enable in MCR.
    flex_can->MCR |= CAN_MCR_FRZ_MASK;
    flex_can->MCR |= CAN_MCR_HALT_MASK;

    // Step2: to check if MDIS bit set in MCR. if yes, clear it.
    if (0U != (flex_can->MCR & CAN_MCR_MDIS_MASK)) {
        flex_can->MCR &= ~CAN_MCR_MDIS_MASK;
    }

    // Step3: Poll the MCR register until the Freeze Acknowledge (FRZACK) bit is set.
    u32TimeoutCount = (u32)FLEXCAN_WAIT_TIMEOUT * 100U;
    while ((0U == (flex_can->MCR & CAN_MCR_FRZACK_MASK)) && (u32TimeoutCount > 0U)) {
        u32TimeoutCount--;
    }

    // Step4: check whether the timeout reached. if no skip step5 to step8.
    if (0U == u32TimeoutCount) {
        // backup MCR and IMASK register. Errata document not descript it, but we need backup for step 8A and 9A.
        u32TempMCR    = flex_can->MCR;
        u32TempIMASK1 = flex_can->IMASK1;
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        u32TempIMASK2 = flex_can->IMASK2;
#endif
        // Step5: Set the Soft Reset bit ((SOFTRST) in the MCR.
        flex_can->MCR |= CAN_MCR_SOFTRST_MASK;

        // Step6: Poll the MCR register until the Soft Reset (SOFTRST) bit is cleared.
        while (CAN_MCR_SOFTRST_MASK == (flex_can->MCR & CAN_MCR_SOFTRST_MASK)) {
        }

        // Step7: reconfig MCR.
        flex_can->MCR = u32TempMCR;

        // Step8: reconfig IMASK.
        flex_can->IMASK1 = u32TempIMASK1;
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        flex_can->IMASK2 = u32TempIMASK2;
#endif
    }
}
#else
////////////////////////////////////////////////////////////////////////////////
/// @brief  FlexCAN enter freeze mode.
/// @param  flex_can: flex_can pointer.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_EnterFreezeMode(Flex_CAN_TypeDef* flex_can)
{
    // Set Freeze, Halt bits.
    flex_can->MCR |= CAN_MCR_FRZ_MASK;
    flex_can->MCR |= CAN_MCR_HALT_MASK;
    while (0U == (flex_can->MCR & CAN_MCR_FRZACK_MASK)) {
    }
}
#endif
////////////////////////////////////////////////////////////////////////////////
/// @brief  FlexCAN enter freeze mode.
/// @param  flex_can: flex_can pointer.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_ExitFreezeMode(Flex_CAN_TypeDef* flex_can)
{
    // Clear Freeze, Halt bits.
    flex_can->MCR &= ~CAN_MCR_HALT_MASK;
    flex_can->MCR &= ~CAN_MCR_FRZ_MASK;

    // Wait until the FlexCAN Module exit freeze mode.
    while (0U != (flex_can->MCR & CAN_MCR_FRZACK_MASK)) {
    }
}

#if !defined(NDEBUG)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Whether the memory buffer of CAN occupied.
/// @param  flex_can: flex_can pointer.
/// @param  mbIdx:memory buffer index.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
bool FLEXCAN_IsMbOccupied(Flex_CAN_TypeDef* flex_can, u8 mbIdx)
{
    u8 lastOccupiedMb;
    bool fgRet;

    // Is Rx FIFO enabled?
    if (0U != (flex_can->MCR & CAN_MCR_RFEN_MASK)) {
        // Get RFFN value.
        lastOccupiedMb = (u8)((flex_can->CTRL2 & CAN_CTRL2_RFFN_MASK) >> CAN_CTRL2_RFFN_SHIFT);
        // Calculate the number of last Message Buffer occupied by Rx FIFO.
        lastOccupiedMb = ((lastOccupiedMb + 1U) * 2U) + 5U;

#if ((defined(FLEXCAN_HAS_ERRATA_5641) && FLEXCAN_HAS_ERRATA_5641) || \
     (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829))
        // the first valid MB should be occupied by ERRATA 5461 or 5829.
        lastOccupiedMb += 1U;
#endif
        fgRet = (mbIdx <= lastOccupiedMb);
    }
    else {
#if ((defined(FLEXCAN_HAS_ERRATA_5641) && FLEXCAN_HAS_ERRATA_5641) || \
     (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829))
        if (0U == mbIdx) {
            fgRet = true;
        }
        else
#endif
        {
            fgRet = false;
        }
    }

    return fgRet;
}
#endif

#if ((defined(FLEXCAN_HAS_ERRATA_5641) && FLEXCAN_HAS_ERRATA_5641) || \
     (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829))
static u8 FLEXCAN_GetFirstValidMb(Flex_CAN_TypeDef* flex_can)
{
    u8 firstValidMbNum;

    if (0U != (flex_can->MCR & CAN_MCR_RFEN_MASK)) {
        firstValidMbNum = (u8)((flex_can->CTRL2 & CAN_CTRL2_RFFN_MASK) >> CAN_CTRL2_RFFN_SHIFT);
        firstValidMbNum = ((firstValidMbNum + 1U) * 2U) + 6U;
    }
    else {
        firstValidMbNum = 0U;
    }

    return firstValidMbNum;
}
#endif
////////////////////////////////////////////////////////////////////////////////
/// @brief  Whether the memory buffer of CAN interrupt enable.
/// @param  flex_can: flex_can pointer.
/// @param  mbIdx:memory buffer index.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
static bool FLEXCAN_IsMbIntEnabled(Flex_CAN_TypeDef* flex_can, u8 mbIdx)
{
    u32 flag = 1U;
    bool fgRet    = false;

#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    if (mbIdx >= 32U) {
        fgRet = (0U != (flex_can->IMASK2 & (flag << (mbIdx - 32U))));
    }
    else
#endif
    {
        fgRet = (0U != (flex_can->IMASK1 & (flag << mbIdx)));
    }

    return fgRet;
}

/////////////////////////////////////////////////////////////////////////////
/// @brief  FlexCAN reset.
/// @param  flex_can: flex_can pointer.
/// @retval None.
/////////////////////////////////////////////////////////////////////////////
static void FLEXCAN_Reset(Flex_CAN_TypeDef* flex_can)
{
    // The module must should be first exit from low power
    // mode, and then soft reset can be applied.
    u8 i;

#if (defined(FLEXCAN_HAS_DOZE_MODE_SUPPORT) && FLEXCAN_HAS_DOZE_MODE_SUPPORT)
    if (0 != (FLEXCAN_INSTANCE_HAS_DOZE_MODE_SUPPORTn(flex_can))) {
        // De-//assert DOZE Enable Bit.
        flex_can->MCR &= ~CAN_MCR_DOZE_MASK;
    }
#endif

    // Wait until FlexCAN exit from any Low Power Mode.
    while (0U != (flex_can->MCR & CAN_MCR_LPMACK_MASK)) {
    }
    // //assert Soft Reset Signal.
    flex_can->MCR |= CAN_MCR_SOFTRST_MASK;
    // Wait until FlexCAN reset completes.
    while (0U != (flex_can->MCR & CAN_MCR_SOFTRST_MASK)) {
    }

    // Reset MCR register.
#if (defined(FLEXCAN_HAS_GLITCH_FILTER) && FLEXCAN_HAS_GLITCH_FILTER)
    flex_can->MCR |= CAN_MCR_WRNEN_MASK | CAN_MCR_WAKSRC_MASK |
                     CAN_MCR_MAXMB((u32)FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(flex_can) - 1U);
#else
    flex_can->MCR |=
        CAN_MCR_WRNEN_MASK | CAN_MCR_MAXMB((u32)FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(flex_can) - 1U);
#endif

    // Reset CTRL1 and CTRL2 register.

    flex_can->CTRL1 = CAN_CTRL1_SMP_MASK;
    flex_can->CTRL2 = CAN_CTRL2_TASD(0x16) | CAN_CTRL2_RRS_MASK | CAN_CTRL2_EACEN_MASK;

    // Clean all individual Rx Mask of Message Buffers.
    for (i = 0; i < (u32)FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(flex_can); i++) {
        flex_can->RXIMR[i] = 0x3FFFFFFF;
    }

    // Clean Global Mask of Message Buffers.
    flex_can->RXMGMASK = 0x3FFFFFFF;
    // Clean Global Mask of Message Buffer 14.
    flex_can->RX14MASK = 0x3FFFFFFF;
    // Clean Global Mask of Message Buffer 15.
    flex_can->RX15MASK = 0x3FFFFFFF;
    // Clean Global Mask of Rx FIFO.
    flex_can->RXFGMASK = 0x3FFFFFFF;

    // Clean all Message Buffer CS fields.
    for (i = 0; i < (u32)FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(flex_can); i++) {
        flex_can->MB[i].CS = 0x0;
    }
}



//Type definitions
//brief Structure type for grouping CAN bus timing related information.
typedef struct t_can_bus_timing {
    u8 timeQuanta;                               // Total number of time quanta
    u8 propSeg;                                  // CAN propagation segment
    u8 phaseSeg1;                                // CAN phase segment 1
    u8 phaseSeg2;                                // CAN phase segment 2
} tCanBusTiming;

////////////////////////////////////////////////////////////////////////////////
///Local constant declarations
///  @brief CAN bit timing table for dynamically calculating the bittiming settings.
///  @details According to the CAN protocol 1 bit-time can be made up of between 8..25
///           time quanta (TQ). The total TQ in a bit is SYNC + TSEG1 + TSEG2 with SYNC
///           always being 1. The sample point is (SYNC + TSEG1) / (SYNC + TSEG1 + TSEG2)
///           * 100%. This array contains possible and valid time quanta configurations
///           with a sample point between 68..78%. A visual representation of the TQ in
///           a bit is:
///             | SYNCSEG |      TIME1SEG       | TIME2SEG  |
///           Or with an alternative representation:
///             | SYNCSEG | PROPSEG | PHASE1SEG | PHASE2SEG |
///           With the alternative representation TIME1SEG = PROPSEG + PHASE1SEG.
////////////////////////////////////////////////////////////////////////////////

static const tCanBusTiming canTiming[] = {
    // Time-Quanta | PROPSEG | PSEG1 | PSEG2 | Sample-Point
    // ----------------------------------------------------
    {  8U, 3U, 2U, 2U },         //1+3+2+1=8 |  3  |  2  |  2  | 75%
    {  9U, 3U, 3U, 2U },              //   9 |  3  |  3  |  2  | 78%
    { 10U, 3U, 3U, 3U },              //  10 |  3  |  3  |  3  | 70%
    { 11U, 4U, 3U, 3U },              //  11 |  4  |  3  |  3  | 73%
    { 12U, 4U, 4U, 3U },              //  12 |  4  |  4  |  3  | 75%
    { 13U, 5U, 4U, 3U },              //  13 |  5  |  4  |  3  | 77%
    { 14U, 5U, 4U, 4U },              //  14 |  5  |  4  |  4  | 71%
    { 15U, 6U, 4U, 4U },              //  15 |  6  |  4  |  4  | 73%
    { 16U, 6U, 5U, 4U },              //  16 |  6  |  5  |  4  | 75%
    { 17U, 7U, 5U, 4U },              //  17 |  7  |  5  |  4  | 76%
    { 18U, 7U, 5U, 5U },              //  18 |  7  |  5  |  5  | 72%
    { 19U, 8U, 5U, 5U },              //  19 |  8  |  5  |  5  | 74%
    { 20U, 8U, 6U, 5U },              //  20 |  8  |  6  |  5  | 75%
    { 21U, 8U, 7U, 5U },              //  21 |  8  |  7  |  5  | 76%
    { 22U, 8U, 7U, 6U },              //  22 |  8  |  7  |  6  | 73%
    { 23U, 8U, 8U, 6U },              //  23 |  8  |  8  |  6  | 74%
    { 24U, 8U, 8U, 7U },              //  24 |  8  |  8  |  7  | 71%
    { 25U, 8U, 8U, 8U }               //  25 |  8  |  8  |  8  | 68%
};

////////////////////////////////////////////////////////////////////////////////
/// @brief  Set FlexCAN baudrate.
/// @param  flex_can: flex_can pointer.
/// @param  sourceClock_Hz: source clock.
/// @param  baudRate_Bps: Bandrate.
/// @param  timingConfig: timing configuration.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
static void FLEXCAN_SetBaudRate(Flex_CAN_TypeDef* flex_can,
                                u32 sourceClock_Hz,
                                u32 baudRate_Bps,
                                flexcan_timing_config_t timingConfig)
{
    u8  cnt;
    u32 priDiv ;
    // Loop through all possible time quanta configurations to find a match.
    for (cnt = 0; cnt < sizeof(canTiming) / sizeof(canTiming[0]); cnt++) {
        if ((sourceClock_Hz % (baudRate_Bps  * canTiming[cnt].timeQuanta)) == 0U) {
            // Compute the prescaler that goes with this TQ configuration.
            priDiv = sourceClock_Hz / (baudRate_Bps  * canTiming[cnt].timeQuanta);
            timingConfig.preDivider = (u16)priDiv;
            // Make sure the prescaler is valid.
            if ((priDiv > 0U) && (priDiv <= 256U)) {

                // Found a good bus timing configuration.
                timingConfig.phaseSeg1 = canTiming[cnt].phaseSeg1;
                timingConfig.phaseSeg2 = canTiming[cnt].phaseSeg2;
                timingConfig.propSeg = canTiming[cnt].propSeg;
                break;
            }
        }
    }
    // Update actual timing characteristic.
    FLEXCAN_SetTimingConfig(flex_can, (const flexcan_timing_config_t*)(u32)&timingConfig);
}


////////////////////////////////////////////////////////////////////////////////
/// @brief Initializes a FlexCAN instance.
///
/// @note  This function initializes the FlexCAN module with user-defined settings.
/// This example shows how to set up the flexcan_config_t parameters and how
/// to call the FLEXCAN_Init function by passing in these parameters.
///  code
///   flexcan_config_t flexcanConfig;
///   flexcanConfig.clkSrc               = Enum_Flexcan_ClkSrc0;
///   flexcanConfig.baudRate             = 1000000U;
///   flexcanConfig.maxMbNum             = 16;
///   flexcanConfig.enableLoopBack       = false;
///   flexcanConfig.enableSelfWakeup     = false;
///   flexcanConfig.enableIndividMask    = false;
///   flexcanConfig.disableSelfReception = false;
///   flexcanConfig.enableListenOnlyMode = false;
///   flexcanConfig.enableDoze           = false;
///   flexcanConfig.timingConfig         = timingConfig;
///   FLEXCAN_Init(CAN0, &flexcanConfig, 8000000UL);
///   endcode
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param pConfig Pointer to the user-defined configuration structure.
/// @param sourceClock_Hz FlexCAN Protocol Engine clock source frequency in Hz.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_Init(Flex_CAN_TypeDef* flex_can, const flexcan_config_t* pConfig, u32 sourceClock_Hz)
{
    u32 mcrTemp;
    u32 ctrl1Temp;

#if defined(CAN_CTRL1_CLKSRC_MASK)
    {
        // Disable FlexCAN Module.
        FLEXCAN_Enable(flex_can, false);
        // Protocol-Engine clock source selection, This bit must be set
        // when FlexCAN Module in Disable Mode.

        flex_can->CTRL1 = (Enum_Flexcan_ClkSrc0 == pConfig->clkSrc) ? (flex_can->CTRL1 & ~CAN_CTRL1_CLKSRC_MASK) :
                          (flex_can->CTRL1 | CAN_CTRL1_CLKSRC_MASK);
    }
#endif // CAN_CTRL1_CLKSRC_MASK 

    // Enable FlexCAN Module for configuration.
    FLEXCAN_Enable(flex_can, true);

    // Reset to known status.
    FLEXCAN_Reset(flex_can);

    // Save current CTRL1 value and enable to enter Freeze mode(enabled by default).
    ctrl1Temp = flex_can->CTRL1;

    // Save current MCR value and enable to enter Freeze mode(enabled by default).
    mcrTemp = flex_can->MCR;

    // Enable Loop Back Mode?
    ctrl1Temp = (pConfig->enableLoopBack) ? (ctrl1Temp | CAN_CTRL1_LPB_MASK) : (ctrl1Temp & ~CAN_CTRL1_LPB_MASK);

    // Enable Timer Sync?
    ctrl1Temp = (pConfig->enableTimerSync) ? (ctrl1Temp | CAN_CTRL1_TSYN_MASK) : (ctrl1Temp & ~CAN_CTRL1_TSYN_MASK);

    // Enable Listen Only Mode?
    ctrl1Temp = (pConfig->enableListenOnlyMode) ? ctrl1Temp | CAN_CTRL1_LOM_MASK : ctrl1Temp & ~CAN_CTRL1_LOM_MASK;

    // Set the maximum number of Message Buffers
    mcrTemp = (mcrTemp & ~CAN_MCR_MAXMB_MASK) | CAN_MCR_MAXMB((u32)pConfig->maxMbNum - 1U);

    // Enable Self Wake Up Mode and configure the wake up source.
    mcrTemp = (pConfig->enableSelfWakeup) ? (mcrTemp | CAN_MCR_SLFWAK_MASK) : (mcrTemp & ~CAN_MCR_SLFWAK_MASK);
    mcrTemp = (Enum_Flexcan_WakeupSrcFiltered == pConfig->wakeupSrc) ? (mcrTemp | CAN_MCR_WAKSRC_MASK) :
              (mcrTemp & ~CAN_MCR_WAKSRC_MASK);

    // Enable Individual Rx Masking?
    mcrTemp = (pConfig->enableIndividMask) ? (mcrTemp | CAN_MCR_IRMQ_MASK) : (mcrTemp & ~CAN_MCR_IRMQ_MASK);

    // Disable Self Reception?
    mcrTemp = (pConfig->disableSelfReception) ? mcrTemp | CAN_MCR_SRXDIS_MASK : mcrTemp & ~CAN_MCR_SRXDIS_MASK;

#if (defined(FLEXCAN_HAS_DOZE_MODE_SUPPORT) && FLEXCAN_HAS_DOZE_MODE_SUPPORT)
    if (0 != FLEXCAN_INSTANCE_HAS_DOZE_MODE_SUPPORTn(flex_can)) {
        // Enable Doze Mode?
        mcrTemp = (pConfig->enableDoze) ? (mcrTemp | CAN_MCR_DOZE_MASK) : (mcrTemp & ~CAN_MCR_DOZE_MASK);
    }
#endif

    // Write back CTRL1 Configuration to register.
    flex_can->CTRL1 = ctrl1Temp;

#if (defined(FLEXCAN_HAS_MEMORY_ERROR_CONTROL) && FLEXCAN_HAS_MEMORY_ERROR_CONTROL)
    // Enable to update in MCER.
    flex_can->CTRL2 |= CAN_CTRL2_ECRWRE_MASK;
    flex_can->MECR &= ~CAN_MECR_ECRWRDIS_MASK;
#endif

    // Write back MCR Configuration to register.
    flex_can->MCR = mcrTemp;

    // Baud Rate Configuration.
    FLEXCAN_SetBaudRate(flex_can, sourceClock_Hz, pConfig->baudRate, pConfig->timingConfig);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief De-initializes a FlexCAN instance.
///
/// @note  This function disables the FlexCAN module clock and sets all register values to the reset value.
/// @param flex_can FlexCAN peripheral Struct Point.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_Deinit(Flex_CAN_TypeDef* flex_can)
{
    // Reset all Register Contents.
    FLEXCAN_Reset(flex_can);

    // Disable FlexCAN module.
    FLEXCAN_Enable(flex_can, false);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Gets the default configuration structure.
///
/// This function initializes the FlexCAN configuration structure to default values. The default
/// values are as follows.
///   flexcanConfig->clkSrc                  = Enum_Flexcan_ClkSrc0;
///   flexcanConfig->baudRate                = 1000000U;
///   flexcanConfig->baudRateFD              = 2000000U;
///   flexcanConfig->maxMbNum                = 16;
///   flexcanConfig->enableLoopBack          = false;
///   flexcanConfig->enableSelfWakeup        = false;
///   flexcanConfig->enableIndividMask       = false;
///   flexcanConfig->disableSelfReception    = false;
///   flexcanConfig->enableListenOnlyMode    = false;
///   flexcanConfig->enableDoze              = false;
///   flexcanConfig.timingConfig             = timingConfig;
///
/// @param pConfig Pointer to the FlexCAN configuration structure.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_GetDefaultConfig(flexcan_config_t* pConfig)
{
    // Initializes the configure structure to zero.
    (void)memset(pConfig, 0, sizeof(*pConfig));

    // Initialize FlexCAN Module config struct with default value.
    pConfig->clkSrc   = Enum_Flexcan_ClkSrc0;
    pConfig->baudRate = 500000U;

    pConfig->maxMbNum             = 16;
    pConfig->enableLoopBack       = false;
    pConfig->enableTimerSync      = true;
    pConfig->enableSelfWakeup     = false;
    pConfig->wakeupSrc            = Enum_Flexcan_WakeupSrcUnfiltered;
    pConfig->enableIndividMask    = false;
    pConfig->disableSelfReception = false;
    pConfig->enableListenOnlyMode = false;
#if (defined(FLEXCAN_HAS_DOZE_MODE_SUPPORT) && FLEXCAN_HAS_DOZE_MODE_SUPPORT)
    pConfig->enableDoze = false;
#endif
    // Default protocol timing configuration, time quantum is 10.
    pConfig->timingConfig.phaseSeg1  = 3;
    pConfig->timingConfig.phaseSeg2  = 2;
    pConfig->timingConfig.propSeg    = 1;
    pConfig->timingConfig.rJumpwidth = 1;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Sets the FlexCAN protocol timing characteristic.
///
/// This function gives user settings to CAN bus timing characteristic.
/// The function is for an experienced user. For less experienced users, call
/// the FLEXCAN_Init() and fill the baud rate field with a desired value.
/// This provides the default timing characteristics to the module.
///
/// Note that calling FLEXCAN_SetTimingConfig() overrides the baud rate set
/// in FLEXCAN_Init().
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param pConfig Pointer to the timing configuration structure.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_SetTimingConfig(Flex_CAN_TypeDef* flex_can, const flexcan_timing_config_t* pConfig)
{
    // Enter Freeze Mode.
    FLEXCAN_EnterFreezeMode(flex_can);

    // Cleaning previous Timing Setting.
    flex_can->CTRL1 &= ~(CAN_CTRL1_PRESDIV_MASK | CAN_CTRL1_RJW_MASK | CAN_CTRL1_PSEG1_MASK | CAN_CTRL1_PSEG2_MASK |
                         CAN_CTRL1_PROPSEG_MASK);

    // Updating Timing Setting according to configuration structure.
    flex_can->CTRL1 |= (CAN_CTRL1_PRESDIV(pConfig->preDivider - 1) | CAN_CTRL1_RJW(pConfig->rJumpwidth) |
                        CAN_CTRL1_PSEG1(pConfig->phaseSeg1 - 1) | CAN_CTRL1_PSEG2(pConfig->phaseSeg2 - 1) |
                        CAN_CTRL1_PROPSEG(pConfig->propSeg - 1));

    // Exit Freeze Mode.
    FLEXCAN_ExitFreezeMode(flex_can);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Sets the FlexCAN receive message buffer global mask.
///
/// @note This function sets the global mask for the FlexCAN message buffer in a matching process.
/// The configuration is only effective when the Rx individual mask is disabled in the FLEXCAN_Init().
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mask Rx Message Buffer Global Mask value.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_SetRxMbGlobalMask(Flex_CAN_TypeDef* flex_can, u32 mask)
{
    // Enter Freeze Mode.
    FLEXCAN_EnterFreezeMode(flex_can);

    // Setting Rx Message Buffer Global Mask value.
    flex_can->RXMGMASK = mask;
    flex_can->RX14MASK = mask;
    flex_can->RX15MASK = mask;

    // Exit Freeze Mode.
    FLEXCAN_ExitFreezeMode(flex_can);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Sets the FlexCAN receive FIFO global mask.
///
/// @note  This function sets the global mask for FlexCAN FIFO in a matching process.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mask Rx Fifo Global Mask value.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_SetRxFifoGlobalMask(Flex_CAN_TypeDef* flex_can, u32 mask)
{
    // Enter Freeze Mode.
    FLEXCAN_EnterFreezeMode(flex_can);

    // Setting Rx FIFO Global Mask value.
    flex_can->RXFGMASK = mask;

    // Exit Freeze Mode.
    FLEXCAN_ExitFreezeMode(flex_can);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Sets the FlexCAN receive individual mask.
///
/// @note  This function sets the individual mask for the FlexCAN matching process.
/// The configuration is only effective when the Rx individual mask is enabled in the FLEXCAN_Init().
/// If the Rx FIFO is disabled, the individual mask is applied to the corresponding Message Buffer.
/// If the Rx FIFO is enabled, the individual mask for Rx FIFO occupied Message Buffer is applied to
/// the Rx Filter with the same index. Note that only the first 32
/// individual masks can be used as the Rx FIFO filter mask.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param maskIdx The Index of individual Mask.
/// @param mask Rx Individual Mask value.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_SetRxIndividualMask(Flex_CAN_TypeDef* flex_can, u8 maskIdx, u32 mask)
{

    // Enter Freeze Mode.
    FLEXCAN_EnterFreezeMode(flex_can);

    // Setting Rx Individual Mask value.
    flex_can->RXIMR[maskIdx] = mask;

    // Exit Freeze Mode.
    FLEXCAN_ExitFreezeMode(flex_can);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Configures a FlexCAN transmit message buffer.
///
/// @note  This function aborts the previous transmission, cleans the Message Buffer, and
/// configures it as a Transmit Message Buffer.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mbIdx The Message Buffer index.
/// @param enable Enable/disable Tx Message Buffer.
///               - true: Enable Tx Message Buffer.
///               - false: Disable Tx Message Buffer.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_SetTxMbConfig(Flex_CAN_TypeDef* flex_can, u8 mbIdx, bool enable)
{
    // Inactivate Message Buffer.
    if (enable) {
        flex_can->MB[mbIdx].CS = CAN_CS_CODE(Enum_Flexcan_TxMbInactive);
    }
    else {
        flex_can->MB[mbIdx].CS = 0;
    }

    // Clean Message Buffer content.
    flex_can->MB[mbIdx].ID    = 0x0;
    flex_can->MB[mbIdx].WORD0 = 0x0;
    flex_can->MB[mbIdx].WORD1 = 0x0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Calculates the segment values for a single bit time for classical CAN
///
/// @param baudRate The data speed in bps
/// @param tqNum Number of time quantas per bit
/// @param pTimingConfig Pointer to the FlexCAN timing configuration structure.
///
/// @return TRUE if Calculates the segment success, FALSE if Calculates the segment success
////////////////////////////////////////////////////////////////////////////////
static bool FLEXCAN_GetSegments(u32 baudRate, u32 tqNum, flexcan_timing_config_t* pTimingConfig)
{
    u32 ideal_sp;
    u32 p1;
    bool fgRet = false;

    // Get ideal sample point. For the Bit field in CTRL1 register can't calculate higher ideal SP, we set it as the
    // lowest one(75%).
    ideal_sp = IDEAL_SP_LOW;

    // distribute time quanta.
    p1                     = tqNum * (u32)ideal_sp;
    pTimingConfig->propSeg = (u8)(p1 / (u32)IDEAL_SP_FACTOR - 2U);
    if (pTimingConfig->propSeg <= (MAX_PSEG1 + MAX_PROPSEG)) {
        if (pTimingConfig->propSeg > MAX_PROPSEG) {
            pTimingConfig->phaseSeg1 = pTimingConfig->propSeg - MAX_PROPSEG;
            pTimingConfig->propSeg   = MAX_PROPSEG;
        }
        else {
            pTimingConfig->phaseSeg1 = 0;
        }

        // The value of prog Seg should be not larger than tqNum -4U.
        if ((pTimingConfig->propSeg + pTimingConfig->phaseSeg1) < ((u8)tqNum - 4U)) {
            pTimingConfig->phaseSeg2 = (u8)tqNum - (pTimingConfig->phaseSeg1 + pTimingConfig->propSeg + 4U);

            if (pTimingConfig->phaseSeg2 <= MAX_PSEG1) {
                if ((pTimingConfig->phaseSeg1 < pTimingConfig->phaseSeg2) &&
                        (pTimingConfig->propSeg > (pTimingConfig->phaseSeg2 - pTimingConfig->phaseSeg1))) {
                    pTimingConfig->propSeg -= (pTimingConfig->phaseSeg2 - pTimingConfig->phaseSeg1);
                    pTimingConfig->phaseSeg1 = pTimingConfig->phaseSeg2;
                }

                // subtract one TQ for sync seg.
                // sjw is 20% of total TQ, rounded to nearest int.
                pTimingConfig->rJumpwidth = ((u8)tqNum + 4U) / 5U - 1U;
                // The max tqNum for CBT will reach to 129, ERJW would not be larger than 26.
                // Considering that max ERJW is 31, rJumpwidth will always be smaller than MAX_ERJW.
                if (pTimingConfig->rJumpwidth > MAX_RJW) {
                    pTimingConfig->rJumpwidth = MAX_RJW;
                }

                fgRet = true;
            }
        }
    }
    return fgRet;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Calculates the improved timing values by specific baudrates for classical CAN
///
/// @param baudRate  The classical CAN speed in bps defined by user
/// @param sourceClock_Hz The Source clock data speed in bps. Zero to disable baudrate switching
/// @param pTimingConfig Pointer to the FlexCAN timing configuration structure.
///
/// @return TRUE if timing configuration found, FALSE if failed to find configuration
////////////////////////////////////////////////////////////////////////////////
bool FLEXCAN_CalculateImprovedTimingValues(u32 baudRate,
        u32 sourceClock_Hz,
        flexcan_timing_config_t* pTimingConfig)
{
    u32 clk;   // the clock is tqNumb x baudRateFD.
    u32 tqNum; // Numbers of TQ.
    bool fgRet = false;

    //  Auto Improved Protocal timing for CTRL1.
    tqNum = CTRL1_MAX_TIME_QUANTA;
    do {
        clk = baudRate * tqNum;
        if (clk > sourceClock_Hz) {
            continue; // tqNum too large, clk has been exceed sourceClock_Hz.
        }

        if ((sourceClock_Hz / clk * clk) != sourceClock_Hz) {
            continue; // Non-supporting: the frequency of clock source is not divisible by target baud rate, the user
            // should change a divisible baud rate.
        }

        pTimingConfig->preDivider = (u16)(sourceClock_Hz / clk) - 1U;
        if (pTimingConfig->preDivider > MAX_PRESDIV) {
            break; // The frequency of source clock is too large or the baud rate is too small, the pre-divider could
            // not handle it.
        }

        // Try to get the best timing configuration.
        if (FLEXCAN_GetSegments(baudRate, tqNum, pTimingConfig)) {
            fgRet = true;
            break;
        }
    } while (--tqNum >= CTRL1_MIN_TIME_QUANTA);

    return fgRet;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Configures a FlexCAN Receive Message Buffer.
///
/// @note This function cleans a FlexCAN build-in Message Buffer and configures it
/// as a Receive Message Buffer.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mbIdx The Message Buffer index.
/// @param pRxMbConfig Pointer to the FlexCAN Message Buffer configuration structure.
/// @param enable Enable/disable Rx Message Buffer.
///               - true: Enable Rx Message Buffer.
///               - false: Disable Rx Message Buffer.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_SetRxMbConfig(Flex_CAN_TypeDef* flex_can, u8 mbIdx, const flexcan_rx_mb_config_t* pRxMbConfig, bool enable)
{
    u32 cs_temp = 0;

    // Inactivate Message Buffer.
    flex_can->MB[mbIdx].CS = 0;

    // Clean Message Buffer content.
    flex_can->MB[mbIdx].ID    = 0x0;
    flex_can->MB[mbIdx].WORD0 = 0x0;
    flex_can->MB[mbIdx].WORD1 = 0x0;

    if (enable) {
        // Setup Message Buffer ID.
        flex_can->MB[mbIdx].ID = pRxMbConfig->id;

        // Setup Message Buffer format.
        if (Enum_Flexcan_FrameFormatExtend == pRxMbConfig->format) {
            cs_temp |= CAN_CS_IDE_MASK;
        }

        // Setup Message Buffer type.
        if (Enum_Flexcan_FrameTypeRemote == pRxMbConfig->type) {
            cs_temp |= CAN_CS_RTR_MASK;
        }

        // Activate Rx Message Buffer.
        cs_temp |= CAN_CS_CODE(Enum_Flexcan_RxMbEmpty);
        flex_can->MB[mbIdx].CS = cs_temp;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Configures the FlexCAN Rx FIFO.
///
/// @note This function configures the Rx FIFO with given Rx FIFO configuration.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param pRxFifoConfig Pointer to the FlexCAN Rx FIFO configuration structure.
/// @param enable Enable/disable Rx FIFO.
///               - true: Enable Rx FIFO.
///               - false: Disable Rx FIFO.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_SetRxFifoConfig(Flex_CAN_TypeDef* flex_can, const flexcan_rx_fifo_config_t* pRxFifoConfig, bool enable)
{

    volatile u32* mbAddr;
    u8 i, j, k, rffn = 0, numMbOccupy;
    u32 setup_mb = 0;

    // Enter Freeze Mode.
    FLEXCAN_EnterFreezeMode(flex_can);

    if (enable) {
        // Get the setup_mb value.
        setup_mb = (u8)((flex_can->MCR & CAN_MCR_MAXMB_MASK) >> CAN_MCR_MAXMB_SHIFT);
        setup_mb = (setup_mb < (u32)FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(flex_can)) ?
                   setup_mb :
                   (u32)FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(flex_can);

        // Determine RFFN value.
        for (i = 0; i <= 0xFU; i++) {
            if ((8U * (i + 1U)) >= pRxFifoConfig->idFilterNum) {
                rffn = i;
                //assert(((setup_mb - 8U) - (2U * rffn)) > 0U);

                flex_can->CTRL2 = (flex_can->CTRL2 & ~CAN_CTRL2_RFFN_MASK) | CAN_CTRL2_RFFN(rffn);
                break;
            }
        }

        // caculate the Number of Mailboxes occupied by RX Legacy FIFO and the filter.
        numMbOccupy = 6U + (rffn + 1U) * 2U;

        // Copy ID filter table to Message Buffer Region (Fix MISRA_C-2012 Rule 18.1).
        j = 0U;
        for (i = 6U; i < numMbOccupy; i++) {
            // Get address for current mail box.
            mbAddr = &(flex_can->MB[i].CS);

            // One Mail box contain 4U DWORD registers.
            for (k = 0; k < 4U; k++) {
                // Fill all valid filter in the mail box occupied by filter.
                // Disable unused Rx FIFO Filter, the other rest of register in the last Mail box occupied by fiter set
                // as 0xffffffff.

                mbAddr[k] = (j < pRxFifoConfig->idFilterNum) ? (pRxFifoConfig->idFilterTable[j]) : 0xFFFFFFFFU;

                // Try to fill next filter in current Mail Box.
                j++;
            }
        }

        // Setup ID Fitlter Type.
        switch (pRxFifoConfig->idFilterType) {
            case Enum_Flexcan_RxFifoFilterTypeA:
                flex_can->MCR = (flex_can->MCR & ~CAN_MCR_IDAM_MASK) | CAN_MCR_IDAM(0x0);
                break;
            case Enum_Flexcan_RxFifoFilterTypeB:
                flex_can->MCR = (flex_can->MCR & ~CAN_MCR_IDAM_MASK) | CAN_MCR_IDAM(0x1);
                break;
            case Enum_Flexcan_RxFifoFilterTypeC:
                flex_can->MCR = (flex_can->MCR & ~CAN_MCR_IDAM_MASK) | CAN_MCR_IDAM(0x2);
                break;
            case Enum_Flexcan_RxFifoFilterTypeD:
                // All frames rejected.
                flex_can->MCR = (flex_can->MCR & ~CAN_MCR_IDAM_MASK) | CAN_MCR_IDAM(0x3);
                break;
            default:
                // All the cases have been listed above, the default clause should not be reached.
                break;
        }

        // Setting Message Reception Priority.
        flex_can->CTRL2 = (pRxFifoConfig->priority == Enum_Flexcan_RxFifoPrioHigh) ? (flex_can->CTRL2 & ~CAN_CTRL2_MRP_MASK) :
                          (flex_can->CTRL2 | CAN_CTRL2_MRP_MASK);

        // Enable Rx Message FIFO.
        flex_can->MCR |= CAN_MCR_RFEN_MASK;
    }
    else {
        rffn = (u8)((flex_can->CTRL2 & CAN_CTRL2_RFFN_MASK) >> CAN_CTRL2_RFFN_SHIFT);
        // caculate the Number of Mailboxes occupied by RX Legacy FIFO and the filter.
        numMbOccupy = 6U + (rffn + 1U) * 2U;

        // Disable Rx Message FIFO.
        flex_can->MCR &= ~CAN_MCR_RFEN_MASK;

        // Clean MB0 ~ MB5 and all MB occupied by ID filters (Fix MISRA_C-2012 Rule 18.1).
        for (i = 0; i < numMbOccupy; i++) {
            FLEXCAN_SetRxMbConfig(flex_can, i, NULL, false);
        }
    }

    // Exit Freeze Mode.
    FLEXCAN_ExitFreezeMode(flex_can);
}

#if (defined(FLEXCAN_HAS_RX_FIFO_DMA) && FLEXCAN_HAS_RX_FIFO_DMA)
////////////////////////////////////////////////////////////////////////////////
/// @brief Enables or disables the FlexCAN Rx FIFO DMA request.
///
/// @note  This function enables or disables the DMA feature of FlexCAN build-in Rx FIFO.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param enable true to enable, false to disable.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_EnableRxFifoDMA(Flex_CAN_TypeDef* flex_can, bool enable)
{
    if (enable) {
        // Enter Freeze Mode.
        FLEXCAN_EnterFreezeMode(flex_can);

        // Enable FlexCAN DMA.
        flex_can->MCR |= CAN_MCR_DMA_MASK;

        // Exit Freeze Mode.
        FLEXCAN_ExitFreezeMode(flex_can);
    }
    else {
        // Enter Freeze Mode.
        FLEXCAN_EnterFreezeMode(flex_can);

        // Disable FlexCAN DMA.
        flex_can->MCR &= ~CAN_MCR_DMA_MASK;

        // Exit Freeze Mode.
        FLEXCAN_ExitFreezeMode(flex_can);
    }
}
#endif // FLEXCAN_HAS_RX_FIFO_DMA 

#if (defined(FLEXCAN_HAS_ERRATA_6032) && FLEXCAN_HAS_ERRATA_6032)

/// FlexCAN: A frame with wrong ID or payload is transmitted into
/// the CAN bus when the Message Buffer under transmission is
/// either aborted or deactivated while the CAN bus is in the Bus Idle state
///
/// This function to do workaround for ERR006032
///
/// param flex_can FlexCAN peripheral Struct Point.
/// param mbIdx The FlexCAN Message Buffer index.

static void FLEXCAN_ERRATA_6032(Flex_CAN_TypeDef* flex_can, volatile u32* mbCSAddr)
{
    u32 dbg_temp      = 0U;
    u32 u32TempCS     = 0U;
    u32 u32Timeout    = DELAY_BUSIDLE;
    u32 u32TempIMASK1 = flex_can->IMASK1;
    //after backup all interruption, disable ALL interruption
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    u32 u32TempIMASK2 = flex_can->IMASK2;
    flex_can->IMASK2           = 0;
#endif
    flex_can->IMASK1 = 0;
    dbg_temp     = (u32)(flex_can->DBG1);
    switch (dbg_temp & CAN_DBG1_CFSM_MASK) {
        case RXINTERMISSION:
            if (CBN_VALUE3 == (dbg_temp & CAN_DBG1_CBN_MASK)) {
                //wait until CFSM is different from RXINTERMISSION
                while (RXINTERMISSION == (flex_can->DBG1 & CAN_DBG1_CFSM_MASK)) {
                    __NOP();
                }
            }
            break;
        case TXINTERMISSION:
            if (CBN_VALUE3 == (dbg_temp & CAN_DBG1_CBN_MASK)) {
                //wait until CFSM is different from TXINTERMISSION
                while (TXINTERMISSION == (flex_can->DBG1 & CAN_DBG1_CFSM_MASK)) {
                    __NOP();
                }
            }
            break;
        default:
            // To avoid MISRA-C 2012 rule 16.4 issue.
            break;
    }
    //Anyway, BUSIDLE need to delay
    if (BUSIDLE == (flex_can->DBG1 & CAN_DBG1_CFSM_MASK)) {
        while (u32Timeout-- > 0U) {
            __NOP();
        }

        //Write 0x0 into Code field of CS word.
        u32TempCS = (u32)(*mbCSAddr);
        u32TempCS &= ~CAN_CS_CODE_MASK;
        *mbCSAddr = u32TempCS;
    }
    //restore interruption
    flex_can->IMASK1 = u32TempIMASK1;
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    flex_can->IMASK2 = u32TempIMASK2;
#endif
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief Writes a FlexCAN Message to the Transmit Message Buffer.
///
/// @note  This function writes a CAN Message to the specified Transmit Message Buffer
/// and changes the Message Buffer state to start CAN Message transmit. After
/// that the function returns immediately.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mbIdx The FlexCAN Message Buffer index.
/// @param pTxFrame Pointer to CAN message frame to be sent.
/// @retval Status_Flexcan_Success - Write Tx Message Buffer Successfully.
/// @retval Status_Flexcan_Fail    - Tx Message Buffer is currently in use.
////////////////////////////////////////////////////////////////////////////////
u32 FLEXCAN_WriteTxMb(Flex_CAN_TypeDef* flex_can, u8 mbIdx, const flexcan_frame_t* pTxFrame)
{
    u32 cs_temp = 0;
    u32 status;

#if (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_6032) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_6032)
    FLEXCAN_ERRATA_6032(flex_can, &(flex_can->MB[mbIdx].CS));
#endif
    // Check if Message Buffer is available.
    if (CAN_CS_CODE(Enum_Flexcan_TxMbDataOrRemote) != (flex_can->MB[mbIdx].CS & CAN_CS_CODE_MASK)) {
        // Inactive Tx Message Buffer.
        flex_can->MB[mbIdx].CS = (flex_can->MB[mbIdx].CS & ~CAN_CS_CODE_MASK) | CAN_CS_CODE(Enum_Flexcan_TxMbInactive);

        // Fill Message ID field.
        flex_can->MB[mbIdx].ID = pTxFrame->id;

        // Fill Message Format field.
        if ((u32)Enum_Flexcan_FrameFormatExtend == pTxFrame->format) {
            cs_temp |= CAN_CS_SRR_MASK | CAN_CS_IDE_MASK;
        }

        // Fill Message Type field.
        if ((u32)Enum_Flexcan_FrameTypeRemote == pTxFrame->type) {
            cs_temp |= CAN_CS_RTR_MASK;
        }

        cs_temp |= CAN_CS_CODE(Enum_Flexcan_TxMbDataOrRemote) | CAN_CS_DLC(pTxFrame->length);

        // Load Message Payload.
        flex_can->MB[mbIdx].WORD0 = pTxFrame->dataWord0;
        flex_can->MB[mbIdx].WORD1 = pTxFrame->dataWord1;

        // Activate Tx Message Buffer.
        flex_can->MB[mbIdx].CS = cs_temp;

#if ((defined(FLEXCAN_HAS_ERRATA_5641) && FLEXCAN_HAS_ERRATA_5641) || \
     (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829))
        flex_can->MB[FLEXCAN_GetFirstValidMb(flex_can)].CS = CAN_CS_CODE(Enum_Flexcan_TxMbInactive);
        flex_can->MB[FLEXCAN_GetFirstValidMb(flex_can)].CS = CAN_CS_CODE(Enum_Flexcan_TxMbInactive);
#endif

        status = Status_Flexcan_Success;
    }
    else {
        // Tx Message Buffer is activated, return immediately.
        status = Status_Flexcan_Fail;
    }
    return status;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Reads a FlexCAN Message from Receive Message Buffer.
///
/// @note  This function reads a CAN message from a specified Receive Message Buffer.
/// The function fills a receive CAN message frame structure with
/// just received data and activates the Message Buffer again.
/// The function returns immediately.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mbIdx The FlexCAN Message Buffer index.
/// @param pRxFrame Pointer to CAN message frame structure for reception.
/// @retval Status_Flexcan_Success            - Rx Message Buffer is full and has been read successfully.
/// @retval Status_Flexcan_RxOverflow - Rx Message Buffer is already overflowed and has been read successfully.
/// @retval Status_Flexcan_Fail               - Rx Message Buffer is empty.
////////////////////////////////////////////////////////////////////////////////
u32 FLEXCAN_ReadRxMb(Flex_CAN_TypeDef* flex_can, u8 mbIdx, flexcan_frame_t* pRxFrame)
{
    u32 cs_temp;
    u32 rx_code;
    u32 status;

    // Read CS field of Rx Message Buffer to lock Message Buffer.
    cs_temp = flex_can->MB[mbIdx].CS;
    // Get Rx Message Buffer Code field.
    rx_code = (cs_temp & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT;

    // Check to see if Rx Message Buffer is full.
    if (((u32)Enum_Flexcan_RxMbFull == rx_code) || ((u32)Enum_Flexcan_RxMbOverrun == rx_code)) {
        // Store Message ID.
        pRxFrame->id = flex_can->MB[mbIdx].ID & (CAN_ID_EXT_MASK | CAN_ID_STD_MASK);

        // Get the message ID and format.
        pRxFrame->format = (cs_temp & CAN_CS_IDE_MASK) != 0U ? (u8)Enum_Flexcan_FrameFormatExtend :
                           (u8)Enum_Flexcan_FrameFormatStandard;

        // Get the message type.
        pRxFrame->type =
            (cs_temp & CAN_CS_RTR_MASK) != 0U ? (u8)Enum_Flexcan_FrameTypeRemote : (u8)Enum_Flexcan_FrameTypeData;

        // Get the message length.
        pRxFrame->length = (u8)((cs_temp & CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT);

        // Get the time stamp.
        pRxFrame->timestamp = (u16)((cs_temp & CAN_CS_TIME_STAMP_MASK) >> CAN_CS_TIME_STAMP_SHIFT);

        // Store Message Payload.
        pRxFrame->dataWord0 = flex_can->MB[mbIdx].WORD0;
        pRxFrame->dataWord1 = flex_can->MB[mbIdx].WORD1;

        // Read free-running timer to unlock Rx Message Buffer.
        (void)flex_can->TIMER;

        if ((u32)Enum_Flexcan_RxMbFull == rx_code) {
            status = Status_Flexcan_Success;
        }
        else {
            status = Status_Flexcan_RxOverflow;
        }
    }
    else {
        // Read free-running timer to unlock Rx Message Buffer.
        (void)flex_can->TIMER;

        status = Status_Flexcan_Fail;
    }
    return status;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Reads a FlexCAN Message from Rx FIFO.
///
/// @note  This function reads a CAN message from the FlexCAN build-in Rx FIFO.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param pRxFrame Pointer to CAN message frame structure for reception.
/// @retval Status_Flexcan_Success - Read Message from Rx FIFO successfully.
/// @retval Status_Flexcan_Fail    - Rx FIFO is not enabled.
////////////////////////////////////////////////////////////////////////////////
u32 FLEXCAN_ReadRxFifo(Flex_CAN_TypeDef* flex_can, flexcan_frame_t* pRxFrame)
{
    u32 cs_temp;
    u32 status;

    // Check if Rx FIFO is Enabled.
    if (0U != (flex_can->MCR & CAN_MCR_RFEN_MASK)) {
        // Read CS field of Rx Message Buffer to lock Message Buffer.
        cs_temp = flex_can->MB[0].CS;

        // Read data from Rx FIFO output port.
        // Store Message ID.
        pRxFrame->id = flex_can->MB[0].ID & (CAN_ID_EXT_MASK | CAN_ID_STD_MASK);

        // Get the message ID and format.
        pRxFrame->format = (cs_temp & CAN_CS_IDE_MASK) != 0U ? (u8)Enum_Flexcan_FrameFormatExtend :
                           (u8)Enum_Flexcan_FrameFormatStandard;

        // Get the message type.
        pRxFrame->type =
            (cs_temp & CAN_CS_RTR_MASK) != 0U ? (u8)Enum_Flexcan_FrameTypeRemote : (u8)Enum_Flexcan_FrameTypeData;

        // Get the message length.
        pRxFrame->length = (u8)((cs_temp & CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT);

        // Get the time stamp.
        pRxFrame->timestamp = (u16)((cs_temp & CAN_CS_TIME_STAMP_MASK) >> CAN_CS_TIME_STAMP_SHIFT);

        // Store Message Payload.
        pRxFrame->dataWord0 = flex_can->MB[0].WORD0;
        pRxFrame->dataWord1 = flex_can->MB[0].WORD1;

        // Store ID Filter Hit Index.
        pRxFrame->idhit = (u16)(flex_can->RXFIR & CAN_RXFIR_IDHIT_MASK);

        // Read free-running timer to unlock Rx Message Buffer.
        (void)flex_can->TIMER;

        status = Status_Flexcan_Success;
    }
    else {
        status = Status_Flexcan_Fail;
    }

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Performs a polling send transaction on the CAN bus.
///
/// @note  Transfer handle does not need to be created  before calling this API.
///
/// @param flex_can FlexCAN peripheral flex_can pointer.
/// @param mbIdx The FlexCAN Message Buffer index.
/// @param pTxFrame Pointer to CAN message frame to be sent.
/// @retval Status_Flexcan_Success - Write Tx Message Buffer Successfully.
/// @retval Status_Flexcan_Fail    - Tx Message Buffer is currently in use.
////////////////////////////////////////////////////////////////////////////////
u32 FLEXCAN_TransferSendBlocking(Flex_CAN_TypeDef* flex_can, u8 mbIdx, flexcan_frame_t* pTxFrame)
{
    u32 status;

    // Write Tx Message Buffer to initiate a data sending.
    if (Status_Flexcan_Success == FLEXCAN_WriteTxMb(flex_can, mbIdx, (const flexcan_frame_t*)(u32)pTxFrame)) {
        // Wait until CAN Message send out.
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        u64 u64flag = 1;
        while (0U == FLEXCAN_GetMbStatusFlags(flex_can, u64flag << mbIdx))
#else
        u32 u32flag = 1;
        while (0U == FLEXCAN_GetMbStatusFlags(flex_can, u32flag << mbIdx))
#endif
        {
        }

        // Clean Tx Message Buffer Flag.
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        FLEXCAN_ClearMbStatusFlags(flex_can, u64flag << mbIdx);
#else
        FLEXCAN_ClearMbStatusFlags(flex_can, u32flag << mbIdx);
#endif
        //After TX MB tranfered success, update the Timestamp from MB[mbIdx].CS register
        pTxFrame->timestamp = (u16)((flex_can->MB[mbIdx].CS & CAN_CS_TIME_STAMP_MASK) >> CAN_CS_TIME_STAMP_SHIFT);

        status = Status_Flexcan_Success;
    }
    else {
        status = Status_Flexcan_Fail;
    }

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Performs a polling receive transaction on the CAN bus.
///
/// @note  Transfer handle does not need to be created  before calling this API.
///
/// @param flex_can FlexCAN peripheral flex_can pointer.
/// @param mbIdx The FlexCAN Message Buffer index.
/// @param pRxFrame Pointer to CAN message frame structure for reception.
/// @retval Status_Flexcan_Success            - Rx Message Buffer is full and has been read successfully.
/// @retval Status_Flexcan_RxOverflow - Rx Message Buffer is already overflowed and has been read successfully.
/// @retval Status_Flexcan_Fail               - Rx Message Buffer is empty.
////////////////////////////////////////////////////////////////////////////////
u32 FLEXCAN_TransferReceiveBlocking(Flex_CAN_TypeDef* flex_can, u8 mbIdx, flexcan_frame_t* pRxFrame)
{
    // Wait until Rx Message Buffer non-empty.
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    u64 u64flag = 1;
    while (0U == FLEXCAN_GetMbStatusFlags(flex_can, u64flag << mbIdx))
#else
    u32 u32flag = 1;
    while (0U == FLEXCAN_GetMbStatusFlags(flex_can, u32flag << mbIdx))
#endif
    {
    }

    // Clean Rx Message Buffer Flag.
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    FLEXCAN_ClearMbStatusFlags(flex_can, u64flag << mbIdx);
#else
    FLEXCAN_ClearMbStatusFlags(flex_can, u32flag << mbIdx);
#endif

    // Read Received CAN Message.
    return FLEXCAN_ReadRxMb(flex_can, mbIdx, pRxFrame);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Performs a polling receive transaction from Rx FIFO on the CAN bus.
///
/// @note  Transfer handle does not need to be created  before calling this API.
///
/// @param flex_can FlexCAN peripheral flex_can pointer.
/// @param pRxFrame Pointer to CAN message frame structure for reception.
/// @retval Status_Flexcan_Success - Read Message from Rx FIFO successfully.
/// @retval Status_Flexcan_Fail    - Rx FIFO is not enabled.
////////////////////////////////////////////////////////////////////////////////
u32 FLEXCAN_TransferReceiveFifoBlocking(Flex_CAN_TypeDef* flex_can, flexcan_frame_t* pRxFrame)
{
    u32 rxFifoStatus;

    // Wait until Rx FIFO non-empty.
    while (0U == FLEXCAN_GetMbStatusFlags(flex_can, (u32)Enum_Flexcan_RxFifoFrameAvlFlag)) {
    }

    //
    rxFifoStatus = FLEXCAN_ReadRxFifo(flex_can, pRxFrame);

    // Clean Rx Fifo available flag.
    FLEXCAN_ClearMbStatusFlags(flex_can, (u32)Enum_Flexcan_RxFifoFrameAvlFlag);

    return rxFifoStatus;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Initializes the FlexCAN handle.
///
/// @note  This function initializes the FlexCAN handle, which can be used for other FlexCAN
/// transactional APIs. Usually, for a specified FlexCAN instance,
/// call this API once to get the initialized handle.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param handle FlexCAN handle pointer.
/// @param callback The callback function.
/// @param userData The parameter of the callback function.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_TransferCreateHandle(Flex_CAN_TypeDef* flex_can,
                                  flexcan_handle_t* handle,
                                  flexcan_transfer_callback_t callback,
                                  void* userData)
{
    // Clean FlexCAN transfer handle.
    (void)memset(handle, 0, sizeof(*handle));

    // Save the context in global variables to support the double weak mechanism.
//    s_flexcanHandle[instance] = handle;

    // Register Callback function.
    handle->callback = callback;
    handle->userData = userData;

//    s_flexcanIsr = FLEXCAN_TransferHandleIRQ;

    // We Enable Error & Status interrupt here, because this interrupt just
    // report current status of FlexCAN module through Callback function.
    // It is insignificance without a available callback function.

    if (handle->callback != NULL) {
        FLEXCAN_EnableInterrupts(
            flex_can, (u32)Enum_Flexcan_BusOffInterruptEnable | (u32)Enum_Flexcan_ErrorInterruptEnable |
            (u32)Enum_Flexcan_RxWarningInterruptEnable | (u32)Enum_Flexcan_TxWarningInterruptEnable |
            (u32)Enum_Flexcan_WakeUpInterruptEnable);
    }
    else {
        FLEXCAN_DisableInterrupts(
            flex_can, (u32)Enum_Flexcan_BusOffInterruptEnable | (u32)Enum_Flexcan_ErrorInterruptEnable |
            (u32)Enum_Flexcan_RxWarningInterruptEnable | (u32)Enum_Flexcan_TxWarningInterruptEnable |
            (u32)Enum_Flexcan_WakeUpInterruptEnable);
    }

}

////////////////////////////////////////////////////////////////////////////////
/// @brief Sends a message using IRQ.
///
/// @note  This function sends a message using IRQ. This is a non-blocking function, which returns
/// right away. When messages have been sent out, the send callback function is called.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param handle FlexCAN handle pointer.
/// @param pMbXfer FlexCAN Message Buffer transfer structure. See the #flexcan_mb_transfer_t.
/// @retval Status_Flexcan_Success        Start Tx Message Buffer sending process successfully.
/// @retval Status_Flexcan_Fail           Write Tx Message Buffer failed.
/// @retval Status_Flexcan_TxBusy Tx Message Buffer is in use.
////////////////////////////////////////////////////////////////////////////////
u32 FLEXCAN_TransferSendNonBlocking(Flex_CAN_TypeDef* flex_can, flexcan_handle_t* handle, flexcan_mb_transfer_t* pMbXfer)
{
    u32 status;

    // Check if Message Buffer is idle.
    if ((u8)Enum_Flexcan_StateIdle == handle->mbState[pMbXfer->mbIdx]) {
        // Distinguish transmit type.
        if ((u32)Enum_Flexcan_FrameTypeRemote == pMbXfer->frame->type) {
            handle->mbState[pMbXfer->mbIdx] = (u8)Enum_Flexcan_StateTxRemote;
        }
        else {
            handle->mbState[pMbXfer->mbIdx] = (u8)Enum_Flexcan_StateTxData;
        }

        if (Status_Flexcan_Success ==
                FLEXCAN_WriteTxMb(flex_can, pMbXfer->mbIdx, (const flexcan_frame_t*)(u32)pMbXfer->frame)) {
            // Enable Message Buffer Interrupt.
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
            u64 u64mask = 1;
            FLEXCAN_EnableMbInterrupts(flex_can, u64mask << pMbXfer->mbIdx);
#else
            u32 u32mask = 1;
            FLEXCAN_EnableMbInterrupts(flex_can, u32mask << pMbXfer->mbIdx);
#endif
            status = Status_Flexcan_Success;
        }
        else {
            handle->mbState[pMbXfer->mbIdx] = (u8)Enum_Flexcan_StateIdle;
            status                          = Status_Flexcan_Fail;
        }
    }
    else {
        status = Status_Flexcan_TxBusy;
    }

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Receives a message using IRQ.
///
/// @note  This function receives a message using IRQ. This is non-blocking function, which returns
/// right away. When the message has been received, the receive callback function is called.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param handle FlexCAN handle pointer.
/// @param pMbXfer FlexCAN Message Buffer transfer structure. See the #flexcan_mb_transfer_t.
/// @retval Status_Flexcan_Success        - Start Rx Message Buffer receiving process successfully.
/// @retval Status_Flexcan_RxBusy - Rx Message Buffer is in use.
////////////////////////////////////////////////////////////////////////////////
u32 FLEXCAN_TransferReceiveNonBlocking(Flex_CAN_TypeDef* flex_can, flexcan_handle_t* handle, flexcan_mb_transfer_t* pMbXfer)
{
    u32 status;
	u32 u32mask = 1;

    // Check if Message Buffer is idle.
    if ((u8)Enum_Flexcan_StateIdle == handle->mbState[pMbXfer->mbIdx]) {
        handle->mbState[pMbXfer->mbIdx] = (u8)Enum_Flexcan_StateRxData;

        // Register Message Buffer.
        handle->mbFrameBuf[pMbXfer->mbIdx] = pMbXfer->frame;

        // Enable Message Buffer Interrupt.
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        u64 u64mask = 1;
        FLEXCAN_EnableMbInterrupts(flex_can, u64mask << pMbXfer->mbIdx);
#else
        FLEXCAN_EnableMbInterrupts(flex_can, u32mask << pMbXfer->mbIdx);
#endif

        status = Status_Flexcan_Success;
    }
    else {
        status = Status_Flexcan_RxBusy;
    }

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Receives a message from Rx FIFO using IRQ.
///
/// @note  This function receives a message using IRQ. This is a non-blocking function, which returns
/// right away. When all messages have been received, the receive callback function is called.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param handle FlexCAN handle pointer.
/// @param pFifoXfer FlexCAN Rx FIFO transfer structure. See the ref flexcan_fifo_transfer_t.
/// @retval Status_Flexcan_Success            - Start Rx FIFO receiving process successfully.
/// @retval Status_Flexcan_RxFifoBusy - Rx FIFO is currently in use.
////////////////////////////////////////////////////////////////////////////////
u32 FLEXCAN_TransferReceiveFifoNonBlocking(Flex_CAN_TypeDef* flex_can,
        flexcan_handle_t* handle,
        flexcan_fifo_transfer_t* pFifoXfer)
{
    u32 status;
    // Check if Message Buffer is idle.
    if ((u8)Enum_Flexcan_StateIdle == handle->rxFifoState) {
        handle->rxFifoState = (u8)Enum_Flexcan_StateRxFifo;

        // Register Message Buffer.
        handle->rxFifoFrameBuf = pFifoXfer->frame;

        // Enable Message Buffer Interrupt.
        FLEXCAN_EnableMbInterrupts(flex_can, (u32)Enum_Flexcan_RxFifoOverflowFlag | (u32)Enum_Flexcan_RxFifoWarningFlag |
                                   (u32)Enum_Flexcan_RxFifoFrameAvlFlag);

        status = Status_Flexcan_Success;
    }
    else {
        status = Status_Flexcan_RxFifoBusy;
    }

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Aborts the interrupt driven message send process.
///
/// @note  This function aborts the interrupt driven message send process.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param handle FlexCAN handle pointer.
/// @param mbIdx The FlexCAN Message Buffer index.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_TransferAbortSend(Flex_CAN_TypeDef* flex_can, flexcan_handle_t* handle, u8 mbIdx)
{
    u16 timestamp;

    // Disable Message Buffer Interrupt.
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    u64 u64mask = 1;
    FLEXCAN_DisableMbInterrupts(flex_can, u64mask << mbIdx);
#else
    u32 u32mask = 1;
    FLEXCAN_DisableMbInterrupts(flex_can, u32mask << mbIdx);
#endif

    // Update the TX frame 's time stamp by MB[mbIdx].cs.
    timestamp                = (u16)((flex_can->MB[mbIdx].CS & CAN_CS_TIME_STAMP_MASK) >> CAN_CS_TIME_STAMP_SHIFT);
    handle->timestamp[mbIdx] = timestamp;

    // Clean Message Buffer.
    FLEXCAN_SetTxMbConfig(flex_can, mbIdx, true);

    handle->mbState[mbIdx] = (u8)Enum_Flexcan_StateIdle;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Aborts the interrupt driven message receive process.
///
/// @note  This function aborts the interrupt driven message receive process.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param handle FlexCAN handle pointer.
/// @param mbIdx The FlexCAN Message Buffer index.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_TransferAbortReceive(Flex_CAN_TypeDef* flex_can, flexcan_handle_t* handle, u8 mbIdx)
{

    // Disable Message Buffer Interrupt.
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    u64 u64mask = 1;
    FLEXCAN_DisableMbInterrupts(flex_can, (u64mask << mbIdx));
#else
    u32 u32mask = 1;
    FLEXCAN_DisableMbInterrupts(flex_can, (u32mask << mbIdx));
#endif

    // Un-register handle.
    handle->mbFrameBuf[mbIdx] = NULL;
    handle->mbState[mbIdx]    = (u8)Enum_Flexcan_StateIdle;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Aborts the interrupt driven message receive from Rx FIFO process.
///
/// @note  This function aborts the interrupt driven message receive from Rx FIFO process.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param handle FlexCAN handle pointer.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_TransferAbortReceiveFifo(Flex_CAN_TypeDef* flex_can, flexcan_handle_t* handle)
{
    // Check if Rx FIFO is enabled.
    if (0U != (flex_can->MCR & CAN_MCR_RFEN_MASK)) {
        // Disable Rx Message FIFO Interrupts.
        FLEXCAN_DisableMbInterrupts(flex_can, (u32)Enum_Flexcan_RxFifoOverflowFlag | (u32)Enum_Flexcan_RxFifoWarningFlag |
                                    (u32)Enum_Flexcan_RxFifoFrameAvlFlag);

        // Un-register handle.
        handle->rxFifoFrameBuf = NULL;
    }

    handle->rxFifoState = (u8)Enum_Flexcan_StateIdle;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Gets the detail index of Mailbox's Timestamp by handle.
///
/// @note Then function can only be used when calling non-blocking Data transfer (TX/RX) API,
/// After TX/RX data transfer done (User can get the status by handler's callback function),
/// we can get the detail index of Mailbox's timestamp by handle,
/// Detail non-blocking data transfer API (TX/RX) contain.
///   -FLEXCAN_TransferSendNonBlocking
///   -FLEXCAN_TransferFDSendNonBlocking
///   -FLEXCAN_TransferReceiveNonBlocking
///   -FLEXCAN_TransferFDReceiveNonBlocking
///   -FLEXCAN_TransferReceiveFifoNonBlocking
///
/// @param handle FlexCAN handle pointer.
/// @param mbIdx The FlexCAN FD Message Buffer index.
/// @return the index of mailbox 's timestamp stored in the handle.
////////////////////////////////////////////////////////////////////////////////
u32 FLEXCAN_GetTimeStamp(flexcan_handle_t* handle, u8 mbIdx)
{
    return (u32)(handle->timestamp[mbIdx]);
}

static bool FLEXCAN_CheckUnhandleInterruptEvents(Flex_CAN_TypeDef* flex_can)
{
    u64 tempmask;
    u64 tempflag;
    bool fgRet = false;

    // Checking exist error flag.
    if (0U == (FLEXCAN_GetStatusFlags(flex_can) &
               ((u32)Enum_Flexcan_TxWarningIntFlag | (u32)Enum_Flexcan_RxWarningIntFlag |
                (u32)Enum_Flexcan_BusOffIntFlag | (u32)Enum_Flexcan_ErrorIntFlag | (u32)Enum_Flexcan_WakeUpIntFlag))) {
        tempmask = (u64)flex_can->IMASK1;
        tempflag = (u64)flex_can->IFLAG1;

#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        // Checking whether exist MB interrupt status and legacy RX FIFO interrupt status.
        tempmask |= ((u64)flex_can->IMASK2) << 32;
        tempflag |= ((u64)flex_can->IFLAG2) << 32;
#endif
        fgRet = (0U != (tempmask & tempflag));
    }
    else {
        fgRet = true;
    }

    return fgRet;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief FlexCAN cub handler for data transfered.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param handle FlexCAN handle structure point.
/// @param pResult result.
////////////////////////////////////////////////////////////////////////////////
static u32 FLEXCAN_SubHandlerForDataTransfered(Flex_CAN_TypeDef* flex_can, flexcan_handle_t* handle, u32* pResult)
{
    u32 status = Status_Flexcan_UnHandled;
    u32 result = 0xFFU;
	u32 u32flag = 1;

    // For this implementation, we solve the Message with lowest MB index first.
    for (result = 0U; result < (u32)FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(flex_can); result++) {
        // Get the lowest unhandled Message Buffer
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        u64 u64flag = 1;
        if (0U != FLEXCAN_GetMbStatusFlags(flex_can, u64flag << result))
#else
        if (0U != FLEXCAN_GetMbStatusFlags(flex_can, u32flag << result))
#endif
        {
            if (FLEXCAN_IsMbIntEnabled(flex_can, (u8)result)) {
                break;
            }
        }
    }

    // find Message to deal with.
    if (result < (u32)FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(flex_can)) {
        // Solve Legacy Rx FIFO interrupt.
        if (((u8)Enum_Flexcan_StateIdle != handle->rxFifoState) && (result <= (u32)CAN_IFLAG1_BUF7I_SHIFT)) {
            u32 u32mask = 1;
            switch (u32mask << result) {
                case Enum_Flexcan_RxFifoOverflowFlag:
                    status = Status_Flexcan_RxFifoOverflow;
                    break;

                case Enum_Flexcan_RxFifoWarningFlag:
                    status = Status_Flexcan_RxFifoWarning;
                    break;

                case Enum_Flexcan_RxFifoFrameAvlFlag:
                    status = FLEXCAN_ReadRxFifo(flex_can, handle->rxFifoFrameBuf);
                    if (Status_Flexcan_Success == status) {
                        // Align the current (index 0) rxfifo timestamp to the timestamp array by handle.
                        handle->timestamp[0] = handle->rxFifoFrameBuf->timestamp;
                        status               = Status_Flexcan_RxFifoIdle;
                    }
                    FLEXCAN_TransferAbortReceiveFifo(flex_can, handle);
                    break;

                default:
                    status = Status_Flexcan_UnHandled;
                    break;
            }
        }
        else {
            // Get current State of Message Buffer.
            switch (handle->mbState[result]) {
                // Solve Rx Data Frame.
                case (u8)Enum_Flexcan_StateRxData:

                    {
                        status = FLEXCAN_ReadRxMb(flex_can, (u8)result, handle->mbFrameBuf[result]);
                        if (Status_Flexcan_Success == status) {
                            // Align the current index of RX MB timestamp to the timestamp array by handle.
                            handle->timestamp[result] = handle->mbFrameBuf[result]->timestamp;
                            status                    = Status_Flexcan_RxIdle;
                        }
                    }

                    {
                        FLEXCAN_TransferAbortReceive(flex_can, handle, (u8)result);
                    }
                    break;

                // Sove Rx Remote Frame.  User need to Read the frame in Mail box in time by Read from MB API.
                case (u8)Enum_Flexcan_StateRxRemote:
                    status = Status_Flexcan_RxRemote;

                    {
                        FLEXCAN_TransferAbortReceive(flex_can, handle, (u8)result);
                    }
                    break;

                // Solve Tx Data Frame.
                case (u8)Enum_Flexcan_StateTxData:
                    status = Status_Flexcan_TxIdle;

                    {
                        FLEXCAN_TransferAbortSend(flex_can, handle, (u8)result);
                    }
                    break;

                // Solve Tx Remote Frame.
                case (u8)Enum_Flexcan_StateTxRemote:
                    handle->mbState[result] = (u8)Enum_Flexcan_StateRxRemote;
                    status                  = Status_Flexcan_TxSwitchToRx;
                    break;

                default:
                    status = Status_Flexcan_UnHandled;
                    break;
            }
        }

        // Clear resolved Message Buffer IRQ.
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        u64 u64flag = 1;
        FLEXCAN_ClearMbStatusFlags(flex_can, u64flag << result);
#else
        FLEXCAN_ClearMbStatusFlags(flex_can, u32flag << result);
#endif
    }

    *pResult = result;

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief FlexCAN IRQ handle function.
///
/// @note  This function handles the FlexCAN Error, the Message Buffer, and the Rx FIFO IRQ request.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param handle FlexCAN handle pointer.
////////////////////////////////////////////////////////////////////////////////
void FLEXCAN_TransferHandleIRQ(Flex_CAN_TypeDef* flex_can, flexcan_handle_t* handle)
{
    u32 status;
    u32 result    = 0xFFU;
    u32 EsrStatus = 0U;
    do {
        // Get Current FlexCAN Module Error and Status.
        EsrStatus = FLEXCAN_GetStatusFlags(flex_can);

        // To handle FlexCAN Error and Status Interrupt first.
        if (0U != (EsrStatus & ((u32)Enum_Flexcan_TxWarningIntFlag | (u32)Enum_Flexcan_RxWarningIntFlag |
                                (u32)Enum_Flexcan_BusOffIntFlag | (u32)Enum_Flexcan_ErrorIntFlag))) {
            status = Status_Flexcan_ErrorStatus;
            // Clear FlexCAN Error and Status Interrupt.
            FLEXCAN_ClearStatusFlags(flex_can, (u32)Enum_Flexcan_TxWarningIntFlag | (u32)Enum_Flexcan_RxWarningIntFlag |
                                     (u32)Enum_Flexcan_BusOffIntFlag | (u32)Enum_Flexcan_ErrorIntFlag);
            result = EsrStatus;
        }
        else if (0U != (EsrStatus & (u32)Enum_Flexcan_WakeUpIntFlag)) {
            status = Status_Flexcan_WakeUp;
            FLEXCAN_ClearStatusFlags(flex_can, (u32)Enum_Flexcan_WakeUpIntFlag);
        }
        else {
            // to handle real data transfer.
            status = FLEXCAN_SubHandlerForDataTransfered(flex_can, handle, &result);
        }

        // Calling Callback Function if has one.
        if (handle->callback != NULL) {
            handle->callback(flex_can, handle, status, result, handle->userData);
        }
    } while (FLEXCAN_CheckUnhandleInterruptEvents(flex_can));
}
/// @}

/// @}

/// @}



