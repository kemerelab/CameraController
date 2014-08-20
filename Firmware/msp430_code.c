#include <msp430.h>
#include "stdint.h"


const unsigned long smclk_freq = 16000000;      // SMCLK frequency in hertz
const unsigned long bps = 9600;                 // Async serial bit rate
int freq = 30;									// Initial frequency when started up, can be changed using command-line interface
int i = 0;										//counter
int ALL = 0;								//flag for which "state" the controller is in
int SYNC1 = 0;
int SYNC2 = 0;
int CAM1 = 0;
int CAM2 = 0;
int SIDE1 = 0;
int SIDE2 = 0;
int SYNC = 0;
int CAM = 0;

char test[18];									//array for storing most recent input command

void out(data){                                 //function to output characters through the hardware uart
    while (!(IFG2 & UCA0TXIFG));
    UCA0TXBUF = data;
}

void pulse_delay(freq){							//return a delay of the width of one pulse, actual time depends on the current frequency
	if (freq == 30){
		_delay_cycles(16000000/60);
	}
	else if (freq == 60){
		_delay_cycles(16000000/120);
	}
	else if (freq == 40){
		_delay_cycles(16000000/80);
	}
	else if (freq == 50){
		_delay_cycles(16000000/100);
	}
	else if (freq == 15){
		_delay_cycles(16000000/30);
	}
}

void cameratest(void){					//test function. outputs 30 pulses
	for (i=30;i>0;i--){
		P2OUT ^= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
		pulse_delay(freq);

		P2OUT ^= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
		pulse_delay(freq);
	}
}

void main(void){
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	BCSCTL1 = CALBC1_16MHZ;						//run the 16 MHz clock
	DCOCTL = CALDCO_16MHZ;
	P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD, use for serial communication
	UCA0CTL1 |= UCSSEL_2;                     // SMCLK

	const unsigned long brd = (smclk_freq + (bps >> 1)) / bps; // Bit rate divisor

	UCA0BR1 = (brd >> 12) & 0xFF;               // High byte of whole divisor
	UCA0BR0 = (brd >> 4) & 0xFF;                // Low byte of whole divisor
	UCA0MCTL = ((brd << 4) & 0xF0) | UCOS16;    // Fractional divisor, oversampling mode

	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
	__bis_SR_register(GIE);
    P2SEL = 0;								// Select all the 2.X pins to general I/O
    P2SEL2 = 0;								// Select all the 2.X pins to general I/O

    P2DIR = BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7; 		//Set all the 2.X pins to output
    P2OUT = BIT0 + BIT2 + BIT4 + BIT6;                                  //Initialize pins 2.0, 2.2, 2.4, and 2.6 to high

    while (1){								//main loop that will continuously output signal if a state is on

		while (ALL){						// output to all 2.X pins
				P2OUT ^= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
				pulse_delay(freq);
				P2OUT ^= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
				pulse_delay(freq);
			}

		while (SYNC1){						// output to BNC 1
				P2OUT ^= BIT4 + BIT5;
				pulse_delay(freq);
				P2OUT ^= BIT4 + BIT5;
				pulse_delay(freq);
			}


		while (SYNC2){						// output to BNC 2
				P2OUT ^= BIT6 + BIT7;
				pulse_delay(freq);
				P2OUT ^= BIT6 + BIT7;
				pulse_delay(freq);
			}


		while (CAM1){						// output to audio jack 1
				P2OUT ^= BIT0 + BIT1;
				pulse_delay(freq);
				P2OUT ^= BIT0 + BIT1;
				pulse_delay(freq);
			}

		while (CAM2){						// output to audio jack 2
				P2OUT ^= BIT2 + BIT3;
				pulse_delay(freq);
				P2OUT ^= BIT2 + BIT3;
				pulse_delay(freq);
			}

		while (SIDE1){						// output to 1 audio jack and 2 BNC
				P2OUT ^= BIT0 + BIT1 + BIT6 + BIT7;
				pulse_delay(freq);
				P2OUT ^= BIT0 + BIT1 + BIT6 + BIT7;
				pulse_delay(freq);
			}

		while (SIDE2){						// output to 1 audio jack and 1 BNC
				P2OUT ^= BIT2 + BIT3 + BIT4 + BIT5;
				pulse_delay(freq);
				P2OUT ^= BIT2 + BIT3 + BIT4 + BIT5;
				pulse_delay(freq);
			}

		while (SYNC){						// output to both BNC
				P2OUT ^= BIT4 + BIT5 + BIT6 + BIT7;
				pulse_delay(freq);
				P2OUT ^= BIT4 + BIT5 + BIT6 + BIT7;
				pulse_delay(freq);
			}

		while (CAM){						// output to both audio jacks
				P2OUT ^= BIT0 + BIT1 + BIT2 + BIT3;
				pulse_delay(freq);
				P2OUT ^= BIT0 + BIT1 + BIT2 + BIT3;
				pulse_delay(freq);
			}

    }

}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{

	 	out(UCA0RXBUF);             //return every character received

	    if (UCA0RXBUF == 0x0d){         //if enter is pressed, check to see if a valid command has been input
	        out('\r');					//return 'Enter'
	        out('\n');
			
			//change current frequency, keeps output state the same
	        if ((test[0] == 'z' || test[0] == 'Z') && (test[1] == 'h' || test[1] == 'H') && (test[2] == '0') && (test[3] == '3')){
	        	freq = 30;
	        }

	        else if ((test[0] == 'z' || test[0] == 'Z') && (test[1] == 'h' || test[1] == 'H') && (test[2] == '0') && (test[3] == '6')){
	        	freq = 60;
	        }

	        else if ((test[0] == 'z' || test[0] == 'Z') && (test[1] == 'h' || test[1] == 'H') && (test[2] == '0') && (test[3] == '4')){
	        	freq = 40;
	        }

	        else if ((test[0] == 'z' || test[0] == 'Z') && (test[1] == 'h' || test[1] == 'H') && (test[2] == '0') && (test[3] == '5')){
	        	freq = 50;
	        }

	        else if ((test[0] == 'z' || test[0] == 'Z') && (test[1] == 'h' || test[1] == 'H') && (test[2] == '5') && (test[3] == '1')){
	        	freq = 15;
	        }

	        else{
	        				ALL = SYNC1 = SYNC2 = CAM1 = CAM2 = SIDE1 = SIDE2 = SYNC = CAM = 0; // if any command other then changing frequency is pressed, clear all outputs before applying new output state

	        				if ((test[0] == 't'||test[0] == 'T') && (test[1] == 's' || test[1] == 'S') && (test[2] == 'e' || test[2] == 'E') && (test[3] == 't' || test[3] == 'T')){ //output test
	        		        	cameratest();
	        		        }
							
							//turn on an output state
	        				else if ((test[0] == 'l'||test[0] == 'L') && (test[1] == 'l' || test[1] == 'L') && (test[2] == 'a' || test[2] == 'A')){
	        		        	ALL ^= 1;
	        		        }

	        				else if ((test[0] == '1') && (test[1]=='c' || test[1]=='C') && (test[2] == 'n' || test[2] == 'N') && (test[3] == 'y' || test[3] == 'Y') && (test[4] == 's' || test[4] == 'S')){
	        		        	SYNC1 ^= 1;
	        		        }

	        				else if ((test[0] == '2') && (test[1]=='c' || test[1]=='C') && (test[2] == 'n' || test[2] == 'N') && (test[3] == 'y' || test[3] == 'Y') && (test[4] == 's' || test[4] == 'S')){
	        		        	SYNC2 ^= 1;
	        		        }

	        				else if ((test[0] == '1') && (test[1]=='m' || test[1]=='M') && (test[2] == 'a' || test[2] == 'A') && (test[3] == 'c' || test[3] == 'C')){
	        		        	CAM1 ^= 1;
	        		        }

	        				else if ((test[0] == '2') && (test[1]=='m' || test[1]=='M') && (test[2] == 'a' || test[2] == 'A') && (test[3] == 'c' || test[3] == 'C')){
	        		        	CAM2 ^= 1;
	        		        }

	        				else if ((test[0] == '1') && (test[1]=='E' || test[1]=='e') && (test[2] == 'd' || test[2] == 'D') && (test[3] == 'i' || test[3] == 'I') && (test[4] == 's' || test[4] == 'S')){
	        		        	SIDE1 ^= 1;
	        		        }

	        				else if ((test[0] == '2') && (test[1]=='E' || test[1]=='e') && (test[2] == 'd' || test[2] == 'D') && (test[3] == 'i' || test[3] == 'I') && (test[4] == 's' || test[4] == 'S')){
	        		        	SIDE2 ^= 1;
	        		        }

	        				else if ((test[0]=='c' || test[0]=='C') && (test[1] == 'n' || test[1] == 'N') && (test[2] == 'y' || test[2] == 'Y') && (test[3] == 's' || test[3] == 'S')){
	        		        	SYNC ^= 1;
	        		        }

	        				else if ((test[0]=='M' || test[0]=='m') && (test[1] == 'a' || test[1] == 'A') && (test[2] == 'c' || test[2] == 'C')){
	        		        	CAM ^= 1;
	        		        }
	        }


	    }
	    for (i = 17; i>0; i--){      //shift the array that stores the character inputs and input the new character at the bottom
	            test[i]=test[i-1];
		}
		test[0]=UCA0RXBUF;

}




