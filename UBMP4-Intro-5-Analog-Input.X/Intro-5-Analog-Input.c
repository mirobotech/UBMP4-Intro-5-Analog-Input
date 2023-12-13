/*==============================================================================
 Project: Intro-5-Analog-Input          Activity: mirobo.tech/intro-5
 Date:    December 12, 2023
 
 This introductory programming activity for the mirobo.tech CHRP4 and UBMP4
 demonstrates analog input, number base conversion, and the use of a simple,
 external serial output function library. (Serial output is a useful technique
 for debugging using a digital oscilloscope with a serial decode function, or
 a digital logic analyzer.)
 
 Additional program analysis activities investigate the use and operation of
 analog constants and functions in the included files, explore bit-wise logical
 operations for testing and selectively modifying bits, utilize analog data
 in other functions, and develop strategies for receiving serial data.
==============================================================================*/

#include    "xc.h"              // Microchip XC8 compiler include file
#include    "stdint.h"          // Include integer definitions
#include    "stdbool.h"         // Include Boolean (true/false) definitions

#include    "UBMP4.h"           // Include UBMP4 constants and functions
#include    "Simple-Serial.h"   // Include simple serial functions

// TODO Set linker ROM ranges to 'default,-0-7FF' under "Memory model" pull-down.
// TODO Set linker code offset to '800' under "Additional options" pull-down.

// ASCII character code definitions
#define LF      10              // ASCII line feed character code
#define CR      13              // ASCII carriage return character code

// Program variable definitions
unsigned char rawADC;           // Raw ADC conversion result

// Decimal digit variables used by binary to decimal conversion function
unsigned char dec0;             // Decimal digit 0 - ones digit
unsigned char dec1;             // Decimal digit 1 - tens digit
unsigned char dec2;             // Decimal digit 2 - hundreds digit

// Convert an 8-bit binary number to 3 decimal digits
void bin_to_dec(unsigned char bin)
{
    dec0 = bin;             // Store number in ones digit
    dec1 = 0;               // Clear tens digit
    dec2 = 0;               // Clear hundreds digit
    
    // Count hundreds digits in dec2 variable
    while(dec0 >= 100)
    {
        dec2 ++;
        dec0 = dec0 - 100;
    }
    // Count tens digits in dec1 variable, dec0 will contain remaining ones
    while(dec0 >= 10)
    {
        dec1 ++;
        dec0 = dec0 - 10;
    }
}

int main(void)
{
    OSC_config();               // Configure internal oscillator for 48 MHz
    UBMP4_config();             // Configure I/O for on-board UBMP4 devices
    ADC_config();               // Configure ADC and enable input on Q1
    H1_serial_config();         // Prepare for serial output on H1
        
    // If Q1 and U2 are not installed, all PORTC outputs can be enabled for
    // debugging using a multimeter by uncommenting the line below:
    // TRISC = 0b00000000;
    
    // Enable the on-die temperature sensor and set high operating Vdd range
    FVRCON = FVRCON | 0b00110000;
    
    // Select the on-die temperature indicator module as the ADC input and wait
    // for the recommended acquisition time before A-D conversion
    ADC_select_channel(ANTIM);
    __delay_us(200);
    
    // If the components are installed, turn on LED D6 and select
    // phototransistor Q1 as the ADC input
    // D6 = 1;                      // Turn LED D6 on and...
    // ADC_select_channel(ANQ1);    // read the light level on Q1
    
    while(1)
    {
        // Read selected ADC input and output the result on the PORTC pins
        rawADC = ADC_read();
        LATC = rawADC;
              
        // Add serial write code from the program analysis activities here:
        
        __delay_ms(100);
        
        // Reset the microcontroller and start the bootloader if SW1 is pressed.
        if(SW1 == 0)
        {
            RESET();
        }
    }
}

/* Learn More -- Program Analysis Activities
 * 
 * 1.   The microcontroller's TRISC register is configured by the UBMP4_config()
 *      function and controls the data direction (input or output) of the PORTC
 *      I/O pins. A value of 0 in any TRIS register bit configures the 
 *      corresponding pin to be an output (think of 0 being like the 'o', in
 *      output), while a value of 1 sets the pin as an input (1 is like the 'i',
 *      in input).
 * 
 *      What is the original value of TRISC set in the UBMP4_config() function?
 *      How would the statement:
 * 
 *      TRISC = 0b00000000;
 * 
 *      in the main() function change the function of the PORTC I/O pins?
 * 
 * 2.   This program converts an analog input into an 8-bit digital value
 *      representing the input voltage, and then outputs each bit of the digital
 *      value to the PIC16F1459 PORTC output pins. A voltmeter can be used to
 *      read the digital result directly from the pins, except for pins LATC2
 *      and LATC3 (pins 14 and 7 on the chip -- see the schematic for all of the
 *      pin numbers). The pins for LATC2 and LATC3 are set as inputs since they
 *      connect to IR demodulator and Q1 phototransistor on the UBMP4 circuit.
 * 
 *      If your UBMP4 has its Q1 phototransistor or U2 IR demodulator installed,
 *      leave the TRISC register as set by the UBMP4_config() function. If the
 *      phototransistor/ambient light sensor and IR demodulator are not
 *      installed, the TRISC register can be set to output digital data on all
 *      of the microcontrollers LATC pins by clearing it:
 
    TRISC = 0b00000000;
 
 *      Read the conversion result by connecting a voltmeter between ground (the
 *      negative BT1 pad, or one of the H1-H8 header pins marked with the ground
 *      symbol on the legend on the circuit board) and each header signal pin.
 * 
 *      RC7 on H8 is the most significant bit of the result, and RC0 on H1 is
 *      the least significant bit. Starting from H8, record all 8 bits of the
 *      analog value, representing voltages close 5V as 1, and voltages near
 *      0V as 0. What was your binary result? What is the value after it is
 *      converted to decimal?
 * 
 * 3.   The 'LATC = rawADC;' statement in the program outputs the digital value
 *      corresponding to the analog input to the microcontroller's output pins.
 * 
 *      Since the 4 most significant bits of PORTC are also physically connected
 *      to LEDs D2-D5 on the circuit, the LEDs will light to represent the first
 *      four bits of data. Do the lit LEDs match your measured result, above?
 * 
 *      The lower 4 bits of the result can be displayed on the LEDs instead of
 *      the upper 4 bits by using a bit shift operator to shift each bit in the
 *      conversion result variable four bits to the left before outputting the
 *      value to LATC. (The upper 4 bits will be lost when doing this shift.)
 * 
 *      Update the LATC expression to add the bit shift operator, below. Rebuild
 *      the program and run it again.
 * 
        LATC = rawADC << 4;     // Shift result 4 bits left to display low nybble
 * 
 *      This is advantagious since the least significant bits of a conversion
 *      result will change most often as an analog input voltage changes. If you
 *      are using the built-in temperature sensor as the input, try heating the
 *      microcontroller with your touch, or by cradling the circuit board in
 *      your hands until you see one or more bits changing on the LEDs.
 * 
 * 4.   The PIC16F1459 microcontroller has 9 analog inputs shared with its
 *      digital I/O pins. Pins used for analog input must be explicity enabled
 *      as both input pins, and also as analog pins. Examine the ADC_config()
 *      function and explain how it configures the Q1 pin for analog input.
 * 
 *      (Amusingly, the microcontroller starts up from a reset with these pins
 *      configured as analog inputs already, and this programs's UBMP4_config()
 *      function reconfigures them as digital I/O. You can explore the function
 *      to see how this is done.)
 * 
 * 5.   One of the 9 analog inputs on the PIC16F1459 can be connected into the
 *      single ADC (analog-to-digital coverter) through an analog multiplexer
 *      (or mux). After switching the mux to a different input, a short delay
 *      is necessary to allow the input potential to 'settle' (by charging an
 *      internal capacitor to the new input voltage) before the conversion
 *      is started. The function call:
 * 
        ADC_select_channel(ANTIM);
 * 
 *      selects the microcontroller's on-board Temperature Indicator Module
 *      (ANTIM) as the ADC input, but does not add a settling delay. What other
 *      analog inputs are available on UBMP4? (Hint: see the UBMP4.h file)
 * 
 *      The ADC_select_channel() function does not add a delay after switching
 *      a new input to the ADC, but there is another function that switches
 *      inputs, adds a delay, and performs the analog to digital conversion.
 *      What is this function called? Why do you think it was not used in this
 *      program?
 * 
 * 6.   This program contains the bin_to_dec() function which demonstrates a
 *      simple method of converting an 8-bit binary value to three decimal
 *      digits. Can you figure out how it works? Explain the function, or create
 *      a flow chart to describe its operation.
 * 
 * 7.   The Simple-Serial.c file includes two serial functions: one to configure
 *      a header I/O pin for serial output, and the second to write RS-232
 *      formatted serial data to the H1 header pin. This will allow a logic
 *      analyser or an oscilloscope to read the value of the serial data more
 *      conveniently and quickly than by us successivley probing each of the
 *      microcontroller output pins using a voltmeter.
 * 
 *      The format of the serial data is straightforward to understand. When no
 *      data is being transmitted, the serial line is 'idle', represented by a
 *      high voltage, or '1' logic level. (This is usually true at the circuit's
 *      TTL logic level, but actual RS-232 transmission uses a level shifter to
 *      make logic 1 a negative voltage. We'll use the circuit's TTL levels.)
 * 
 *      Every data transmission is preceeded by a low voltage, or logic 0 level
 *      'Start' bit. Next, the data bits themselves are transmitted in order
 *      from LSB (least-significant bit) to MSB (most-significant bit). The
 *      transmission is ended with a logic 1 level 'Stop' bit.
 * 
 *      Each data bit, 0 or 1, is transmitted as a low or high voltage level,
 *      respectively, and each bit remains at its logic level for the entire bit
 *      duration (this is commonly known as NRZ, or non-return-to-zero format).
 * 
 *      The duration of each bit is a reciprocal of the data rate. For example
 *      at the commonly-used 9600 bps (bits-per-second) data rate, each bit is
 *      1/9600s, or roughly 104us (microseconds) in length.
 * 
 *      If you have access to an oscilloscope or logic analyzer, add the
 *      following code to the program (below the LATC output statement) to
 *      enable serial output. Then, probe H1 to view the serial data:
 
        // Write the ADC result to H1 as binary serial data
        H1_serial_write(rawADC);
        
 *      Can you record or capture the digital serial data? What is the digital 
 *      value that was transmitted?
 * 
 * 8.   Rather than interpreting or converting the binary value yourself, this
 *      next block of code can be added to your program to convert the analog
 *      result to three decimal digits, transmit each of the digits serially as
 *      ASCII characters, and add the character codes signifying the end of a
 *      line of text (CR) and the start of a new line of text (LF). Logic
 *      analysers or digital oscilloscopes may be able to decode these digits,
 *      or the output can also be connected to a serial terminal to display the
 *      text on screen. (Depending on the requirements of the serial terminal,
 *      both of the CR and LF characters may not be needed.)
 
        // Convert ADC result to decimal and write each digit serially in ASCII
        bin_to_dec(rawADC);
        H1_serial_write(dec2 + 0x30);   // Convert each digit to ASCII
        H1_serial_write(dec1 + 0x30);
        H1_serial_write(dec0 + 0x30);
        H1_serial_write(CR);            // Carriage return character
        H1_serial_write(LF);            // Line feed character
                
 * 
 * 9.   The ADC_select_channel(), ADC_read_channel(), H1_serial_config(), and
 *      H1_serial_write() functions all make use of bit-wise logical operators.
 *      Below is the AND (&) operator used by the H1_serial_config() function:
 * 
        TRISB = TRISB & 0b11101111;
 * 
 *      Can you determine what this statement is doing, and why an AND operation
 *      is being used instead of just over-writing TRISB with a new value?
 * 
 * 10.  The data transmission loop of the H1_serial_write() function is shown
 *      below:
 * 
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
 *  
 *      Explain how the AND operation used in the if condition in this function
 *      can determine the state of the least significant data bit.
 * 
 * 11.  The AND operation examined in 10, above, only tests the state of the
 *      least significant bit of the data to be transmitted. How are the
 *      remaining seven bits of the data transmitted in this same loop?
 * 
 * 12.  The ADC_select_channel() function uses the following two instructions to
 *      switch the ADC's input mux to the selected analog input.
 * 
    ADCON0 = (ADCON0 & 0b10000011); // Clear channel select (CHS) bits by ANDing
    ADCON0 = (ADCON0 | channel);	// Set channel by ORing with channel constant
 * 
 *      How will the ADCON0 register be changed after calling the function with
 *      the ADQ1 definition from the UBMP4.h file?
 * 
 * Programming Activities
 * 
 * 1.   The bin_to_dec() function converts a single byte into three decimal
 *      digits. Step 8 in the program analysis activities, above, converts each
 *      of these numeric digits to the ASCII code representing the digit. Can
 *      you make a similar bin_to_ASCII() function that would eliminate the need
 *      to manually offset the values to ASCII as was done here?
 * 
 * 2.   Does your UBMP4 have ambient light sensor or phototransistor Q1
 *      installed, along with LED6 (if the phototransistor is installed)?
 *      If so, it may be easier to use one of the optical devices as the analog
 *      input devices instead of the temperature module. Un-comment the
 *      statements to turn LED D6 on and to select the Q1 phototransistor input
 *      in the program code.
 
    D6 = 1;                      // Turn on LED D6 to..
    ADC_select_channel(ANQ1);    // read the light level on Q1
 
 *      Unfortunately, it won't be possible to use a multimeter to read the
 *      result of the conversion from the header pins now because one of the
 *      pins is shared with the optical input circuit. Writing to LATC will
 *      prevent the optical sensor from being read. We know that a serial
 *      device could be used for debugging instead, but one may not be readily
 *      available.
 * 
 *      Fortunately, there is another way to display the output data on the
 *      built-in LEDs without overwriting the port pin used to get input from
 *      the phototransistor circuit by using logical operations. Replace the
 *      line 'LATC = rawADC;' in your program with the following lines of code
 *      to display the 8-bit ADC data on the LEDs four bits at a time:
 
        // Output the ADC result on the LEDs 4bits at a time
        LATC = (LATC & 0b00001111) | (rawADC & 0b11110000);
        __delay_ms(500);
        LATC = (LATC & 0b00001111) | ((rawADC << 4) & 0b11110000);
        __delay_ms(1000);
        LATC = 0;
        __delay_ms(100);

 *      Can you figure out how these statements work? Map out their operation
 *      and verify that none of the data on the least significant four pins of
 *      PORTC (RC0 - RC3) are changed by this code.
 * 
 *      Using logical AND and OR operators allows the selective modification of
 *      any bit(s) within memory registers and is a powerful technique to
 *      understand.
 * 
 * 3.   Create a program to light an LED when a specific light threshold is
 *      crossed. Start by determining the analog output level corresponding
 *      a medium intensity of light using either the code modifications from
 *      step 2, above, or a serial decoder.
 * 
 *      Next, set bright and dark thresholds that are a few steps above and
 *      below the number measured as your medium light level. This implements
 *      a simple form of hysteresis (a separation between the getting-brighter
 *      and getting-darker transitions) to ensure more accurate destinction of
 *      light and dark. 
 * 
 *      Then, write the program so that an LED turn on when the sensed light
 *      is brighter than the bright level, and turn off when the sensed light
 *      is darker than the dark level.
 * 
 * 4.   Analog input values can be fed directly into output processes. Create a
 *      program that produces a PWM output proportional to an analog value, or
 *      that creates a tone having a pitch proportional to an analog value.
 * 
 * 5.   If you have an oscilloscope available, investigate how fast you can get
 *      the serial output function to transmit data. Try setting the bit delays
 *      inside the serial write function to 1 microsecond of delay, instead of
 *      104 and 103 as used in the current function, and try the program. Does
 *      it work the way it should? Is each bit actually 1us in duration? What do
 *      you think is affecting the length of the bits?
 * 
 * 6.   Creating a serial data transmission function is relatively straight-
 *      forward since the bits are simply output sequentially. Receiving serial
 *      data reliably can be done in a very similar way but needs to account for
 *      possible differences in clock timing between the transmitting and
 *      receving microcontrollers. Think about how you would create a function
 *      to receive serial data that takes slight clock timing differences into
 *      account. Create the pseudocode or a flowchart describing a serial
 *      receive function.
 * 
 * 7.   Transmitting serial data in this program involves isolating one bit of
 *      a variable at a time using a bit-wise logical operator, outputting the 
 *      bit for the proper time, bit-shifting the data, and repeating the entire
 *      process for the remainder of the data bits. Receving serial data happens
 *      in the opposite way, first sensing and validating each bit, and then
 *      re-assembling the bits by shifting them into a variable, and repeating
 *      the process until all of the bits have been received.
 * 
 *      Try to create a function, or just the main code loop of a function, that
 *      successively reads serial data bits from an input pin and assembles the
 *      values into an 8-bit variable.
 * 
 * 8.   Microcontrollers such as the PIC16F1459 used in this circuit have built-
 *      in EUSART hardware to transmit and receive serial data without using
 *      software functions. What would the advantage of using the EUSART to
 *      transmit or receive serial data be? Why do you think it was not used in
 *      these examples for this circuit?
 */
