/**
 * @file      main.h
 * @author    Julio Cesar Bernal Mendez
 * @brief     This file includes the necessary headers for the different peripherals to work properly.
 * @version   1.0
 * @date      2024-01-26
 * 
 * @copyright This project was created for learning purposes only.
 */

#ifndef MAIN_H
#define MAIN_H

    #include <stdint.h>
    #include "stm32f0xx.h"
    #include "spi.h"
    #include "timer.h"
    #include "can.h"

    /* STM32F070RB Nucleo board's user button function */
    void Board_Button_Init( void );

#endif
