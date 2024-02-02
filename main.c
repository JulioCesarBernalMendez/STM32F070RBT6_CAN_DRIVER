/**
 * @file      main.c
 * @author    Julio Cesar Bernal Mendez
 * 
 * @brief     This file contains examples for the implementation of the functions of the CAN controller driver (can.c).
 *            Everything was designed for the STM32F070RBT6 Nucleo Board from ST Microelectronics using bare-metal programming only.
 * 
 *            The STM32F070RB and MCP2515 connection pinout for this example is as follows:
 * 
 *                             ---------------------------------------------
 *                            |   Nucleo Board   | CAN Controller (MCP2515) |
 *                            |------------------|--------------------------|
 *                            | PA4 (SPI1_CS)    |     Controller1_CS       |
 *                            | PA5 (SPI1_SCK)   |     Controller1_SCK      |
 *                            | PA6 (SPI1_MISO)  |     Controller1_MISO     |
 *                            | PA7 (SPI1_MOSI)  |     Controller1_MOSI     |
 *                            |                  |                          |
 *                            | PB12 (SPI2_CS)   |     Controller2_CS       |
 *                            | PB13 (SPI2_SCK)  |     Controller2_SCK      |
 *                            | PB14 (SPI2_MISO) |     Controller2_MISO     |
 *                            | PB15 (SPI2_MOSI) |     Controller2_MOSI     |
 *                             ---------------------------------------------
 * 
 *            Note: code in this file is tested using a debugger for setting breakpoints and reading
 *                  different variables, also a logic analyzer is needed for visualizing the data sent
 *                  and received on the MCP2515 SPI and CAN lines.
 *                  Comments above each code line provides further information.
 *               
 * @version   1.0
 * @date      2024-01-26
 * 
 * @copyright This project was created for learning purposes only.
 */

#include "main.h"

/* System clock frequency global variable (system_stm32f0xx.c) */
extern uint32_t SystemCoreClock;

/**
 * @brief  Main function used for CAN controller driver testing
 */
int main( void )
{  
   /* Configuration of CAN Controllers (MCP2515) #1 and #2 */
   CAN_Control_HandleTypeDef CAN1_Handler = { 0U };
   CAN_Control_HandleTypeDef CAN2_Handler = { 0U };

   /* Mask and filter values of MCP2515 #2 */
   CAN_Control_RX_Mask   CAN2_Masks   = { 0U };
   CAN_Control_RX_Filter CAN2_Filters = { 0U };

   /* TX and RX buffer values (ID, data and more, refer to the structure definitions in can.h)
      of MCP2515 #1 and #2 respectively */
   CAN_Control_TX CAN1_TX = { 0U };
   CAN_Control_RX CAN2_RX = { 0U };

   /* TX statuses of TXB0, TXB1 and TXB2 respectively for MCP2515 #1 */
   uint8_t tx_status[ 3 ] = { 0U };

   /* Interrupt flags for MCP2515 #1 and #2 respectively */
   uint8_t int_status[ 2 ] = { 0U };

   /* uint8_t Error flags for MCP2515 #1 */
   uint8_t err_status = 0U;

   /* Transmission Error Counter (TEC) and Receive Error Counter (REC) of the MCP2515 #1 */
   uint8_t tec = 0U;
   uint8_t rec = 0U;

   /* Milliseconds variable used for testing */
   uint16_t ms;

   /* Update SystemCoreClock global variable (should be read as 48000000 after this function's execution) */
   SystemCoreClockUpdate();

   /* Initialize TIM3 peripheral for debugging purposes (0.5us time base) */
   TIM3_Init();

   /* 3 secs delay for debugging purposes */
   for ( ms = 0U; ms < 3000U; ms++ )
   {  
      /* 1ms delay */
      TIM3_Delay_us( 1000U );
   }

   /* Initialize Nucleo board's user button */
   Board_Button_Init();

   /* Initialize CAN controller MCP2515 #1 (uses SPI1, see pinout at the top of this file) */
   CAN1_Handler.spi               = CAN_SPI1;
   CAN1_Handler.baudrate          = CAN_BAUD_125_KBPS;
   CAN1_Handler.oneshot           = ONE_SHOT_MSG_REATTEMPT;
   CAN1_Handler.samplepoint       = SAMPLE_POINT_ONCE;
   CAN1_Handler.wakeupfilter      = WAKE_UP_FILTER_DISABLED;
   CAN1_Handler.rxbufferopmode    = RXB0_RECEIVE_VALID_MSG | RXB1_TURN_MASKS_FILTERS_OFF;
   CAN1_Handler.rxbuffer0rollover = RXB0_ROLLOVER_DISABLED;
   CAN1_Handler.opmode            = NORMAL_OP_MODE;
   CAN_Control_Init( &CAN1_Handler );

   /* Initialize CAN controller MCP2515 #2 (uses SPI2, see pinout at the top of this file) */
   CAN2_Handler.spi               = CAN_SPI2;
   CAN2_Handler.baudrate          = CAN_BAUD_125_KBPS;
   CAN2_Handler.oneshot           = ONE_SHOT_MSG_REATTEMPT;
   CAN2_Handler.samplepoint       = SAMPLE_POINT_ONCE;
   CAN2_Handler.wakeupfilter      = WAKE_UP_FILTER_DISABLED;
   CAN2_Handler.rxbufferopmode    = RXB0_RECEIVE_VALID_MSG | RXB1_RECEIVE_VALID_MSG;
   CAN2_Handler.rxbuffer0rollover = RXB0_ROLLOVER_DISABLED;
   CAN2_Handler.opmode            = NORMAL_OP_MODE;
   CAN_Control_Init( &CAN2_Handler );

   /* Set MCP2515 #2 into configuration mode in order to write to its mask and filter registers */
   CAN_Control_Set_Op_Mode( &CAN2_Handler, CONFIGURATION_OP_MODE );

   /* Set all the 11 bits (standard ID field) in the RXM0 mask (RX buffer 0)
      and set all the 29 bit (standard ID and extended ID fields) in the RXM1 mask (RX buffer 1)
      (this means that receiving CAN frames must match the value provided in the configured filter 0 or 1 (for RX buffer 0)
      or filter 2 or 3 or 4 or 5 (for RX buffer 1) to be accepted).
      Also take into consideration the operation mode used for the RX buffer mode (see CAN2_Handler.rxbufferopmode) */
   CAN2_Masks.rxmasknmbr = RXM0 | RXM1;        /* masks to configure */
   CAN2_Masks.rxmaskvalue[ 0 ] = 0x1FFC0000UL; /* mask value for RXM0 (applies to RXB0 receiving buffer) */
   CAN2_Masks.rxmaskvalue[ 1 ] = 0x1FFFFFFFUL; /* mask value for RXM1 (applies to RXB1 receiving buffer) */
   CAN_Control_Set_RX_Mask( &CAN2_Handler, &CAN2_Masks );

   /* Set filter 0 (applies to RXB0) and filter 2 (applies to RXB1) values. In this case due to the configurations of the RXB0 and RXB1 modes and their masks values,
      both registers will only accept frames with IDs whose values match those of the filters, more specifically: 
      - Filter 0 or 1 apply to RX buffer 0, for this only CAN frames with a standard ID of (filter 0) 0x555 (and 'undetermined' ID due to filter 1 not being set)
        are accepted by RX buffer 0.
      - Filter 2 or 3 or 4 or 5 apply to RX buffer 1, for this only CAN frames with a complete ID (filter 2) of 0x1D0CAFC8 (and 3 more 'undetermined' IDs due
        to filter 3, 4 and 5 not being set) are accepted by RX buffer 1 */
   CAN2_Filters.rxfilternmbr       = RXF0 | RXF2;  /* Filters to configure */
   CAN2_Filters.rxfiltervalue[ 0 ] = 0x15540000UL; /* RXF0: Standard ID = 0x555 */
   CAN2_Filters.rxfiltervalue[ 2 ] = 0x1D0CAFC8UL; /* RXF2: Complete ID: 0x1D0CAFC8, standard ID = 0x743, extended ID = 0xAFC8 */
   CAN2_Filters.extendedidenable   = RXF0_EXTENDED_ID_DISABLED | RXF2_EXTENDED_ID_ENABLED; /* Filter 0 applied to standard CAN frames, Filter 2 applied to extended CAN frames */
   CAN_Control_Set_RX_Filter( &CAN2_Handler, &CAN2_Filters );

   /* Set MCP2515 #2 back to normal mode */
   CAN_Control_Set_Op_Mode( &CAN2_Handler, NORMAL_OP_MODE );

   /* Enable TXB0 buffer empty (TXB0 msg sent successfully) interrupt on MCP2515 #1
      and RXB0 buffer full (RXB0 msg received) interrupt on MCP2515 #2 */
   CAN_Control_Enable_INT( &CAN1_Handler, TX0IE_TXB0_EMPTY_INTERRUPT_ENABLED );
   CAN_Control_Enable_INT( &CAN2_Handler, RX0IE_RXB0_FULL_INTERRUPT_ENABLED );

   /* Enable CAN frame sending of TX buffers 0, 1 and 2 for the MCP2515 #1, specify for each:
      frame type, IDs, data length code and data to be sent */
   CAN1_TX.txbuffernmbr    = TXB0 | TXB1 | TXB2;        /* Request CAN transmission for all the TX buffers (0, 1 and 2) of the MCP2515 */
   /* TXB0 */
   CAN1_TX.txframetype[ 0 ] = TX_STANDARD_DATA_FRAME;   /* TXB0 is Standard Data frame */
   CAN1_TX.txid[ 0 ]        = 0x555UL;                  /* TXB0 Standard ID (matches standard ID from filter 0, RXB0 is expected to receive stand. frames with this ID) */
   CAN1_TX.datalength[ 0 ]  = 2U;                       /* TXB0 DLC = 2 bytes */
   CAN1_TX.data[ 0 ][ 0 ]   = 0x0DU;                    /* TXB0 data byte 0 */
   CAN1_TX.data[ 0 ][ 1 ]   = 0xD0U;                    /* TXB1 data byte 1 */
   /* TXB1 */
   CAN1_TX.txframetype[ 1 ] = TX_EXTENDED_DATA_FRAME;   /* TXB1 is Extended Data frame */
   CAN1_TX.txid[ 1 ]        = 0x1D0CAFC8UL;             /* TXB1 Extended ID (matches ID configured in filter 2, RXB1 is expected to received exten. frames with this ID) */
   CAN1_TX.datalength[ 1 ]  = 5U;                       /* TXB1 DLC = 5 bytes */
   CAN1_TX.data[ 1 ][ 0 ]   = 0x01U;                    /* TXB1 data byte 0 */
   CAN1_TX.data[ 1 ][ 1 ]   = 0x02U;                    /* TXB1 data byte 1 */
   CAN1_TX.data[ 1 ][ 2 ]   = 0x03U;                    /* TXB1 data byte 2 */
   CAN1_TX.data[ 1 ][ 3 ]   = 0x04U;                    /* TXB1 data byte 3 */
   CAN1_TX.data[ 1 ][ 4 ]   = 0x05U;                    /* TXB1 data byte 4 */
   /* TXB2 */
   CAN1_TX.txframetype[ 2 ] = TX_EXTENDED_REMOTE_FRAME; /* TXB2 is Extended Remote frame */
   CAN1_TX.txid[ 2 ]       = 0x34DUL;                   /* TXB2 Extended ID, no filter matches this value, neither RXB0 or RXB1 should accept frames with this ID */
   CAN1_TX.datalength[ 2 ] = 8U;                        /* TXB2 DLC = 8 bytes. NOTE: a remote frame does not contain a Data Field */
   CAN_Control_Send_CAN_Frame( &CAN1_Handler, &CAN1_TX );

   /* Read TX status of TX buffers 0, 1 and 2 of the MCP2515 #1 */
   tx_status[ 0 ] = CAN_Control_TX_CAN_Status( &CAN1_Handler, TXB0 ); /* should be read TX_SUCCESS (0x05) */
   tx_status[ 1 ] = CAN_Control_TX_CAN_Status( &CAN1_Handler, TXB1 ); /* should be read TX_SUCCESS (0x05) */
   tx_status[ 2 ] = CAN_Control_TX_CAN_Status( &CAN1_Handler, TXB2 ); /* should be read TX_SUCCESS (0x05) */

   /* Read interrupt flags of both MCP2515 #1 and MCP2515 #2 */
   int_status[ 0 ] = CAN_Control_INT_Status( &CAN1_Handler ); /* should be TX2IF = 1, TX1IF = 1 and TX0IF = 1 since successful CAN frames sent */
   int_status[ 1 ] = CAN_Control_INT_Status( &CAN2_Handler ); /* should be RX1IF = 1 and RX0IF = 1 since successful CAN frames received */

   /* Read any CAN frames received on RXB0 and RXB1 buffers of the MCP2515 #2 */
   CAN2_RX.rxbuffernmbr = RXB0 | RXB1;
   CAN_Control_Read_CAN_Frame( &CAN2_Handler, &CAN2_RX );

   /* Disable TXB0 buffer empty (TXB0 msg sent successfully) interrupt on MCP2515 #1 */
   CAN_Control_Enable_INT( &CAN1_Handler, TX0IE_TXB0_EMPTY_INTERRUPT_DISABLED );

   /* Read interrupt flags of MCP2515 #1 again */
   int_status[ 0 ] = CAN_Control_INT_Status( &CAN1_Handler ); /* should be TX2IF = 1, TX1IF = 1 and TX0IF = 1 since 
                                                                 disabling interrupts does not clear any flag */

   /* Clear Interrupt flags on both MCP2515 #1 and #2 */
   CAN_Control_Clear_INT_Status( &CAN1_Handler, TX0IE_TXB0_EMPTY_INTERRUPT_ENABLED );
   CAN_Control_Clear_INT_Status( &CAN2_Handler, RX0IE_RXB0_FULL_INTERRUPT_ENABLED );

   /* Enable msg error interrupt on MCP2515 #1 */
   CAN_Control_Enable_INT( &CAN1_Handler, ERRIE_ERROR_INTERRUPT_ENABLED );

   /* *** Reminder: up to this point interrupts enabled are:
          - msg error (transmission or reception) interrupt on MCP2515 #1
          - RXB0 buffer full (RXB0 msg received) interrupt on MCP2515 #2 *** */

   /* Attempt to send the CAN Frame on MCP2515 #1 TXB0
      Note: before executing this, shortcircuit the CAN bus in order to intentionally generate an error */
   CAN_Control_Register_Bit( &CAN1_Handler, TXB0CTRL_REG, TXREQ_PENDING, TXREQ_PENDING );

   /* Read TX status of TX buffer 0 on the MCP2515 #1 */
   tx_status[ 0 ] = CAN_Control_TX_CAN_Status( &CAN1_Handler, TXB0 ); /* should be read TX_BUS_ERROR (0x02) due to shortcircuit */

   /* Read MCP2515 #1 Errors
      Note: before executing this, undo the bus shortcircuit to prevent the MCP2515 to go from bus-off to active-error (automatically)
            to passive-error (due to shortcircuit) to bus-off (due to shortcircuit) and so on...
            Undoing the shortcircuit when the MCP2515 is at error-active or error-passive will cause the TXB0 CAN frame from MCP2515 #1
            to be sent successfully (due to one-shot disabled and msgs reattempt transmission).

      There are 3 possibilites:
      - MCP2515 is in bus-off state (TXBO = 1). It will automatically recover back to error-active state.
      - MCP2515 is in error-passive state (TXEP = 1 and TXBO = 0). If bus error is not corrected it will eventually go to bus-off.
      - MCP2515 is in error-active state. (TXEP = 0 and TXBO = 0). If bus error is not corrected it will eventually go to passive-error */
   err_status = CAN_Control_ERR_Status( &CAN1_Handler );

   /* Read TX status of TX buffer 0 on the MCP2515 #1 */
   tx_status[ 0 ] = CAN_Control_TX_CAN_Status( &CAN1_Handler, TXB0 ); /* should be read TX_SUCCESS (0x05) */

   /* Continue only if shortcirtuit was undone before executing previous line and TXEP = 1 in err_status,
      the idea is the MCP2515 to be in passive-error state, unless error counters TEC and REC are cleared */

   while( 1 )
   {  
      /* Read both MCP2515 #1 TEC and REC counters.
         Their values should be fixed since the MCP2515 is stuck at passive-error state
         and there are no more errors on the bus */
      CAN_Control_Register_Read( &CAN1_Handler, TEC_REG, &tec, 1U );
      CAN_Control_Register_Read( &CAN1_Handler, REC_REG, &rec, 1U );

      /* If user button on the Nucleo board is pressed */
      if ( ( GPIOC->IDR & GPIO_IDR_13 ) == 0U )
      {
         /* Set MCP2515 #1 into configuration mode in order to clear
            TEC and REC error counters and return to active-error state */
         CAN_Control_Set_Op_Mode( &CAN1_Handler, CONFIGURATION_OP_MODE );
         
         /* Set MCP2515 #1 back to normal operation mode */
         CAN_Control_Set_Op_Mode( &CAN1_Handler, NORMAL_OP_MODE );

         /* Read MCP2515 #1 Errors */
         err_status = CAN_Control_ERR_Status( &CAN1_Handler ); /* no more errors */
      }

      /* 50 ms delay */
      TIM3_Delay_us( 50000 );
   }

   return 0;
}

/**
 * @brief Initialize the Nucleo board's user button as digital input
 * 
 */
void Board_Button_Init( void )
{  
   /* Enable GPIOC clock access */
   RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

   /* Enable GPIOC13 as digital input */
   GPIOC->MODER &= ~GPIO_MODER_MODER13_1;
   GPIOC->MODER &= ~GPIO_MODER_MODER13_0;
}
