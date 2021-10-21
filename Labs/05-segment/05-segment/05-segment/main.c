/***********************************************************************
 * 
 * Decimal counter with 7-segment output.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2018-Present Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/
#ifndef F_CPU
#define F_CPU 16000000
#endif
/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include "segment.h"        // Seven-segment display library for AVR-GCC
#include <util/delay.h>


uint16_t maxNumber = 0;
/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Display decimal counter values on SSD (Seven-segment 
 *           display) when 16-bit Timer/Counter1 overflows.
 * Returns:  none
 **********************************************************************/
int main(void)
{
    // Configure SSD signals
    SEG_init();
    SEG_clear();
    
    /*
    int i;
    for (i=0;i<100;i++){
        
   
    // Test of SSD: display number '3' at position 0
    SEG_update_shift_regs(0b10011111, 0b10000000);
    _delay_ms(4);
    SEG_update_shift_regs(0b10011001, 0b01000000);
    _delay_ms(4);
    SEG_update_shift_regs(0b00000001, 0b00100000);
    _delay_ms(4);
    SEG_update_shift_regs(0b00000001, 0b00010000);
    _delay_ms(4);
    }
   */
    // Configure 16-bit Timer/Counter1 for Decimal counter
    // Set the overflow prescaler to 262 ms and enable interrupt
    
    //counting time
    TIM1_overflow_1s();
    //display time
    TIM0_overflow_128us();
    TIM1_overflow_interrupt_enable();
    TIM0_overflow_interrupt_enable();
    
    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines ISRs */
    }

    // Will never reach this
    return 0;
}

/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Increment decimal counter value and display it on SSD.
 **********************************************************************/


ISR(TIMER1_OVF_vect)
{
    maxNumber++;
    if (maxNumber == 60) maxNumber = 0;
}

ISR(TIMER0_OVF_vect)
{
    //This line will only run the first time
    static uint8_t tempPosition = 0;      
    static int pow10[5] = {1, 10, 100, 1000, 10000}; 
           
    uint16_t tempValue;
    tempValue = (maxNumber % pow10[tempPosition+1]) / (pow10[tempPosition]);
    
    //segment.c will get new value
    SEG_update_shift_regs(tempValue, tempPosition);
    
    //display shifting
    tempPosition++;
    if (tempPosition == 4) tempPosition = 0;
} 
 