////////////////////////////////////////////////////////////////////////////////
/// @file     hal_flexcan.h
/// @author   AE TEAM
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE CAN
///           FIRMWARE LIBRARY.
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
#ifndef __HAL_FLEXCAN_H
#define __HAL_FLEXCAN_H

// Files includes
#include "types.h"
#include "reg_flexcan.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup CAN_HAL
/// @brief  CAN HAL modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup CAN_Exported_Types
/// @{



/// @brief FlexCAN Frame ID helper macro.
#define FLEXCAN_ID_STD(id) \
    (((u32)(((u32)(id)) << CAN_ID_STD_SHIFT)) & CAN_ID_STD_MASK) // ! < Standard Frame ID helper macro.
#define FLEXCAN_ID_EXT(id)                                \
    (((u32)(((u32)(id)) << CAN_ID_EXT_SHIFT)) & \
     (CAN_ID_EXT_MASK | CAN_ID_STD_MASK)) // ! < Extend Frame ID helper macro.

/// @brief FlexCAN Rx Message Buffer Mask helper macro.
#define FLEXCAN_RX_MB_STD_MASK(id, rtr, ide)                                   \
    (((u32)((u32)(rtr) << 31) | (u32)((u32)(ide) << 30)) | \
     FLEXCAN_ID_STD(id)) // ! < Standard Rx Message Buffer Mask helper macro.
#define FLEXCAN_RX_MB_EXT_MASK(id, rtr, ide)                                   \
    (((u32)((u32)(rtr) << 31) | (u32)((u32)(ide) << 30)) | \
     FLEXCAN_ID_EXT(id)) // ! < Extend Rx Message Buffer Mask helper macro.

/// @brief FlexCAN Rx FIFO Mask helper macro.
#define FLEXCAN_RX_FIFO_STD_MASK_TYPE_A(id, rtr, ide)                          \
    (((u32)((u32)(rtr) << 31) | (u32)((u32)(ide) << 30)) | \
     (FLEXCAN_ID_STD(id) << 1)) // ! < Standard Rx FIFO Mask helper macro Type A helper macro.
#define FLEXCAN_RX_FIFO_STD_MASK_TYPE_B_HIGH(id, rtr, ide)                     \
    (((u32)((u32)(rtr) << 31) | (u32)((u32)(ide) << 30)) | \
     (((u32)(id)&0x7FF) << 19)) // ! < Standard Rx FIFO Mask helper macro Type B upper part helper macro.
#define FLEXCAN_RX_FIFO_STD_MASK_TYPE_B_LOW(id, rtr, ide)                      \
    (((u32)((u32)(rtr) << 15) | (u32)((u32)(ide) << 14)) | \
     (((u32)(id)&0x7FF) << 3)) // ! < Standard Rx FIFO Mask helper macro Type B lower part helper macro.
#define FLEXCAN_RX_FIFO_STD_MASK_TYPE_C_HIGH(id) \
    (((u32)(id)&0x7F8) << 21) // ! < Standard Rx FIFO Mask helper macro Type C upper part helper macro.
#define FLEXCAN_RX_FIFO_STD_MASK_TYPE_C_MID_HIGH(id) \
    (((u32)(id)&0x7F8) << 13) // ! < Standard Rx FIFO Mask helper macro Type C mid-upper part helper macro.
#define FLEXCAN_RX_FIFO_STD_MASK_TYPE_C_MID_LOW(id) \
    (((u32)(id)&0x7F8) << 5) // ! < Standard Rx FIFO Mask helper macro Type C mid-lower part helper macro.
#define FLEXCAN_RX_FIFO_STD_MASK_TYPE_C_LOW(id) \
    (((u32)(id)&0x7F8) >> 3) // ! < Standard Rx FIFO Mask helper macro Type C lower part helper macro.
#define FLEXCAN_RX_FIFO_EXT_MASK_TYPE_A(id, rtr, ide)                          \
    (((u32)((u32)(rtr) << 31) | (u32)((u32)(ide) << 30)) | \
     (FLEXCAN_ID_EXT(id) << 1)) // ! < Extend Rx FIFO Mask helper macro Type A helper macro.
#define FLEXCAN_RX_FIFO_EXT_MASK_TYPE_B_HIGH(id, rtr, ide)                        \
    (                                                                             \
        ((u32)((u32)(rtr) << 31) | (u32)((u32)(ide) << 30)) | \
        ((FLEXCAN_ID_EXT(id) & 0x1FFF8000)                                        \
         << 1)) // ! < Extend Rx FIFO Mask helper macro Type B upper part helper macro.
#define FLEXCAN_RX_FIFO_EXT_MASK_TYPE_B_LOW(id, rtr, ide)                      \
    (((u32)((u32)(rtr) << 15) | (u32)((u32)(ide) << 14)) | \
     ((FLEXCAN_ID_EXT(id) & 0x1FFF8000) >>                                     \
      15)) // ! < Extend Rx FIFO Mask helper macro Type B lower part helper macro.
#define FLEXCAN_RX_FIFO_EXT_MASK_TYPE_C_HIGH(id) \
    ((FLEXCAN_ID_EXT(id) & 0x1FE00000) << 3) // ! < Extend Rx FIFO Mask helper macro Type C upper part helper macro.
#define FLEXCAN_RX_FIFO_EXT_MASK_TYPE_C_MID_HIGH(id) \
    ((FLEXCAN_ID_EXT(id) & 0x1FE00000) >>            \
     5) // ! < Extend Rx FIFO Mask helper macro Type C mid-upper part helper macro.
#define FLEXCAN_RX_FIFO_EXT_MASK_TYPE_C_MID_LOW(id) \
    ((FLEXCAN_ID_EXT(id) & 0x1FE00000) >>           \
     13) // ! < Extend Rx FIFO Mask helper macro Type C mid-lower part helper macro.
#define FLEXCAN_RX_FIFO_EXT_MASK_TYPE_C_LOW(id) \
    ((FLEXCAN_ID_EXT(id) & 0x1FE00000) >> 21) // ! < Extend Rx FIFO Mask helper macro Type C lower part helper macro.

/// @brief FlexCAN Rx FIFO Filter helper macro.
#define FLEXCAN_RX_FIFO_STD_FILTER_TYPE_A(id, rtr, ide) \
    FLEXCAN_RX_FIFO_STD_MASK_TYPE_A(id, rtr, ide) // ! < Standard Rx FIFO Filter helper macro Type A helper macro.
#define FLEXCAN_RX_FIFO_STD_FILTER_TYPE_B_HIGH(id, rtr, ide) \
    FLEXCAN_RX_FIFO_STD_MASK_TYPE_B_HIGH(                    \
        id, rtr, ide) // ! < Standard Rx FIFO Filter helper macro Type B upper part helper macro.
#define FLEXCAN_RX_FIFO_STD_FILTER_TYPE_B_LOW(id, rtr, ide) \
    FLEXCAN_RX_FIFO_STD_MASK_TYPE_B_LOW(                    \
        id, rtr, ide) // ! < Standard Rx FIFO Filter helper macro Type B lower part helper macro.
#define FLEXCAN_RX_FIFO_STD_FILTER_TYPE_C_HIGH(id) \
    FLEXCAN_RX_FIFO_STD_MASK_TYPE_C_HIGH(          \
        id) // ! < Standard Rx FIFO Filter helper macro Type C upper part helper macro.
#define FLEXCAN_RX_FIFO_STD_FILTER_TYPE_C_MID_HIGH(id) \
    FLEXCAN_RX_FIFO_STD_MASK_TYPE_C_MID_HIGH(          \
        id) // ! < Standard Rx FIFO Filter helper macro Type C mid-upper part helper macro.
#define FLEXCAN_RX_FIFO_STD_FILTER_TYPE_C_MID_LOW(id) \
    FLEXCAN_RX_FIFO_STD_MASK_TYPE_C_MID_LOW(          \
        id) // ! < Standard Rx FIFO Filter helper macro Type C mid-lower part helper macro.
#define FLEXCAN_RX_FIFO_STD_FILTER_TYPE_C_LOW(id) \
    FLEXCAN_RX_FIFO_STD_MASK_TYPE_C_LOW(          \
        id) // ! < Standard Rx FIFO Filter helper macro Type C lower part helper macro. 
#define FLEXCAN_RX_FIFO_EXT_FILTER_TYPE_A(id, rtr, ide) \
    FLEXCAN_RX_FIFO_EXT_MASK_TYPE_A(id, rtr, ide) // ! < Extend Rx FIFO Filter helper macro Type A helper macro.
#define FLEXCAN_RX_FIFO_EXT_FILTER_TYPE_B_HIGH(id, rtr, ide) \
    FLEXCAN_RX_FIFO_EXT_MASK_TYPE_B_HIGH(                    \
        id, rtr, ide) // ! < Extend Rx FIFO Filter helper macro Type B upper part helper macro.
#define FLEXCAN_RX_FIFO_EXT_FILTER_TYPE_B_LOW(id, rtr, ide) \
    FLEXCAN_RX_FIFO_EXT_MASK_TYPE_B_LOW(                    \
        id, rtr, ide) // ! < Extend Rx FIFO Filter helper macro Type B lower part helper macro.
#define FLEXCAN_RX_FIFO_EXT_FILTER_TYPE_C_HIGH(id) \
    FLEXCAN_RX_FIFO_EXT_MASK_TYPE_C_HIGH(          \
        id) // ! < Extend Rx FIFO Filter helper macro Type C upper part helper macro.          
#define FLEXCAN_RX_FIFO_EXT_FILTER_TYPE_C_MID_HIGH(id) \
    FLEXCAN_RX_FIFO_EXT_MASK_TYPE_C_MID_HIGH(          \
        id) // ! < Extend Rx FIFO Filter helper macro Type C mid-upper part helper macro.
#define FLEXCAN_RX_FIFO_EXT_FILTER_TYPE_C_MID_LOW(id) \
    FLEXCAN_RX_FIFO_EXT_MASK_TYPE_C_MID_LOW(          \
        id) // ! < Extend Rx FIFO Filter helper macro Type C mid-lower part helper macro.
#define FLEXCAN_RX_FIFO_EXT_FILTER_TYPE_C_LOW(id) \
    FLEXCAN_RX_FIFO_EXT_MASK_TYPE_C_LOW(id) // ! < Extend Rx FIFO Filter helper macro Type C lower part helper macro.


/// @brief FlexCAN transfer status.
#define    Status_Flexcan_TxBusy         0 // ! < Tx Message Buffer is Busy.
#define    Status_Flexcan_TxIdle         1 // ! < Tx Message Buffer is Idle.
#define    Status_Flexcan_TxSwitchToRx   2 // ! < Remote Message is send out and Message buffer changed to Receive one.
#define    Status_Flexcan_RxBusy         3 // ! < Rx Message Buffer is Busy.
#define    Status_Flexcan_RxIdle         4 // ! < Rx Message Buffer is Idle.
#define    Status_Flexcan_RxOverflow     5 // ! < Rx Message Buffer is Overflowed.
#define    Status_Flexcan_RxFifoBusy     6 // ! < Rx Message FIFO is Busy.
#define    Status_Flexcan_RxFifoIdle     7 // ! < Rx Message FIFO is Idle.
#define    Status_Flexcan_RxFifoOverflow   8 // ! < Rx Message FIFO is overflowed.
#define    Status_Flexcan_RxFifoWarning  9 // ! < Rx Message FIFO is almost overflowed.
#define    Status_Flexcan_ErrorStatus    10// ! < FlexCAN Module Error and Status.
#define    Status_Flexcan_WakeUp         11 // ! < FlexCAN is waken up from STOP mode.
#define    Status_Flexcan_UnHandled      12 // ! < UnHadled Interrupt asserted.
#define    Status_Flexcan_RxRemote       13 // ! < Rx Remote Message Received in Mail box.



#define    Status_Flexcan_Success        0   // ! < Generic status for Success.
#define    Status_Flexcan_Fail           1      // ! < Generic status for Fail.
#define    Status_Flexcan_ReadOnly       2    // ! < Generic status for read only failure.
#define    Status_Flexcan_OutOfRange     3   // ! < Generic status for out of range access.
#define    Status_Flexcan_InvalidArgument  4   // ! < Generic status for invalid argument check.
#define    Status_Flexcan_Timeout        5   // ! < Generic status for timeout.
#define    Status_Flexcan_NoTransferInProgress   6   // ! < Generic status for no transfer in progress.

enum _flexcan_state {
    Enum_Flexcan_StateIdle     = 0x0, // ! < MB/RxFIFO idle.
    Enum_Flexcan_StateRxData   = 0x1, // ! < MB receiving.
    Enum_Flexcan_StateRxRemote = 0x2, // ! < MB receiving remote reply.
    Enum_Flexcan_StateTxData   = 0x3, // ! < MB transmitting.
    Enum_Flexcan_StateTxRemote = 0x4, // ! < MB transmitting remote request.
    Enum_Flexcan_StateRxFifo   = 0x5, // ! < RxFIFO receiving.
};

/// @brief FlexCAN message buffer CODE for Rx buffers.
enum _flexcan_mb_code_rx {
    Enum_Flexcan_RxMbInactive = 0x0, // ! < MB is not active.
    Enum_Flexcan_RxMbFull     = 0x2, // ! < MB is full.
    Enum_Flexcan_RxMbEmpty    = 0x4, // ! < MB is active and empty.
    Enum_Flexcan_RxMbOverrun  = 0x6, // ! < MB is overwritten into a full buffer.
    Enum_Flexcan_RxMbBusy     = 0x8, // ! < FlexCAN is updating the contents of the MB. The CPU must not access the MB.
    Enum_Flexcan_RxMbRanswer = 0xA,  // ! < A frame was configured to recognize a Remote Request Frame and transmit a Response Frame in return.
    Enum_Flexcan_RxMbNotUsed = 0xF,  // ! < Not used.
};

/// @brief FlexCAN message buffer CODE FOR Tx buffers.
enum _flexcan_mb_code_tx {
    Enum_Flexcan_TxMbInactive     = 0x8, // ! < MB is not active.
    Enum_Flexcan_TxMbAbort        = 0x9, // ! < MB is aborted.
    Enum_Flexcan_TxMbDataOrRemote = 0xC, // ! < MB is a TX Data Frame(when MB RTR = 0) or MB is a TX Remote Request Frame (when MB RTR = 1).
    Enum_Flexcan_TxMbTanswer = 0xE,      // ! < MB is a TX Response Request Frame from an incoming Remote Request Frame.
    Enum_Flexcan_TxMbNotUsed = 0xF,      // ! < Not used.
};
/// @brief FlexCAN frame format.
typedef enum _flexcan_frame_format {
    Enum_Flexcan_FrameFormatStandard = 0x0U, // ! < Standard frame format attribute.
    Enum_Flexcan_FrameFormatExtend   = 0x1U, // ! < Extend frame format attribute.
} flexcan_frame_format_t;

/// @brief FlexCAN frame type.
typedef enum _flexcan_frame_type {
    Enum_Flexcan_FrameTypeData   = 0x0U, // ! < Data frame type attribute.
    Enum_Flexcan_FrameTypeRemote = 0x1U, // ! < Remote frame type attribute.
} flexcan_frame_type_t;

/// @brief FlexCAN clock source.
/// @deprecated Do not use the Enum_Flexcan_ClkSrcOs.  It has been superceded Enum_Flexcan_ClkSrc0
/// @deprecated Do not use the Enum_Flexcan_ClkSrcPeri.  It has been superceded Enum_Flexcan_ClkSrc1

typedef enum _flexcan_clock_source {
    Enum_Flexcan_ClkSrcOsc  = 0x0U, // ! < FlexCAN Protocol Engine clock from Oscillator.
    Enum_Flexcan_ClkSrcPeri = 0x1U, // ! < FlexCAN Protocol Engine clock from Peripheral Clock.
    Enum_Flexcan_ClkSrc0    = 0x0U, // ! < FlexCAN Protocol Engine clock selected by user as SRC == 0.
    Enum_Flexcan_ClkSrc1    = 0x1U, // ! < FlexCAN Protocol Engine clock selected by user as SRC == 1.
} flexcan_clock_source_t;

/// @brief FlexCAN wake up source.
typedef enum _flexcan_wake_up_source {
    Enum_Flexcan_WakeupSrcUnfiltered = 0x0U, // ! < FlexCAN uses unfiltered Rx input to detect edge.
    Enum_Flexcan_WakeupSrcFiltered   = 0x1U, // ! < FlexCAN uses filtered Rx input to detect edge.
} flexcan_wake_up_source_t;

/// @brief FlexCAN Rx Fifo Filter type.
typedef enum _flexcan_rx_fifo_filter_type {
    Enum_Flexcan_RxFifoFilterTypeA = 0x0U, // ! < One full ID (standard and extended) per ID Filter element.
    Enum_Flexcan_RxFifoFilterTypeB = 0x1U, // ! < Two full standard IDs or two partial 14-bit ID slices per ID Filter Table element.
    Enum_Flexcan_RxFifoFilterTypeC = 0x2U, // ! < Four partial 8-bit Standard or extended ID slices per ID Filter Table element.
    Enum_Flexcan_RxFifoFilterTypeD = 0x3U, // ! < All frames rejected.
} flexcan_rx_fifo_filter_type_t;


/// @brief  FlexCAN Rx FIFO priority.
///
/// The matching process starts from the Rx MB(or Rx FIFO) with higher priority.
/// If no MB(or Rx FIFO filter) is satisfied, the matching process goes on with
/// the Rx FIFO(or Rx MB) with lower priority.

typedef enum _flexcan_rx_fifo_priority {
    Enum_Flexcan_RxFifoPrioLow  = 0x0U, // ! < Matching process start from Rx Message Buffer first
    Enum_Flexcan_RxFifoPrioHigh = 0x1U, // ! < Matching process start from Rx FIFO first
} flexcan_rx_fifo_priority_t;


/// @brief  FlexCAN interrupt configuration structure, default settings all disabled.
///
/// This structure contains the settings for all of the FlexCAN Module interrupt configurations.
/// Note: FlexCAN Message Buffers and Rx FIFO have their own interrupts.

enum _flexcan_interrupt_enable {
    Enum_Flexcan_BusOffInterruptEnable    = CAN_CTRL1_BOFFMSK_MASK, // ! < Bus Off interrupt.
    Enum_Flexcan_ErrorInterruptEnable     = CAN_CTRL1_ERRMSK_MASK,  // ! < Error interrupt.
    Enum_Flexcan_RxWarningInterruptEnable = CAN_CTRL1_RWRNMSK_MASK, // ! < Rx Warning interrupt.
    Enum_Flexcan_TxWarningInterruptEnable = CAN_CTRL1_TWRNMSK_MASK, // ! < Tx Warning interrupt.
    Enum_Flexcan_WakeUpInterruptEnable    = CAN_MCR_WAKMSK_MASK,    // ! < Wake Up interrupt.
};


/// @brief  FlexCAN status flags.
///
/// This provides constants for the FlexCAN status flags for use in the FlexCAN functions.
/// Note: The CPU read action clears FlEXCAN_ErrorFlag, therefore user need to
/// read FlEXCAN_ErrorFlag and distinguish which error is occur using
/// @ref _flexcan_error_flags enumerations.

enum _flexcan_flags {
    Enum_Flexcan_SynchFlag            = CAN_ESR1_SYNCH_MASK,   // ! < CAN Synchronization Status.
    Enum_Flexcan_TxWarningIntFlag     = CAN_ESR1_TWRNINT_MASK, // ! < Tx Warning Interrupt Flag.
    Enum_Flexcan_RxWarningIntFlag     = CAN_ESR1_RWRNINT_MASK, // ! < Rx Warning Interrupt Flag.
    Enum_Flexcan_TxErrorWarningFlag   = CAN_ESR1_TXWRN_MASK,   // ! < Tx Error Warning Status.
    Enum_Flexcan_RxErrorWarningFlag   = CAN_ESR1_RXWRN_MASK,   // ! < Rx Error Warning Status.
    Enum_Flexcan_IdleFlag             = CAN_ESR1_IDLE_MASK,    // ! < CAN IDLE Status Flag.
    Enum_Flexcan_FaultConfinementFlag = CAN_ESR1_FLTCONF_MASK, // ! < Fault Confinement State Flag.
    Enum_Flexcan_TransmittingFlag     = CAN_ESR1_TX_MASK,      // ! < FlexCAN In Transmission Status.
    Enum_Flexcan_ReceivingFlag        = CAN_ESR1_RX_MASK,      // ! < FlexCAN In Reception Status.
    Enum_Flexcan_BusOffIntFlag        = CAN_ESR1_BOFFINT_MASK, // ! < Bus Off Interrupt Flag.
    Enum_Flexcan_ErrorIntFlag         = CAN_ESR1_ERRINT_MASK,  // ! < Error Interrupt Flag.
    Enum_Flexcan_WakeUpIntFlag        = CAN_ESR1_WAKINT_MASK,  // ! < Wake-Up Interrupt Flag.
    Enum_Flexcan_ErrorFlag            = (int)(                 // ! < All FlexCAN Error Status.
                                            CAN_ESR1_BIT1ERR_MASK | CAN_ESR1_BIT0ERR_MASK | CAN_ESR1_ACKERR_MASK |
                                            CAN_ESR1_CRCERR_MASK | CAN_ESR1_FRMERR_MASK | CAN_ESR1_STFERR_MASK),
};


/// @brief  FlexCAN error status flags.
///
/// The FlexCAN Error Status enumerations is used to report current error of the FlexCAN bus.
/// This enumerations should be used with Enum_Flexcan_ErrorFlag in @ref _flexcan_flags enumerations
/// to ditermine which error is generated.

enum _flexcan_error_flags {
    Enum_Flexcan_StuffingError = CAN_ESR1_STFERR_MASK,  // ! < Stuffing Error.
    Enum_Flexcan_FormError     = CAN_ESR1_FRMERR_MASK,  // ! < Form Error.
    Enum_Flexcan_CrcError      = CAN_ESR1_CRCERR_MASK,  // ! < Cyclic Redundancy Check Error.
    Enum_Flexcan_AckError      = CAN_ESR1_ACKERR_MASK,  // ! < Received no ACK on transmission.
    Enum_Flexcan_Bit0Error     = CAN_ESR1_BIT0ERR_MASK, // ! < Unable to send dominant bit.
    Enum_Flexcan_Bit1Error     = CAN_ESR1_BIT1ERR_MASK, // ! < Unable to send recessive bit.
};


/// @brief  FlexCAN Rx FIFO status flags.
///
/// The FlexCAN Rx FIFO Status enumerations are used to determine the status of the
/// Rx FIFO. Because Rx FIFO occupy the MB0 ~ MB7 (Rx Fifo filter also occupies
/// more Message Buffer space), Rx FIFO status flags are mapped to the corresponding
/// Message Buffer status flags.

enum {
    Enum_Flexcan_RxFifoOverflowFlag = CAN_IFLAG1_BUF7I_MASK, // ! < Rx FIFO overflow flag.
    Enum_Flexcan_RxFifoWarningFlag  = CAN_IFLAG1_BUF6I_MASK, // ! < Rx FIFO almost full flag.
    Enum_Flexcan_RxFifoFrameAvlFlag = CAN_IFLAG1_BUF5I_MASK, // ! < Frames available in Rx FIFO flag.
};

#if defined(__CC_ARM)
#pragma anon_unions
#endif
/// @brief FlexCAN message frame structure.
typedef struct _flexcan_frame {
    struct {
        u32 timestamp : 16; // ! < FlexCAN internal Free-Running Counter Time Stamp.
        u32 length : 4;     // ! < CAN frame payload length in bytes(Range: 0~8).
        u32 type : 1;       // ! < CAN Frame Type(DATA or REMOTE).
        u32 format : 1;     // ! < CAN Frame Identifier(STD or EXT format).
        u32 : 1;            // ! < Reserved.
        u32 idhit : 9;      // ! < CAN Rx FIFO filter hit id(This value is only used in Rx FIFO receive mode).
    };
    struct {
        u32 id : 29; // ! < CAN Frame Identifier, should be set using FLEXCAN_ID_EXT() or FLEXCAN_ID_STD() macro.
        u32 : 3;     // ! < Reserved.
    };
    union {
        struct {
            u32 dataWord0; // ! < CAN Frame payload word0.
            u32 dataWord1; // ! < CAN Frame payload word1.
        };
        struct {
            u8 dataByte3; // ! < CAN Frame payload byte3.
            u8 dataByte2; // ! < CAN Frame payload byte2.
            u8 dataByte1; // ! < CAN Frame payload byte1.
            u8 dataByte0; // ! < CAN Frame payload byte0.
            u8 dataByte7; // ! < CAN Frame payload byte7.
            u8 dataByte6; // ! < CAN Frame payload byte6.
            u8 dataByte5; // ! < CAN Frame payload byte5.
            u8 dataByte4; // ! < CAN Frame payload byte4.
        };
    };
} flexcan_frame_t;

/// @brief FlexCAN protocol timing characteristic configuration structure.
typedef struct _flexcan_timing_config {
    u16 preDivider; // ! < Clock Pre-scaler Division Factor.
    u8 rJumpwidth;  // ! < Re-sync Jump Width.
    u8 phaseSeg1;   // ! < Phase Segment 1.
    u8 phaseSeg2;   // ! < Phase Segment 2.
    u8 propSeg;     // ! < Propagation Segment.
} flexcan_timing_config_t;

/// @brief FlexCAN module configuration structure.
typedef struct _flexcan_config {
    u32 baudRate; // ! < FlexCAN baud rate in bps.
    flexcan_clock_source_t clkSrc;      // ! < Clock source for FlexCAN Protocol Engine.
    flexcan_wake_up_source_t wakeupSrc; // ! < Wake up source selection.
    u8 maxMbNum;                   // ! < The maximum number of Message Buffers used by user.
    bool enableLoopBack;                // ! < Enable or Disable Loop Back Self Test Mode.
    bool enableTimerSync;               // ! < Enable or Disable Timer Synchronization.
    bool enableSelfWakeup;              // ! < Enable or Disable Self Wakeup Mode.
    bool enableIndividMask;             // ! < Enable or Disable Rx Individual Mask.
    bool disableSelfReception;          // ! < Enable or Disable Self Reflection.
    bool enableListenOnlyMode;          // ! < Enable or Disable Listen Only Mode.
#if (defined(FSL_FEATURE_FLEXCAN_HAS_DOZE_MODE_SUPPORT) && FSL_FEATURE_FLEXCAN_HAS_DOZE_MODE_SUPPORT)
    bool enableDoze; // ! < Enable or Disable Doze Mode.
#endif
    flexcan_timing_config_t timingConfig; /// Protocol timing .
} flexcan_config_t;


/// @brief  FlexCAN Receive Message Buffer configuration structure
///
/// This structure is used as the parameter of FLEXCAN_SetRxMbConfig() function.
/// The FLEXCAN_SetRxMbConfig() function is used to configure FlexCAN Receive
/// Message Buffer. The function abort previous receiving process, clean the
/// Message Buffer and activate the Rx Message Buffer using given Message Buffer
/// setting.

typedef struct _flexcan_rx_mb_config {
    u32 id;                   // ! < CAN Message Buffer Frame Identifier, should be set using FLEXCAN_ID_EXT() or FLEXCAN_ID_STD() macro.
    flexcan_frame_format_t format; // ! < CAN Frame Identifier format(Standard of Extend).
    flexcan_frame_type_t type;     // ! < CAN Frame Type(Data or Remote).
} flexcan_rx_mb_config_t;

/// @brief FlexCAN Rx FIFO configuration structure.
typedef struct _flexcan_rx_fifo_config {
    u32* idFilterTable;                    // ! < Pointer to the FlexCAN Rx FIFO identifier filter table.
    u8 idFilterNum;                        // ! < The quantity of filter elements.
    flexcan_rx_fifo_filter_type_t idFilterType; // ! < The FlexCAN Rx FIFO Filter type.
    flexcan_rx_fifo_priority_t priority;        // ! < The FlexCAN Rx FIFO receive priority.
} flexcan_rx_fifo_config_t;

/// @brief FlexCAN Message Buffer transfer.
typedef struct _flexcan_mb_transfer {
    flexcan_frame_t* frame; // ! < The buffer of CAN Message to be transfer.
    u8 mbIdx;          // ! < The index of Message buffer used to transfer Message.
} flexcan_mb_transfer_t;

/// @brief FlexCAN Rx FIFO transfer.
typedef struct _flexcan_fifo_transfer {
    flexcan_frame_t* frame; // ! < The buffer of CAN Message to be received from Rx FIFO.
} flexcan_fifo_transfer_t;

/// @brief FlexCAN handle structure definition.
typedef struct _flexcan_handle flexcan_handle_t;

/// @brief FlexCAN transfer callback function.
///
///  The FlexCAN transfer callback returns a value from the underlying layer.
///  If the status equals to Status_Flexcan_ErrorStatus, the result parameter is the Content of
///  FlexCAN status register which can be used to get the working status(or error status) of FlexCAN module.
///  If the status equals to other FlexCAN Message Buffer transfer status, the result is the index of
///  Message Buffer that generate transfer event.
///  If the status equals to other FlexCAN Message Buffer transfer status, the result is meaningless and should be
///  Ignored.

typedef void (*flexcan_transfer_callback_t)(
    Flex_CAN_TypeDef* flex_can, flexcan_handle_t* handle, u32 status, u32 result, void* userData);

/// @brief FlexCAN handle structure.
struct _flexcan_handle {
    flexcan_transfer_callback_t callback; // ! < Callback function.
    void* userData;                       // ! < FlexCAN callback function parameter.
    flexcan_frame_t* volatile mbFrameBuf[CAN_WORD1_COUNT];

    // ! < The buffer for received data from Message Buffers.
    flexcan_frame_t* volatile rxFifoFrameBuf;     // ! < The buffer for received data from Rx FIFO.
    volatile u8 mbState[CAN_WORD1_COUNT];    // ! < Message Buffer transfer state.
    volatile u8 rxFifoState;                 // ! < Rx FIFO transfer state.
    volatile u32 timestamp[CAN_WORD1_COUNT]; // ! < Mailbox transfer timestamp.
};


/// API


#if defined(__cplusplus)
extern "C" {
#endif



void FLEXCAN_EnterFreezeMode(Flex_CAN_TypeDef* flex_can);

void FLEXCAN_ExitFreezeMode(Flex_CAN_TypeDef* flex_can);


u32 FLEXCAN_GetInstance(Flex_CAN_TypeDef* flex_can);

bool FLEXCAN_CalculateImprovedTimingValues(u32 baudRate,
        u32 sourceClock_Hz,
        flexcan_timing_config_t* pTimingConfig);

void FLEXCAN_Init(Flex_CAN_TypeDef* flex_can, const flexcan_config_t* pConfig, u32 sourceClock_Hz);


void FLEXCAN_Deinit(Flex_CAN_TypeDef* flex_can);

void FLEXCAN_GetDefaultConfig(flexcan_config_t* pConfig);

/// @}


/// @name Configuration.
/// @{



/// @brief  Sets the FlexCAN protocol timing characteristic.
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

void FLEXCAN_SetTimingConfig(Flex_CAN_TypeDef* flex_can, const flexcan_timing_config_t* pConfig);


/// @brief  Sets the FlexCAN receive message buffer global mask.
///
/// This function sets the global mask for the FlexCAN message buffer in a matching process.
/// The configuration is only effective when the Rx individual mask is disabled in the FLEXCAN_Init().
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mask Rx Message Buffer Global Mask value.

void FLEXCAN_SetRxMbGlobalMask(Flex_CAN_TypeDef* flex_can, u32 mask);


/// @brief  Sets the FlexCAN receive FIFO global mask.
///
/// This function sets the global mask for FlexCAN FIFO in a matching process.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mask Rx Fifo Global Mask value.

void FLEXCAN_SetRxFifoGlobalMask(Flex_CAN_TypeDef* flex_can, u32 mask);


/// @brief  Sets the FlexCAN receive individual mask.
///
/// This function sets the individual mask for the FlexCAN matching process.
/// The configuration is only effective when the Rx individual mask is enabled in the FLEXCAN_Init().
/// If the Rx FIFO is disabled, the individual mask is applied to the corresponding Message Buffer.
/// If the Rx FIFO is enabled, the individual mask for Rx FIFO occupied Message Buffer is applied to
/// the Rx Filter with the same index. Note that only the first 32
/// individual masks can be used as the Rx FIFO filter mask.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param maskIdx The Index of individual Mask.
/// @param mask Rx Individual Mask value.

void FLEXCAN_SetRxIndividualMask(Flex_CAN_TypeDef* flex_can, u8 maskIdx, u32 mask);


/// @brief  Configures a FlexCAN transmit message buffer.
///
/// This function aborts the previous transmission, cleans the Message Buffer, and
/// configures it as a Transmit Message Buffer.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mbIdx The Message Buffer index.
/// @param enable Enable/disable Tx Message Buffer.
///               - true: Enable Tx Message Buffer.
///               - false: Disable Tx Message Buffer.

void FLEXCAN_SetTxMbConfig(Flex_CAN_TypeDef* flex_can, u8 mbIdx, bool enable);
/// @brief Configures a FlexCAN Receive Message Buffer.
///         This function cleans a FlexCAN build-in Message Buffer and configures it.
///         as a Receive Message Buffer.
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mbIdx The Message Buffer index.
/// @param pRxMbConfig Pointer to the FlexCAN Message Buffer configuration structure.
/// @param enable Enable or disable Rx Message Buffer.
///               - true: Enable Rx Message Buffer.
///               - false: Disable Rx Message Buffer.
void FLEXCAN_SetRxMbConfig(Flex_CAN_TypeDef* flex_can, u8 mbIdx, const flexcan_rx_mb_config_t* pRxMbConfig, bool enable);
///
/// @brief Configures the FlexCAN Rx FIFO.
/// This function configures the Rx FIFO with given Rx FIFO configuration.
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param pRxFifoConfig Pointer to the FlexCAN Rx FIFO configuration structure.
/// @param enable Enable/disable Rx FIFO.
///               - true: Enable Rx FIFO.
///               - false: Disable Rx FIFO.
///
void FLEXCAN_SetRxFifoConfig(Flex_CAN_TypeDef* flex_can, const flexcan_rx_fifo_config_t* pRxFifoConfig, bool enable);

/// @}

/// @name Status
/// @{



/// @brief  Gets the FlexCAN module interrupt flags.
///
/// This function gets all FlexCAN status flags. The flags are returned as the logical
/// OR value of the enumerators @ref _flexcan_flags. To check the specific status,
/// compare the return value with enumerators in @ref _flexcan_flags.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @return FlexCAN status flags which are ORed by the enumerators in the _flexcan_flags.

static u32 FLEXCAN_GetStatusFlags(Flex_CAN_TypeDef* flex_can)
{
    return (flex_can->ESR1);
}


/// @brief  Clears status flags with the provided mask.
///
/// This function clears the FlexCAN status flags with a provided mask. An automatically cleared flag
/// can't be cleared by this function.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mask The status flags to be cleared, it is logical OR value of @ref _flexcan_flags.
static inline void FLEXCAN_ClearStatusFlags(Flex_CAN_TypeDef* flex_can, u32 mask)
{
    /// Write 1 to clear status flag.
    flex_can->ESR1 = mask;
}


/// @brief  Gets the FlexCAN Bus Error Counter value.
///
/// This function gets the FlexCAN Bus Error Counter value for both Tx and
/// Rx direction. These values may be needed in the upper layer error handling.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param txErrBuf Buffer to store Tx Error Counter value.
/// @param rxErrBuf Buffer to store Rx Error Counter value.
static inline void FLEXCAN_GetBusErrCount(Flex_CAN_TypeDef* flex_can, u8* txErrBuf, u8* rxErrBuf)
{
    if (NULL != txErrBuf) {
        *txErrBuf = (u8)((flex_can->ECR & CAN_ECR_TXERRCNT_MASK) >> CAN_ECR_TXERRCNT_SHIFT);
    }

    if (NULL != rxErrBuf) {
        *rxErrBuf = (u8)((flex_can->ECR & CAN_ECR_RXERRCNT_MASK) >> CAN_ECR_RXERRCNT_SHIFT);
    }
}


/// @brief  Gets the FlexCAN Message Buffer interrupt flags.
///
/// This function gets the interrupt flags of a given Message Buffers.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mask The ORed FlexCAN Message Buffer mask.
/// @return The status of given Message Buffers.
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
static inline u64 FLEXCAN_GetMbStatusFlags(Flex_CAN_TypeDef* flex_can, u64 mask)
#else
static inline u32 FLEXCAN_GetMbStatusFlags(Flex_CAN_TypeDef* flex_can, u32 mask)
#endif
{
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    u64 tempflag = (u64)flex_can->IFLAG1;
    return (tempflag | (((u64)flex_can->IFLAG2) << 32)) & mask;
#else
    return (flex_can->IFLAG1 & mask);
#endif
}


/// @brief  Clears the FlexCAN Message Buffer interrupt flags.
///
/// This function clears the interrupt flags of a given Message Buffers.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mask The ORed FlexCAN Message Buffer mask.

#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
static inline void FLEXCAN_ClearMbStatusFlags(Flex_CAN_TypeDef* flex_can, u64 mask)
#else
static inline void FLEXCAN_ClearMbStatusFlags(Flex_CAN_TypeDef* flex_can, u32 mask)
#endif
{
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    flex_can->IFLAG1 = (u32)(mask & 0xFFFFFFFFU);
    flex_can->IFLAG2 = (u32)(mask >> 32);
#else
    flex_can->IFLAG1 = mask;
#endif
}

/// @}


/// @name Interrupts
/// @{



/// @brief  Enables FlexCAN interrupts according to the provided mask.
///
/// This function enables the FlexCAN interrupts according to the provided mask. The mask
/// is a logical OR of enumeration members, see @ref _flexcan_interrupt_enable.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mask The interrupts to enable. Logical OR of @ref _flexcan_interrupt_enable.
static inline void FLEXCAN_EnableInterrupts(Flex_CAN_TypeDef* flex_can, u32 mask)
{
    /// Solve Wake Up Interrupt.
    if ((u32)Enum_Flexcan_WakeUpInterruptEnable == (mask & (u32)Enum_Flexcan_WakeUpInterruptEnable)) {
        flex_can->MCR |= CAN_MCR_WAKMSK_MASK;
    }

    /// Solve others.
    flex_can->CTRL1 |= (mask & (~((u32)Enum_Flexcan_WakeUpInterruptEnable)));
}


/// @brief  Disables FlexCAN interrupts according to the provided mask.
///
/// This function disables the FlexCAN interrupts according to the provided mask. The mask
/// is a logical OR of enumeration members, see @ref _flexcan_interrupt_enable.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mask The interrupts to disable. Logical OR of @ref _flexcan_interrupt_enable.
static inline void FLEXCAN_DisableInterrupts(Flex_CAN_TypeDef* flex_can, u32 mask)
{
    /// Solve Wake Up Interrupt.
    if ((u32)Enum_Flexcan_WakeUpInterruptEnable == (mask & (u32)Enum_Flexcan_WakeUpInterruptEnable)) {
        flex_can->MCR &= ~CAN_MCR_WAKMSK_MASK;
    }

    /// Solve others.
    flex_can->CTRL1 &= ~(mask & (~((u32)Enum_Flexcan_WakeUpInterruptEnable)));
}

/// @brief  Enables FlexCAN Message Buffer interrupts.
///
/// This function enables the interrupts of given Message Buffers.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mask The ORed FlexCAN Message Buffer mask.

#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
static inline void FLEXCAN_EnableMbInterrupts(Flex_CAN_TypeDef* flex_can, u64 mask)
#else
static inline void FLEXCAN_EnableMbInterrupts(Flex_CAN_TypeDef* flex_can, u32 mask)
#endif
{
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    flex_can->IMASK1 |= (u32)(mask & 0xFFFFFFFFU);
    flex_can->IMASK2 |= (u32)(mask >> 32);
#else
    flex_can->IMASK1 |= mask;
#endif
}


/// @brief  Disables FlexCAN Message Buffer interrupts.
///
/// This function disables the interrupts of given Message Buffers.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param mask The ORed FlexCAN Message Buffer mask.

#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
static inline void FLEXCAN_DisableMbInterrupts(Flex_CAN_TypeDef* flex_can, u64 mask)
#else
static inline void FLEXCAN_DisableMbInterrupts(Flex_CAN_TypeDef* flex_can, u32 mask)
#endif
{
#if (defined(FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    flex_can->IMASK1 &= ~((u32)(mask & 0xFFFFFFFFU));
    flex_can->IMASK2 &= ~((u32)(mask >> 32));
#else
    flex_can->IMASK1 &= ~mask;
#endif
}

/// @}

#if (defined(FLEXCAN_HAS_RX_FIFO_DMA) && FLEXCAN_HAS_RX_FIFO_DMA)

/// @name DMA Control
/// @{



/// @brief  Enables or disables the FlexCAN Rx FIFO DMA request.
///
/// This function enables or disables the DMA feature of FlexCAN build-in Rx FIFO.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @param enable true to enable, false to disable.

void FLEXCAN_EnableRxFifoDMA(Flex_CAN_TypeDef* flex_can, bool enable);


/// @brief  Gets the Rx FIFO Head address.
///
/// This function returns the FlexCAN Rx FIFO Head address, which is mainly used for the DMA/eDMA use case.
///
/// @param flex_can FlexCAN peripheral Struct Point.
/// @return FlexCAN Rx FIFO Head address.

static inline u32 FLEXCAN_GetRxFifoHeadAddr(Flex_CAN_TypeDef* flex_can)
{
    return (u32) & (flex_can->MB[0].CS);
}

/// @}
#endif /// FLEXCAN_HAS_RX_FIFO_DMA


/// @name Bus Operations
/// @{



/// @brief  Enables or disables the FlexCAN module operation.
///
/// This function enables or disables the FlexCAN module.
///
/// @param flex_can FlexCAN flex_can pointer.
/// @param enable true to enable, false to disable.

static inline void FLEXCAN_Enable(Flex_CAN_TypeDef* flex_can, bool enable)
{
    if (enable) {
        flex_can->MCR &= ~CAN_MCR_MDIS_MASK;

        /// Wait FlexCAN exit from low-power mode.
        while (0U != (flex_can->MCR & CAN_MCR_LPMACK_MASK)) {
        }
    }
    else {
        flex_can->MCR |= CAN_MCR_MDIS_MASK;

        /// Wait FlexCAN enter low-power mode.
        while (0U == (flex_can->MCR & CAN_MCR_LPMACK_MASK)) {
        }
    }
}

u32 FLEXCAN_WriteTxMb(Flex_CAN_TypeDef* flex_can, u8 mbIdx, const flexcan_frame_t* pTxFrame);

u32 FLEXCAN_ReadRxMb(Flex_CAN_TypeDef* flex_can, u8 mbIdx, flexcan_frame_t* pRxFrame);



u32 FLEXCAN_ReadRxFifo(Flex_CAN_TypeDef* flex_can, flexcan_frame_t* pRxFrame);


u32 FLEXCAN_TransferSendBlocking(Flex_CAN_TypeDef* flex_can, u8 mbIdx, flexcan_frame_t* pTxFrame);


u32 FLEXCAN_TransferReceiveBlocking(Flex_CAN_TypeDef* flex_can, u8 mbIdx, flexcan_frame_t* pRxFrame);


u32 FLEXCAN_TransferReceiveFifoBlocking(Flex_CAN_TypeDef* flex_can, flexcan_frame_t* pRxFrame);


void FLEXCAN_TransferCreateHandle(Flex_CAN_TypeDef* flex_can,
                                  flexcan_handle_t* handle,
                                  flexcan_transfer_callback_t callback,
                                  void* userData);


u32 FLEXCAN_TransferSendNonBlocking(Flex_CAN_TypeDef* flex_can, flexcan_handle_t* handle, flexcan_mb_transfer_t* pMbXfer);

u32 FLEXCAN_TransferReceiveNonBlocking(Flex_CAN_TypeDef* flex_can, flexcan_handle_t* handle, flexcan_mb_transfer_t* pMbXfer);

u32 FLEXCAN_TransferReceiveFifoNonBlocking(Flex_CAN_TypeDef* flex_can,
        flexcan_handle_t* handle,
        flexcan_fifo_transfer_t* pFifoXfer);

u32 FLEXCAN_GetTimeStamp(flexcan_handle_t* handle, u8 mbIdx);


void FLEXCAN_TransferAbortSend(Flex_CAN_TypeDef* flex_can, flexcan_handle_t* handle, u8 mbIdx);


void FLEXCAN_TransferAbortReceive(Flex_CAN_TypeDef* flex_can, flexcan_handle_t* handle, u8 mbIdx);


void FLEXCAN_TransferAbortReceiveFifo(Flex_CAN_TypeDef* flex_can, flexcan_handle_t* handle);


void FLEXCAN_TransferHandleIRQ(Flex_CAN_TypeDef* flex_can, flexcan_handle_t* handle);

/// @}

#if defined(__cplusplus)
}
#endif



/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////

