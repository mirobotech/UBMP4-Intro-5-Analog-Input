/*==============================================================================
 File:  SimpleSerial.h
 Date:  May 16, 2023
 
 CHRP4 Simple serial output function prototypes
 
 Function prototypes for software functions to create RS-232 style serial output
 which is useful for debugging programs using an oscilloscope with serial bus
 decoding or a serial terminal program.
==============================================================================*/

/** ** *
 * Function: void H1_serial_config(void)
 * 
 * Configure H1 for serial output and set the output pin high (idle state).
 */
void H1_serial_config(void);

/**
 * Function: void H1_serial_write(unsigned char)
 * 
 * Write one byte of serial data out to header H1.
 */
void H1_serial_write(unsigned char);
