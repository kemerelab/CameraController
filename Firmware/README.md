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

Recommended Resistors and Capacitors Values:
C1 - 10 uF
C2 - .1 uF
C3 - .1 uF
R1 - 330 ohms
R2 - 47k ohms
R3 - 330 ohms
R4 - 10k ohms
R5 - 330 ohms
R6 - 330 ohms
R7 - 330 ohms
R8 - 330 ohms
R9 - 330 ohms

Default MSP430 code provided (see msp430_code.c). Can be altered as needed.

Default Input Commands:
'15hz' - set the frequency of all signals to 15 Hz
'30hz' - set the frequency of all signals to 30 Hz (default frequency)
'40hz' - set the frequency of all signals to 40 Hz
'50hz' - set the frequency of all signals to 50 Hz
'60hz' - set the frequency of all signals to 60 Hz
'test' - generates 100 square pulses through all outputs at the current frequency (cannot be stopped)
'all' - continuously generate square waves through all outputs until stopped
'cam' - continuously generate square waves through both audio jacks until stopped
'sync' - continuously generate square waves through both BNC connectors until stopped
'side1' - continuously generate square waves through audio jack 1 and BNC connector 2 until stopped
'side2' - continuously generate square waves through audio jack 2 and BNC connector 1 until stopped
'cam1' - continuously generate a square wave through audio jack 1 until stopped
'cam2' - continuously generate a square wave through audio jack 2 until stopped
'sync1' - continuously generate a square wave through BNC connector 1 until stopped
'sync2' - continuously generate a square wave through BNC connector 2 until stopped
Enter - stop generating square waves
