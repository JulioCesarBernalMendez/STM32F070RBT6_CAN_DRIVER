/**
 * @file      can.c
 * @author    Julio Cesar Bernal Mendez
 * 
 * @brief     This file contains the function implementations for the CAN Controller Module
 *            which includes one MCP2515 CAN Controller Driver and one TJA1050 CAN Transceiver.
 *            Everything was designed for the STM32F070RBT6 Nucleo Board from ST Microelectronics.
 * 
 * @version   1.0
 * @date      2024-01-26
 * 
 * @copyright This project was created for learning purposes only.
 */

#include "can.h"

/**
 * @brief Initialize the MCP2515 CAN Controller Driver according to the parameters provided in the CAN_Control_HandleTypeDef:
 *        - reset the CAN module registers to their default values (refer to datasheet)
 *        - set the specified CAN baud rate
 *        - turn masks/filters ON or OFF
 *        - either enable or disable RX buffer 0 rollover
 *        - either enable or disable one-shot mode
 *        - and force the device to work at the defined operation mode.
 * 
 *        Use the specified SPI1 or SPI2 on the Nucleo Board for both reading and writing to the MCP2515
 *        (refer to spi.c for the Nucleo Board's SPI1 and SPI2 pinout).
 * 
 * @param hcan pointer to an MCP2515 configuration structure (CAN module to be initialized).
 *             Refer to the CAN_Control_HandleTypeDef structure definition in can.h for its possible values
 */
void CAN_Control_Init( CAN_Control_HandleTypeDef *hcan )
{
    uint8_t spi_write = 0U; /* RXB0CTRL and RXB1CTRL register */

    /* Make sure the SPI instance selected to control the MCP2515 is valid */
    if ( ( hcan->spi == CAN_SPI1 ) || ( hcan->spi == CAN_SPI2 ) )
    {
        /* Initialize selected SPI peripheral (6MHz) */
        ( hcan->spi == CAN_SPI1 ) ? SPI1_Init() : SPI2_Init();
        
        /* Reset MCP2515 */
        CAN_Control_Reset( hcan );

        /* Set CAN baud rate, sample point configuration and wake-up filter selected by user */
        CAN_Control_Set_Baud_Rate( hcan, hcan->baudrate );
        
        /* If RX buffer 0 will ignore masks and filters (i.e. accept any CAN frame) */
        if ( ( hcan->rxbufferopmode & RXB0_TURN_MASKS_FILTERS_OFF ) == RXB0_TURN_MASKS_FILTERS_OFF )
        {   
            /* Set RXM bits in RXB0CTRL register so that RXB0 accepts any CAN frame */
            spi_write |= RXM_RECEIVE_ANY_MESSAGE;
        }
        /* ... otherwise RX buffer 0 will only receive CAN frames that meet the masks and filter criteria */

        /* If RX buffer 0 rollover is enabled */
        if ( ( hcan->rxbuffer0rollover & RXB0_ROLLOVER_ENABLED ) == RXB0_ROLLOVER_ENABLED )
        {
            /* Set BUKT bit in RXB0CTRL register */
            spi_write |= BUKT_RXB0_ROLLOVER_ENABLED;
        }

        /* if value to be written to RXB0CTRL register is other than 0
           (either RX buffer 0 ignores masks and filters or RX buffer 0 rollover is enabled ) */
        if ( spi_write != 0U )
        {
            /* Write so to RXB0CTRL */
            CAN_Control_Register_Write( hcan, RXB0CTRL_REG, &spi_write, 1U );
        }

        /* If RX buffer 1 will ignore masks and filters (i.e. accept any CAN frame) */
        if ( ( hcan->rxbufferopmode & RXB1_TURN_MASKS_FILTERS_OFF ) == RXB1_TURN_MASKS_FILTERS_OFF )
        {
            /* Set RXM bits in RXB1CTRL register so that RXB1 accepts any CAN frame */
            spi_write = RXM_RECEIVE_ANY_MESSAGE;
            CAN_Control_Register_Write( hcan, RXB1CTRL_REG, &spi_write, 1U );
        }
        /* ... otherwise RX buffer 1 will only receive CAN frames that meet the masks and filter criteria */

        /* Set CAN controller operation and one-shot modes selected by user */
        CAN_Control_Set_Op_Mode( hcan, hcan->opmode );
    }
}

/**
 * @brief Bring the MCP2515 to its reset state via the SPI command.
 *        This sets the internal registers to their default values (refer to datasheet)
 *        and puts the device into configuration operation mode.
 * 
 * @param hcan pointer to an MCP2515 configuration structure (CAN module to be reset)
 */
void CAN_Control_Reset( CAN_Control_HandleTypeDef *hcan )
{
    uint8_t instruction = RESET_INS;
    uint8_t ost = GET_OST( OSC1_FREQ );

    /* If SPI1 peripheral handles the CAN controller */
    if ( ( hcan->spi == CAN_SPI1 ) )
    {
        /* Send RESET instruction to the CAN controller connected to SPI1.
           Reset registers to the default state and set MCP2515 to configuration mode */
        SPI1_CS_Enable();
        SPI1_Write( &instruction, 1U );
        SPI1_CS_Disable();
    }
    /* If SPI2 peripheral handles the CAN controller */
    else if ( ( hcan->spi == CAN_SPI2 ) )
    {
        /* Send RESET instruction to the CAN controller connected to SPI2.
           Reset registers to the default state and set MCP2515 to configuration mode*/
        SPI2_CS_Enable();
        SPI2_Write( &instruction, 1U );
        SPI2_CS_Disable();
    }
    else
    {
        /* Do nothing */
    }

    /* Wait 50us for the instruction to be processed by the MCP2515 (time is not specified in datasheet) */
    TIM3_Delay_us( 50U );

    /* MCP2515 must wait for an OST period for the oscillator to stabilize */
    TIM3_Delay_us( ost );
}

/**
 * @brief Set the MCP2515 to the specified operation mode.
 * 
 * @param hcan   pointer to an MCP2515 configuration structure (CAN module to be configured)
 * @param opmode MCP2515 operation mode. Refer to 'MCP2515 operation mode definitions' in can.h
 */
void CAN_Control_Set_Op_Mode( CAN_Control_HandleTypeDef *hcan, uint8_t opmode )
{
    /* get user's oneshot configuration selected */
    uint8_t spi_write = hcan->oneshot; /* CANCTRL register */

    /* Determine operation mode to be set */
    switch ( opmode )
    {
        case NORMAL_OP_MODE:
            /* Set normal operation mode and user's one shot configuration by writting to CANCTRL in the MCP2515 */
            spi_write |= REQOP_NORMAL_MODE;
            CAN_Control_Register_Write( hcan, CANCTRL_REG, &spi_write, 1U );
            break;

        case SLEEP_OP_MODE:
            /* Set sleep operation mode and user's one shot configuration by writting to CANCTRL in the MCP2515 */
            spi_write |= REQOP_SLEEP_MODE;
            CAN_Control_Register_Write( hcan, CANCTRL_REG, &spi_write, 1U );
            break;
        
        case LOOPBACK_OP_MODE:
            /* Set loopback operation mode and user's one shot configuration by writting to CANCTRL in the MCP2515 */
            spi_write |= REQOP_LOOPBACK_MODE;
            CAN_Control_Register_Write( hcan, CANCTRL_REG, &spi_write, 1U );
            break;

        case LISTEN_ONLY_OP_MODE:
            /* Set listen-only operation mode and user's one shot configuration by writting to CANCTRL in the MCP2515 */
            spi_write |= REQOP_LISTEN_MODE;
            CAN_Control_Register_Write( hcan, CANCTRL_REG, &spi_write, 1U );
            break;

        case CONFIGURATION_OP_MODE:
            /* Set configuration operation mode and user's one shot configuration by writting to CANCTRL in the MCP2515 */
            spi_write |= REQOP_CONFIGURATION_MODE;
            CAN_Control_Register_Write( hcan, CANCTRL_REG, &spi_write, 1U );
            break;

        default:
            /* Wrong operation mode, do not do anything */
            break;
    }
}

/**
 * @brief Configure the MCP2515 CAN baud rate by updatting its bit timing registers.
 *        Bit timing details can be found in the comments below each case.
 * 
 *        Note: MCP2515 must be set to configuration operation mode before calling this function
 *              in order to be able to write to the configuration registers CNF3, CNF2 and CNF1.
 * 
 * @param hcan     pointer to an MCP2515 configuration structure (CAN module to be configured)
 * @param baudrate MCP2515 CAN baud rate. Refer to 'MCP2515 baud rates definitions' in can.h
 */
void CAN_Control_Set_Baud_Rate( CAN_Control_HandleTypeDef *hcan, uint32_t baudrate )
{
    uint8_t spi_write[ 3 ];

    /* Set CAN baud rate by writting to the bit timing registers of the MCP2515 */
    switch ( baudrate )
    {
        case CAN_BAUD_500_KBPS:
            /* BRP     = 0
               TQ      = 2(BRP + 1)/fosc = 2/8MHz = 250ns
               SyncSeg = 1TQ
               PropSeg = 2TQ
               PS1     = 2TQ
               PS2     = 3TQ
               SJW     = 1TQ
               Sampling point = 62.5% */
            spi_write[ 0 ] = hcan->wakeupfilter | PHSEG2_3TQ;                                      /* CNF3 register */
            spi_write[ 1 ] = BTLMODE_PS2_PHSEG2_CNF3 | hcan->samplepoint | PHSEG1_2TQ | PRSEG_2TQ; /* CNF2 register */
            spi_write[ 2 ] = SJW_1TQ;                                                              /* CNF1 register */
            CAN_Control_Register_Write( hcan, CNF3_REG, spi_write, 3U );
            break;

        case CAN_BAUD_250_KBPS:
            /* BRP     = 0
               TQ      = 2(BRP + 1)/fosc = 2/8MHz = 250ns
               SyncSeg = 1TQ
               PropSeg = 4TQ
               PS1     = 5TQ
               PS2     = 6TQ
               SJW     = 1TQ
               Sampling point = 62.5% */
            spi_write[ 0 ] = hcan->wakeupfilter | PHSEG2_6TQ;                                      /* CNF3 register */
            spi_write[ 1 ] = BTLMODE_PS2_PHSEG2_CNF3 | hcan->samplepoint | PHSEG1_5TQ | PRSEG_4TQ; /* CNF2 register */
            spi_write[ 2 ] = SJW_1TQ;                                                              /* CNF1 register */
            CAN_Control_Register_Write( hcan, CNF3_REG, spi_write, 3U );
            break;

        case CAN_BAUD_125_KBPS:
            /* BRP     = 1
               TQ      = 2(BRP + 1)/fosc = 4/8MHz = 500ns
               SyncSeg = 1TQ
               PropSeg = 3TQ
               PS1     = 6TQ
               PS2     = 6TQ
               SJW     = 1TQ
               Sampling point = 62.5% */
            spi_write[ 0 ] = hcan->wakeupfilter | PHSEG2_6TQ;                                      /* CNF3 register */
            spi_write[ 1 ] = BTLMODE_PS2_PHSEG2_CNF3 | hcan->samplepoint | PHSEG1_6TQ | PRSEG_3TQ; /* CNF2 register */
            spi_write[ 2 ] = SJW_1TQ | BRP_BIT_0;                                                  /* CNF1 register */
            CAN_Control_Register_Write( hcan, CNF3_REG, spi_write, 3U );
            break;

        case CAN_BAUD_100_KBPS:
            /* BRP     = 1
               TQ      = 2(BRP + 1)/fosc = 4/8MHz = 500ns
               SyncSeg = 1TQ
               PropSeg = 6TQ
               PS1     = 6TQ
               PS2     = 7TQ
               SJW     = 1TQ
               Sampling point = 65% */
            spi_write[ 0 ] = hcan->wakeupfilter | PHSEG2_7TQ;                                      /* CNF3 register */
            spi_write[ 1 ] = BTLMODE_PS2_PHSEG2_CNF3 | hcan->samplepoint | PHSEG1_6TQ | PRSEG_6TQ; /* CNF2 register */
            spi_write[ 2 ] = SJW_1TQ | BRP_BIT_0;                                                  /* CNF1 register */
            CAN_Control_Register_Write( hcan, CNF3_REG, spi_write, 3U );
            break;

        case CAN_BAUD_50_KBPS:
            /* BRP     = 3
               TQ      = 2(BRP + 1)/fosc = 8/8MHz = 1us
               SyncSeg = 1TQ
               PropSeg = 6TQ
               PS1     = 6TQ
               PS2     = 7TQ
               SJW     = 1TQ
               Sampling point = 65% */
            spi_write[ 0 ] = hcan->wakeupfilter | PHSEG2_7TQ;                                      /* CNF3 register */
            spi_write[ 1 ] = BTLMODE_PS2_PHSEG2_CNF3 | hcan->samplepoint | PHSEG1_6TQ | PRSEG_6TQ; /* CNF2 register */
            spi_write[ 2 ] = SJW_1TQ | BRP_BIT_1 | BRP_BIT_0;                                      /* CNF1 register */
            CAN_Control_Register_Write( hcan, CNF3_REG, spi_write, 3U );
            break;

        default:
            /* Wrong baud rate selected */
            break;
    }
}

/**
 * @brief Set the receiving mask ID values of the selected RX masks in the CAN_Control_RX_Mask
 *        for the MCP2515 specified by CAN_Control_HandleTypeDef.
 *        This writes to the mask registers: RXMnSIDH, RXMnSIDL, RXMnEID8 and RXMnEID0.
 * 
 *        Note: mask registers are only modifiable when the MCP2515 is in configuration mode, it is advised then,
 *              to set it to the proper operation mode before and after calling this function.
 * 
 * @param hcan  pointer to an MCP2515 configuration structure (CAN module to be configured)
 * @param hmask pointer to a receiving mask configuration structure.
 *              Refer to the CAN_Control_RX_Mask structure definition in the can.h file for its possible values
 */
void CAN_Control_Set_RX_Mask( CAN_Control_HandleTypeDef *hcan, CAN_Control_RX_Mask *hmask )
{
    uint8_t spi_write[ 4 ];

    /* If mask 0 (applies to RX buffer 0) is selected for configuration */
    if ( ( hmask->rxmasknmbr & RXM0 ) == RXM0 )
    {
        /* Get the mask 0 register values */
        spi_write[ 0 ]  =   hmask->rxmaskvalue[ 0 ] >> 21;           /* RXM0SIDH.SID[10:3]  */
        spi_write[ 1 ]  = ( hmask->rxmaskvalue[ 0 ] >> 13 ) & 0xE0U; /* RXM0SIDL.SID[2:0]   */
        spi_write[ 1 ] |= ( hmask->rxmaskvalue[ 0 ] >> 16 ) & 0x03U; /* RXM0SIDL.EID[17:16] */
        spi_write[ 2 ]  =   hmask->rxmaskvalue[ 0 ] >> 8;            /* RXM0EID8.EID[15:8]  */
        spi_write[ 3 ]  =   hmask->rxmaskvalue[ 0 ];                 /* RXM0EID0.EID[7:0]   */

        /* Write the mask values to the mask 0 registers */
        CAN_Control_Register_Write( hcan, RXM0SIDH_REG, spi_write, 4U );
    }

    /* If mask 1 (applies to RX buffer 1) is selected for configuration */
    if ( ( hmask->rxmasknmbr & RXM1 ) == RXM1 )
    {
        /* Get the mask 1 register values */
        spi_write[ 0 ]  =   hmask->rxmaskvalue[ 1 ] >> 21;           /* RXM1SIDH.SID[10:3]  */
        spi_write[ 1 ]  = ( hmask->rxmaskvalue[ 1 ] >> 13 ) & 0xE0U; /* RXM1SIDL.SID[2:0]   */
        spi_write[ 1 ] |= ( hmask->rxmaskvalue[ 1 ] >> 16 ) & 0x03U; /* RXM1SIDL.EID[17:16] */
        spi_write[ 2 ]  =   hmask->rxmaskvalue[ 1 ] >> 8;            /* RXM1EID8.EID[15:8]  */
        spi_write[ 3 ]  =   hmask->rxmaskvalue[ 1 ];                 /* RXM1EID0.EID[7:0]   */

        /* Write the mask values to the mask 1 registers */
        CAN_Control_Register_Write( hcan, RXM1SIDH_REG, spi_write, 4U );
    }
}

/**
 * @brief Set the receiving filter ID values for the selected RX filters in the CAN_Control_RX_Filter
 *        for the MCP2515 specified by CAN_Control_HandleTypeDef.
 *        This writes to the filter registers: RXFnSIDH, RXFnSIDL, RXFnEID8 and RXFnEID0.
 * 
 *        Note: filter registers are only modifiable when the MCP2515 is in configuration mode, it is advised then,
 *              to set it to the proper operation mode before and after calling this function.
 * 
 * @param hcan    pointer to an MCP2515 configuration structure (CAN module to be configured)
 * @param hfilter pointer to a receiving filter configuration structure.
 *                Refer to the CAN_Control_RX_Filter structure definition in can.h for its possible values
 */
void CAN_Control_Set_RX_Filter( CAN_Control_HandleTypeDef *hcan, CAN_Control_RX_Filter *hfilter )
{
    uint8_t spi_write[ 4 ];
    
    /* If filter 0 is selected for configuration */
    if ( ( hfilter->rxfilternmbr & RXF0 ) == RXF0 )
    {
        /* Get the filter 0 register values */
        spi_write[ 0 ]  =   hfilter->rxfiltervalue[ 0 ] >> 21;           /* RXF0SIDH.SID[10:3]  */
        spi_write[ 1 ]  = ( hfilter->rxfiltervalue[ 0 ] >> 13 ) & 0xE0U; /* RXF0SIDL.SID[2:0]   */
        spi_write[ 1 ] |= ( hfilter->rxfiltervalue[ 0 ] >> 16 ) & 0x03U; /* RXF0SIDL.EID[17:16] */
        spi_write[ 2 ]  =   hfilter->rxfiltervalue[ 0 ] >> 8;            /* RXF0EID8.EID[15:8]  */
        spi_write[ 3 ]  =   hfilter->rxfiltervalue[ 0 ];                 /* RXF0EID0.EID[7:0]   */
	
        /* If filter 0 is to be applied only to extended frames */
        if ( ( hfilter->extendedidenable & RXF0_EXTENDED_ID_ENABLED ) == RXF0_EXTENDED_ID_ENABLED )
        {
            /* set EXIDE bit in RXF0SIDL register */
            spi_write[ 1 ] |= EXIDE_FILTER_APPLY_ONLY_EXTENDED_FRAMES;
        }

        /* Write the filter values to the filter 0 registers */
        CAN_Control_Register_Write( hcan, RXF0SIDH_REG, spi_write, 4U );
    }

    /* If filter 1 is selected for configuration */
    if ( ( hfilter->rxfilternmbr & RXF1 ) == RXF1 )
    {
        /* Get the filter 1 register values */
        spi_write[ 0 ]  =   hfilter->rxfiltervalue[ 1 ] >> 21;           /* RXF1SIDH.SID[10:3]  */
        spi_write[ 1 ]  = ( hfilter->rxfiltervalue[ 1 ] >> 13 ) & 0xE0U; /* RXF1SIDL.SID[2:0]   */
        spi_write[ 1 ] |= ( hfilter->rxfiltervalue[ 1 ] >> 16 ) & 0x03U; /* RXF1SIDL.EID[17:16] */
        spi_write[ 2 ]  =   hfilter->rxfiltervalue[ 1 ] >> 8;            /* RXF1EID8.EID[15:8]  */
        spi_write[ 3 ]  =   hfilter->rxfiltervalue[ 1 ];                 /* RXF1EID0.EID[7:0]   */
	
        /* If filter 1 is to be applied only to extended frames */
        if ( ( hfilter->extendedidenable & RXF1_EXTENDED_ID_ENABLED ) == RXF1_EXTENDED_ID_ENABLED )
        {
            /* set EXIDE bit in RXF1SIDL register */
            spi_write[ 1 ] |= EXIDE_FILTER_APPLY_ONLY_EXTENDED_FRAMES;
        }

        /* Write the filter values to the filter 1 registers */
        CAN_Control_Register_Write( hcan, RXF1SIDH_REG, spi_write, 4U );
    }

    /* If filter 2 is selected for configuration */
    if ( ( hfilter->rxfilternmbr & RXF2 ) == RXF2 )
    {
        /* Get the filter 2 register values */
        spi_write[ 0 ]  =   hfilter->rxfiltervalue[ 2 ] >> 21;           /* RXF2SIDH.SID[10:3]  */
        spi_write[ 1 ]  = ( hfilter->rxfiltervalue[ 2 ] >> 13 ) & 0xE0U; /* RXF2SIDL.SID[2:0]   */
        spi_write[ 1 ] |= ( hfilter->rxfiltervalue[ 2 ] >> 16 ) & 0x03U; /* RXF2SIDL.EID[17:16] */
        spi_write[ 2 ]  =   hfilter->rxfiltervalue[ 2 ] >> 8;            /* RXF2EID8.EID[15:8]  */
        spi_write[ 3 ]  =   hfilter->rxfiltervalue[ 2 ];                 /* RXF2EID0.EID[7:0]   */
	
	    /* If filter 2 is to be applied only to extended frames */
        if ( ( hfilter->extendedidenable & RXF2_EXTENDED_ID_ENABLED ) == RXF2_EXTENDED_ID_ENABLED )
        {
            /* set EXIDE bit in RXF2SIDL register */
            spi_write[ 1 ] |= EXIDE_FILTER_APPLY_ONLY_EXTENDED_FRAMES;
        }

        /* Write the filter values to the filter 2 registers */
        CAN_Control_Register_Write( hcan, RXF2SIDH_REG, spi_write, 4U );
    }
    
    /* If filter 3 is selected for configuration */
    if ( ( hfilter->rxfilternmbr & RXF3 ) == RXF3 )
    {
        /* Get the filter 3 register values */
        spi_write[ 0 ]  =   hfilter->rxfiltervalue[ 3 ] >> 21;           /* RXF3SIDH.SID[10:3]  */
        spi_write[ 1 ]  = ( hfilter->rxfiltervalue[ 3 ] >> 13 ) & 0xE0U; /* RXF3SIDL.SID[2:0]   */
        spi_write[ 1 ] |= ( hfilter->rxfiltervalue[ 3 ] >> 16 ) & 0x03U; /* RXF3SIDL.EID[17:16] */
        spi_write[ 2 ]  =   hfilter->rxfiltervalue[ 3 ] >> 8;            /* RXF3EID8.EID[15:8]  */
        spi_write[ 3 ]  =   hfilter->rxfiltervalue[ 3 ];                 /* RXF3EID0.EID[7:0]   */
	
	    /* If filter 3 is to be applied only to extended frames */
        if ( ( hfilter->extendedidenable & RXF3_EXTENDED_ID_ENABLED ) == RXF3_EXTENDED_ID_ENABLED )
        {
            /* set EXIDE bit in RXF3SIDL register */
            spi_write[ 1 ] |= EXIDE_FILTER_APPLY_ONLY_EXTENDED_FRAMES;
        }

        /* Write the filter values to the filter 3 registers */
        CAN_Control_Register_Write( hcan, RXF3SIDH_REG, spi_write, 4U );
    }

    /* If filter 4 is selected for configuration */
    if ( ( hfilter->rxfilternmbr & RXF4 ) == RXF4 )
    {
        /* Get the filter 4 register values */
        spi_write[ 0 ]  =   hfilter->rxfiltervalue[ 4 ] >> 21;           /* RXF4SIDH.SID[10:3]  */
        spi_write[ 1 ]  = ( hfilter->rxfiltervalue[ 4 ] >> 13 ) & 0xE0U; /* RXF4SIDL.SID[2:0]   */
        spi_write[ 1 ] |= ( hfilter->rxfiltervalue[ 4 ] >> 16 ) & 0x03U; /* RXF4SIDL.EID[17:16] */
        spi_write[ 2 ]  =   hfilter->rxfiltervalue[ 4 ] >> 8;            /* RXF4EID8.EID[15:8]  */
        spi_write[ 3 ]  =   hfilter->rxfiltervalue[ 4 ];                 /* RXF4EID0.EID[7:0]   */
	
	    /* If filter 4 is to be applied only to extended frames */
        if ( ( hfilter->extendedidenable & RXF4_EXTENDED_ID_ENABLED ) == RXF4_EXTENDED_ID_ENABLED )
        {
            /* set EXIDE bit in RXF4SIDL register */
            spi_write[ 1 ] |= EXIDE_FILTER_APPLY_ONLY_EXTENDED_FRAMES;
        }

        /* Write the filter values to the filter 4 registers */
        CAN_Control_Register_Write( hcan, RXF4SIDH_REG, spi_write, 4U );
    }

    /* If filter 5 is selected for configuration */
    if ( ( hfilter->rxfilternmbr & RXF5 ) == RXF5 )
    {
        /* Get the filter 5 register values */
        spi_write[ 0 ]  =   hfilter->rxfiltervalue[ 5 ] >> 21;           /* RXF5SIDH.SID[10:3]  */
        spi_write[ 1 ]  = ( hfilter->rxfiltervalue[ 5 ] >> 13 ) & 0xE0U; /* RXF5SIDL.SID[2:0]   */
        spi_write[ 1 ] |= ( hfilter->rxfiltervalue[ 5 ] >> 16 ) & 0x03U; /* RXF5SIDL.EID[17:16] */
        spi_write[ 2 ]  =   hfilter->rxfiltervalue[ 5 ] >> 8;            /* RXF5EID8.EID[15:8]  */
        spi_write[ 3 ]  =   hfilter->rxfiltervalue[ 5 ];                 /* RXF5EID0.EID[7:0]   */
	
	    /* If filter 5 is to be applied only to extended frames */
        if ( ( hfilter->extendedidenable & RXF5_EXTENDED_ID_ENABLED ) == RXF5_EXTENDED_ID_ENABLED )
        {
            /* set EXIDE bit in RXF5SIDL register */
            spi_write[ 1 ] |= EXIDE_FILTER_APPLY_ONLY_EXTENDED_FRAMES;
        }

        /* Write the filter values to the filter 5 registers */
        CAN_Control_Register_Write( hcan, RXF5SIDH_REG, spi_write, 4U );
    }
}

/**
 * @brief Write the number of bytes defined in 'size' from the 'data' array to the address specified by 'reg_addr'
 *        of the MCP2515 specified by CAN_Control_HandleTypeDef
 * 
 *        If the number of bytes is greater than 1, for every subsequent byte, it will be written into the next address in
 *        the register map of the MCP2515 (refer to datasheet for more details).
 * 
 *        Note: configuration mode is the only mode where the following registers are modifiable
 *              - CNF1, CNF2, CNF3
 *              - TXRTSCTRL
 *              - RXMnSIDLH, RXMnSIDL, RXMnEID8, RXMnEID0
 *              - RXFnSIDLH, RXFnSIDL, RXFnEID8, RXFnEID0
 * 
 * @param hcan     pointer to an MCP2515 configuration structure (CAN module to write to)
 * @param reg_addr register address to write to. Refer to 'MCP2515 register addresses definitions' in can.h
 * @param data     data array that contains the information to be written
 * @param size     number of bytes to write from the data array to the register
 */
void CAN_Control_Register_Write( CAN_Control_HandleTypeDef *hcan, uint8_t reg_addr, uint8_t *data, uint8_t size )
{   
    uint8_t instruction = WRITE_INS;

    /* If SPI1 peripheral handles the CAN controller */
    if ( ( hcan->spi == CAN_SPI1 ) )
    {
        SPI1_CS_Enable();
        SPI1_Write( &instruction, 1U ); /* Request WRITE to the CAN controller (MCP2515) ... */
        SPI1_Write( &reg_addr, 1U );    /* ... to this address */
        SPI1_Write( data, size );       /* Write 'size' number of bytes from the data array */
        SPI1_CS_Disable();
    }
    /* If SPI2 peripheral handles the CAN controller */
    else if ( ( hcan->spi == CAN_SPI2 ) )
    {
        SPI2_CS_Enable();
        SPI2_Write( &instruction, 1U ); /* Request WRITE to the CAN controller (MCP2515) ... */
        SPI2_Write( &reg_addr, 1U );    /* ... to this address */
        SPI2_Write( data, size );       /* Write 'size' number of bytes from the data array */
        SPI2_CS_Disable();
    }
    else
    {
        /* Do nothing */
    }

    /* Wait 50us for the data to be processed by the MCP2515 (time is not specified in datasheet) */
    TIM3_Delay_us( 50U );
}

/**
 * @brief Read the number of bytes defined in 'size' from the register address 'reg_addr' of the MCP2515
 *        specified by CAN_Control_HandleTypeDef and store the information read into the 'data' array.
 * 
 *        Bear in mind that the mask and filter registers are read as zeros in any mode except configuration mode, therefore
 *        for this case the MCP2515 must be set to configuration mode before any reading attempt is made from them.
 *
 * @param hcan     pointer to an MCP2515 configuration structure (CAN module to read from)
 * @param reg_addr register address to read from. Refer to 'MCP2515 register addresses definitions' in can.h
 * @param data     data array that will store the information retrieved
 * @param size     number of bytes to read from the register address
 */
void CAN_Control_Register_Read( CAN_Control_HandleTypeDef *hcan, uint8_t reg_addr, uint8_t *data, uint8_t size )
{
    uint8_t instruction = READ_INS;

    /* If SPI1 peripheral handles the CAN controller */
    if ( hcan->spi == CAN_SPI1 )
    {
        SPI1_CS_Enable();
        SPI1_Write( &instruction, 1U ); /* Request READ to the CAN controller (MCP2515) ... */
        SPI1_Write( &reg_addr, 1U );    /* ... from this address */
        SPI1_Read( data, size );        /* Read back 'size' bytes and store them into the data array */
        SPI1_CS_Disable();
    }
    /* If SPI2 peripheral handles the CAN controller */
    else if ( hcan->spi == CAN_SPI2 )
    {
        SPI2_CS_Enable();
        SPI2_Write( &instruction, 1U ); /* Request READ to the CAN controller (MCP2515) ... */
        SPI2_Write( &reg_addr, 1U );    /* ... from this address */
        SPI2_Read( data, size );        /* Read back 'size' bytes and store them into the data array */
        SPI2_CS_Disable();
    }
    else
    {
        /* Do nothing */
    }

    /* Wait 50us for the data to be processed by the MCP2515 (time is not specified in datasheet) */
    TIM3_Delay_us( 50U );
}

/**
 * @brief Allow for single bit modifications in the registers of the MCP2515 that support it:
 *        - TXBnCTRL
 *        - RXBnCTRL
 *        - CNF1, CNF2, CNF3
 *        - BFPCTRL, TXRTSCTRL
 *        - CANINTE, CANINTF
 *        - EFLG
 *        - CANCTRL
 * 
 *        Note: executing the BIT MODIFY command on non bit-modifiable registers forces the mask to 0xFF.
 *              This causes byte WRITES to the registers, not BIT MODIFY.
 * 
 * @param hcan     pointer to an MCP2515 configuration structure (CAN module to be bit modified)
 * @param reg_addr register address to bit modify. Refer to 'MCP2515 register addresses definitions' in can.h
 * @param mask     8-bit mask value. Determines which bits in the register will be modified. 1 = modify, 0 = not modify
 * @param data     8-bit data. Determines what value the modified bits in the register will be changed to
 */
void CAN_Control_Register_Bit( CAN_Control_HandleTypeDef *hcan, uint8_t reg_addr, uint8_t mask, uint8_t data )
{
    uint8_t instruction = BIT_MODIFY_INS;

    /* If SPI1 peripheral handles the CAN controller */
    if ( ( hcan->spi == CAN_SPI1 ) )
    {
        SPI1_CS_Enable();
        SPI1_Write( &instruction, 1U ); /* Request BIT_MODIFY to the CAN controller (MCP2515) ... */
        SPI1_Write( &reg_addr, 1U );    /* ... to this address */
        SPI1_Write( &mask, 1U );        /* Send the mask byte value */
        SPI1_Write( &data, 1U );        /* Send the data byte value */
        SPI1_CS_Disable();
    }
    /* If SPI2 peripheral handles the CAN controller */
    else if ( ( hcan->spi == CAN_SPI2 ) )
    {
        SPI2_CS_Enable();
        SPI2_Write( &instruction, 1U ); /* Request BIT_MODIFY to the CAN controller (MCP2515) ... */
        SPI2_Write( &reg_addr, 1U );    /* ... to this address */
        SPI2_Write( &mask, 1U );        /* Send the mask byte value */
        SPI2_Write( &data, 1U );        /* Send the data byte value */
        SPI2_CS_Disable();
    }
    else
    {
        /* Do nothing */
    }

    /* Wait 50us for the data to be processed by the MCP2515 (time is not specified in datasheet) */
    TIM3_Delay_us( 50U );    
}

/**
 * @brief Send one or various CAN frames from the TX buffers specified in the CAN_Control_TX structure. Parameters such as
 *         TX frame IDs, frame types, data length and data can also be found in the TX configuration structure CAN_Control_TX.
 *
 *        Note: due to this function's construction, the TX buffers' priority is as follows:
 *              - TXB0 (if selected) is sent first.
 *              - TXB1 (if selected) is sent after TXB0 but before TXB2.
 *              - TXB2 (if selected) is sent last.
 *              Ignoring then, the priorities set by the TXP bits in the TXBnCTRL register of the MCP2515.
 * 
 * @param hcan  pointer to an MCP2515 configuration structure (CAN sending node)
 * @param txcan pointer to the TX configuration structure, it contains the transmission parameters of the CAN sending node.
 *              Refer to the CAN_Control_TX structure definition in can.h for its possible values
 */
void CAN_Control_Send_CAN_Frame( CAN_Control_HandleTypeDef *hcan, CAN_Control_TX *txcan )
{
    uint8_t spi_write[ 5 ];

    /* If buffer TXB0 is selected for transmission */
    if ( ( txcan->txbuffernmbr & TXB0 ) == TXB0 )
    {
        /* If CAN frame to be sent is extended (either data or remote frame) */
        if ( ( txcan->txframetype[ 0 ] == TX_EXTENDED_DATA_FRAME ) || ( txcan->txframetype[ 0 ] == TX_EXTENDED_REMOTE_FRAME ) )
        {
            /* Get the TX buffer 0 COMPLETE ID register values */
            spi_write[ 0 ]  =   txcan->txid[ 0 ] >> 21;           /* TXB0SIDH.SID[10:3]  */
            spi_write[ 1 ]  = ( txcan->txid[ 0 ] >> 13 ) & 0xE0U; /* TXB0SIDL.SID[2:0]   */
            spi_write[ 1 ] |= ( txcan->txid[ 0 ] >> 16 ) & 0x03U; /* TXB0SIDL.EID[17:16] */
            spi_write[ 2 ]  =   txcan->txid[ 0 ] >> 8;            /* TXB0EID8.EID[15:8]  */
            spi_write[ 3 ]  =   txcan->txid[ 0 ];                 /* TXB0EID0.EID[7:0]   */

            /* Set EXIDE bit in TXB0SIDL register to indicate extended identifier is to be transmitted */
            spi_write[ 1 ] |= EXIDE_MSG_TRANSMIT_EXTENDED_ID;
        }
        /* If CAN frame to be sent is standard (either data or remote frame) */
        else
        {
            /* Get the TX buffer 0 STANDARD ID register values */
            spi_write[ 0 ] = txcan->txid[ 0 ] >> 3; /* TXB0SIDH.SID[10:3] */
            spi_write[ 1 ] = txcan->txid[ 0 ] << 5; /* TXB0SIDL.SID[2:0]  */
            
            /* NOTE: TXB0SIDL.EID[17:16], TXB0EID8.EID[15:8], TXB0EID0.EID[7:0] and TXB0SIDL.EXIDE bit will be sent as 0s */
            spi_write[ 2 ] = 0U; /* TXB0EID8.EID[15:8] */
            spi_write[ 3 ] = 0U; /* TXB0EID0.EID[7:0]  */
        }

        /* Set number of data bytes to be transmitted/requested (TXB0DLC) in the CAN frame using TX buffer 0 */
        spi_write[ 4 ] = txcan->datalength[ 0 ]; /* TXB0DLC */

        /* If CAN frame to be sent is remote (either standard or extended frame) */
        if ( ( txcan->txframetype[ 0 ] == TX_STANDARD_REMOTE_FRAME ) || ( txcan->txframetype[ 0 ] == TX_EXTENDED_REMOTE_FRAME ) )
        {
            /* Set RTR bit in TXB0DLC register to indicate a remote request is to be transmitted */
            spi_write[ 4 ] |= RTR_TRANSMIT_REMOTE_FRAME_REQUEST;

            /* Write to the MCP2515 TX buffer 0 ID and TX buffer 0 data length code registers */
            CAN_Control_Register_Write( hcan, TXB0SIDH_REG, spi_write, 5U );

            /* there's no need for writting to the TXB0 data registers (TXB0Dm) since this is a remote frame */
        }
        /* If CAN frame to be sent is data (either standard or extended frame) */
        else
        {
            /* Write to the MCP2515 TX buffer 0 ID and TX buffer 0 data length code registers */
            CAN_Control_Register_Write( hcan, TXB0SIDH_REG, spi_write, 5U );

            /* Write to the MCP2515 TX buffer 0 Data (TXB0Dm) registers */
            CAN_Control_Register_Write( hcan, TXB0D0_REG, txcan->data[ 0 ], txcan->datalength[ 0 ] );
        }

        /* Request transmission of TX buffer 0 by setting the TXREQ bit in the TXB0CTRL register */
        spi_write[ 0 ] = TXREQ_PENDING; /* TXB0CTRL */
        CAN_Control_Register_Write( hcan, TXB0CTRL_REG, spi_write, 1U );

        /* Wait for the CAN frame to be sent on the CAN bus ...
           If frame is extended data frame */
        if ( txcan->txframetype[ 0 ] == TX_EXTENDED_DATA_FRAME )
        {
            /* Longest possible delay for an extended CAN frame of length 'datalength[ 0 ]' and 'baudrate' bps */
            WAIT_SEND_EXTENDED_DATA_FRAME( txcan->datalength[ 0 ], hcan->baudrate );
        }
        /* If frame is standard data frame */
        else if ( txcan->txframetype[ 0 ] == TX_STANDARD_DATA_FRAME )
        {
            /* Longest possible delay for a standard CAN frame of length 'datalength[ 0 ]' and 'baudrate' bps */
            WAIT_SEND_STANDARD_DATA_FRAME( txcan->datalength[ 0 ], hcan->baudrate );
        }
        /* If frame is extended remote frame */
        else if ( txcan->txframetype[ 0 ] == TX_EXTENDED_REMOTE_FRAME )
        {
            /* Longest possible delay for a 'baudrate' bps extended remote CAN frame */
            WAIT_SEND_EXTENDED_REMOTE_FRAME( hcan->baudrate );
        }
        /* If frame is standard remote frame */
        else
        {
            /* Longest possible delay for a 'baudrate' bps standard remote CAN frame */
            WAIT_SEND_STANDARD_REMOTE_FRAME( hcan->baudrate );
        }
    }

    /* If buffer TXB1 is selected for transmission */
    if ( ( txcan->txbuffernmbr & TXB1 ) == TXB1 )
    {
        /* If CAN frame to be sent is extended (either data or remote frame) */
        if ( ( txcan->txframetype[ 1 ] == TX_EXTENDED_DATA_FRAME ) || ( txcan->txframetype[ 1 ] == TX_EXTENDED_REMOTE_FRAME ) )
        {
            /* Get the TX buffer 1 COMPLETE ID register values */
            spi_write[ 0 ]  =   txcan->txid[ 1 ] >> 21;           /* TXB1SIDH.SID[10:3]  */
            spi_write[ 1 ]  = ( txcan->txid[ 1 ] >> 13 ) & 0xE0U; /* TXB1SIDL.SID[2:0]   */
            spi_write[ 1 ] |= ( txcan->txid[ 1 ] >> 16 ) & 0x03U; /* TXB1SIDL.EID[17:16] */
            spi_write[ 2 ]  =   txcan->txid[ 1 ] >> 8;            /* TXB1EID8.EID[15:8]  */
            spi_write[ 3 ]  =   txcan->txid[ 1 ];                 /* TXB1EID0.EID[7:0]   */

            /* Set EXIDE bit in TXB1SIDL register to indicate extended identifier is to be transmitted */
            spi_write[ 1 ] |= EXIDE_MSG_TRANSMIT_EXTENDED_ID;
        }
        /* If CAN frame to be sent is standard (either data or remote frame) */
        else
        {
            /* Get the TX buffer 1 STANDARD ID register values */
            spi_write[ 0 ] = txcan->txid[ 1 ] >> 3; /* TXB1SIDH.SID[10:3] */
            spi_write[ 1 ] = txcan->txid[ 1 ] << 5; /* TXB1SIDL.SID[2:0]  */
            
            /* NOTE: TXB1SIDL.EID[17:16], TXB1EID8.EID[15:8], TXB1EID0.EID[7:0] and TXB1SIDL.EXIDE bit will be sent as 0s */
            spi_write[ 2 ] = 0U; /* TXB1EID8.EID[15:8] */
            spi_write[ 3 ] = 0U; /* TXB1EID0.EID[7:0]  */
        }

        /* Set number of data bytes to be transmitted/requested (TXB1DLC) in the CAN frame using TX buffer 1 */
        spi_write[ 4 ] = txcan->datalength[ 1 ]; /* TXB1DLC */

        /* If CAN frame to be sent is remote (either standard or extended frame) */
        if ( ( txcan->txframetype[ 1 ] == TX_STANDARD_REMOTE_FRAME ) || ( txcan->txframetype[ 1 ] == TX_EXTENDED_REMOTE_FRAME ) )
        {
            /* Set RTR bit in TXB1DLC register to indicate a remote request is to be transmitted */
            spi_write[ 4 ] |= RTR_TRANSMIT_REMOTE_FRAME_REQUEST;

            /* Write to the MCP2515 TX buffer 1 ID and TX buffer 1 data length code registers */
            CAN_Control_Register_Write( hcan, TXB1SIDH_REG, spi_write, 5U );

            /* there's no need for writting to the TXB1 data registers (TXB1Dm) since this is a remote frame */
        }
        /* If CAN frame to be sent is data (either standard or extended frame) */
        else
        {
            /* Write to the MCP2515 TX buffer 1 ID and TX buffer 1 data length code registers */
            CAN_Control_Register_Write( hcan, TXB1SIDH_REG, spi_write, 5U );

            /* Write to the MCP2515 TX buffer 1 Data (TXB1Dm) registers */
            CAN_Control_Register_Write( hcan, TXB1D0_REG, txcan->data[ 1 ], txcan->datalength[ 1 ] );
        }

        /* Request transmission of TX buffer 1 by setting the TXREQ bit in the TXB1CTRL register */
        spi_write[ 0 ] = TXREQ_PENDING; /* TXB1CTRL */
        CAN_Control_Register_Write( hcan, TXB1CTRL_REG, spi_write, 1U );

        /* Wait for the CAN frame to be sent on the CAN bus ...
           If frame is extended data frame */
        if ( txcan->txframetype[ 1 ] == TX_EXTENDED_DATA_FRAME )
        {
            /* Longest possible delay for an extended CAN frame of length 'datalength[ 1 ]' and 'baudrate' bps */
            WAIT_SEND_EXTENDED_DATA_FRAME( txcan->datalength[ 1 ], hcan->baudrate );
        }
        /* If frame is standard data frame */
        else if ( txcan->txframetype[ 1 ] == TX_STANDARD_DATA_FRAME )
        {
            /* Longest possible delay for a standard CAN frame of length 'datalength[ 1 ]' and 'baudrate' bps */
            WAIT_SEND_STANDARD_DATA_FRAME( txcan->datalength[ 1 ], hcan->baudrate );
        }
        /* If frame is extended remote frame */
        else if ( txcan->txframetype[ 1 ] == TX_EXTENDED_REMOTE_FRAME )
        {
            /* Longest possible delay for a 'baudrate' bps extended remote CAN frame */
            WAIT_SEND_EXTENDED_REMOTE_FRAME( hcan->baudrate );
        }
        /* If frame is standard remote frame */
        else
        {
            /* Longest possible delay for a 'baudrate' bps standard remote CAN frame */
            WAIT_SEND_STANDARD_REMOTE_FRAME( hcan->baudrate );
        }
    }

    /* If buffer TXB2 is selected for transmission */
    if ( ( txcan->txbuffernmbr & TXB2 ) == TXB2 )
    {
        /* If CAN frame to be sent is extended (either data or remote frame) */
        if ( ( txcan->txframetype[ 2 ] == TX_EXTENDED_DATA_FRAME ) || ( txcan->txframetype[ 2 ] == TX_EXTENDED_REMOTE_FRAME ) )
        {
            /* Get the TX buffer 2 COMPLETE ID register values */
            spi_write[ 0 ]  =   txcan->txid[ 2 ] >> 21;           /* TXB2SIDH.SID[10:3]  */
            spi_write[ 1 ]  = ( txcan->txid[ 2 ] >> 13 ) & 0xE0U; /* TXB2SIDL.SID[2:0]   */
            spi_write[ 1 ] |= ( txcan->txid[ 2 ] >> 16 ) & 0x03U; /* TXB2SIDL.EID[17:16] */
            spi_write[ 2 ]  =   txcan->txid[ 2 ] >> 8;            /* TXB2EID8.EID[15:8]  */
            spi_write[ 3 ]  =   txcan->txid[ 2 ];                 /* TXB2EID0.EID[7:0]   */

            /* Set EXIDE bit in TXB2SIDL register to indicate extended identifier is to be transmitted */
            spi_write[ 1 ] |= EXIDE_MSG_TRANSMIT_EXTENDED_ID;
        }
        /* If CAN frame to be sent is standard (either data or remote frame) */
        else
        {
            /* Get the TX buffer 2 STANDARD ID register values */
            spi_write[ 0 ] = txcan->txid[ 2 ] >> 3; /* TXB2SIDH.SID[10:3] */
            spi_write[ 1 ] = txcan->txid[ 2 ] << 5; /* TXB2SIDL.SID[2:0]  */
            
            /* NOTE: TXB2SIDL.EID[17:16], TXB2EID8.EID[15:8], TXB2EID0.EID[7:0] and TXB2SIDL.EXIDE bit will be sent as 0s */
            spi_write[ 2 ] = 0U; /* TXB2EID8.EID[15:8] */
            spi_write[ 3 ] = 0U; /* TXB2EID0.EID[7:0]  */
        }

        /* Set number of data bytes to be transmitted/requested (TXB2DLC) in the CAN frame using TX buffer 2 */
        spi_write[ 4 ] = txcan->datalength[ 2 ]; /* TXB2DLC */

        /* If CAN frame to be sent is remote (either standard or extended frame) */
        if ( ( txcan->txframetype[ 2 ] == TX_STANDARD_REMOTE_FRAME ) || ( txcan->txframetype[ 2 ] == TX_EXTENDED_REMOTE_FRAME ) )
        {
            /* Set RTR bit in TXB2DLC register to indicate a remote request is to be transmitted */
            spi_write[ 4 ] |= RTR_TRANSMIT_REMOTE_FRAME_REQUEST;

            /* Write to the MCP2515 TX buffer 2 ID and TX buffer 2 data length code registers */
            CAN_Control_Register_Write( hcan, TXB2SIDH_REG, spi_write, 5U );

            /* there's no need for writting to the TXB2 data registers (TXB2Dm) since this is a remote frame */
        }
        /* If CAN frame to be sent is data (either standard or extended frame) */
        else
        {
            /* Write to the MCP2515 TX buffer 2 ID and TX buffer 2 data length code registers */
            CAN_Control_Register_Write( hcan, TXB2SIDH_REG, spi_write, 5U );

            /* Write to the MCP2515 TX buffer 2 Data (TXB2Dm) registers */
            CAN_Control_Register_Write( hcan, TXB2D0_REG, txcan->data[ 2 ], txcan->datalength[ 2 ] );
        }

        /* Request transmission of TX buffer 2 by setting the TXREQ bit in the TXB2CTRL register */
        spi_write[ 0 ] = TXREQ_PENDING; /* TXB2CTRL */
        CAN_Control_Register_Write( hcan, TXB2CTRL_REG, spi_write, 1U );

        /* Wait for the CAN frame to be sent on the CAN bus ...
           If frame is extended data frame */
        if ( txcan->txframetype[ 2 ] == TX_EXTENDED_DATA_FRAME )
        {
            /* Longest possible delay for an extended CAN frame of length 'datalength[ 2 ]' and 'baudrate' bps */
            WAIT_SEND_EXTENDED_DATA_FRAME( txcan->datalength[ 2 ], hcan->baudrate );
        }
        /* If frame is standard data frame */
        else if ( txcan->txframetype[ 2 ] == TX_STANDARD_DATA_FRAME )
        {
            /* Longest possible delay for a standard CAN frame of length 'datalength[ 2 ]' and 'baudrate' bps */
            WAIT_SEND_STANDARD_DATA_FRAME( txcan->datalength[ 2 ], hcan->baudrate );
        }
        /* If frame is extended remote frame */
        else if ( txcan->txframetype[ 2 ] == TX_EXTENDED_REMOTE_FRAME )
        {
            /* Longest possible delay for a 'baudrate' bps extended remote CAN frame */
            WAIT_SEND_EXTENDED_REMOTE_FRAME( hcan->baudrate );
        }
        /* If frame is standard remote frame */
        else
        {
            /* Longest possible delay for a 'baudrate' bps standard remote CAN frame */
            WAIT_SEND_STANDARD_REMOTE_FRAME( hcan->baudrate );
        }
    }
}

/**
 * @brief Read CAN data from the selected receiving buffers in the CAN_Control_RX structure regardless if a frame was received or not.
 *        The values read from the receiving buffers such as frame types, RX IDs, data length, acceptance filters, RX buffer 0 roll over status
 *        and data are stored in the CAN RX structure.
 * 
 *        RX1IE (receive buffer 1 full) and RX0IE (receive buffer 0 full) interrupts can be used to determine when
 *        a new CAN frame has been received.
 * 
 * @param hcan  pointer to an MCP2515 configuration structure (CAN receiving node)
 * @param rxcan pointer to the CAN RX structure, it will contain any data that concerns the RX buffers of the CAN receiving node.
 *              Refer to the CAN_Control_RX structure definition in can.h for its possible values
 */
void CAN_Control_Read_CAN_Frame( CAN_Control_HandleTypeDef *hcan, CAN_Control_RX *rxcan )
{
    uint8_t spi_read[ 6 ];

    /* If buffer RXB0 is selected for data reading */
    if ( ( rxcan->rxbuffernmbr & RXB0 ) == RXB0 )
    {	
        /* Read RXB0CTRL, RXB0SIDH, RXB0SIDL, RXB0EID8, RXB0EID0 and RXB0DLC registers
           from the MCP2515 in that order and store them into the spi_read[] array */
        CAN_Control_Register_Read( hcan, RXB0CTRL_REG, spi_read, 6U );

        /* Get the acceptance filter that made RXB0 store the CAN frame (FILHIT0 bit in RXB0CTRL register) */
        rxcan->accfilter[ 0 ] = spi_read[ 0 ] & FILHIT_BIT_0;

        /* Store the data length of the CAN frame received by RXB0 buffer into datalength[0]
           (keep only the DLC bits of the RXB0DLC register) */
        rxcan->datalength[ 0 ] = spi_read[ 5 ] & ( DLC_BIT_3 | DLC_BIT_2 | DLC_BIT_1 | DLC_BIT_0 );

        /* If CAN frame received in RXB0 is extended (IDE bit in RXB0SIDL) */
        if ( ( spi_read[ 2 ] & IDE_RECEIVED_EXTENDED_FRAME ) == IDE_RECEIVED_EXTENDED_FRAME )
        {
            /* Store the RXB0 full ID (standard + extended) into rxid[ 0 ] */
            rxcan->rxid[ 0 ]  =   spi_read[ 1 ] << 21;                                           /* RXB0SIDH.SID[10:3]  */
            rxcan->rxid[ 0 ] |= ( spi_read[ 2 ] & ( SID_BIT_2 | SID_BIT_1 | SID_BIT_0 ) ) << 18; /* RXB0SIDL.SID[2:0]   */
            rxcan->rxid[ 0 ] |= ( spi_read[ 2 ] & ( EID_BIT_17 | EID_BIT_16 ) ) << 16;           /* RXB0SIDL.EID[17:16] */
            rxcan->rxid[ 0 ] |=   spi_read[ 3 ] << 8;                                            /* RXB0EID8.EID[15:8]  */
            rxcan->rxid[ 0 ] |=   spi_read[ 4 ];                                                 /* RXB0EID0.EID[7:0]   */

            /* If received extended CAN frame is a remote request (RTR bit in RXB0DLC) */
            if ( ( spi_read[ 5 ] & RTR_RECEIVED_REMOTE_FRAME_REQUEST ) == RTR_RECEIVED_REMOTE_FRAME_REQUEST )
            {
                /* Indicate RXB0 received an extended remote request frame */
                rxcan->rxframetype[ 0 ] = RX_EXTENDED_REMOTE_FRAME;

                /* There's no need for reading the RXB0 data registers since this is a remote request
                   (data[0][] structure member should be ignored for this same reason, that is a remote
                    CAN frame does not contain data field) */
            }
            /* If received extended CAN frame is a data frame */
            else
            {
                /* Indicate RXB0 received an extended data frame */
                rxcan->rxframetype[ 0 ] = RX_EXTENDED_DATA_FRAME;

                /* Check if buffer 0 rollover occurred (BUKT, BUKT1 and FILHIT0 bits in RXB0CTRL register) */
                if ( ( spi_read[ 0 ] & ( BUKT_RXB0_ROLLOVER_ENABLED | BUKT1_RXB0_ROLLOVER_ENABLED | FILHIT_BIT_0 ) ) >= ROLLOVER_ACCEPTANCE_FILTER_0 )
                {
                    /* Indicate an RX buffer 0 rollover occurred (data was stored into RXB1) */
                    rxcan->rolloverstatus = ROLLOVER_OCCURRED;

                   /* Read 'datalength[ 0 ]' number of bytes from the RXB1 data registers and store them into data[0][0-7] */
                   CAN_Control_Register_Read( hcan, RXB1D0_REG, rxcan->data[ 0 ], rxcan->datalength[ 0 ] );
                }
                /* RXB0 rollover has not occurred */
                else
                {
                    /* Indicate RX buffer 0 rollover did not occur (data was stored into RXB0) */
                    rxcan->rolloverstatus = ROLLOVER_NOT_OCCURRED;

                    /* Read 'datalength[ 0 ]' number of bytes from the RXB0 data registers and store them into data[0][0-7] */
                    CAN_Control_Register_Read( hcan, RXB0D0_REG, rxcan->data[ 0 ], rxcan->datalength[ 0 ] );
                }
            }
        }
        /* If CAN frame received in RXB0 is standard */
        else
        {
            /* Store the RXB0 standard ID into rxid[ 0 ] */
            rxcan->rxid[ 0 ]  = spi_read[ 1 ] << 3; /* RXB0SIDH.SID[10:3]  */
            rxcan->rxid[ 0 ] |= spi_read[ 2 ] >> 5; /* RXB0SIDL.SID[2:0]   */

            /* If received standard CAN frame is a remote request (SRR bit in RXB0SIDL) */
            if ( ( spi_read[ 2 ] & SRR_RECEIVED_STANDARD_REMOTE_REQUEST ) == SRR_RECEIVED_STANDARD_REMOTE_REQUEST )
            {
                /* Indicate RXB0 received a standard remote request frame */
                rxcan->rxframetype[ 0 ] = RX_STANDARD_REMOTE_FRAME;

                /* There's no need for reading the RXB0 data registers since this is a remote request */
            }
            /* If received standard CAN frame is a data frame */
            else
            {
                /* Indicate RXB0 received a standard data frame */
                rxcan->rxframetype[ 0 ] = RX_STANDARD_DATA_FRAME;

                /* Check if buffer 0 rollover occurred (BUKT, BUKT1 and FILHIT0 bits in RXB0CTRL register) */
                if ( ( spi_read[ 0 ] & ( BUKT_RXB0_ROLLOVER_ENABLED | BUKT1_RXB0_ROLLOVER_ENABLED | FILHIT_BIT_0 ) ) >= ROLLOVER_ACCEPTANCE_FILTER_0 )
                {
                    /* Indicate an RX buffer 0 rollover occurred (data was stored into RXB1) */
                    rxcan->rolloverstatus = ROLLOVER_OCCURRED;

                   /* Read 'datalength[ 0 ]' number of bytes from the RXB1 data registers and store them into data[0][0-7] */
                   CAN_Control_Register_Read( hcan, RXB1D0_REG, rxcan->data[ 0 ], rxcan->datalength[ 0 ] );
                }
                /* RXB0 rollover has not occurred */
                else
                {
                    /* Indicate RX buffer 0 rollover did not occur (data was stored into RXB0) */
                    rxcan->rolloverstatus = ROLLOVER_NOT_OCCURRED;

                    /* Read 'datalength[ 0 ]' number of bytes from the RXB0 data registers and store them into data[0][0-7] */
                    CAN_Control_Register_Read( hcan, RXB0D0_REG, rxcan->data[ 0 ], rxcan->datalength[ 0 ] );
                }
            }
        }
    }

    /* If buffer RXB1 is selected for data reading */
    if ( ( rxcan->rxbuffernmbr & RXB1 ) == RXB1 )
    {	
        /* Read RXB1CTRL, RXB1SIDH, RXB1SIDL, RXB1EID8, RXB1EID0 and RXB1DLC registers
           from the MCP2515 in that order and store them into the spi_read[] array */
        CAN_Control_Register_Read( hcan, RXB1CTRL_REG, spi_read, 6U );

        /* Get the acceptance filter that made RXB1 store the CAN frame (FILHIT bits in RXB1CTRL register) */
        rxcan->accfilter[ 1 ] = spi_read[ 0 ] & ( FILHIT_BIT_2 | FILHIT_BIT_1 | FILHIT_BIT_0 );

        /* Store the data length of the CAN frame received by RXB1 buffer into datalength[1]
           (keep only the DLC bits of the RXB1DLC register) */
        rxcan->datalength[ 1 ] = spi_read[ 5 ] & ( DLC_BIT_3 | DLC_BIT_2 | DLC_BIT_1 | DLC_BIT_0 );

        /* If CAN frame received in RXB1 is extended (IDE bit in RXB1SIDL) */
        if ( ( spi_read[ 2 ] & IDE_RECEIVED_EXTENDED_FRAME ) == IDE_RECEIVED_EXTENDED_FRAME )
        {
            /* Store the RXB1 full ID (standard + extended) into rxid[ 1 ] */
            rxcan->rxid[ 1 ]  =   spi_read[ 1 ] << 21;                                           /* RXB1SIDH.SID[10:3]  */
            rxcan->rxid[ 1 ] |= ( spi_read[ 2 ] & ( SID_BIT_2 | SID_BIT_1 | SID_BIT_0 ) ) << 18; /* RXB1SIDL.SID[2:0]   */
            rxcan->rxid[ 1 ] |= ( spi_read[ 2 ] & ( EID_BIT_17 | EID_BIT_16 ) ) << 16;           /* RXB1SIDL.EID[17:16] */
            rxcan->rxid[ 1 ] |=   spi_read[ 3 ] << 8;                                            /* RXB1EID8.EID[15:8]  */
            rxcan->rxid[ 1 ] |=   spi_read[ 4 ];                                                 /* RXB1EID0.EID[7:0]   */

            /* If received extended CAN frame is a remote request (RTR bit in RXB1DLC) */
            if ( ( spi_read[ 5 ] & RTR_RECEIVED_REMOTE_FRAME_REQUEST ) == RTR_RECEIVED_REMOTE_FRAME_REQUEST )
            {
                /* Indicate RXB1 received an extended remote request frame */
                rxcan->rxframetype[ 1 ] = RX_EXTENDED_REMOTE_FRAME;

                /* There's no need for reading the RXB1 data registers since this is a remote request
                   (data[1][] structure member should be ignored for this same reason, that is a remote
                    CAN frame does not contain data field) */
            }
            /* If received extended CAN frame is a data frame */
            else
            {
                /* Indicate RXB1 received an extended data frame */
                rxcan->rxframetype[ 1 ] = RX_EXTENDED_DATA_FRAME;

                /* Read 'datalength[ 1 ]' number of bytes from the RXB1 data registers and store them into data[1][0-7] */
                CAN_Control_Register_Read( hcan, RXB1D0_REG, rxcan->data[ 1 ], rxcan->datalength[ 1 ] );
            }
        }
        /* If CAN frame received in RXB1 is standard */
        else
        {
            /* Store the RXB1 standard ID into rxid[ 1 ] */
            rxcan->rxid[ 1 ]  = spi_read[ 1 ] << 3; /* RXB1SIDH.SID[10:3]  */
            rxcan->rxid[ 1 ] |= spi_read[ 2 ] >> 5; /* RXB1SIDL.SID[2:0]   */

            /* If received standard CAN frame is a remote request (SRR bit in RXB1SIDL) */
            if ( ( spi_read[ 2 ] & SRR_RECEIVED_STANDARD_REMOTE_REQUEST ) == SRR_RECEIVED_STANDARD_REMOTE_REQUEST )
            {
                /* Indicate RXB1 received a standard remote request frame */
                rxcan->rxframetype[ 1 ] = RX_STANDARD_REMOTE_FRAME;

                /* There's no need for reading the RXB1 data registers since this is a remote request */
            }
            /* If received standard CAN frame is a data frame */
            else
            {
                /* Indicate RXB1 received a standard data frame */
                rxcan->rxframetype[ 1 ] = RX_STANDARD_DATA_FRAME;

                /* Read 'datalength[ 1 ]' number of bytes from the RXB1 data registers and store them into data[1][0-7] */
                CAN_Control_Register_Read( hcan, RXB1D1_REG, rxcan->data[ 1 ], rxcan->datalength[ 1 ] );
            }
        }
    }
}

/**
 * @brief Read the current transmission state of the CAN frame sent for the selected TXBn buffer.
 * 
 *        Note: Only one TXBn buffer can be selected at a time.
 * 
 *              If status is read as aborted, make sure the ABAT bit in the CANCTRL register is
 *              cleared in order to allow new transmissions.
 * 
 * @param hcan       pointer to an MCP2515 configuration structure (CAN sending node)
 * @param tx_buffer  TX buffer whose status is desired to be known. Refer to 'MCP2515 TX buffer number definitions' in can.h
 * @return uint8_t   selected TX buffer's CAN frame status. Refer to 'MCP2515 CAN frame TX states definitions' in can.h
 */
uint8_t CAN_Control_TX_CAN_Status( CAN_Control_HandleTypeDef *hcan, uint8_t tx_buffer )
{   
    uint8_t spi_read;
    uint8_t tx_state = TX_PENDING;

    /* If selected transmission buffer is valid */
    if ( tx_buffer <= TXB2 )
    {   
        /* Read respective TX buffer control register (TXBnCTRL) */
        switch( tx_buffer )
        {
            case TXB0:
                /* Read TXB0CTRL register from the MCP2515 */
                CAN_Control_Register_Read( hcan, TXB0CTRL_REG, &spi_read, 1U );
                break;

            case TXB1:
                /* Read TXB1CTRL register from the MCP2515 */
                CAN_Control_Register_Read( hcan, TXB1CTRL_REG, &spi_read, 1U );
                break;

            case TXB2:
                /* Read TXB2CTRL register from the MCP2515 */
                CAN_Control_Register_Read( hcan, TXB2CTRL_REG, &spi_read, 1U );
                break;

            default:
                /* Do nothing */
                break;
        }

        /* If transmission is pending (TXREQ in TXBnCTRL register)
           and message was not aborted (ABTF in TXBnCTRL) */
        if ( ( ( spi_read & TXREQ_PENDING ) == TXREQ_PENDING ) && ( ( spi_read & ABTF_MESSAGE_ABORTED ) == ABTF_TRANSMISSION_COMPLETE ) )
        {
            /* If bus error occurred (TXERR in TXBnCTRL) and message lost arbitration (MLOA in TXBnCTRL) */
            if ( ( spi_read & ( TXERR_BUS_ERROR | MLOA_LOST_ARBITRATION ) ) == ( TXERR_BUS_ERROR | MLOA_LOST_ARBITRATION ) )
            {   
                /* Transmission error detected and message is lost */
                tx_state = TX_BUS_ERROR_AND_LOST_ARBITRATION;
            }
            /* If a bus error occured while the message was being transmitted (TXERR in TXBnCTRL) */
            else if ( ( spi_read & TXERR_BUS_ERROR ) == TXERR_BUS_ERROR )
            {
                /* Message transmission error detected */
                tx_state = TX_BUS_ERROR;
            }
            /* If message lost arbitration (MLOA in TXBnCTRL) */
            else if ( ( spi_read & MLOA_LOST_ARBITRATION ) == MLOA_LOST_ARBITRATION )
            {
                /* Message is lost */
                tx_state = TX_LOST_ARBITRATION;
            }
            /* If message did not lose arbitration and no bus error was detected (TXERR = 0 and MLOA = 0) */
            else
            {
                /* Message is pending */
                tx_state = TX_PENDING;
            }
        }
        /* If abort flag is set (ABTF in TXBnCTRL) */
        else if ( ( spi_read & ABTF_MESSAGE_ABORTED ) == ABTF_MESSAGE_ABORTED )
        {   
            /* Message is aborted */
            tx_state = TX_ABORTED;
        }
        /* If transmission is neither pending nor aborted */
        else
        {
            /* Message sent successfully */
            tx_state = TX_SUCCESS;
        }
    }

    /* Return transmission state */
    return tx_state;
}

/**
 * @brief Abort pending CAN frame transmission of the selected TX buffers by clearing the TXREQ bit
 *        in their respective TXBnCTRL registers.
 * 
 *        Note: messages that were transmitting when abort was requested will continue to transmit.
 *              Abort has effect only when a message does not successfully transmit (lost arbitration
 *              or interrupt by error frame).
 * 
 * @param hcan      pointer to an MCP2515 configuration structure (CAN sending node to abort)
 * @param tx_buffer TX buffers to abort transmission. Refer to 'MCP2515 TX buffer number definitions' in can.h
 */
void CAN_Control_TX_CAN_Abort( CAN_Control_HandleTypeDef *hcan, uint8_t tx_buffer )
{
    /* If transmission buffer selected is TXB0 */
    if ( ( tx_buffer & TXB0 ) == TXB0 )
    {
        /* Clear only the TXREQ bit in the TXB0CTRL to stop transmission request */
        CAN_Control_Register_Bit( hcan, TXB0CTRL_REG, TXREQ_PENDING, TXREQ_NO_PENDING );
    }

    /* If transmission buffer selected is TXB1 */
    if ( ( tx_buffer & TXB1 ) == TXB1 )
    {
        /* Clear only the TXREQ bit in the TXB1CTRL to stop transmission request */
        CAN_Control_Register_Bit( hcan, TXB1CTRL_REG, TXREQ_PENDING, TXREQ_NO_PENDING );
    }

    /* If transmission buffer selected is TXB2 */
    if ( ( tx_buffer & TXB2 ) == TXB2 )
    {
        /* Clear only the TXREQ bit in the TXB0CTRL to stop transmission request */
        CAN_Control_Register_Bit( hcan, TXB2CTRL_REG, TXREQ_PENDING, TXREQ_NO_PENDING );
    }
}

/**
 * @brief Abort pending CAN frame transmissions for all the TX buffers by setting the ABAT bit in
 *        the CANCTRL register.
 * 
 *        Note: messages that were transmitting when abort was requested will continue to transmit.
 *              Abort has effect only when a message does not successfully transmit (lost arbitration
 *              or interrupt by error frame).
 * 
 * @param hcan pointer to an MCP2515 configuration structure (CAN sending node to abort)
 */
void CAN_Control_TX_CAN_Abort_All( CAN_Control_HandleTypeDef *hcan )
{
    /* Set abort all pending transmissions bit (ABAT) in CANCTRL register */
    CAN_Control_Register_Bit( hcan, CANCTRL_REG, ABAT_REQ_ABORT_TX, ABAT_REQ_ABORT_TX );

    /* Clear ABAT bit in order to allow new CAN frame transmissions. */
    CAN_Control_Register_Bit( hcan, CANCTRL_REG, ABAT_REQ_ABORT_TX, ABAT_TERMINATE_REQ_ABORT_TX );
}

/**
 * @brief Enable or disable the selected interrupts for the MCP2515 specified by CAN_Control_HandleTypeDef
 *        Interrupts not enabled are disabled by default.
 * 
 *        Note: interrupts enabled are mapped to the INT pin (idle state is HIGH).
 *              If more than one interrupt is enabled, whichever happens first will drive the INT pin LOW.
 * 
 * @param hcan       pointer to an MCP2515 configuration structure (CAN node to enable interrupts)
 * @param interrupts interrupts to be enabled. Refer to 'MCP2515 bit definitions for CANINTE' in can.h
 */
void CAN_Control_Enable_INT( CAN_Control_HandleTypeDef *hcan, uint8_t interrupts )
{   
    /* Enable the selected interrupts in the CANINTE register */
    CAN_Control_Register_Write( hcan, CANINTE_REG, &interrupts, 1U );
}

/**
 * @brief Return the interrupts that are pending (i.e. occurred) in the CANINTF register
 *        for the MCP2515 specified by CAN_Control_HandleTypeDef
 * 
 *        Note: - ERRIF flag can be set by multiple error conditions (refer to EFLG register in the datasheet for more details).
 *              - Respective flags must cleared by the MCU to reset the interrupt conditions.
 * 
 * @param hcan     pointer to an MCP2515 configuration structure (CAN node to read interrupt status from)
 * @return uint8_t CANINTF register value. Refer to 'MCP2515 bit definitions for CANINTF' in can.h
 */
uint8_t CAN_Control_INT_Status( CAN_Control_HandleTypeDef *hcan )
{
    uint8_t spi_read;

    /* Read CANINTF register and store it into spi_read variable */
    CAN_Control_Register_Read( hcan, CANINTF_REG, &spi_read, 1U );

    /* return value stored in CANINTF */
    return spi_read;
}

/**
 * @brief Clear the flags for the selected pending interrupts in the CANINTF register.
 *        Unselected flags remain untouched in CANINTF.
 * 
 * @param hcan       pointer to an MCP2515 configuration structure (CAN sending node to clear interrupt flags) 
 * @param interrupts interrupts to be cleared. Refer to 'MCP2515 bit definitions for CANINTF' in can.h
 */
void CAN_Control_Clear_INT_Status( CAN_Control_HandleTypeDef *hcan, uint8_t interrupts )
{   
    /* Clear the selected interrupt flags in the CANINTF register */
    CAN_Control_Register_Bit( hcan, CANINTF_REG, interrupts, 0U );
}

/**
 * @brief Return the value of the EFLG register for the configured MCP2515 by CAN_Control_HandleTypeDef.
 *
 *        The following errors in the EFLG registers must be cleared by the MCU:
 *        - RX1OVR (Receive Buffer 1 Overflow)
 *        - RX0OVR (Receive Buffer 0 Overflow)
 * 
 *        TXEP (Transmit Error-Passive) and RXEP (Receive Error-Pasive) flags are cleared when TEC and REC counters
 *        are less than 128 respectively.
 * 
 *        TXWAR (Transmit Error Warning) and RXWAR (Receive Error Warning) flags are cleared when TEC and REC
 *        counters are less than 96 respectively.
 * 
 *        EWARN (Error Warning) flag is cleared when both TEC and REC counters are less than 96.
 * 
 *        Note: The MCP2515 after going bus-off, indicated by TXBO (Bus-Off error flag) will recover back to error-active
 *              without any intervention by the MCU if the bus remains IDLE for 128 x 11 bit times (= 128 x baudrate).
 *              
 *              TEC and REC counters can be cleared by setting the MCP2515 to CONFIGURATION MODE, this will bring
 *              it to its default active-error state, thus clearing the TXEP, RXEP, TXWAR, RXWAR and EWARN flags.
 * 
 * @param hcan     pointer to an MCP2515 configuration structure (CAN node to read error status from)
 * @return uint8_t EFLG register value. Refer to 'MCP2515 bit definitions for EFLG register' in can.h
 */
uint8_t CAN_Control_ERR_Status( CAN_Control_HandleTypeDef *hcan )
{
    uint8_t spi_read;

    /* Read EFLG register and store it into spi_read variable */
    CAN_Control_Register_Read( hcan, EFLG_REG, &spi_read, 1U );

    /* return value stored in EFLG */
    return spi_read;
}

/**
 * @brief Clear the flags for the selected errors in the EFLG register. Unselected flags remain untouched in EFLG.
 * 
 *        The only Flags that must be reset by the MCU are:
 *        - RX1OVR and RX0OVR
 * 
 *        Any other flag cannot be reset even if attempted:
 *        - TXBO
 *        - TXEP, RXEP
 *        - TXWAR, RXWAR and EWARN
 *        Read above function's description for a correct procedure for clearing these flags.
 *
 *        Note: The MCP2515 after going bus-off, indicated by TXBO (Bus-Off error flag) will recover back to error-active
 *              without any intervention by the MCU if the bus remains IDLE for 128 x 11 bit times (= 128 x baudrate).
 *              This resets the TXBO flag in EFLG.
 * 
 *              TEC and REC counters can be cleared by setting the MCP2515 to CONFIGURATION MODE, this will bring
 *              it to its default active-error state, thus clearing the TXEP, RXEP, TXWAR, RXWAR and EWARN flags.
 * 
 * @param hcan    pointer to an MCP2515 configuration structure (CAN node to clear error status)
 * @param errors  errors to be cleared. Refer to 'MCP2515 bit definitions for EFLG' in can.h
 */
void CAN_Control_Clear_ERR_Status( CAN_Control_HandleTypeDef *hcan, uint8_t errors )
{
    /* Clear the selected error flags in the EFLG register */
    CAN_Control_Register_Bit( hcan, EFLG_REG, errors, 0U );
}
