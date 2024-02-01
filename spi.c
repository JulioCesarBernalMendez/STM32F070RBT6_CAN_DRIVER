/**
 * @file      spi.c
 * @author    Julio Cesar Bernal Mendez
 * 
 * @brief     This file contains the High-Level implementations for the SPI 1 and 2 peripherals of the Nucleo Board.
 *            Everything was designed for the STM32F070RBT6 Nucleo Board from ST Microelectronics.
 * 
 *            STM32F070RB SPI1 and SPI2 pinout:
 *            ---------------------------------
 *            - PA4 (SPI1_CS)
 *            - PA5 (SPI1_SCK)
 *            - PA6 (SPI1_MISO)
 *            - PA7 (SPI1_MOSI)
 *
 *            - PB12 (SPI2_CS)
 *            - PB13 (SPI2_SCK)
 *            - PB14 (SPI2_MISO)
 *            - PB15 (SPI2_MOSI)
 * 
 * @version   1.0
 * @date      2024-01-26
 * 
 * @copyright This project was created for learning purposes only.
 */

#include "spi.h"

/**
 * @brief Disable (pin is HIGH) the CS line of the SPI1 peripheral of the Nucleo Board
 */
void SPI1_CS_Disable( void )
{
    /* Set SPI1 CS pin (GPIOA4) to IDLE state (HIGH) */
    GPIOA->ODR |= GPIO_ODR_4;
}

/**
 * @brief Disable (pin is HIGH) the CS line of the SPI2 peripheral of the Nucleo Board
 */
void SPI2_CS_Disable( void )
{
    /* Set SPI2 CS pin (GPIOB12) to IDLE state (HIGH) */
    GPIOB->ODR |= GPIO_ODR_12;
}

/**
 * @brief Enable the CS line (pin is LOW) of the SPI1 peripheral of the Nucleo Board
 */
void SPI1_CS_Enable( void )
{
    /* Set SPI1 CS pin (GPIOA4) to LOW state (slave selected) */
    GPIOA->ODR &= ~GPIO_ODR_4;
}

/**
 * @brief Enable the CS line (pin is LOW) of the SPI2 peripheral of the Nucleo Board
 */
void SPI2_CS_Enable( void )
{
    /* Set SPI2 CS pin (GPIOB12) to LOW state (slave selected) */
    GPIOB->ODR &= ~GPIO_ODR_12;
}

/**
 * @brief Initialize and configure the necessary alternate functions for the GPIOA used to handle
 *        the SPI1 peripheral of the Nucleo Board (see pinout at the top of this file)
 */
void SPI1_GPIO_Init( void )
{
    /* enable GPIOA clock access */
    GPIOA_CLK_ENBL();

    /* PA4 (SPI1_CS) as output */
    GPIOA->MODER &= ~GPIO_MODER_MODER4_1;
    GPIOA->MODER |=  GPIO_MODER_MODER4_0;

    /* PA5 (SPI1_SCK), PA6 (SPI1_MISO) and PA7 (SPI1_MOSI)
       in alternate function mode */
    GPIOA->MODER |=  GPIO_MODER_MODER5_1;
    GPIOA->MODER &= ~GPIO_MODER_MODER5_0;
    GPIOA->MODER |=  GPIO_MODER_MODER6_1;
    GPIOA->MODER &= ~GPIO_MODER_MODER6_0;
    GPIOA->MODER |=  GPIO_MODER_MODER7_1;
    GPIOA->MODER &= ~GPIO_MODER_MODER7_0;

    /* PA4 (SPI1_CS), PA5 (SPI1_SCK), PA6 (SPI1_MISO) and PA7 (SPI1_MOSI)
       in output push-pull mode */
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_4;

    /* PA4 (SPI_CS) set to high speed output */
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR4_1;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR4_0;

    /* PA5 in AF0 (SPI1_SCK) */
    GPIOA->AFR[ 0 ] &= ~GPIO_AFRL_AFSEL5_3;
    GPIOA->AFR[ 0 ] &= ~GPIO_AFRL_AFSEL5_2;
    GPIOA->AFR[ 0 ] &= ~GPIO_AFRL_AFSEL5_1;
    GPIOA->AFR[ 0 ] &= ~GPIO_AFRL_AFSEL5_0;

    /* PA6 in AF0 (SPI1_MISO) */
    GPIOA->AFR[ 0 ] &= ~GPIO_AFRL_AFSEL6_3;
    GPIOA->AFR[ 0 ] &= ~GPIO_AFRL_AFSEL6_2;
    GPIOA->AFR[ 0 ] &= ~GPIO_AFRL_AFSEL6_1;
    GPIOA->AFR[ 0 ] &= ~GPIO_AFRL_AFSEL6_0;

    /* PA7 in AF0 (SPI1_MOSI) */
    GPIOA->AFR[ 0 ] &= ~GPIO_AFRL_AFSEL7_3;
    GPIOA->AFR[ 0 ] &= ~GPIO_AFRL_AFSEL7_2;
    GPIOA->AFR[ 0 ] &= ~GPIO_AFRL_AFSEL7_1;
    GPIOA->AFR[ 0 ] &= ~GPIO_AFRL_AFSEL7_0;
}

/**
 * @brief Initialize and configure the necessary alternate functions for the GPIOB used to handle
 *        the SPI2 peripheral of the Nucleo Board (see pinout at the top of this file)
 */
void SPI2_GPIO_Init( void )
{
    /* enable GPIOB clock access */
    GPIOB_CLK_ENBL();

    /* PB12 (SPI2_CS) as output */
    GPIOB->MODER &= ~GPIO_MODER_MODER12_1;
    GPIOB->MODER |=  GPIO_MODER_MODER12_0;

    /* PB13 (SPI2_SCK), PB14 (SPI2_MISO) and PB15 (SPI2_MOSI)
       in alternate function mode */
    GPIOB->MODER |=  GPIO_MODER_MODER13_1;
    GPIOB->MODER &= ~GPIO_MODER_MODER13_0;
    GPIOB->MODER |=  GPIO_MODER_MODER14_1;
    GPIOB->MODER &= ~GPIO_MODER_MODER14_0;
    GPIOB->MODER |=  GPIO_MODER_MODER15_1;
    GPIOB->MODER &= ~GPIO_MODER_MODER15_0;

    /* PB12 (SPI2_CS) in output push-pull mode */
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_12;

    /* PB12 (SPI2_CS) set to high speed output */
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR12_1;
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR12_0;

    /* PB13 in AF0 (SPI2_SCK) */
    GPIOB->AFR[ 1 ] &= ~GPIO_AFRH_AFSEL13_3;
    GPIOB->AFR[ 1 ] &= ~GPIO_AFRH_AFSEL13_2;
    GPIOB->AFR[ 1 ] &= ~GPIO_AFRH_AFSEL13_1;
    GPIOB->AFR[ 1 ] &= ~GPIO_AFRH_AFSEL13_0;

    /* PB14 in AF0 (SPI2_MISO) */
    GPIOB->AFR[ 1 ] &= ~GPIO_AFRH_AFSEL14_3;
    GPIOB->AFR[ 1 ] &= ~GPIO_AFRH_AFSEL14_2;
    GPIOB->AFR[ 1 ] &= ~GPIO_AFRH_AFSEL14_1;
    GPIOB->AFR[ 1 ] &= ~GPIO_AFRH_AFSEL14_0;

    /* PB15 in AF0 (SPI2_MOSI) */
    GPIOB->AFR[ 1 ] &= ~GPIO_AFRH_AFSEL15_3;
    GPIOB->AFR[ 1 ] &= ~GPIO_AFRH_AFSEL15_2;
    GPIOB->AFR[ 1 ] &= ~GPIO_AFRH_AFSEL15_1;
    GPIOB->AFR[ 1 ] &= ~GPIO_AFRH_AFSEL15_0;
}

/**
 * @brief Initialize GPIOA port in alternate function mode for SPI1, enable and configure the SPI1 module
 *        of the Nucleo Board to the following parameters (see pinout at the top of this file):
 *        - 6MHz Master Full-Duplex mode
 *        - Mode 0,0
 *        - MSB sent/received first
 *        - 8-bit data size
 */
void SPI1_Init( void )
{
    /* enable GPIO resources for SPI1 */
    SPI1_GPIO_Init();
   
    /* enable SPI1 clock access */
    SPI1_CLK_ENBL();

    /* SPI1 2-line unidirectional data mode */
    SPI1->CR1 &= ~SPI_CR1_BIDIMODE;
    
    /* SPI1 CRC disabled */
    SPI1->CR1 &= ~SPI_CR1_CRCEN;

    /* SPI1 in full-duplex mode */
    SPI1->CR1 &= ~SPI_CR1_RXONLY;

    /* SPI1 software slave management disabled */
    SPI1->CR1 &= ~SPI_CR1_SSM;

    /* SPI1 MSB transmitted/received first */
    SPI1->CR1 &= ~SPI_CR1_LSBFIRST;

    /* SPI1 baud rate fPCLK/8.
       With fPCLK = 48MHz (refer to SystemInit() in system_stm32f0xx.c),
       therefore SPI1 baud = 6MHz */
    SPI1->CR1 &= ~SPI_CR1_BR_2;
    SPI1->CR1 |=  SPI_CR1_BR_1;
    SPI1->CR1 &= ~SPI_CR1_BR_0;

    /* SPI1 as master */
    SPI1->CR1 |= SPI_CR1_MSTR;

    /* SPI1 clock is LOW when idle */
    SPI1->CR1 &= ~SPI_CR1_CPOL;

    /* SPI1 clock phase, 1st clock transition is the
       1st data capture edge */
    SPI1->CR1 &= ~SPI_CR1_CPHA;

    /* SPI1 RXNE event generated if FIFO >= 8 bits */
    SPI1->CR2 |= SPI_CR2_FRXTH;

    /* SPI1 8 bits Data size */
    SPI1->CR2 &= ~SPI_CR2_DS_3;
    SPI1->CR2 |=  SPI_CR2_DS_2;
    SPI1->CR2 |=  SPI_CR2_DS_1;
    SPI1->CR2 |=  SPI_CR2_DS_0;

    /* SPI1 Motorola mode frame format */
    SPI1->CR2 &= ~SPI_CR2_FRF;

    /* SPI1 no NSS pulse */
    SPI1->CR2 &= ~SPI_CR2_NSSP;

    /* SPI1 SS output enabled */
    SPI1->CR2 |= SPI_CR2_SSOE;
    
    /* Set CS pin (GPIOA4) to IDLE state (HIGH) */
    SPI1_CS_Disable();

    /* enable SPI1 peripheral */
    SPI1->CR1 |= SPI_CR1_SPE;
}

/**
 * @brief Initialize GPIOB port in alternate function mode for SPI1, enable and configure the SPI2 module
 *        of the Nucleo Board to the following parameters (see pinout at the top of this file):
 *        - 6MHz Master Full-Duplex mode
 *        - Mode 0,0
 *        - MSB sent/received first
 *        - 8-bit data size
 */
void SPI2_Init( void )
{
    /* enable GPIO resources for SPI2 */
    SPI2_GPIO_Init();
   
    /* enable SPI2 clock access */
    SPI2_CLK_ENBL();

    /* SPI2 2-line unidirectional data mode */
    SPI2->CR1 &= ~SPI_CR1_BIDIMODE;
    
    /* SPI2 CRC disabled */
    SPI2->CR1 &= ~SPI_CR1_CRCEN;

    /* SPI2 in full-duplex mode */
    SPI2->CR1 &= ~SPI_CR1_RXONLY;

    /* SPI2 software slave management disabled */
    SPI2->CR1 &= ~SPI_CR1_SSM;

    /* SPI2 MSB transmitted/received first */
    SPI2->CR1 &= ~SPI_CR1_LSBFIRST;

    /* SPI2 baud rate fPCLK/8.
       With fPCLK = 48MHz (refer to SystemInit() in system_stm32f0xx.c),
       therefore SPI2 baud = 6MHz */
    SPI2->CR1 &= ~SPI_CR1_BR_2;
    SPI2->CR1 |=  SPI_CR1_BR_1;
    SPI2->CR1 &= ~SPI_CR1_BR_0;

    /* SPI1 as master */
    SPI2->CR1 |= SPI_CR1_MSTR;

    /* SPI2 clock is LOW when idle */
    SPI2->CR1 &= ~SPI_CR1_CPOL;

    /* SPI2 clock phase, 1st clock transition is the
       1st data capture edge */
    SPI2->CR1 &= ~SPI_CR1_CPHA;

    /* SPI2 RXNE event generated if FIFO >= 8 bits */
    SPI2->CR2 |= SPI_CR2_FRXTH;

    /* SPI2 8 bits Data size */
    SPI2->CR2 &= ~SPI_CR2_DS_3;
    SPI2->CR2 |=  SPI_CR2_DS_2;
    SPI2->CR2 |=  SPI_CR2_DS_1;
    SPI2->CR2 |=  SPI_CR2_DS_0;

    /* SPI2 Motorola mode frame format */
    SPI2->CR2 &= ~SPI_CR2_FRF;

    /* SPI2 no NSS pulse */
    SPI2->CR2 &= ~SPI_CR2_NSSP;

    /* SPI2 SS output enabled */
    SPI2->CR2 |= SPI_CR2_SSOE;
    
    /* Set CS pin (GPIOB12) to IDLE state (HIGH) */
    SPI2_CS_Disable();

    /* enable SPI2 peripheral */
    SPI2->CR1 |= SPI_CR1_SPE;
}

/**
 * @brief Write data over the SPI1 MOSI line. The SPI1 chip-select enabling and disabling functions must be
 *        called before and after this function is executed respectively in order for the writing to take effect.
 *        The byte read on SPI1 MISO (due to the full-duplex configuration) is simply ignored.
 * 
 * @param data data array
 * @param size number in bytes to be sent from the data array
 */
void SPI1_Write( uint8_t *data, uint8_t size )
{
    uint8_t item;
    uint8_t temp;

    /* Wait for SPI1 bus to be free */
    while ( ( SPI1->SR & SPI_SR_BSY ) == SPI_SR_BSY )
    {
        /* Do nothing */
    }
    
    /* Loop through the data array */
    for ( item = 0U; item < size; item++ )
    {   
        /* Send current byte from the data array */
        *( uint8_t * )( &( SPI1->DR ) ) = data[ item ];

        /* Wait for SPI1 transmit buffer to be empty */
        while ( ( SPI1->SR & SPI_SR_TXE ) != SPI_SR_TXE )
        {
            /* Do nothing */
        }

        /* NOTE: SPI1 is configured in 2-line unidirectional and full duplex mode,
                 therefore for each data sent over MISO, data received in the MISO
                 pin is sampled every clock cycle, meaning that we send and receive
                 data at the same time although that may not be what we intend. */

        /* Wait for RX buffer to receive 1 data byte (FRXTH = 1) */
        while ( ( SPI1->SR & SPI_SR_RXNE ) != SPI_SR_RXNE )
        {
            /* Do nothing */
        }

        /* Store data read on MISO (this data will be ignored and not used)
           in order to clear the RX buffer. Also by reading data the 
           RXNE flag (receive buffer not empty) is automatically cleared */
        temp = ( uint8_t )( SPI1->DR );
    }
      
    /* Wait for SPI1 bus to be free */
    while ( ( SPI1->SR & SPI_SR_BSY ) == SPI_SR_BSY )
    {
        /* Do nothing */
    }

    /* Clear the overrun flag just in case the buffer is full and 
       attempt to store more receiving data on MISO is made */
    SPI1->SR &= ~SPI_SR_OVR;
}

/**
 * @brief Write data over the SPI2 MOSI line. The SPI2 chip-select enabling and disabling functions must be
 *        called before and after this function is executed respectively in order for the writing to take effect.
 *        The byte read on SPI2 MISO (due to the full-duplex configuration) is simply ignored.
 * 
 * @param data data array
 * @param size number of bytes to be sent from the data array
 */
void SPI2_Write( uint8_t *data, uint8_t size )
{
    uint8_t item;
    uint8_t temp;

    /* Wait for SPI2 bus to be free */
    while ( ( SPI2->SR & SPI_SR_BSY ) == SPI_SR_BSY )
    {
        /* Do nothing */
    }
    
    /* Loop through the data array */
    for ( item = 0U; item < size; item++ )
    {   
        /* Send current byte from the data array */
        *( uint8_t * )( &( SPI2->DR ) ) = data[ item ];

        /* Wait for SPI2 transmit buffer to be empty */
        while ( ( SPI2->SR & SPI_SR_TXE ) != SPI_SR_TXE )
        {
            /* Do nothing */
        }

        /* NOTE: SPI2 is configured in 2-line unidirectional and full duplex mode,
                 therefore for each data sent over MISO, data received in the MISO
                 pin is sampled every clock cycle, meaning that we send and receive
                 data at the same time although that may not be what we intend. */

        /* Wait for RX buffer to receive 1 data byte (FRXTH = 1) */
        while ( ( SPI2->SR & SPI_SR_RXNE ) != SPI_SR_RXNE )
        {
            /* Do nothing */
        }

        /* Store data read on MISO (this data will be ignored and not used)
           in order to clear the RX buffer. Also by reading data the 
           RXNE flag (receive buffer not empty) is automatically cleared */
        temp = ( uint8_t )( SPI2->DR );
    }
      
    /* Wait for SPI2 bus to be free */
    while ( ( SPI2->SR & SPI_SR_BSY ) == SPI_SR_BSY )
    {
        /* Do nothing */
    }

    /* Clear the overrun flag just in case the buffer is full and 
       attempt to store more receiving data on MISO is made */
    SPI2->SR &= ~SPI_SR_OVR;
}

/**
 * @brief Read data from the SPI1 MISO line by sending a zero dummy byte value on SPI1 MOSI for each byte to read
 *        in order to trigger the sending of information from the slave.
 * 
 * @param read data array to store the information read
 * @param size number of bytes to be read
 */
void SPI1_Read( uint8_t *read, uint8_t size )
{   
    uint8_t item;

    /* NOTE: SPI1 is configured in 2-line unidirectional and 
             full duplex mode, therefore for each data sent over MISO,
             data received in the MISO pin is sampled every clock cycle,
             meaning that we send and receive data at the same time
             although that may not be our intention. */
    
    /* store each data byte into the array */
    for ( item = 0; item < size; item++, read++ )
    {
        /* send dummy data (this allows us to receive data on MISO) */
        *( uint8_t * )( &( SPI1->DR ) ) = 0U;

        /* wait for RX buffer to receive data */
        while ( ( SPI1->SR & SPI_SR_RXNE ) == 0U )
        {
            /* do nothing */
        }

        /* store received byte (this also clears
        the SPI1 RX buffer not empty flag) */
        *read = ( uint8_t )( SPI1->DR );
    }
}

/**
 * @brief Read data from the SPI2 MISO line by sending a zero dummy byte value on SPI2 MOSI for each byte to read
 *        in order to trigger the sending of information from the slave.
 * 
 * @param read data array to store the information read
 * @param size number of bytes to be read
 */
void SPI2_Read( uint8_t *read, uint8_t size )
{
    uint8_t item;

    /* NOTE: SPI2 is configured in 2-line unidirectional and 
             full duplex mode, therefore for each data sent over MISO,
             data received in the MISO pin is sampled every clock cycle,
             meaning that we send and receive data at the same time
             although that may not be our intention. */
    
    /* store each data byte into the array */
    for ( item = 0; item < size; item++, read++ )
    {
        /* send dummy data (this allows us to receive data on MISO) */
        *( uint8_t * )( &( SPI2->DR ) ) = 0U;

        /* wait for RX buffer to receive data */
        while ( ( SPI2->SR & SPI_SR_RXNE ) != SPI_SR_RXNE )
        {
            /* do nothing */
        }

        /* store received byte (this also clears
        the SPI2 RX buffer not empty flag) */
        *read = ( uint8_t )( SPI2->DR );
    }
}
