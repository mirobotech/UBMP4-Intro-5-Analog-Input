/*==============================================================================
 Library:   SimpleSerial
 Date:      December 12, 2023
 
 Simple 9600 bits-per-second (bps) serial output example function. Sets up 
 header H1 for RS-232-style serial data output and writes bits by manipulating
 the port pin in software instead of using the built-in hardware EUSART (the 
 EUSART's TX output is on a different header pin).
 
 Serial output is useful for monitoring data using a logic analyzer or by using
 an oscilloscope with a serial decode function. Serial output can also be used
 for communicating with another microcontroller, or older peripheral devices.
 =============================================================================*/

#include    "xc.h"              // Microchip XC8 compiler include file
#include    "stdint.h"          // Include integer definitions
#include    "stdbool.h"         // Include Boolean (true/false) definitions

#include    "UBMP4.h"           // Include UBMP4 constants and functions

// Configure H1 for serial output and set output pin high for idle state
void H1_serial_config(void)
{
    TRISB = TRISB & 0b11101111;
    H1OUT = 1;
}

// Write one byte of 9600,8,N,1 serial data (9600 bps, 8 data bits, no parity,
// and 1 stop bit) to H1
void H1_serial_write(unsigned char data)
{
    // Write the Start bit (0)
    H1OUT = 0;
    __delay_us(104);    // Delay for 1 bit time (equal to 1/9600 s)
    
    // Shift 8 data bits out LSB first
    for(unsigned char bits = 8; bits != 0; bits--)
    {
        if((data & 0b00000001) == 0)    // Check the least significant bit state
        {
            H1OUT = 0;
        }
        else
        {
            H1OUT = 1;
        }
        __delay_us(103);    // Shorter delay to account for 'for' loop overhead
        data = data >> 1;   // Prepare next bit by shifting data right 1 bit
    }
    
    // Finish the transmission by writing a Stop bit (1 - same as the idle state)
    H1OUT = 1;
    __delay_us(104);
}
