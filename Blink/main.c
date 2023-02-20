#include <msp430.h> 

int main(void)
{
    int i=0;
	WDTCTL = WDTPW | WDTHOLD;	        // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;               // Disable the GPIO power-on default high-impedance mode

    P1DIR |= (1<<0);
    P4DIR |= (1<<0);

    while(1)
    {
        P4OUT   ^= (1<<0);
        P1OUT   ^= (1<<0);
        for(i=0; i<30000;i++);
    }
	return 0;
}
