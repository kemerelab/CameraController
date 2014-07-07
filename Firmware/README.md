/*
Code for the MSP430
COMMANDS: test,all,cam1,cam2,sync1,sync2,cam,sync,side1,side2
*/

#include <msp430.h>
#include "stdint.h"

#define frequency 30 //Hz (30)
#define pulse_delay 16000000/(2*frequency)		//One-half of the period of the signal

const unsigned long smclk_freq = 16000000;      // SMCLK frequency in hertz
const unsigned long bps = 9600;                 // Async serial bit rate
int i = 0;										//counter
int ALL = 0;								//flag
int SYNC1 = 0;
int SYNC2 = 0;
int CAM1 = 0;
int CAM2 = 0;
int SIDE1 = 0;
int SIDE2 = 0;
int SYNC = 0;
int CAM = 0;

char test[18];

void out(data){                                 //function to output characters through the hardware uart
    while (!(IFG2 & UCA0TXIFG));
    UCA0TXBUF = data;
}

void cameratest(void){					//test function. outputs 30 pulses
	for (i=30;i>0;i--){
		P2OUT ^= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
		_delay_cycles(pulse_delay);

		P2OUT ^= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
		_delay_cycles(pulse_delay);
	}
}

void main(void){
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	BCSCTL1 = CALBC1_16MHZ;						//run the 16 MHz clock
	DCOCTL = CALDCO_16MHZ;
	P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
	UCA0CTL1 |= UCSSEL_2;                     // SMCLK

	const unsigned long brd = (smclk_freq + (bps >> 1)) / bps; // Bit rate divisor

	UCA0BR1 = (brd >> 12) & 0xFF;               // High byte of whole divisor
	UCA0BR0 = (brd >> 4) & 0xFF;                // Low byte of whole divisor
	UCA0MCTL = ((brd << 4) & 0xF0) | UCOS16;    // Fractional divisor, oversampling mode

	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
	__bis_SR_register(GIE);
    P2SEL = 0;									// Set all the 2.x pins as general I/O
    P2SEL2 = 0;

    P2DIR = BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;	// Set all the 2.x pins as output pins
    P2OUT = BIT0 + BIT2 + BIT4 + BIT6;								// Initializes the signal pins to high

    while (1){														//main loop

		while (ALL){												// Outputs all signals at 30 Hz
				P2OUT ^= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
				_delay_cycles(pulse_delay);
				P2OUT ^= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
				_delay_cycles(pulse_delay);
			}

		while (SYNC1){												// Outputs SYNC1 (BNC) at 30 Hz
				P2OUT ^= BIT4 + BIT5;
				_delay_cycles(pulse_delay);
				P2OUT ^= BIT4 + BIT5;
				_delay_cycles(pulse_delay);
			}


		while (SYNC2){												// Outputs SYNC2 (BNC) at 30 Hz
				P2OUT ^= BIT6 + BIT7;
				_delay_cycles(pulse_delay);
				P2OUT ^= BIT6 + BIT7;
				_delay_cycles(pulse_delay);
			}


		while (CAM1){												// Outputs CAM1 (audio jack) at 30 Hz
				P2OUT ^= BIT0 + BIT1;
				_delay_cycles(pulse_delay);
				P2OUT ^= BIT0 + BIT1;
				_delay_cycles(pulse_delay);
			}

		while (CAM2){												// Outputs CAM2 (audio jack) at 30 Hz
				P2OUT ^= BIT2 + BIT3;
				_delay_cycles(pulse_delay);
				P2OUT ^= BIT2 + BIT3;
				_delay_cycles(pulse_delay);
			}

		while (SIDE1){												// Outputs CAM1 and SYNC2 at 30 Hz
				P2OUT ^= BIT0 + BIT1 + BIT6 + BIT7;
				_delay_cycles(pulse_delay);
				P2OUT ^= BIT0 + BIT1 + BIT6 + BIT7;
				_delay_cycles(pulse_delay);
			}

		while (SIDE2){												// Outputs CAM2 and SYNC1 at 30 Hz
				P2OUT ^= BIT2 + BIT3 + BIT4 + BIT5;
				_delay_cycles(pulse_delay);
				P2OUT ^= BIT2 + BIT3 + BIT4 + BIT5;
				_delay_cycles(pulse_delay);
			}

		while (SYNC){												// Outputs SYNC1 and SYNC2 at 30 Hz
				P2OUT ^= BIT4 + BIT5 + BIT6 + BIT7;
				_delay_cycles(pulse_delay);
				P2OUT ^= BIT4 + BIT5 + BIT6 + BIT7;
				_delay_cycles(pulse_delay);
			}

		while (CAM){												// Outputs CAM1 and CAM2 at 30 Hz
				P2OUT ^= BIT0 + BIT1 + BIT2 + BIT3;
				_delay_cycles(pulse_delay);
				P2OUT ^= BIT0 + BIT1 + BIT2 + BIT3;
				_delay_cycles(pulse_delay);
			}

    }

}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{

	 	out(UCA0RXBUF);             //return every character received

	    if (UCA0RXBUF == 0x0d){         //if enter is pressed, check to see if a valid command has been input, and turn on the corresponding state
	        out('\r');
	        out('\n');
	        ALL = SYNC1 = SYNC2 = CAM1 = CAM2 = SIDE1 = SIDE2 = SYNC = CAM = 0;

	        if ((test[0] == 't'||test[0] == 'T') && (test[1] == 's' || test[1] == 'S') && (test[2] == 'e' || test[2] == 'E') && (test[3] == 't' || test[3] == 'T')){ //outputs '30pulses@"x"hz
	        	cameratest();
	        	out('3');out('0');out('p');out('u');out('l');out('s');out('e');out('s');out('@');out(frequency / 10 +48); out(frequency % 10 + 48);out('h');out('z');out('\r');out('\n');
	        }

	        if ((test[0] == 'l'||test[0] == 'L') && (test[1] == 'l' || test[1] == 'L') && (test[2] == 'a' || test[2] == 'A')){		//starts pulses
	        	ALL ^= 1;
	        }

	        if ((test[0] == '1') && (test[1]=='c' || test[1]=='C') && (test[2] == 'n' || test[2] == 'N') && (test[3] == 'y' || test[3] == 'Y') && (test[4] == 's' || test[4] == 'S')){
	        	SYNC1 ^= 1;
	        }

	        if ((test[0] == '2') && (test[1]=='c' || test[1]=='C') && (test[2] == 'n' || test[2] == 'N') && (test[3] == 'y' || test[3] == 'Y') && (test[4] == 's' || test[4] == 'S')){
	        	SYNC2 ^= 1;
	        }

	        if ((test[0] == '1') && (test[1]=='m' || test[1]=='M') && (test[2] == 'a' || test[2] == 'A') && (test[3] == 'c' || test[3] == 'C')){
	        	CAM1 ^= 1;
	        }

	        if ((test[0] == '2') && (test[1]=='m' || test[1]=='M') && (test[2] == 'a' || test[2] == 'A') && (test[3] == 'c' || test[3] == 'C')){
	        	CAM2 ^= 1;
	        }

	        if ((test[0] == '1') && (test[1]=='E' || test[1]=='e') && (test[2] == 'd' || test[2] == 'D') && (test[3] == 'i' || test[3] == 'I') && (test[4] == 's' || test[4] == 'S')){
	        	SIDE1 ^= 1;
	        }

	        if ((test[0] == '2') && (test[1]=='E' || test[1]=='e') && (test[2] == 'd' || test[2] == 'D') && (test[3] == 'i' || test[3] == 'I') && (test[4] == 's' || test[4] == 'S')){
	        	SIDE2 ^= 1;
	        }

	        if ((test[0]=='c' || test[0]=='C') && (test[1] == 'n' || test[1] == 'N') && (test[2] == 'y' || test[2] == 'Y') && (test[3] == 's' || test[3] == 'S')){
	        	SYNC ^= 1;
	        }

	        if ((test[0]=='M' || test[0]=='m') && (test[1] == 'a' || test[1] == 'A') && (test[2] == 'c' || test[2] == 'C')){
	        	CAM ^= 1;
	        }



	    }
	    for (i = 17; i>0; i--){      //shift the array that stores the character inputs and input the new character at the bottom
	            test[i]=test[i-1];
		}
		test[0]=UCA0RXBUF;

}