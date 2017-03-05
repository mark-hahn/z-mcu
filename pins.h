
#ifndef PINS_H
#define	PINS_H

// MCU pin assignments for Z add-on board for XY base

// TRIS, tri-state selects
#define SPI_SS_TRIS       TRISC3 // SPI Slave Select input, SS
#define SPI_CLK_TRIS      TRISC0 // SPI Clock input, SCLK
#define SPI_DATA_IN_TRIS  TRISC1 // SPI Data input,  MOSI
#define SPI_DATA_OUT_TRIS TRISC2 // SPI Data output, MISO

// pin input values unlatched
#define SPI_SS            RC3    // SPI Slave Select level, no latch
#define SPI_CLK           RC0    

// PWM output
#define PWM_TRIS          TRISC5 
#define PWM_LAT           LATC5 

#endif	/* PINS_B_H */

 