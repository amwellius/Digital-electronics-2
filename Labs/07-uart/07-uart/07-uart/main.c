/***********************************************************************
 * 
 * Analog-to-digital conversion with displaying result on LCD and 
 * transmitting via UART.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2018-Present Tomas Fryza
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
#include "uart.h"           // Peter Fleury's UART library

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Use Timer/Counter1 and start ADC conversion four times 
 *           per second. Send value to LCD and UART.
 * Returns:  none
 **********************************************************************/

/** @brief  Size of the circular receive buffer, must be power of 2
 *
 *  You may need to adapt this constant to your target and your application by adding
 *  CDEFS += -DUART_RX_BUFFER_SIZE=nn to your Makefile.
 */
#ifndef UART_RX_BUFFER_SIZE
# define UART_RX_BUFFER_SIZE 64
#endif

/** @brief  Size of the circular transmit buffer, must be power of 2
 *
 *  You may need to adapt this constant to your target and your application by adding
 *  CDEFS += -DUART_TX_BUFFER_SIZE=nn to your Makefile.
 */
#ifndef UART_TX_BUFFER_SIZE
#define UART_TX_BUFFER_SIZE 64
#endif

#ifndef F_CPU
#define F_CPU 16000000
#endif


int main(void)
{
    // Initialize LCD display
    lcd_init(LCD_DISP_ON);
    lcd_gotoxy(1, 0); lcd_puts("value:");
    lcd_gotoxy(3, 1); lcd_puts("key:");
    lcd_gotoxy(8, 0); lcd_puts("a");    // Put ADC value in decimal
    lcd_gotoxy(13,0); lcd_puts("b");    // Put ADC value in hexadecimal
    lcd_gotoxy(8, 1); lcd_puts("c");    // Put button name here

    // Configure ADC to convert PC0[A0] analog value
    // Set ADC reference to AVcc
    ADMUX = ADMUX | (1<<REFS0);
    ADMUX = ADMUX &~ (1<<REFS1);        //0
    
    // Set input channel to ADC0
    ADMUX = ADMUX &~ (1<<MUX3);
    ADMUX = ADMUX &~ (1<<MUX2);
    ADMUX = ADMUX &~ (1<<MUX1);
    ADMUX = ADMUX &~ (1<<MUX0);
    
    // Enable ADC module
    ADCSRA = ADCSRA | (1<<ADEN);
    
    // Enable conversion complete interrupt
    ADCSRA = ADCSRA | (1<<ADIE);

    // Set clock prescaler to 128
    ADCSRA = ADCSRA | (1<<ADPS2);
    ADCSRA = ADCSRA | (1<<ADPS1);
    ADCSRA = ADCSRA | (1<<ADPS0);
    

    // Configure 16-bit Timer/Counter1 to start ADC conversion
    // Set prescaler to 262 ms and enable overflow interrupt
    TIM1_overflow_262ms();
    TIM1_overflow_interrupt_enable();   

    // Initialize UART to asynchronous, 8N1, 9600
    uart_init(UART_BAUD_SELECT(9600, F_CPU));

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
 * Purpose:  Use single conversion mode and start conversion four times
 *           per second.
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{
    // Start ADC conversion
    /*
    static uint8_t number_of_overflows = 0;
    number_of_overflows++;
    if (number_of_overflows >= 1)
    {
        */
        ADCSRA = ADCSRA | (1<<ADSC);
   // }    

}

/**********************************************************************
 * Function: ADC complete interrupt
 * Purpose:  Display value on LCD and send it to UART.
 **********************************************************************/
ISR(ADC_vect)
{
    uint16_t value = 0;
    char lcd_string[4] = "0000";
    
    //ADC je 16 bitova hodnota ktoru on precita naraz
    value = ADC;                  // Copy ADC result to 16-bit variable
    itoa(value, lcd_string, 10);  // Convert decimal value to string 

    // A value displaying
    lcd_gotoxy(8,0);
    lcd_puts("    ");
    lcd_gotoxy(8,0);     
    lcd_puts(lcd_string);
    //uart communication
    uart_puts(lcd_string);
    uart_puts("\r\n");    
    
    // B value
    value = ADC;                  // Copy ADC result to 16-bit variable
    itoa(value, lcd_string, 16);
    
    lcd_gotoxy(13,0);
    //display clearing
    lcd_puts("   ");
    lcd_gotoxy(13,0);    
    lcd_puts(lcd_string);
    //uart communication
    uart_puts(lcd_string);
    uart_puts("\r\n");
        
    // C value
    value = ADC;                  // Copy ADC result to 16-bit variable
    itoa(value, lcd_string, 2);
    lcd_gotoxy(8,1);
    lcd_puts("        ");
    //uart communication
    uart_puts(lcd_string);
    uart_puts("\r\n");
    
    switch(value){
        case 0 ... 90       : lcd_gotoxy(8,1); lcd_puts("right"); uart_puts("right"); uart_puts("\r\n");  break;        //0
        case 91 ... 200     : lcd_gotoxy(8,1); lcd_puts("up"); uart_puts("up"); uart_puts("\r\n");  break;           //99
        case 201 ... 400    : lcd_gotoxy(8,1); lcd_puts("down"); uart_puts("down"); uart_puts("\r\n");  break;         //255
        case 401 ... 600    : lcd_gotoxy(8,1); lcd_puts("left"); uart_puts("left"); uart_puts("\r\n");  break;         //408
        case 601 ... 1000   : lcd_gotoxy(8,1); lcd_puts("select"); uart_puts("select"); uart_puts("\r\n");  break;       //638
        case 1001 ... 1050  : lcd_gotoxy(8,1); lcd_puts("none"); uart_puts("none"); uart_puts("\r\n");  break;         //1023
        default:lcd_puts("       ");
    }      
}
