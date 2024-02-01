/**
 * @file      gpio.h
 * @author    Julio Cesar Bernal Mendez
 * 
 * @brief     This file contains the bit definitions for the STM32F070RBT6 board's GPIOs needed for the
 *            SPI 1 and 2 peripherals that can handle the CAN Controller Driver.
 *            Everything was designed for the STM32F070RBT6 Nucleo Board from ST Microelectronics.
 * 
 * @version   1.0
 * @date      2024-01-26
 * 
 * @copyright This project was created for learning purposes only.
 */

#ifndef GPIO_H
#define GPIO_H

    #include "stm32f0xx.h"

    /* --------------------------------- SPI1 --------------------------------- */
    /* Macro to enable GPIOA clock in the RCC */
    #define GPIOA_CLK_ENBL()      (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)

    /* GPIOx pin 5 alternate function mode bit definitions */
    #define GPIO_AFRL_AFSEL5_3    (0x01U << 23)
    #define GPIO_AFRL_AFSEL5_2    (0x01U << 22)
    #define GPIO_AFRL_AFSEL5_1    (0x01U << 21)
    #define GPIO_AFRL_AFSEL5_0    (0x01U << 20)

    /* GPIOx pin 6 alternate function mode bit definitions */
    #define GPIO_AFRL_AFSEL6_3    (0x01U << 27)
    #define GPIO_AFRL_AFSEL6_2    (0x01U << 26)
    #define GPIO_AFRL_AFSEL6_1    (0x01U << 25)
    #define GPIO_AFRL_AFSEL6_0    (0x01U << 24)

    /* GPIOx pin 7 alternate function mode bit definitions */
    #define GPIO_AFRL_AFSEL7_3    (0x01U << 31)
    #define GPIO_AFRL_AFSEL7_2    (0x01U << 30)
    #define GPIO_AFRL_AFSEL7_1    (0x01U << 29)
    #define GPIO_AFRL_AFSEL7_0    (0x01U << 28)

    /* --------------------------------- SPI2 --------------------------------- */
    /* Macro to enable GPIOB clock in the RCC */
    #define GPIOB_CLK_ENBL()      (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)

    /* GPIOx pin 13 alternate function mode bit definitions */
    #define GPIO_AFRH_AFSEL13_3   (0x01U << 19)
    #define GPIO_AFRH_AFSEL13_2   (0x01U << 18)
    #define GPIO_AFRH_AFSEL13_1   (0x01U << 17)
    #define GPIO_AFRH_AFSEL13_0   (0x01U << 16)

    /* GPIOx pin 14 alternate function mode bit definitions */
    #define GPIO_AFRH_AFSEL14_3   (0x01U << 27)
    #define GPIO_AFRH_AFSEL14_2   (0x01U << 26)
    #define GPIO_AFRH_AFSEL14_1   (0x01U << 25)
    #define GPIO_AFRH_AFSEL14_0   (0x01U << 24)

    /* GPIOx pin 15 alternate function mode bit definitions */
    #define GPIO_AFRH_AFSEL15_3   (0x01U << 31)
    #define GPIO_AFRH_AFSEL15_2   (0x01U << 30)
    #define GPIO_AFRH_AFSEL15_1   (0x01U << 29)
    #define GPIO_AFRH_AFSEL15_0   (0x01U << 28)

#endif
