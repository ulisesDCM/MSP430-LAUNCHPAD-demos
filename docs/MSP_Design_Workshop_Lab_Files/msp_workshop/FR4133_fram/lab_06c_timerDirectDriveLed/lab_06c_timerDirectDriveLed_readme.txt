lab_06c_timerDirectDriveLed

This lab is a minor adaptation of the code from the previous exercise. The main
difference is that you will connect the GPIO pin to the output of the Timer_A CCR2 
pin (TA1.2).

Unfortunately, the Launchpad does not have an LED connected directly to a
timer output pin, therefore we'll need to use a jumper in order to make the 
proper connection - here's an excerpt from the lab solution:

    // When running this lab exercise, you will need to pull the JP1 jumper and
    // use a jumper wire to connect signal from P8.3 (on boosterpack pins) to
    // JP1.2 (bottom pin) of LED1 jumper ... this lets the TA1.2 signal drive
    // LED1 directly (without having to use interrupts)

The rate that we're toggling the LED was a convenient, if arbitrary, choice.
What rate will we toggle the LED with the timer in Lab6c? We answer that by 
comparing it with the LED toggle rates for our preceeding two labs:

  - Lab6a toggled the LED at a 4 second rate (an interrupt every 2 seconds)
  - Lab6b toggled the LED at a 2 second rate (an interrupt every second)
  - Lab6c will toggle the LED every second (no interrupt needed)


Basic Steps:
- Copy project from lab_06b_upTimer (your solution or the workshop solution)
- Delete (or comment out) GPIO pin setup for P1.0
- Add GPIO setup to connect P8.3 to peripheral module function (i.e. timer)
- In timer setup, add code to configure CCR2 for compare mode
- Turn off CCR0 interrupt (as this isn't needed this for this exercise)
- Set a breakpoint - if you hit it, then something isn't correct
- Run and observe both LED's are (still) flashing


File source code in this readme file:
- main.c
- myGpio.c
- myTimers.c

Final code ... you can copy from this if you want to save typing time & effort.


// ----------------------------------------------------------------------------
// main.c  (for lab_06c_timerDirectDriveLed project)  ('FR4133 Launchpad)
// ----------------------------------------------------------------------------

//***** Header Files **********************************************************
#include <driverlib.h>
#include "myGpio.h"
#include "myClocks.h"
#include "myTimers.h"

//***** Prototypes ************************************************************


//***** Defines ***************************************************************


//***** Global Variables ******************************************************


//*****************************************************************************
// Main
//*****************************************************************************
void main (void)
{
    // Stop watchdog timer
    WDT_A_hold( WDT_A_BASE );

    // Initialize GPIO
    initGPIO();

    // Initialize clocks
    initClocks();

    // Initialize timers
    initTimers();

    __bis_SR_register( GIE );                                                   // Enable interrupts globally

    while(1) {
        __no_operation();                                                       // Placeholder for while loop (not required)
    }
}



// ----------------------------------------------------------------------------
// myGpio.c  (for lab_06c_timerDirectDriveLed project)  ('FR4133 Launchpad)
// ----------------------------------------------------------------------------

//***** Header Files **********************************************************
#include <driverlib.h>                                                          // DriverLib include file
#include "myGpio.h"


//***** Defines ***************************************************************


//***** Global Variables ******************************************************


//*****************************************************************************
// Initialize GPIO
//*****************************************************************************
void initGPIO(void) {

    //**************************************************************************
	// Configure LaunchPad LEDs
    //**************************************************************************
    // Set pin P1.0 to output direction and turn LED off
    GPIO_setAsOutputPin( GPIO_PORT_P1, GPIO_PIN0 );                             // Red LED (LED1)
    GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 );

    // Set pin P4.0 to output direction and turn LED off
    GPIO_setAsOutputPin( GPIO_PORT_P4, GPIO_PIN0 );                             // Green LED (LED2)
    GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN0 );

    //**************************************************************************
    // Unlock pins (required for most FRAM devices)
    // Unless waking from LPMx.5, this should be done before clearing and enabling GPIO port interrupts
    //**************************************************************************
    PMM_unlockLPM5();

    //**************************************************************************
    // Configure LaunchPad Buttons
    //**************************************************************************
//    // Set P1.2 as input with pull-up resistor (for push button S1)
//    //  configure interrupt on low-to-high transition
//    //  and then clear flag and enable the interrupt
//    GPIO_setAsInputPinWithPullUpResistor( GPIO_PORT_P1, GPIO_PIN2 );
//    GPIO_selectInterruptEdge ( GPIO_PORT_P1, GPIO_PIN2, GPIO_LOW_TO_HIGH_TRANSITION );
//    GPIO_clearInterrupt ( GPIO_PORT_P1, GPIO_PIN2 );
//    GPIO_enableInterrupt ( GPIO_PORT_P1, GPIO_PIN2 );

//    // Set P2.6 as input with pull-up resistor (for push button S2)
//    //  configure interrupt on low-to-high transition
//    //  and then clear flag and enable the interrupt
//    GPIO_setAsInputPinWithPullUpResistor( GPIO_PORT_P2, GPIO_PIN6 );
//    GPIO_selectInterruptEdge ( GPIO_PORT_P2, GPIO_PIN6, GPIO_LOW_TO_HIGH_TRANSITION );
//    GPIO_clearInterrupt ( GPIO_PORT_P2, GPIO_PIN6 );
//    GPIO_enableInterrupt ( GPIO_PORT_P2, GPIO_PIN6 );

    //**************************************************************************
    // Configure Timer Output pin
    //**************************************************************************
    // When running this lab exercise, you will need to pull the JP1 jumper and
    // use a jumper wire to connect signal from P8.3 (on boosterpack pins) to
    // JP1.2 (bottom pin) of LED1 jumper ... this lets the TA1.2 signal drive
    // LED1 directly (without having to use interrupts)
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P8,
            GPIO_PIN3,                                                          // TA1.2 on P8.3 (Boosterpack pin 10)
            GPIO_PRIMARY_MODULE_FUNCTION
    );

    //**************************************************************************
    // Configure external crystal pins
    //**************************************************************************
    // Set LFXT (low freq crystal pins) to crystal input (rather than GPIO)
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P4,
            GPIO_PIN1 +                          // XIN  on P4.1
            GPIO_PIN2 ,                          // XOUT on P4.2
            GPIO_PRIMARY_MODULE_FUNCTION
    );

    //**************************************************************************
    // Output MSP clock signals to external pins
    // - This allows verifying the clocks with a logic analyzer
    //**************************************************************************
    // Output the ACLK and SMCLK signals to their respective pins to allow watching them with logic analyzer
//    GPIO_setAsPeripheralModuleFunctionOutputPin(
//            GPIO_PORT_P8,
//            GPIO_PIN0 +                                                       // SMCLK on P8.0 (Boosterpack pin 6)
//            GPIO_PIN1 ,                                                       // ACLK  on P8.1 (Boosterpack pin 2)
//            GPIO_PRIMARY_MODULE_FUNCTION
//    );
}


//*****************************************************************************
// Interrupt Service Routines
//*****************************************************************************
//#pragma vector=PORT1_VECTOR
//__interrupt void pushbutton_ISR (void)
//{
//        // Toggle the LED on/off (initial code; moved into switch statement below)
//        //GPIO_toggleOutputOnPin( GPIO_PORT_P1, GPIO_PIN0 );
//
//    switch( __even_in_range( P1IV, P1IV_P1IFG7 )) {
//        case P1IV_NONE:   break;                               // None
//        case P1IV_P1IFG0:                                      // Pin 0
//             __no_operation();
//             break;
//       case P1IV_P1IFG1:                                       // Pin 1
//            __no_operation();
//            break;
//       case P1IV_P1IFG2:                                       // Pin 2 (button 1)
//            GPIO_toggleOutputOnPin( GPIO_PORT_P1, GPIO_PIN0 );
//            break;
//       case P1IV_P1IFG3:                                       // Pin 3
//            __no_operation();
//            break;
//       case P1IV_P1IFG4:                                       // Pin 4
//            __no_operation();
//            break;
//       case P1IV_P1IFG5:                                       // Pin 5
//            __no_operation();
//            break;
//       case P1IV_P1IFG6:                                       // Pin 6
//            __no_operation();
//            break;
//       case P1IV_P1IFG7:                                       // Pin 7
//            __no_operation();
//            break;
//       default:   _never_executed();
//    }
//}

//#pragma vector=PORT2_VECTOR
//__interrupt void pushbutton2_ISR (void)
//{
//    switch( __even_in_range( P2IV, P2IV_P2IFG7 )) {
//        case P2IV_NONE:   break;                               // None
//        case P2IV_P2IFG0:                                      // Pin 0
//             __no_operation();
//             break;
//       case P2IV_P2IFG1:                                       // Pin 1
//           __no_operation();
//            break;
//       case P2IV_P2IFG2:                                       // Pin 2
//            __no_operation();
//            break;
//       case P2IV_P2IFG3:                                       // Pin 3
//            __no_operation();
//            break;
//       case P2IV_P2IFG4:                                       // Pin 4
//            __no_operation();
//            break;
//       case P2IV_P2IFG5:                                       // Pin 5
//            __no_operation();
//            break;
//       case P2IV_P2IFG6:                                       // Pin 6 (button 2)
//           GPIO_toggleOutputOnPin( GPIO_PORT_P4, GPIO_PIN0 );
//            break;
//       case P2IV_P2IFG7:                                       // Pin 7
//            __no_operation();
//            break;
//       default:   _never_executed();
//    }
//}


// ----------------------------------------------------------------------------
// myTimers.c  (for lab_06c_timerDirectDriveLed project) ('FR4133 Launchpad)
// ----------------------------------------------------------------------------

//***** Header Files **********************************************************
#include <driverlib.h>
#include "myTimers.h"

//***** Defines ***************************************************************


//***** Global Variables ******************************************************


//*****************************************************************************
// Initialize Timer
//*****************************************************************************
void initTimers(void)
{
    // 1. Setup Timer (TAR, TACTL)
    //    TimerA1 in Up mode using ACLK
    //    Toggle LED1 (Red) on/off every second directly from the timers CCR2 output pin
    //    Toggle LED2 (Green) on/off every second using TA1IFG
    Timer_A_initUpModeParam initUpParam = { 0 };
        initUpParam.clockSource = TIMER_A_CLOCKSOURCE_ACLK;                       // Use ACLK (slower clock)
        initUpParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;           // Input clock = ACLK / 1 = 32KHz
        initUpParam.timerPeriod = 0xFFFF / 2;                                     // Period (0xFFFF/2):  8000 / 32Khz = 1 second
        initUpParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;    // Enable TAR -> 0 interrupt
        initUpParam.captureCompareInterruptEnable_CCR0_CCIE =
            TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;                                  // No CCR0 interrupt
        initUpParam.timerClear = TIMER_A_DO_CLEAR;                                // Clear TAR & clock divider
        initUpParam.startTimer = false;                                           // Don't start the timer, yet
    Timer_A_initUpMode( TIMER_A1_BASE, &initUpParam );                            // Set up Timer A1

    // 2. Setup Capture & Compare features
    Timer_A_initCompareModeParam initCcr2Param = { 0 };
        initCcr2Param.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;        // Use CCR2 for compare
        initCcr2Param.compareInterruptEnable =
            TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;                             // Since directly driving LED, interrup not req'd
        initCcr2Param.compareOutputMode = TIMER_A_OUTPUTMODE_TOGGLE_RESET;        // Toggle provides a 1 sec period based on CCR0 and CCR2 values
        initCcr2Param.compareValue = 0x4000;                                      // Compare value: 4000 = 1/2 second
    Timer_A_initCompareMode( TIMER_A1_BASE, &initCcr2Param );

    // 3. Clear/enable flags and start timer
    Timer_A_clearTimerInterrupt( TIMER_A1_BASE );                                 // Clear TA1IFG

    Timer_A_startCounter(
        TIMER_A1_BASE,
        TIMER_A_UP_MODE
    );
}

//*****************************************************************************
// Interrupt Service Routine
//*****************************************************************************
#pragma vector=TIMER1_A1_VECTOR
__interrupt void timer1_ISR (void)
{
    // 4. Timer ISR and vector

    switch( __even_in_range( TA1IV, TA1IV_TAIFG )) {
     case TA1IV_NONE: break;                 // (0x00) None
     case TA1IV_TACCR1:                      // (0x02) CCR1 IFG
          _no_operation();
           break;
     case TA1IV_TACCR2:                      // (0x04) CCR2 IFG
          _no_operation();
           break;
     case TA1IV_3: break;                    // (0x06) Reserved
     case TA1IV_4: break;                    // (0x08) Reserved
     case TA1IV_5: break;                    // (0x0A) Reserved
     case TA1IV_6: break;                    // (0x0C) Reserved
     case TA1IV_TAIFG:                       // (0x0E) TA1IFG - TAR overflow
          // Toggle LED2 (Green) LED on/off
          GPIO_toggleOutputOnPin( GPIO_PORT_P4, GPIO_PIN0 );
          break;
     default: _never_executed();
    }
}


