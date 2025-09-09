#include <msp430.h> 


/**
 * main.c
 * Julia Larsen
 * Project2.6
 * EELE 371
 * 3/25/2024
 * STEPPERMOTOR SHOULD NOT MOVE FASTER THAN 6 RPM
 */
int i = 0;
int step = 12;
int place = 0b0001;
int sw1 = 0;
int sw2 = 0;
unsigned int ADC_VALUE;
int HIGH = 3005;        // Minimum LED1 value for 2.3V  (2^12/3.3)*2.3
int FC = 1;
int FO = 0;
int main(void)
{
    //  SET UP
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    P1DIR |= BIT0;      //CONFIGURE LED1




    //  CONFIGURE ADC
    ADCCTL0 &= ~ADCSHT;     //CLEAR ADCSHT DEF OF =01
    ADCCTL0 |= ADCSHT_2;    //CONVERSION CYCLES = 16
    ADCCTL0 |= ADCON;       //TURN ADC ON

    ADCCTL1 |= ADCSSEL_2;   //ADC CLOCK SOURCE SMCLK
    ADCCTL1 |= ADCSHP;      // SAMPLE TIMER

    ADCCTL2 &= ~ADCRES;     // CLEAR ADC
    ADCCTL2 |= ADCRES_2;    // 12 BIT RESOLUTION

    ADCMCTL0 |= ADCINCH_8;  // ADC INPUT CHANNEL = A(P5.0)
    ADCIE |= ADCIE0;        // ENABLE ADC CONVERSION COMPLETE IRQ

    // LED SET UP
    P1DIR |= BIT0;          //SET LED 1 AS OUTPUT
    P6DIR |= BIT6;          //SET LED 2 AS OUTPUT
        // END LED SET UP
    // pin setup -------------------------------------------------------
    P5SEL1 |= BIT0;     //CONFIGURE P5.0 FOR A2
    P5SEL0 |= BIT0;     //
    //------------------------------------------------------------------
        // LED PIN SET UP
    P3DIR |= BIT4;          //SET UP PIN 3.4 - 3.7 AS OUTPUTS
    P3DIR |= BIT5;
    P3DIR |= BIT6;
    P3DIR |= BIT7;


    //------------------------------------------------------------------
        // SWITCH SET UP
    P4DIR &= ~BIT1;         //CLEAR P1.4 DIRECTION (SWITCH 1)
    P4REN |= BIT1;          // ENABLE RESSITOR
    P4OUT |= BIT1;          // MAKE PULL UP RESISTOR
    P4IES |= BIT1;          // ENABLE MASKABLE IRQ

    P2DIR &= ~BIT3;         //CLEAR P1.4 DIRECTION (SWITCH 1)
    P2REN |= BIT3;          // ENABLE RESSITOR
    P2OUT |= BIT3;          // MAKE PULL UP RESISTOR
    P2IES |= BIT3;          // ENABLE MASKABLE IRQ
        // END SWITCH SET UP
    //  IRQ SET UP
    P4IFG &= ~BIT1;         // CLEAR PORT 4.1 FLAG
    P4IE |= BIT1;           // ENABLE PORT 4.1 IRQ

    P2IFG &= ~BIT3;         // CLEAR PORT 2.3 FLAG
    P2IE |= BIT3;           // ENABLE PORT 2.3 IRQ
    //------------------------------------------------------------------
    //------------------------------------------------------------------

    // Configure Timer
        //SET UP
    TB0CTL |= TBCLR;        //CLEAR THE TABLE
    TB0CTL |= TBSSEL__SMCLK; //USE SMCLOCK
    TB0CTL |= MC__UP;   // CONTINUOUS COUNTER
    TB0CTL |= CNTL_0;       // 16 BIT LENGTH
    TB0CTL |= ID__8;        // DIVIDE BY 8
    TB0EX0 |= TBIDEX__7;        //DIVIDE BY 7

    TB0CCR0 = 130;        // COMPARE VALUE
        //SET UP TIMER OVERFLOW
    TB0CCTL0 |= CCIE;         //ENABLE TB0CC OVERLFLOW IRQ
    TB0CCTL0 &= ~CCIFG;       //CLEAR TB0CC FLAG

    //------------------------------------------------------------------

    PM5CTL0 &= ~LOCKLPM5;
    __enable_interrupt();   //ENABLE MASKABLE IRQS
    //                  END SETUP
//----------------------------------------------------------------------
    //                  MAIN FUNTIONS
    P3OUT |= BIT4;
    P3OUT &= ~BIT5;
    P3OUT &= ~BIT6;
    P3OUT &= ~BIT7;



    while(1){
    }
    return 0;
}

// ------INTERRUPT SERVICE ROUTINES-------------
#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR(void){
    ADC_VALUE = ADCMEM0;                // READ ADC RESULT

        if(ADC_VALUE > HIGH){               //IF A11 > 2.3V
            P1OUT |= BIT0;                  //LED1=ON

        }

        else {
            P1OUT &= ~BIT0;                 //LED1 = OFF

        }
//--------------------------------------------------------------------
}
//      Switch 1 ISR
#pragma vector = PORT4_VECTOR
__interrupt void ISR_Port4_S1(void){        //declare interrupt being used
sw1 = 1;
P4IFG &= ~BIT1;
}
//--------------------------------------------
//          Switch 2 ISR
#pragma vector = PORT2_VECTOR
__interrupt void ISR_Port2_S3(void){            // declare the interrupt being used
sw2 = 1;
P2IFG &= ~BIT3;
}
//---------------------------------------------
//          TIMER 0 INTERRUPT
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void){
    ADCCTL0 |= ADCENC | ADCSC;           //ENABLE AND START CONVERSION
    while((ADCIFG & ADCIFG0) == 0);     // WAIT FOR CONVERSION TO COMPLETE
                 //Uses count to determine the LED to be turned on
if((ADC_VALUE > HIGH) && (FO == 0)){
    TB0CCR0 = 80;        // COMPARE VALUE
    if(i<4096){
        switch(place){
                case 0b0001:    P3OUT |= BIT4;         // if 0 both LED are off
                                P3OUT &= ~BIT5;
                                P3OUT &= ~BIT6;
                                P3OUT &= ~BIT7;
                                place = 0b0011;
                                break;

                case 0b0011:    P3OUT |= BIT4;         // if 0 both LED are off
                                P3OUT |= BIT5;
                                P3OUT &= ~BIT6;
                                P3OUT &= ~BIT7;
                                place = 0b0010;
                                break;

                case 0b0010:    P3OUT &= ~BIT4;         // if 0 both LED are off
                                P3OUT |= BIT5;
                                P3OUT &= ~BIT6;
                                P3OUT &= ~BIT7;
                                place = 0b0110;
                                break;

                case 0b0110:    P3OUT &= ~BIT4;         // if 0 both LED are off
                                P3OUT |= BIT5;
                                P3OUT |= BIT6;
                                P3OUT &= ~BIT7;
                                place = 0b0100;
                                break;

                case 0b0100:    P3OUT &= ~BIT4;         // if 0 both LED are off
                                P3OUT &= ~BIT5;
                                P3OUT |= BIT6;
                                P3OUT &= ~BIT7;
                                place = 0b1100;
                                break;

                case 0b1100:    P3OUT &= ~BIT4;         // if 0 both LED are off
                                P3OUT &= ~BIT5;
                                P3OUT |= BIT6;
                                P3OUT |= BIT7;
                                place = 0b1000;
                                break;

                case 0b1000:    P3OUT &= ~BIT4;         // if 0 both LED are off
                                P3OUT &= ~BIT5;
                                P3OUT &= ~BIT6;
                                P3OUT |= BIT7;
                                place = 0b1001;
                                break;

                case 0b1001:    P3OUT |= BIT4;         // if 0 both LED are off
                                P3OUT &= ~BIT5;
                                P3OUT &= ~BIT6;
                                P3OUT |= BIT7;
                                place = 0b0001;
                                break;

                }
            i = i+1;
    }

}
else{
    TB0CCR0 = 60;        // COMPARE VALUE
    if(i>0){
        switch(place){
                case 0b0001:    P3OUT |= BIT4;         // if 0 both LED are off
                                P3OUT &= ~BIT5;
                                P3OUT &= ~BIT6;
                                P3OUT &= ~BIT7;
                                place = 0b1001;
                                break;

                case 0b0011:    P3OUT |= BIT4;         // if 0 both LED are off
                                P3OUT |= BIT5;
                                P3OUT &= ~BIT6;
                                P3OUT &= ~BIT7;
                                place = 0b0001;
                                break;

                case 0b0010:    P3OUT &= ~BIT4;         // if 0 both LED are off
                                P3OUT |= BIT5;
                                P3OUT &= ~BIT6;
                                P3OUT &= ~BIT7;
                                place = 0b0011;
                                break;

                case 0b0110:    P3OUT &= ~BIT4;         // if 0 both LED are off
                                P3OUT |= BIT5;
                                P3OUT |= BIT6;
                                P3OUT &= ~BIT7;
                                place = 0b0010;
                                break;

                case 0b0100:    P3OUT &= ~BIT4;         // if 0 both LED are off
                                P3OUT &= ~BIT5;
                                P3OUT |= BIT6;
                                P3OUT &= ~BIT7;
                                place = 0b0110;
                                break;

                case 0b1100:    P3OUT &= ~BIT4;         // if 0 both LED are off
                                P3OUT &= ~BIT5;
                                P3OUT |= BIT6;
                                P3OUT |= BIT7;
                                place = 0b0100;
                                break;

                case 0b1000:    P3OUT &= ~BIT4;         // if 0 both LED are off
                                P3OUT &= ~BIT5;
                                P3OUT &= ~BIT6;
                                P3OUT |= BIT7;
                                place = 0b1100;
                                break;

                case 0b1001:    P3OUT |= BIT4;         // if 0 both LED are off
                                P3OUT &= ~BIT5;
                                P3OUT &= ~BIT6;
                                P3OUT |= BIT7;
                                place = 0b1000;
                                break;

                }
            i = i-1;
        }

    }

TB0CCTL0 &= ~CCIFG;       //CLEAR THE FLAG
}
