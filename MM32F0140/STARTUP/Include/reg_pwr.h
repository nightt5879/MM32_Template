////////////////////////////////////////////////////////////////////////////////
/// @file     reg_pwr.h
/// @author   AE TEAM
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE SERIES OF
///           MM32 FIRMWARE LIBRARY.
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

#ifndef __REG_PWR_H
#define __REG_PWR_H

// Files includes

#include <stdint.h>
#include <stdbool.h>
#include "types.h"




#if defined ( __CC_ARM )
#pragma anon_unions
#endif








////////////////////////////////////////////////////////////////////////////////
/// @brief PWR Base Address Definition
////////////////////////////////////////////////////////////////////////////////
#define PWR_BASE                        (APB1PERIPH_BASE + 0x7000)              ///< Base Address: 0x40007000





////////////////////////////////////////////////////////////////////////////////
/// @brief PWR Register Structure Definition
////////////////////////////////////////////////////////////////////////////////
typedef struct {
    union {
        __IO u32 CR;                                                            ///< Control register,                              offset: 0x00
        __IO u32 CR1;
    };
    union {
        __IO u32 CSR;                                                           ///< Control Status register                        offset: 0x04
        __IO u32 CSR1;
    };
    __IO u32 ANATST;                                                            ///< BOR Control register                           offset: 0x08
    __IO u32 CR2;                                                               ///< Control register  2                            offset: 0x0C
    __IO u32 SR;                                                                ///< Status  register                               offset: 0x10
    __IO u32 SCR;                                                               ///< clear status register                          offset: 0x14
    __IO u32 RESERVED[3];                                                       ///< Reserved,                                      offset: 0x18
    __IO u32 CFGR;                                                              ///< Configuration register                         offset: 0x24
} PWR_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief PWR type pointer Definition
////////////////////////////////////////////////////////////////////////////////
#define PWR                             ((PWR_TypeDef*) PWR_BASE)



////////////////////////////////////////////////////////////////////////////////
/// @brief PWR_CR1 register Bit definition
////////////////////////////////////////////////////////////////////////////////
#define PWR_CR_LDPS_Pos                 (0)
#define PWR_CR_LDPS                     (0x01U  << PWR_CR_LDPS_Pos)             ///< Domain Write Protction

#define PWR_CR_PDDS_Pos                 (1)
#define PWR_CR_PDDS                     (0x01U  << PWR_CR_PDDS_Pos)             ///< Power Down Deepsleep

#define PWR_CR_CWUF_Pos                 (2)
#define PWR_CR_CWUF                     (0x01U  << PWR_CR_CWUF_Pos)             ///< Clear Standby Flag
#define PWR_CR_CSBF_Pos                 (3)
#define PWR_CR_CSBF                     (0x01U  << PWR_CR_CSBF_Pos)             ///< Clear Standby Flag

#define PWR_CR_PVDE_Pos                 (4)
#define PWR_CR_PVDE                     (0x01U  << PWR_CR_PVDE_Pos)             ///< Power Voltage Detector Enable
#define PWR_CR_DBP_Pos                  (8)
#define PWR_CR_DBP                      (0x01U  << PWR_CR_DBP_Pos)              ///< BackUp Domain Write Protect
#define PWR_CR_PLS_Pos                  (9)
#define PWR_CR_PLS                      (0x0FU  << PWR_CR_PLS_Pos)
#define PWR_CR_PLS_0                    (0x00U  << PWR_CR_PLS_Pos)             ///< PVD level selection  1.8v
#define PWR_CR_PLS_1                    (0x01U  << PWR_CR_PLS_Pos)             ///< PVD level selection  2.1v
#define PWR_CR_PLS_2                    (0x02U  << PWR_CR_PLS_Pos)             ///< PVD level selection  2.4v
#define PWR_CR_PLS_3                    (0x03U  << PWR_CR_PLS_Pos)             ///< PVD level selection  2.7v
#define PWR_CR_PLS_4                    (0x04U  << PWR_CR_PLS_Pos)             ///< PVD level selection  3v
#define PWR_CR_PLS_5                    (0x05U  << PWR_CR_PLS_Pos)             ///< PVD level selection  3.3v
#define PWR_CR_PLS_6                    (0x06U  << PWR_CR_PLS_Pos)             ///< PVD level selection  3.6v
#define PWR_CR_PLS_7                    (0x07U  << PWR_CR_PLS_Pos)             ///< PVD level selection  3.9v
#define PWR_CR_PLS_8                    (0x08U  << PWR_CR_PLS_Pos)             ///< PVD level selection  4.2v
#define PWR_CR_PLS_9                    (0x09U  << PWR_CR_PLS_Pos)             ///< PVD level selection  4.5v
#define PWR_CR_PLS_10                   (0x0AU  << PWR_CR_PLS_Pos)             ///< PVD level selection  4.8v

#define PWR_CR_PLS_1V8                  (0x00U  << PWR_CR_PLS_Pos)              ///< PVD level 1.8V
#define PWR_CR_PLS_2V1                  (0x01U  << PWR_CR_PLS_Pos)              ///< PVD level 2.1V
#define PWR_CR_PLS_2V4                  (0x02U  << PWR_CR_PLS_Pos)              ///< PVD level 2.4V
#define PWR_CR_PLS_2V7                  (0x03U  << PWR_CR_PLS_Pos)              ///< PVD level 2.7V
#define PWR_CR_PLS_3V0                  (0x04U  << PWR_CR_PLS_Pos)              ///< PVD level 3.0V
#define PWR_CR_PLS_3V3                  (0x05U  << PWR_CR_PLS_Pos)              ///< PVD level 3.3V
#define PWR_CR_PLS_3V6                  (0x06U  << PWR_CR_PLS_Pos)              ///< PVD level 3.6V
#define PWR_CR_PLS_3V9                  (0x07U  << PWR_CR_PLS_Pos)              ///< PVD level 3.9V
#define PWR_CR_PLS_4V2                  (0x08U  << PWR_CR_PLS_Pos)              ///< PVD level 4.2V
#define PWR_CR_PLS_4V5                  (0x09U  << PWR_CR_PLS_Pos)              ///< PVD level 4.5V
#define PWR_CR_PLS_4V8                  (0x0AU  << PWR_CR_PLS_Pos)              ///< PVD level 4.8V

#define PWR_CR_STDBYFSWK_Pos            (14)
#define PWR_CR_STDBYFSWK_0              (0x00U  << PWR_CR_STDBYFSWK_Pos)       ///< 9 LSI40K cycles to wake up the standby
#define PWR_CR_STDBYFSWK_1              (0x01U  << PWR_CR_STDBYFSWK_Pos)       ///< 7 LSI40K cycles to wake up the standby
#define PWR_CR_STDBYFSWK_2              (0x02U  << PWR_CR_STDBYFSWK_Pos)       ///< 5 LSI40K cycles to wake up the standby
#define PWR_CR_STDBYFSWK_3              (0x03U  << PWR_CR_STDBYFSWK_Pos)       ///< 3 LSI40K cycles to wake up the standby

////////////////////////////////////////////////////////////////////////////////
/// @brief PWR_CSR register Bit definition
////////////////////////////////////////////////////////////////////////////////
#define PWR_CSR_WUF_Pos                 (0)
#define PWR_CSR_WUF                     (0x01U  << PWR_CSR_WUF_Pos)             ///< Wakeup Flag
#define PWR_CSR_SBF_Pos                 (1)
#define PWR_CSR_SBF                     (0x01U  << PWR_CSR_SBF_Pos)             ///< Standby Flag
#define PWR_CSR_PVDO_Pos                (2)
#define PWR_CSR_PVDO                    (0x01U  << PWR_CSR_PVDO_Pos)            ///< PVD Output
#define PWR_CSR_EWUP_Pos                (8)
#define PWR_CSR_EWUP                    (0x01U  << PWR_CSR_EWUP_Pos)            ///< Enable WKUP pin
////////////////////////////////////////////////////////////////////////////////
/// @brief PWR_ANATST register Bit definition
////////////////////////////////////////////////////////////////////////////////
#define PWR_ANATST_BOR_EN_Pos               (0)
#define PWR_ANATST_BOR_EN                   (0x01U << PWR_ANATST_BOR_EN_Pos)           ///< Enable BOR
#define PWR_ANATST_BOR_THRESHOLDSEL_Pos     (1)
#define PWR_ANATST_BOR_THRESHOLDSEL_0       (0x00U << PWR_ANATST_BOR_THRESHOLDSEL_Pos)   ///< BOR threshold voltage select 0.9v
#define PWR_ANATST_BOR_THRESHOLDSEL_1       (0x01U << PWR_ANATST_BOR_THRESHOLDSEL_Pos)   ///< BOR threshold voltage select 1.0v
#define PWR_ANATST_BOR_THRESHOLDSEL_2       (0x02U << PWR_ANATST_BOR_THRESHOLDSEL_Pos)   ///< BOR threshold voltage select 1.1v
#define PWR_ANATST_BOR_THRESHOLDSEL_3       (0x03U << PWR_ANATST_BOR_THRESHOLDSEL_Pos)   ///< BOR threshold voltage select 1.2v
#define PWR_ANATST_PWR_SYNC_Pos             (3)
#define PWR_ANATST_PWR_SYNC                 (0x01U << PWR_ANATST_PWR_SYNC_Pos)           ///< Synchronization signal of power control module
#define PWR_ANATST_BOR_BYP_Pos              (4)
#define PWR_ANATST_BOR_BYP                  (0x01U << PWR_ANATST_BOR_BYP_Pos)           ///<BOR Bypass control bit
#define PWR_ANATST_BOR_FILTERSEL_Pos        (16)
#define PWR_ANATST_BOR_FILTERSEL            (0xFFFU << PWR_ANATST_BOR_FILTERSEL_Pos) ///< BOR Filter length
////////////////////////////////////////////////////////////////////////////////
/// @brief PWR_CR register Bit definition
////////////////////////////////////////////////////////////////////////////////
#define PWR_CR_wkp_edge_Pos                (0)
#define PWR_CR_wkp_edge                    (0x01U << PWR_CR_wkp_edge_Pos)               ///<  Wake up on rising edge
////////////////////////////////////////////////////////////////////////////////
/// @brief PWR_SR register Bit definition
////////////////////////////////////////////////////////////////////////////////
#define PWR_SR_WUF1_Pos                (0)
#define PWR_SR_WUF1                    (0x01U << PWR_SR_WUF1_Pos)               ///<  wake-up flag 1
////////////////////////////////////////////////////////////////////////////////
/// @brief PWR_SCR register Bit definition
////////////////////////////////////////////////////////////////////////////////
#define PWR_SCR_CWUF1_Pos              (0)
#define PWR_SCR_CWUF1                  (0x01U << PWR_SCR_CWUF1_Pos)             ///< clear wake-up flag 1
////////////////////////////////////////////////////////////////////////////////
/// @brief PWR_CFGR register Bit definition
////////////////////////////////////////////////////////////////////////////////
#define PWR_CFGR_LSICALSEL_Pos         (0)
#define PWR_CFGR_LSICALSEL             (0x1FU << PWR_CFGR_LSICALSEL_Pos)        ///< Enable internal clock calibration 
#define PWR_CFGR_LSICAL_Pos            (5)
#define PWR_CFGR_LSICAL                (0x1FU << PWR_CFGR_LSICAL_Pos)           ///< Internal high-speed clock calibration

/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
