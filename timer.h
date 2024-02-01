/**
 * @file      timer.h
 * @author    Julio Cesar Bernal Mendez
 * 
 * @brief     This file contains the function prototypes for the 16-bit general purpose timer 3 of the Nucleo Board.
 *            Everything was designed for the STM32F070RBT6 Nucleo Board from ST Microelectronics.
 * 
 * @version   1.0
 * @date      2024-01-26
 * 
 * @copyright This project was created for learning purposes only.
 */

#ifndef TIMER_H
#define TIMER_H

    /* Macro to enable TIM3 clock in the RCC */
    #define TIM3_CLK_ENBL()    (RCC->APB1ENR |= RCC_APB1ENR_TIM3EN)

    /* TIM3 initialization function */
    void TIM3_Init( void );

    /* TIM3 microseconds delay function */
    void TIM3_Delay_us( uint32_t us );

#endif
