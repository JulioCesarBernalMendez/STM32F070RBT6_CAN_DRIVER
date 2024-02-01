/**
 * @file      can.h
 * @author    Julio Cesar Bernal Mendez
 * 
 * @brief     This file contains all the definitions and function prototypes for the CAN Controller Module
 *            which includes one MCP2515 CAN Controller Driver and one TJA1050 CAN Transceiver.
 *            Everything was designed for the STM32F070RBT6 Nucleo Board from ST Microelectronics.
 * 
 * @version   1.0
 * @date      2024-01-26
 * 
 * @copyright This project was created for learning purposes only.
 */

#ifndef CAN_H
#define CAN_H

    #include <stdint.h>
    #include "stm32f0xx.h"
    #include "spi.h"
    #include "timer.h"

    /* External crystal oscillator frequency on the MCP2515 */
    #define OSC1_FREQ                                   (8000000U)

    /* Nucleo Board's SPI peripheral to be used for handling the CAN controller */
    #define CAN_SPI1                                    (0x00U)
    #define CAN_SPI2                                    (0x01U)

    /* Macro to compute the OST (Oscillator Start-Up Timer = 128 x OSC1 clock cycles) for the MCP2515 in microseconds */
    #define GET_OST( osc_freq )                        (128000000UL / osc_freq)

    /* Macros to compute the necessary delays for the different CAN frames to be sent. d = number of data bytes, b = CAN baud rate.
       For a standard  data   CAN frame, largest number of bits (after stuffing) = 8n + 44 + floor((34 + 8n - 1) / 4).
       For an extended data   CAN frame, largest number of bits (after stuffing) = 8n + 64 + floor((54 + 8n - 1) / 4).
       For a standard  remote CAN frame, largest number of bits (after stuffing) = 50.
       For an extended remote CAN frame, largest number of bits (after stuffing) = 73 */
    #define WAIT_SEND_STANDARD_DATA_FRAME( d, b )       TIM3_Delay_us( ( 8UL * d + 44UL + ( ( 33UL + 8UL * d ) / 4UL ) ) * ( 1000000UL / b ) )
    #define WAIT_SEND_EXTENDED_DATA_FRAME( d, b )       TIM3_Delay_us( ( 8UL * d + 64UL + ( ( 53UL + 8UL * d ) / 4UL ) ) * ( 1000000UL / b ) )
    #define WAIT_SEND_STANDARD_REMOTE_FRAME( b )        TIM3_Delay_us( 50UL * ( 1000000UL / b ) )
    #define WAIT_SEND_EXTENDED_REMOTE_FRAME( b )        TIM3_Delay_us( 73UL * ( 1000000UL / b ) )

    /* MCP2515 instruction definitions */
    #define RESET_INS                                   (0xC0U)
    #define WRITE_INS                                   (0x02U)
    #define READ_INS                                    (0x03U)
    #define READ_RX_BUFFER_RXB0SIDH_INS                 (0x90U)
    #define READ_RX_BUFFER_RXB1SIDH_INS                 (0x94U)
    #define READ_RX_BUFFER_RXB0D0_INS                   (0x92U)
    #define READ_RX_BUFFER_RXB1D0_INS                   (0x96U)
    #define LOAD_TX_BUFFER_TXB0SIDH_INS                 (0x40U)
    #define LOAD_TX_BUFFER_TXB1SIDH_INS                 (0x42U)
    #define LOAD_TX_BUFFER_TXB2SIDH_INS                 (0x44U)
    #define LOAD_TX_BUFFER_TXB0D0_INS                   (0x41U)
    #define LOAD_TX_BUFFER_TXB1D0_INS                   (0x43U)
    #define LOAD_TX_BUFFER_TXB2D0_INS                   (0x45U)
    #define RTS_TXB0_INS                                (0x01U)
    #define RTS_TXB1_INS                                (0x02U)
    #define RTS_TXB2_INS                                (0x04U)
    #define RTS_TXB0_TXB1_INS                           (0x03U)
    #define RTS_TXB0_TXB2_INS                           (0x05U)
    #define RTS_TXB1_TXB2_INS                           (0x06U)
    #define RTS_TXB0_TXB1_TXB2_INS                      (0x07U)
    #define READ_STATUS_INS                             (0xA0U)
    #define RX_STATUS_INS                               (0xB0U)
    #define BIT_MODIFY_INS                              (0x05U)

    /* MCP2515 baud rates definitions */
    /* #define CAN_BAUD_1_MBPS                          (1000000U), Even though the MCP2515 supports it, sampling time falls outside the
                                                                    60-70% of the bit due to the 8MHz external oscillator on the CAN module */
    #define CAN_BAUD_500_KBPS                           (500000U)
    #define CAN_BAUD_250_KBPS                           (250000U)
    #define CAN_BAUD_125_KBPS                           (125000U)
    #define CAN_BAUD_100_KBPS                           (100000U)
    #define CAN_BAUD_50_KBPS                            (50000U)

    /* MCP2515 operation mode definitions */
    #define NORMAL_OP_MODE                              (0x00U)
    #define SLEEP_OP_MODE                               (0x01U)
    #define LOOPBACK_OP_MODE                            (0x02U)
    #define LISTEN_ONLY_OP_MODE                         (0x03U)
    #define CONFIGURATION_OP_MODE                       (0x04U)

    /* MCP2515 RX buffer operation mode definitions */
    #define RXB0_RECEIVE_VALID_MSG                      (0x00U)
    #define RXB0_TURN_MASKS_FILTERS_OFF                 (0x01U)
    #define RXB1_RECEIVE_VALID_MSG                      (0x00U)
    #define RXB1_TURN_MASKS_FILTERS_OFF                 (0x02U)

    /* MCP2515 RXB0 rollover definitions */
    #define RXB0_ROLLOVER_DISABLED                      (0x00U)
    #define RXB0_ROLLOVER_ENABLED                       (0x01U)

    /* MCP2515 RX buffer 0 rollover status definitions */
    #define ROLLOVER_OCCURRED                           (0x01U)
    #define ROLLOVER_NOT_OCCURRED                       (0x00U)

    /* MCP2515 sample point definitions */
    #define SAMPLE_POINT_ONCE                           SAM_BUS_SAMPLED_ONCE
    #define SAMPLE_POINT_THREE                          SAM_BUS_SAMPLED_THREE
    
    /* MCP2515 wake-up filter definitions */
    #define WAKE_UP_FILTER_DISABLED                     WAKFIL_DISABLED
    #define WAKE_UP_FILTER_ENABLED                      WAKFIL_ENABLED

    /* MCP2515 TX buffer number definitions */
    #define TXB0                                        (0x01U)
    #define TXB1                                        (0x02U)
    #define TXB2                                        (0x04U)

    /* MCP2515 RX buffer number definitions */
    #define RXB0                                        (0x01U)
    #define RXB1                                        (0x02U)

    /* MCP2515 TX buffer frame type definitions */
    #define TX_STANDARD_DATA_FRAME                      (0x00U)
    #define TX_EXTENDED_DATA_FRAME                      (0x01U)
    #define TX_STANDARD_REMOTE_FRAME                    (0x02U)
    #define TX_EXTENDED_REMOTE_FRAME                    (0x03U)

    /* MCP2515 RX buffer frame type definitions */
    #define RX_STANDARD_DATA_FRAME                      (0x00U)
    #define RX_EXTENDED_DATA_FRAME                      (0x01U)
    #define RX_STANDARD_REMOTE_FRAME                    (0x02U)
    #define RX_EXTENDED_REMOTE_FRAME                    (0x03U)

    /* MCP2515 one-shot mode definitions */
    #define ONE_SHOT_MSG_REATTEMPT                      OSM_DISABLED
    #define ONE_SHOT_MSG_NO_REATTEMPT                   OSM_ENABLED

    /* MCP2515 RX mask number definitions */
    #define RXM0                                        (0x01U)
    #define RXM1                                        (0x02U)

    /* MCP2515 RX filter number definitions */
    #define RXF0                                        (0x01U)
    #define RXF1                                        (0x02U)
    #define RXF2                                        (0x04U)
    #define RXF3                                        (0x08U)
    #define RXF4                                        (0x10U)
    #define RXF5                                        (0x20U)

    /* MCP2515 RX filter extended identifier definitions */
    #define RXF0_EXTENDED_ID_ENABLED                    (0x01U)
    #define RXF0_EXTENDED_ID_DISABLED                   (0x00U)
    #define RXF1_EXTENDED_ID_ENABLED                    (0x02U)
    #define RXF1_EXTENDED_ID_DISABLED                   (0x00U)
    #define RXF2_EXTENDED_ID_ENABLED                    (0x04U)
    #define RXF2_EXTENDED_ID_DISABLED                   (0x00U)
    #define RXF3_EXTENDED_ID_ENABLED                    (0x08U)
    #define RXF3_EXTENDED_ID_DISABLED                   (0x00U)
    #define RXF4_EXTENDED_ID_ENABLED                    (0x10U)
    #define RXF4_EXTENDED_ID_DISABLED                   (0x00U)
    #define RXF5_EXTENDED_ID_ENABLED                    (0x20U)
    #define RXF5_EXTENDED_ID_DISABLED                   (0x00U)

    /* MCP2515 CAN frame TX states definitions */
    #define TX_PENDING                                  (0x00U)
    #define TX_LOST_ARBITRATION                         (0x01U)
    #define TX_BUS_ERROR                                (0x02U)
    #define TX_BUS_ERROR_AND_LOST_ARBITRATION           (0x03U)
    #define TX_ABORTED                                  (0x04U)
    #define TX_SUCCESS                                  (0x05U)

    /* MCP2515 register addresses definitions */
    #define RXF0SIDH_REG                                (0x00U)
    #define RXF0SIDL_REG                                (0x01U)
    #define RXF0EID8_REG                                (0x02U)
    #define RXF0EID0_REG                                (0x03U)
    #define RXF1SIDH_REG                                (0x04U)
    #define RXF1SIDL_REG                                (0x05U)
    #define RXF1EID8_REG                                (0x06U)
    #define RXF1EID0_REG                                (0x07U)
    #define RXF2SIDH_REG                                (0x08U)
    #define RXF2SIDL_REG                                (0x09U)
    #define RXF2EID8_REG                                (0x0AU)
    #define RXF2EID0_REG                                (0x0BU)
    #define BFPCTRL_REG                                 (0x0CU)
    #define TXRTSCTRL_REG                               (0x0DU)
    #define CANSTAT_REG                                 (0x0EU)
    #define CANCTRL_REG                                 (0x0FU)
    #define RXF3SIDH_REG                                (0x10U)
    #define RXF3SIDL_REG                                (0x11U)
    #define RXF3EID8_REG                                (0x12U)
    #define RXF3EID0_REG                                (0x13U)
    #define RXF4SIDH_REG                                (0x14U)
    #define RXF4SIDL_REG                                (0x15U)
    #define RXF4EID8_REG                                (0x16U)
    #define RXF4EID0_REG                                (0x17U)
    #define RXF5SIDH_REG                                (0x18U)
    #define RXF5SIDL_REG                                (0x19U)
    #define RXF5EID8_REG                                (0x1AU)
    #define RXF5EID0_REG                                (0x1BU)
    #define TEC_REG                                     (0x1CU)
    #define REC_REG                                     (0x1DU)
    #define RXM0SIDH_REG                                (0x20U)
    #define RXM0SIDL_REG                                (0x21U)
    #define RXM0EID8_REG                                (0x22U)
    #define RXM0EID0_REG                                (0x23U)
    #define RXM1SIDH_REG                                (0x24U)
    #define RXM1SIDL_REG                                (0x25U)
    #define RXM1EID8_REG                                (0x26U)
    #define RXM1EID0_REG                                (0x27U)
    #define CNF3_REG                                    (0x28U)
    #define CNF2_REG                                    (0x29U)
    #define CNF1_REG                                    (0x2AU)
    #define CANINTE_REG                                 (0x2BU)
    #define CANINTF_REG                                 (0x2CU)
    #define EFLG_REG                                    (0x2DU)
    #define TXB0CTRL_REG                                (0x30U)
    #define TXB0SIDH_REG                                (0x31U)
    #define TXB0SIDL_REG                                (0x32U)
    #define TXB0EID8_REG                                (0x33U)
    #define TXB0EID0_REG                                (0x34U)
    #define TXB0DLC_REG                                 (0x35U)
    #define TXB0D0_REG                                  (0x36U)
    #define TXB0D1_REG                                  (0x37U)
    #define TXB0D2_REG                                  (0x38U)
    #define TXB0D3_REG                                  (0x39U)
    #define TXB0D4_REG                                  (0x3AU)
    #define TXB0D5_REG                                  (0x3BU)
    #define TXB0D6_REG                                  (0x3CU)
    #define TXB0D7_REG                                  (0x3DU)
    #define TXB1CTRL_REG                                (0x40U)
    #define TXB1SIDH_REG                                (0x41U)
    #define TXB1SIDL_REG                                (0x42U)
    #define TXB1EID8_REG                                (0x43U)
    #define TXB1EID0_REG                                (0x44U)
    #define TXB1DLC_REG                                 (0x45U)
    #define TXB1D0_REG                                  (0x46U)
    #define TXB1D1_REG                                  (0x47U)
    #define TXB1D2_REG                                  (0x48U)
    #define TXB1D3_REG                                  (0x49U)
    #define TXB1D4_REG                                  (0x4AU)
    #define TXB1D5_REG                                  (0x4BU)
    #define TXB1D6_REG                                  (0x4CU)
    #define TXB1D7_REG                                  (0x4DU)
    #define TXB2CTRL_REG                                (0x50U)
    #define TXB2SIDH_REG                                (0x51U)
    #define TXB2SIDL_REG                                (0x52U)
    #define TXB2EID8_REG                                (0x53U)
    #define TXB2EID0_REG                                (0x54U)
    #define TXB2DLC_REG                                 (0x55U)
    #define TXB2D0_REG                                  (0x56U)
    #define TXB2D1_REG                                  (0x57U)
    #define TXB2D2_REG                                  (0x58U)
    #define TXB2D3_REG                                  (0x59U)
    #define TXB2D4_REG                                  (0x5AU)
    #define TXB2D5_REG                                  (0x5BU)
    #define TXB2D6_REG                                  (0x5CU)
    #define TXB2D7_REG                                  (0x5DU)
    #define RXB0CTRL_REG                                (0x60U)
    #define RXB0SIDH_REG                                (0x61U)
    #define RXB0SIDL_REG                                (0x62U)
    #define RXB0EID8_REG                                (0x63U)
    #define RXB0EID0_REG                                (0x64U)
    #define RXB0DLC_REG                                 (0x65U)
    #define RXB0D0_REG                                  (0x66U)
    #define RXB0D1_REG                                  (0x67U)
    #define RXB0D2_REG                                  (0x68U)
    #define RXB0D3_REG                                  (0x69U)
    #define RXB0D4_REG                                  (0x6AU)
    #define RXB0D5_REG                                  (0x6BU)
    #define RXB0D6_REG                                  (0x6CU)
    #define RXB0D7_REG                                  (0x6DU)
    #define RXB1CTRL_REG                                (0x70U)
    #define RXB1SIDH_REG                                (0x71U)
    #define RXB1SIDL_REG                                (0x72U)
    #define RXB1EID8_REG                                (0x73U)
    #define RXB1EID0_REG                                (0x74U)
    #define RXB1DLC_REG                                 (0x75U)
    #define RXB1D0_REG                                  (0x76U)
    #define RXB1D1_REG                                  (0x77U)
    #define RXB1D2_REG                                  (0x78U)
    #define RXB1D3_REG                                  (0x79U)
    #define RXB1D4_REG                                  (0x7AU)
    #define RXB1D5_REG                                  (0x7BU)
    #define RXB1D6_REG                                  (0x7CU)
    #define RXB1D7_REG                                  (0x7DU)

    /* MCP2515 bit definitions for the following registers:
       - RXFnSIDH
       - RXMnSIDH
       - TXBnSIDH
       - RXBnSIDH */
    #define SID_BIT_10                                  (0x80U)
    #define SID_BIT_9                                   (0x40U)
    #define SID_BIT_8                                   (0x20U)
    #define SID_BIT_7                                   (0x10U)
    #define SID_BIT_6                                   (0x08U)
    #define SID_BIT_5                                   (0x04U)
    #define SID_BIT_4                                   (0x02U)
    #define SID_BIT_3                                   (0x01U)

    /* MCP2515 bit definitions for the following registers:
       - RXFnSIDL
       - RXMnSIDL
       - TXBnSIDL
       - RXBnSIDL */
    #define SID_BIT_2                                   (0x80U)
    #define SID_BIT_1                                   (0x40U)
    #define SID_BIT_0                                   (0x20U)
    #define SRR_RECEIVED_STANDARD_REMOTE_REQUEST        (0x10U) /* Applies only to RXBnSIDL */
    #define SRR_RECEIVED_STANDARD_DATA_FRAME            (0x00U) /* Applies only to RXBnSIDL */
    #define IDE_RECEIVED_EXTENDED_FRAME                 (0x08U) /* Applies only to RXBnSIDL */
    #define IDE_RECEIVED_STANDARD_FRAME                 (0x00U) /* Applies only to RXBnSIDL */
    #define EXIDE_MSG_TRANSMIT_EXTENDED_ID              (0x08U) /* Applies to TXBnSIDL      */
    #define EXIDE_MSG_TRANSMIT_STANDARD_ID              (0x00U) /* Applies to TXBnSIDL      */
    #define EXIDE_FILTER_APPLY_ONLY_EXTENDED_FRAMES     (0x08U) /* Applies to RXFnSIDL      */
    #define EXIDE_FILTER_APPLY_ONLY_STANDARD_FRAMES     (0x00U) /* Applies to RXFnSIDL      */
    #define EID_BIT_17                                  (0x02U)
    #define EID_BIT_16                                  (0x01U)

    /* MCP2515 bit definitions for the following registers:
       - RXFnEID8
       - RXMnEID8
       - TXBnEID8
       - RXBnEID8 */
    #define EID_BIT_15                                  (0x80U)
    #define EID_BIT_14                                  (0x40U)
    #define EID_BIT_13                                  (0x20U)
    #define EID_BIT_12                                  (0x10U)
    #define EID_BIT_11                                  (0x08U)
    #define EID_BIT_10                                  (0x04U)
    #define EID_BIT_9                                   (0x02U)
    #define EID_BIT_8                                   (0x01U)

    /* MCP2515 bit definitions for the following registers:
       - RXFnEID0
       - RXMnEID0
       - TXBnEID0
       - RXBnEID0 */
    #define EID_BIT_7                                   (0x80U)
    #define EID_BIT_6                                   (0x40U)
    #define EID_BIT_5                                   (0x20U)
    #define EID_BIT_4                                   (0x10U)
    #define EID_BIT_3                                   (0x08U)
    #define EID_BIT_2                                   (0x04U)
    #define EID_BIT_1                                   (0x02U)
    #define EID_BIT_0                                   (0x01U)

    /* MCP2515 bit definitions for the following registers:
       - TXBnDLC
       - RXBnDLC */
    #define RTR_TRANSMIT_REMOTE_FRAME_REQUEST           (0x40U) /* Applies to TXBnDLC                      */
    #define RTR_TRANSMIT_DATA_FRAME                     (0x00U) /* Applies to TXBnDLC                      */
    #define RTR_RECEIVED_REMOTE_FRAME_REQUEST           (0x40U) /* Applies to RXBnDLC                      */
    #define RTR_RECEIVED_DATA_FRAME                     (0x00U) /* Applies to RXBnDLC                      */
    #define RB_BIT_1                                    (0x20U) /* Reserved bit 1, applies only to RXBnDLC */
    #define RB_BIT_0                                    (0x10U) /* Reserved bit 0, applies only to RXBnDLC */
    #define DLC_BIT_3                                   (0x08U)
    #define DLC_BIT_2                                   (0x04U)
    #define DLC_BIT_1                                   (0x02U)
    #define DLC_BIT_0                                   (0x01U)

    /* MCP2515 bit definitions for TXBnDm registers */
    #define TXBnDm_BIT_7                                (0x80U)
    #define TXBnDm_BIT_6                                (0x40U)
    #define TXBnDm_BIT_5                                (0x20U)
    #define TXBnDm_BIT_4                                (0x10U)
    #define TXBnDm_BIT_3                                (0x08U)
    #define TXBnDm_BIT_2                                (0x04U)
    #define TXBnDm_BIT_1                                (0x02U)
    #define TXBnDm_BIT_0                                (0x01U)

    /* MCP2515 bit definitions for RXBnDm registers */
    #define RBnD_BIT_7                                  (0x80U)
    #define RBnD_BIT_6                                  (0x40U)
    #define RBnD_BIT_5                                  (0x20U)
    #define RBnD_BIT_4                                  (0x10U)
    #define RBnD_BIT_3                                  (0x08U)
    #define RBnD_BIT_2                                  (0x04U)
    #define RBnD_BIT_1                                  (0x02U)
    #define RBnD_BIT_0                                  (0x01U)

    /* MCP2515 bit definitions for TXBnCTRL registers */
    #define ABTF_MESSAGE_ABORTED                        (0x40U)
    #define ABTF_TRANSMISSION_COMPLETE                  (0x00U)
    #define MLOA_LOST_ARBITRATION                       (0x20U)
    #define MLOA_NO_LOST_ARBITRATION                    (0x00U)
    #define TXERR_BUS_ERROR                             (0x10U)
    #define TXERR_NO_BUS_ERROR                          (0x00U)
    #define TXREQ_PENDING                               (0x08U)
    #define TXREQ_NO_PENDING                            (0x00U)
    #define TXP_HIGHEST_PRIORITY                        (0x03U)
    #define TXP_HIGH_INTER_PRIORITY                     (0x02U)
    #define TXP_LOW_INTER_PRIORITY                      (0x01U)
    #define TXP_LOWEST_PRIORITY                         (0x00U)
    #define TXP_BIT_1                                   (0x02U)
    #define TXP_BIT_0                                   (0x01U)

    /* MCP2515 bit definitions for RXBnCTRL registers */
    #define RXM_RECEIVE_ANY_MESSAGE                     (0x60U)
    #define RXM_RECEIVE_ONLY_VALID_MESSAGE              (0x00U)
    #define RXM_BIT_1                                   (0x40U)
    #define RXM_BIT_0                                   (0x20U)
    #define RXRTR_REMOTE_REQUEST_RECEIVED               (0x08U)
    #define RXRTR_NO_REMOTE_REQUEST_RECEIVED            (0x00U)
    #define ROLLOVER_ACCEPTANCE_FILTER_1                (0x07U)
    #define ROLLOVER_ACCEPTANCE_FILTER_0                (0x06U)
    #define BUKT_RXB0_ROLLOVER_ENABLED                  (0x04U) /* Applies only to RXB0CTRL */
    #define BUKT_RXB0_ROLLOVER_DISABLED                 (0x00U) /* Applies only to RXB0CTRL */
    #define BUKT1_RXB0_ROLLOVER_ENABLED                 (0x02U) /* Applies only to RXB0CTRL */
    #define BUKT1_RXB0_ROLLOVER_DISABLED                (0x00U) /* Applies only to RXB0CTRL */
    #define FILHIT_ACCEPTANCE_FILTER_5                  (0x05U) /* Applies only to RXB1CTRL */
    #define FILHIT_ACCEPTANCE_FILTER_4                  (0x04U) /* Applies only to RXB1CTRL */
    #define FILHIT_ACCEPTANCE_FILTER_3                  (0x03U) /* Applies only to RXB1CTRL */
    #define FILHIT_ACCEPTANCE_FILTER_2                  (0x02U) /* Applies only to RXB1CTRL */
    #define FILHIT_ACCEPTANCE_FILTER_1                  (0x01U)
    #define FILHIT_ACCEPTANCE_FILTER_0                  (0x00U)
    #define FILHIT_BIT_2                                (0x04U) /* Applies only to RXB1CTRL */
    #define FILHIT_BIT_1                                (0x02U) /* Applies only to RXB1CTRL */
    #define FILHIT_BIT_0                                (0x01U)

    /* MCP2515 bit definitions for BFPCTRL register */
    #define B1BFS_RX1BF_DIGITAL_OUTPUT_HIGH             (0x20U)
    #define B1BFS_RX1BF_DIGITAL_OUTPUT_LOW              (0x00U)
    #define B0BFS_RX0BF_DIGITAL_OUTPUT_HIGH             (0x10U)
    #define B0BFS_RX0BF_DIGITAL_OUTPUT_LOW              (0x00U)
    #define B1BFE_RX1BF_PIN_FUNCTION_ENABLED            (0x08U)
    #define B1BFE_RX1BF_PIN_FUNCTION_DISABLED           (0x00U)
    #define B0BFE_RX0BF_PIN_FUNCTION_ENABLED            (0x04U)
    #define B0BFE_RX0BF_PIN_FUNCTION_DISABLED           (0x00U)
    #define B1BFM_RX1BF_PIN_INTERRUPT_VALID_MSG_RXB1    (0x02U)
    #define B1BFM_RX1BF_PIN_DIGITAL_OUTPUT_MODE         (0x00U)
    #define B0BFM_RX0BF_PIN_INTERRUPT_VALID_MSG_RXB0    (0x01U)
    #define B0BFM_RX0BF_PIN_DIGITAL_OUTPUT_MODE         (0x00U)

    /* MCP2515 bit definitions for TXRTSCTRL register */
    #define B2RTS_TX2RTS_DIGITAL_INPUT_HIGH             (0x20U)
    #define B2RTS_TX2RTS_DIGITAL_INPUT_LOW              (0x00U)
    #define B1RTS_TX1RTS_DIGITAL_INPUT_HIGH             (0x10U)
    #define B1RTS_TX1RTS_DIGITAL_INPUT_LOW              (0x00U)
    #define B0RTS_TX0RTX_DIGITAL_INPUT_HIGH             (0x08U)
    #define B0RTS_TX0RTX_DIGITAL_INPUT_LOW              (0x00U)
    #define B2RTSM_TX2RTS_PIN_REQUEST_TX_TXB2           (0x04U)
    #define B2RTSM_TX2RTS_PIN_DIGITAL_INPUT_MODE        (0x00U)
    #define B1RTSM_TX1RTS_PIN_REQUEST_TX_TXB1           (0x02U)
    #define B1RTSM_TX1RTS_PIN_DIGITAL_INPUT_MODE        (0x00U)
    #define B0RTSM_TX0RTS_PIN_REQUEST_TX_TXB0           (0x01U)
    #define B0RTSM_TX0RTS_PIN_DIGITAL_INPUT_MODE        (0x00U)

    /* MCP2515 bit definitions for CANCTRL register */
    #define REQOP_CONFIGURATION_MODE                    (0x80U)
    #define REQOP_LISTEN_MODE                           (0x60U)
    #define REQOP_LOOPBACK_MODE                         (0x40U)
    #define REQOP_SLEEP_MODE                            (0x20U)
    #define REQOP_NORMAL_MODE                           (0x00U)
    #define REQOP_MASK                                  (0xE0U)
    #define ABAT_REQ_ABORT_TX                           (0x10U)
    #define ABAT_TERMINATE_REQ_ABORT_TX                 (0x00U)
    #define OSM_ENABLED                                 (0x08U)
    #define OSM_DISABLED                                (0x00U)
    #define CLKEN_CLKOUT_PIN_ENABLED                    (0x04U)
    #define CLKEN_CLKOUT_PIN_DISABLED                   (0x00U)
    #define CLKPRE_SYSTEMCLK_DIV_8                      (0x03U)
    #define CLKPRE_SYSTEMCLK_DIV_4                      (0x02U)
    #define CLKPRE_SYSTEMCLK_DIV_2                      (0x01U)
    #define CLKPRE_SYSTEMCLK_NO_DIV                     (0x00U)

    /* MCP2515 bit definitions for TEC register */
    #define TEC_BIT_7                                   (0x80U)
    #define TEC_BIT_6                                   (0x40U)
    #define TEC_BIT_5                                   (0x20U)
    #define TEC_BIT_4                                   (0x10U)
    #define TEC_BIT_3                                   (0x08U)
    #define TEC_BIT_2                                   (0x04U)
    #define TEC_BIT_1                                   (0x02U)
    #define TEC_BIT_0                                   (0x01U)

    /* MCP2515 bit definitions for REC register */
    #define REC_BIT_7                                   (0x80U)
    #define REC_BIT_6                                   (0x40U)
    #define REC_BIT_5                                   (0x20U)
    #define REC_BIT_4                                   (0x10U)
    #define REC_BIT_3                                   (0x08U)
    #define REC_BIT_2                                   (0x04U)
    #define REC_BIT_1                                   (0x02U)
    #define REC_BIT_0                                   (0x01U)

    /* MCP2515 bit definitions for CNF3 register */
    #define SOF_CLKOUT_PIN_SOF                          (0x80U)
    #define SOF_CLKOUT_PIN_CLKOUT                       (0x00U)
    #define WAKFIL_ENABLED                              (0x40U)
    #define WAKFIL_DISABLED                             (0x00U)
    #define PHSEG2_2TQ                                  (0x01U)
    #define PHSEG2_3TQ                                  (0x02U)
    #define PHSEG2_4TQ                                  (0x03U)
    #define PHSEG2_5TQ                                  (0x04U)
    #define PHSEG2_6TQ                                  (0x05U)
    #define PHSEG2_7TQ                                  (0x06U)
    #define PHSEG2_8TQ                                  (0x07U)
    #define PHSEG2_BIT_2                                (0x04U)
    #define PHSEG2_BIT_1                                (0x02U)
    #define PHSEG2_BIT_0                                (0x01U)

    /* MCP2515 bit definitions for CNF2 register */
    #define BTLMODE_PS2_PHSEG2_CNF3                     (0x80U)
    #define BTLMODE_PS2_GREATER_PS1_IPT                 (0x00U)
    #define SAM_BUS_SAMPLED_THREE                       (0x40U)
    #define SAM_BUS_SAMPLED_ONCE                        (0x00U)
    #define PHSEG1_1TQ                                  (0x00U)
    #define PHSEG1_2TQ                                  (0x08U)
    #define PHSEG1_3TQ                                  (0x10U)
    #define PHSEG1_4TQ                                  (0x18U)
    #define PHSEG1_5TQ                                  (0x20U)
    #define PHSEG1_6TQ                                  (0x28U)
    #define PHSEG1_7TQ                                  (0x30U)
    #define PHSEG1_8TQ                                  (0x38U)
    #define PHSEG1_BIT_2                                (0x20U)
    #define PHSEG1_BIT_1                                (0x10U)
    #define PHSEG1_BIT_0                                (0x08U)
    #define PRSEG_1TQ                                   (0x00U)
    #define PRSEG_2TQ                                   (0x01U)
    #define PRSEG_3TQ                                   (0x02U)
    #define PRSEG_4TQ                                   (0x03U)
    #define PRSEG_5TQ                                   (0x04U)
    #define PRSEG_6TQ                                   (0x05U)
    #define PRSEG_7TQ                                   (0x06U)
    #define PRSEG_8TQ                                   (0x07U)
    #define PRSEG_BIT_2                                 (0x04U)
    #define PRSEG_BIT_1                                 (0x02U)
    #define PRSEG_BIT_0                                 (0x01U)

    /* MCP2515 bit definitions for CNF1 register */
    #define SJW_4TQ                                     (0xC0U)
    #define SJW_3TQ                                     (0x80U)
    #define SJW_2TQ                                     (0x40U)
    #define SJW_1TQ                                     (0x00U)
    #define SJW_BIT_1                                   (0x80U)
    #define SJW_BIT_0                                   (0x40U)
    #define BRP_BIT_5                                   (0x20U)
    #define BRP_BIT_4                                   (0x10U)
    #define BRP_BIT_3                                   (0x08U)
    #define BRP_BIT_2                                   (0x04U)
    #define BRP_BIT_1                                   (0x02U)
    #define BRP_BIT_0                                   (0x01U)

    /* MCP2515 bit definitions for the following registers:
       - CANINTE
       - CANINTF */
    #define MERRE_MSG_ERROR_INTERRUPT_ENABLED           (0x80U)
    #define MERRE_MSG_ERROR_INTERRUPT_DISABLED          (0x00U)
    #define WAKIE_WAKEUP_INTERRUPT_ENABLED              (0x40U)
    #define WAKIE_WAKEUP_INTERRUPT_DISABLED             (0x00U)
    #define ERRIE_ERROR_INTERRUPT_ENABLED               (0x20U)
    #define ERRIE_ERROR_INTERRUPT_DISABLED              (0x00U)
    #define TX2IE_TXB2_EMPTY_INTERRUPT_ENABLED          (0x10U)
    #define TX2IE_TXB2_EMPTY_INTERRUPT_DISABLED         (0x00U)
    #define TX1IE_TXB1_EMPTY_INTERRUPT_ENABLED          (0x08U)
    #define TX1IE_TXB1_EMPTY_INTERRUPT_DISABLED         (0x00U)
    #define TX0IE_TXB0_EMPTY_INTERRUPT_ENABLED          (0x04U)
    #define TX0IE_TXB0_EMPTY_INTERRUPT_DISABLED         (0x00U)
    #define RX1IE_RXB1_FULL_INTERRUPT_ENABLED           (0x02U)
    #define RX1IE_RXB1_FULL_INTERRUPT_DISABLED          (0x00U)
    #define RX0IE_RXB0_FULL_INTERRUPT_ENABLED           (0x01U)
    #define RX0IE_RXB0_FULL_INTERRUPT_DISABLED          (0x00U)

    /* MCP2515 bit definitions for EFLG register */
    #define RX1OVR_RXB1_OVERFLOW                        (0x80U)
    #define RX1OVR_RXB1_NO_OVERFLOW                     (0x00U)
    #define RX0OVR_RXB0_OVERFLOW                        (0x40U)
    #define RX0OVR_RXB0_NO_OVERFLOW                     (0x00U)
    #define TXB0_BUS_OFF_ERROR                          (0x20U)
    #define TXB0_NO_BUS_OFF_ERROR                       (0x00U)
    #define TXEP_TEC_GREATER_127                        (0x10U)
    #define TXEP_TEC_LESS_128                           (0x00U)
    #define RXEP_REC_GREATER_127                        (0x08U)
    #define RXEP_REC_LESS_128                           (0x00U)
    #define TXWAR_TEC_GREATER_95                        (0x04U)
    #define TXWAR_TEC_LESS_96                           (0x00U)
    #define RXWAR_REC_GREATER_95                        (0x02U)
    #define RXWAR_REC_LESS_96                           (0x00U)
    #define EWARN_TEC_OR_REC_GREATER_95                 (0x01U)
    #define EWARN_TEC_AND_REC_LESS_96                   (0x00U)

    /* Structure that holds the configuration parameters for the receiving masks:
       - RXM0 (applies to receiving buffer RXB0 only)
       - and RXM1 (applies to receiving buffer RXB1 only) */
    typedef struct
    {
        uint8_t  rxmasknmbr;       /* RX mask/s to be configured (refer to 'RX mask number definitions')     */
        uint32_t rxmaskvalue[ 2 ]; /* Mask value for RXM0 and RXM1 respectively (only the 29 LSBs are valid) */
    } CAN_Control_RX_Mask;

    /* Structure that holds the configuration parameters for the receiving filters:
       - RXF0 and RXF1 (apply only to receiving mask RXM0)
       - RXF2, RXF3, RXF4 and RXF5 (apply only to receiving mask RXM1) */
    typedef struct
    {
        uint8_t   rxfilternmbr;       /* RX filter/s to be configured (refer to 'RX filter number definitions')                          */
        uint8_t   extendedidenable;   /* RX filter/s will be applied to standard or extended frames
                                         (refer to 'RX filter extended identifier definitions')                                          */
        uint32_t  rxfiltervalue[ 6 ]; /* Filter value for RXF0, RXF1, RXF2, RXF3, RX4 and RXF5 respectively (only the 29 LSBs are valid) */
    } CAN_Control_RX_Filter;
    
    /* Structure that holds the configuration parameters and data for transmission buffers TXB0, TXB1 and TXB2 */
    typedef struct
    {	
        uint8_t  txbuffernmbr;     /* TX buffer/s to be configured (refer to 'TX buffer number definitions')                           */
        uint8_t  txframetype[ 3 ]; /* TX frame type for TXB0, TXB1 and TXB2 respectively (refer to 'TX buffer frame type definitions') */
        uint8_t  datalength[ 3 ];  /* Byte data length (DLC) for TXB0, TXB1 and TXB2 respectively (valid values: 0 to 8)               */
        uint8_t  data[ 3 ][ 8 ];   /* TX buffers data registers (aka data to be sent).
                                      - data[0][m] = TXB0Dm registers
                                      - data[1][m] = TXB1Dm registers
                                      - data[2][m] = TXB2Dm registers                                                                  */
        uint32_t txid[ 3 ];        /* Sending frame CAN ID for TXB0, TXB1 and TXB2 respectively (used in conjunction with txframetype):
                                      - Only the 29 LSBs are used if the frame is EXTENDED, remaining  3 MSBs are ignored
                                      - Only the 11 LSBs are used if the frame is STANDARD, remaining 21 MSBs are ignored              */
    } CAN_Control_TX;
    
    /* Structure that holds the configuration parameters and data for receiving buffers RXB0 and RXB1 */
    typedef struct
    {	
        uint8_t  rxbuffernmbr;     /* RX buffer/s to be configured (refer to 'RX buffer number definitions')                                  */
        uint8_t  rxframetype[ 2 ]; /* RX frame type for RXB0 and RXB1 respectively (refer to 'RX buffer frame type definitions')              */
        uint8_t  datalength[ 2 ];  /* Received frame's data length (DLC) for RXB0 and RXB1 respectively (0 to 8)                              */
        uint8_t  accfilter[ 2 ];   /* Acceptance filter that allowed the reception of the frame on RXB0
                                      and RXB1 respectively (refer to 'RX filter number definitions')                                          */
        uint8_t  rolloverstatus;   /* Indicates if a rollover from RXB0 to RXB1 occurred (refer to 'RX buffer 0 rollover status definitions') */
        uint8_t  data[ 2 ][ 8 ];   /* RX buffers data registers (aka data received).
                                      - data[0][m] = RXB0Dm registers
                                      - data[1][m] = RXB1Dm registers                                                                         */
        uint32_t rxid[ 2 ];        /* Receiving frame CAN ID for RXB0 and RXB1 (used in conjunction with rxframetype)                         */
    } CAN_Control_RX;

    /* Structure that holds the main configuration parameters for the CAN Controller (MCP2515) */
    typedef struct
    {   
        uint8_t                spi;                /* Nucleo Board's SPI peripheral to handle the MCP2515 CAN Controller
                                                      (refer to 'Nucleo Board's SPI peripheral to be used for handling the CAN controller' */
        uint8_t                opmode;             /* CAN Controller operation mode (refer to 'MCP2515 operation mode definitions')        */
        uint8_t                oneshot;            /* CAN Controller one-shot mode (refer to 'one-shot mode definitions')                  */
        uint8_t                samplepoint;        /* CAN Controller sampling point (refer to 'sample point definitions')                  */
        uint8_t                wakeupfilter;       /* CAN Controller wake-up filter mode (refer to 'wake-up filter definitions')           */
        uint8_t                rxbufferopmode;     /* RX buffer/s operation mode (refer to 'RX buffer operation mode definitions')         */
        uint8_t                rxbuffer0rollover;  /* RX buffer 0 rollover configuration (refer to 'RXB0 rollover definitions')            */
        uint32_t               baudrate;           /* CAN controller baud rate (refer to 'MCP2515 baud rates')                             */
    } CAN_Control_HandleTypeDef;

    /* MCP2515 initialization and reset functions */
    void CAN_Control_Init( CAN_Control_HandleTypeDef *hcan );
    void CAN_Control_Reset( CAN_Control_HandleTypeDef *hcan );

    /* MCP2515 operation mode and baud rate configuration functions */
    void CAN_Control_Set_Op_Mode( CAN_Control_HandleTypeDef *hcan, uint8_t opmode );
    void CAN_Control_Set_Baud_Rate( CAN_Control_HandleTypeDef *hcan, uint32_t baudrate );

    /* MCP2515 mask and filter configuration funtions */
    void CAN_Control_Set_RX_Mask( CAN_Control_HandleTypeDef *hcan, CAN_Control_RX_Mask *hmask );
    void CAN_Control_Set_RX_Filter( CAN_Control_HandleTypeDef *hcan, CAN_Control_RX_Filter *hfilter );

    /* MCP2515 register write, read and bit modify functions */
    void CAN_Control_Register_Write( CAN_Control_HandleTypeDef *hcan, uint8_t reg_addr, uint8_t *data, uint8_t size );
    void CAN_Control_Register_Read( CAN_Control_HandleTypeDef *hcan, uint8_t reg_addr, uint8_t *data, uint8_t size );
    void CAN_Control_Register_Bit( CAN_Control_HandleTypeDef *hcan, uint8_t reg_addr, uint8_t mask, uint8_t data );

    /* MCP2515 CAN frame write and read functions */
    void CAN_Control_Send_CAN_Frame( CAN_Control_HandleTypeDef *hcan, CAN_Control_TX *txcan );
    void CAN_Control_Read_CAN_Frame( CAN_Control_HandleTypeDef *hcan, CAN_Control_RX *rxcan );

    /* MCP2515 TX CAN frame status and aborting functions */
    uint8_t CAN_Control_TX_CAN_Status( CAN_Control_HandleTypeDef *hcan, uint8_t tx_buffer );
    void CAN_Control_TX_CAN_Abort( CAN_Control_HandleTypeDef *hcan, uint8_t tx_buffer );
    void CAN_Control_TX_CAN_Abort_All( CAN_Control_HandleTypeDef *hcan );

    /* MCP2515 Interrupt enabling, status and clearing functions */
    void CAN_Control_Enable_INT( CAN_Control_HandleTypeDef *hcan, uint8_t interrupts );
    uint8_t CAN_Control_INT_Status( CAN_Control_HandleTypeDef *hcan );
    void CAN_Control_Clear_INT_Status( CAN_Control_HandleTypeDef *hcan, uint8_t interrupts );
    
    /* MCP2515 Error status and clearing functions */
    uint8_t CAN_Control_ERR_Status( CAN_Control_HandleTypeDef *hcan );
    void CAN_Control_Clear_ERR_Status( CAN_Control_HandleTypeDef *hcan, uint8_t errors );

#endif
