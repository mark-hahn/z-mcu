
// Main file for Z add-on module to XY, mcu: PIC16F1503 

#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

#include <xc.h>
#include "mcu-cpu.h"
#include "pins-b.h"
#include "main.h"
#include "spi.h"


void main(void) {
  INTCON = 0; // all ints off all the time
  
  statusRec.rec.len       = STATUS_SPI_BYTE_COUNT_MIN;
  statusRec.rec.type      = STATUS_REC;
  statusRec.rec.mfr       = MFR;  
  statusRec.rec.prod      = PROD; 
  statusRec.rec.vers      = VERS; 

  ANSELA = 0; // no analog inputs
  ANSELC = 0;

  initSpi();
  initPwm();

  spiLoop(); // doesn't return
}

void initPwm() {
  // timer 2 -- input to pwm module
  T2CON = 4;   // prescaler and postscaler set to 1:1, TMR2ON = 1
  PR2 = 0x3F;  // period reg, 78.12 KHz
  
  // pwm module 1
  PWM_LAT  = 0; // start with pin low
  PWM_TRIS = 0; // RC5 is output
  PWM1DCL             = 0;  // d7-d6 are 2 lsb bits of duty cycle
  PWM1DCH             = 0;  // d5-d0 are 6 msb bits, both set by spi commands
  PWM1CONbits.PWM1OE  = 1;  // pwm output on RC5
  PWM1CONbits.PWM1POL = 0;  // active-high
  PWM1CONbits.PWM1EN  = 1;  // pwm enabled
}
