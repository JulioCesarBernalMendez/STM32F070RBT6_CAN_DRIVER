/**
 * @file      spi.h
 * @author    Julio Cesar Bernal Mendez
 * 
 * @brief     This file contains the function prototypes for the SPI 1 and 2 peripherals of the Nucleo Board.
 *            Everything was designed for the STM32F070RBT6 Nucleo Board from ST Microelectronics.
 * 
 * @version   1.0
 * @date      2024-01-26
 * 
 * @copyright This project was created for learning purposes only.
 */

#ifndef SPI_H
#define SPI_H

    #include <stdint.h>
    #include "stm32f0xx.h"
    #include "gpio.h"
    
    /* Macros to enable SPI1 and SPI2 clocks in the RCC */
    #define SPI1_CLK_ENBL()    (RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
    #define SPI2_CLK_ENBL()    (RCC->APB1ENR |= RCC_APB1ENR_SPI2EN)

    /* SPI1 and SPI2 chip select disabling and enabling functions */
    void SPI1_CS_Disable( void );
    void SPI2_CS_Disable( void );
    void SPI1_CS_Enable( void );
    void SPI2_CS_Enable( void );

    /* SPI1 and SPI2 initialization functions */
    void SPI1_GPIO_Init( void );
    void SPI2_GPIO_Init( void );
    void SPI1_Init( void );
    void SPI2_Init( void );

    /* SPI1 and SPI2 write and read functions */
    void SPI1_Write( uint8_t *data, uint8_t size );
    void SPI2_Write( uint8_t *data, uint8_t size );
    void SPI1_Read( uint8_t *read, uint8_t size );
    void SPI2_Read( uint8_t *read, uint8_t size );

#endif
