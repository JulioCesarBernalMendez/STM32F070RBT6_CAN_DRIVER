/**
 * @file      timer.c
 * @author    Julio Cesar Bernal Mendez
 * 
 * @brief     This file contains the High-Level implementations for the 16-bit general purpose timer 3 of the Nucleo Board.
 *            Everything was designed for the STM32F070RBT6 Nucleo Board from ST Microelectronics.
 * 
 * @version   1.0
 * @date      2024-01-26
 * 
 * @copyright This project was created for learning purposes only.
 */

#include <stdint.h>
#include "stm32f0xx.h"
#include "timer.h"

/**
 * @brief Initialize the Nucleo Board's 16-bit General Purpose Timer 3 peripheral to the following parameters:
 *        - 0.5 microseconds timebase
 *        - Up-counter (0 to programmed reload value) 
 *        - Generation of an update event enabled (overflow flag in this case, used for the TIM3 delay function)
 *        - 1 microsecond reload value (timer 3 overflow flag generation)
 */
void TIM3_Init( void )
{   
    /* enable TIM3 clock */
    TIM3_CLK_ENBL();

    /* TIM3 counts up (counts from 0 up to the auto-reload value) */
    TIM3->CR1 &= ~TIM_CR1_DIR;

    /* TIM3 UEV (update event) generation enabled */
    TIM3->CR1 &= ~TIM_CR1_UDIS;

    /* TIM3 auto-reload preload disabled */
    TIM3->CR1 &= ~TIM_CR1_ARPE;
    
    /* TIM3 prescaler.
       CK_CNT period = (1 / PCLK)(PSC + 1) = (1 / 48MHz)(23 + 1) = 0.5us
       with PCLK = 48MHz (refer to SystemInit() in system_stm32f0xx.c) */
    TIM3->PSC = 0x17U;

    /* TIM3 reload value, when TIM3 counts to ARR + 1 (= 1 + 1 = 2), (0.5us)(2) = 1us has passed
       NOTE: this reload value is necessary for the TIM3_Delay_us() function */
    TIM3->ARR = 0x01U;
}

/**
 * @brief Generate the specified microseconds delay using the Nucleo Board's Timer 3 as a time source.
 * 
 * @param us microseconds delay
 */
void TIM3_Delay_us( uint32_t us )
{
    uint32_t wait;

    /* clear TIM3 counter */
    TIM3->CNT = 0x00U;

    /* enable TIM3 peripheral */
    TIM3->CR1 |= TIM_CR1_CEN;

    /* generate 'us' microseconds delay */
    for ( wait = 0U; wait < us; wait++ )
    {
        /* wait for TIM3 update interrupt flag (1us) */
        while ( ( TIM3->SR & TIM_SR_UIF ) != TIM_SR_UIF )
        {
            /* do nothing */
        }

        /* clear TIM3 update interrupt flag */
        TIM3->SR &= ~TIM_SR_UIF;
    }

    /* disable TIM3 peripheral */
    TIM3->CR1 &= ~TIM_CR1_CEN;
}
