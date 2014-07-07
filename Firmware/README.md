Overview: 
This product is designed as a signal generator that can outputs square waves (3.3V high) to up to two audio jacks and two BNC connectors. It is controlled by a computer via serial communications.

Input: 1 Mini USB, 11 pin headers for unused FT232RL pins, 4 pin headers for programming MSP430
Output: 2 Audio Jacks, 2 BNC connectors

Major Internal Components:
FT232RL - receives power and USB input from the computer, provides power and converts USB to serial communications between computer and MSP430, unused pins connected to pin headers
MSP430G2553 - receives power and serial input from FT232RL, outputs square waves and returns input to FT232RL, can be programmed using pin headers
TXLED - indicates when FT232RL is sending serial signals to MSP430
RXLED - indicates when FT232RL is receiving serial signals from MSP430
PWR_LED - indicates that the MSP430 is receiving power, and that the whole system is powered on
CAM1_LED - flashes when a square wave is output to corresponding audio jack, off otherwise
CAM2_LED - flashes when a square wave is output to corresponding audio jack, off otherwise
SYNC1_LED - flashes when a square wave is output to corresponding BNC connector, off otherwise
SYNC2_LED - flashes when a square wave is output to corresponding BNC connector, off otherwise

Initial MSP430 code provided. Can be altered as needed.