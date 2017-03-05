
#include <xc.h>
#include "mcu-cpu.h"
#include "pins-b.h"
#include "main.h"
#include "spi.h"

StatusRecU statusRec;
#define STATUS_REC_IDLE  -2
#define STATUS_REC_START -1
int8_t  statusRecIdx, statusRecIdx;

void sendStateByte() {
  if(SPI_SS) SSP1BUF = typeState | 1; // status is always 1
}

void sendStatusRecByte() {
  char outbuf = 0;
  if (statusRecIdx == STATUS_REC_START) {
    // state byte before rec
    sendStateByte();
    statusRecIdx = statusRecIdx = 0;
  }
  else if(statusRecIdx == STATUS_SPI_BYTE_COUNT_MIN) {
    // state byte after rec
    sendStateByte();
    statusRecIdx == STATUS_REC_IDLE;
  }
  else {
    // pack every 3 8-bit statusRec bytes into 4 6-bit spi bytes 
    switch(statusRecIdx % 4) {
      case 0: 
        outbuf = (typeData | (statusRec.bytes[statusRecIdx] >> 2));
        break;
      case 1: {
        char left2 = (statusRec.bytes[statusRecIdx] & 0x03) << 4;
        statusRecIdx++;
        outbuf = typeData | left2 | 
                  ((statusRec.bytes[statusRecIdx]   & 0xf0) >> 4);
        break;
      }
      case 2: {
        char left4 = (statusRec.bytes[statusRecIdx] & 0x0f) << 2;
        statusRecIdx++;
        outbuf = typeData | left4 | 
                  ((statusRec.bytes[statusRecIdx]   & 0xc0) >> 6);
        break;
      }
      case 3:
        outbuf = typeData | (statusRec.bytes[statusRecIdx] & 0x3f);
        statusRecIdx++;
        break;
    }
    statusRecIdx++;
  }
  if(outbuf && SPI_SS) SSP1BUF = outbuf;
}

void initSpi() {
  APFCONbits.SSSEL = 0;  // ss is on RC3
  
  SPI_SS_TRIS       = 1;  // SPI TRIS select input (SS)
  SPI_CLK_TRIS      = 1;  // SPI TRIS clock input  (SCK)
  SPI_DATA_IN_TRIS  = 1;  // SPI TRIS data input   (MOSI)
  SPI_DATA_OUT_TRIS = 0;  // SPI TRIS data output  (MISO)

  SSP1CON1bits.SSPM  = 4; // mode: spi slave with slave select enabled
  SSP1STATbits.SMP   = 0; // input sample edge (must be zero for slave mode)
  SSP1CON1bits.CKP   = 0; // 0: xmit clk low is idle
  SSP1STATbits.CKE   = 1; // in/out clk edge (1: active -> idle, high to low)
  SSP1CON3bits.BOEN  = 0; // disable buffer input overflow check (SSPOV))
  
  /* From datasheet: Before enabling the module in SPI Slave mode, the clock
   line must match the proper Idle state (CKP) */
  while(SPI_CLK);
  
  // start on byte boundary
  while(!SPI_SS);
  
  SSP1CON1bits.SSPOV = 0; // clear errors
  WCOL               = 0;
  BF                 = 0; // spi buffer full flag
  SSP1CON1bits.SSPEN = 1; // enable SPI
}

void spiLoop() {
  while(1) {
    WCOL = 0;
    if(BF) {
      // got a byte
      char byte = SSP1BUF; // also clears BF
      switch (byte & 0xc0) {
        case Z_SET_CURL_CMD: 
          PWM1DCL = byte << 6;   // d1-d0 of duty cycle are in d0-d1 of spi byte
          break;
          
        case Z_SET_CURH_CMD: 
          PWM1DCH = byte & 0x3f; // d7-d2 of duty cycle are in d5-d0 of spi byte
          break;
          
        default:
          if(byte == statusCmd) statusRecIdx = STATUS_REC_START;
          break;
      }
      if(statusRecIdx != STATUS_REC_IDLE) sendStatusRecByte();
      else sendStateByte();
    }
  }
}

