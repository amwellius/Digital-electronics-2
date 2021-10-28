/***********************************************************************
 * 
 * Stopwatch with LCD display output.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2017-Present Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include "lcd.h"            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for conversion function


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Update stopwatch value on LCD display when 8-bit 
 *           Timer/Counter2 overflows.
 * Returns:  none
 **********************************************************************/

uint8_t customChar2[8] = {
    0b11111,
    0b10010,
    0b10100,
    0b11000,
    0b10100,
    0b10010,
    0b10001,
    0b11111
};

uint8_t customChar1[8] = {
    0b10011,
    0b10111,
    0b11011,
    0b11000,
    0b01000,
    0b10111,
    0b11001,
    0b00011
};


int main(void)
{
    // Initialize LCD display
    lcd_init(LCD_DISP_ON);

/*
    // Put string(s) at LCD display
    lcd_gotoxy(1, 1);
    lcd_puts("CZ Shadow 2");
    lcd_putc('!');
    
    lcd_gotoxy(1, 0);
    lcd_puts("CZ UB Czechia !");
*/
    lcd_gotoxy(1, 0);
    lcd_puts("0 :");
    
    // Configure 8-bit Timer/Counter2 for Stopwatch
    // Set the overflow prescaler to 16 ms and enable interrupt
        TIM2_overflow_16ms();
        TIM2_overflow_interrupt_enable();

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
 * Function: Timer/Counter2 overflow interrupt
 * Purpose:  Update the stopwatch on LCD display every sixth overflow,
 *           ie approximately every 100 ms (6 x 16 ms = 100 ms).
 **********************************************************************/
ISR(TIMER2_OVF_vect)
{
    static uint8_t number_of_overflows = 0;
    static uint8_t tens = 0;        // Tenths of a second
    static uint8_t secs = 0;        // Seconds
    static uint8_t seconds = 0;
    static uint8_t minutes = 0;
    //char lcd_string[2] = "  ";      // String for converting numbers by itoa()
    char lcd_string_tens[2] = "  ";
    char lcd_string_secs[3] = "  ";
    char lcd_string_mins[3] = "  ";

    number_of_overflows++;
    if (number_of_overflows >= 6)
    {
        // Do this every 6 x 16 ms = 100 ms
        number_of_overflows = 0;
        //TENS
        tens++;
        if (tens == 9) {
            tens=0;
        }          
        //SECS/MINS
        secs++;
        if (secs==10){
            seconds++;
            secs=0;
        }
        if (seconds == 60) {
            minutes++;
            if (minutes == 60) minutes=0;
            seconds = 0;
        }    
        
        // Convert decimal value to string
        itoa(tens, lcd_string_tens, 10);     
        itoa(seconds, lcd_string_secs, 10);
        itoa(minutes, lcd_string_mins, 10);            
        
        // Show decimal dot and tens                
        lcd_gotoxy(6, 0);
        lcd_puts(".");
        lcd_gotoxy(7, 0);
        lcd_puts(lcd_string_tens);
            

        if (seconds < 10){
            lcd_gotoxy(4, 0);
            lcd_puts("0");
            lcd_gotoxy(5, 0);
            lcd_puts(lcd_string_secs);
        }
        else{

            lcd_gotoxy(4, 0);
            lcd_puts(lcd_string_secs);                
        }
        if (minutes < 10) {
            lcd_gotoxy(1, 0);
            lcd_puts("0");
            lcd_gotoxy(2, 0);
            lcd_puts(lcd_string_mins);
        }
        else {
            lcd_gotoxy(1, 0);
            lcd_puts(lcd_string_mins);
        }
        // Else do nothing and exit the ISR
   
    }     
           
}