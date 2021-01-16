 /******************************************************/
/* Examples Program For "ET-BASE dsPIC30F4011"  Board */
/* Hardware    : ET-BASE dsPIC30F4011                 */
/* Target MCU  : dsPIC30F4011                         */
/*       	   : X-TAL : 7.3728 MHz                   */
/*             : Run 117.9648MHz                      */
/*             : Selec OSC Mode = XT w/PLL 16x        */
/* Compiler    : MPLAB v8.40 + C30 v3.20B      	      */
/* Last Update : 12/October/2009                      */
/******************************************************/
/* Function    : Use RD0 Gen PWM Control Servo        */
/******************************************************/
/* Used Timer2 Control OC1 Generate PWM Signal        */
/* Used RD0 = OC1 Generate PWM (56.2Hz)               */
/* Period = 17.79mS (271.6 nS x 65536)                */
/* Duty Cycle = 1.0mS <--> 2.0mS                      */
/******************************************************/

#include <p30f4011.h>                                		// dsPIC30F4011 MPU Register
#include "outcompare.h"										// Used Output Compare Library Function
#include "timer.h"                               		// dsPIC30F4011 MPU Register
#include "ports.h"											// Used Timer Library Function

/* Setup Configuration For ET-BASE dsPIC30F4011 */
_FOSC(CSW_FSCM_OFF & XT_PLL16);								// Disable Clock Switching,Enable Fail-Salf Clock
                                                            // Clock Source = Primary XT + (PLL x 16)
_FWDT(WDT_OFF);												// Disable Watchdog 
_FBORPOR(PBOR_OFF & PWRT_64 & MCLR_EN);					    // Disable Brown-Out ,Power ON = 64mS,Enable MCLR
_FGS(CODE_PROT_OFF);										// Code Protect OFF
/* End Configuration For ET-BASE dsPIC30F4011 */

/* pototype  section */

void delay_led(unsigned long int count1);
void init_pwm(void);										// Initial PWM Function
void delay_led(unsigned long int);							// Delay Time Function
void _ISR _INT0Interrupt(void);

#define led1 _RD1
#define led2 _RD2
#define ir1  _RD3 

void __attribute__((__interrupt__, no_auto_psv)) _INT0Interrupt(void)
{  
  //while(PORTEbits.RE8 |= 0){								// Wait INT1 Signal Release
						// Debounce
	
	SetDCOC1PWM(5523);
	delay_led(5000000); 
	SetDCOC1PWM(2762);  									// 0.75 mS(Left)
 	  
	//IFS0bits.INT1IF = 0;
	_INT0IF = 0;
		
	//}
  
}


int main(void)
{ 
	    ConfigINT0(FALLING_EDGE_INT &								// Falling Edge Trigger Interrupt
             EXT_INT_ENABLE &								// Enable INT1 Interrupt 
             EXT_INT_PRI_7);							// INT1 Interrupt Priority = 7
	
	//init_pwm();
	TRISDbits.TRISD1 = 0;
	TRISDbits.TRISD2 = 0;
	TRISDbits.TRISD3 = 1;
//	SetDCOC1PWM(5523);
	
  
while(1)													// Loop Continue   
{ 

   if (ir1 == 0)
	{	
		led1 = 1;
		led2 = 0;
  	}
	else 
	{
		led1 = 0;
		led2 = 1;
	 
	}
}
}
/********************************/
/* Initial PWM for dsPIC30F4011 */
/* -> Period = 20mS(17.79mS)    */
/* -> Duty Cycle = 1.0 - 2.0 mS */
/********************************/
void init_pwm()
{		  
  CloseOC1();												// Disable OC1 Before New Config
  CloseTimer2();											// Disable Timer2 Befor New Config

  // Config OC1 Interrupt Control
  ConfigIntOC1(OC_INT_OFF &									// Disable OC1 Interrupt
    		   OC_INT_PRIOR_7);								// OC1 Interrupt Priority = 7
   
  // Config Timer2 Interrupt Control
  ConfigIntTimer2(T2_INT_OFF &								// Disable Timer2 Interrupt
                  T2_INT_PRIOR_7);							// Timer2 Interrupt Priority = 7
  
  // Config Timer2 For Generate PWM on OC1 
  // Period = 20mS (PRy)
  // Duty Cycle = 1.0mS  - 2.0mS (OC1RS)
  // 
  //***************************************************
  // ET-BASE dsPIC30F4011 Hardware Board
  // XTAL = 7.3728MHz
  // Fosc = 7.3728 MHz x 16 = 117.9648 MHz
  // Fcy  = Fosc / 4 
  //      = 117.9648 / 4 = 29.4912 MHz
  // Tcy  = 1 / 29.4912 MHz
  //      = 33.90842 nS
  //***************************************************
  // Desire PWM1 Freq = 50Hz (20mS Period)
  // Desire Timer Prescale = 8  
  // PWM Period = (PRy+1) x (Tcy) x (TMRy Prescale)
  // 1 / 50Hz   = (PRy+1) x (Tcy) x (TMRy Prescale)
  //            = (PRy+1) x (33.90842 nS) x (8)         
  //     	    = (PRy+1) x 0.2716 uS
  // 20mS       = (PRy+1) x 0.2716 uS  
  // Pry        = [20mS / 0.2716 uS] - 1
  //            = 73637.7025 - 1            
  //            = 73636.7025
  //
  // Period Maximum = 65536 Cycle = 56.18 Hz
  // 1 Cycle = 271.6 nS
  // Duty Cycle 0.75 mS = 2762 Cycle
  // Duty Cycle 1.00 mS = 3682 Cycle
  // Duty Cycle 1.50 mS = 5523 Cycle 
  // Duty Cycle 2.00 mS = 7363 Cycle
  // Duty Cycle 2.25 mS = 8284 Cycle
  
  OpenTimer2(T2_ON &										// ON Timer2
			 T2_IDLE_STOP &									// Disable Timer2 in IDLE Mode
             T2_32BIT_MODE_OFF &							// Timer2 = 16 Bit Timer
			 T2_GATE_OFF &									// Disable Timer2 Gate Control
             T2_PS_1_8 &									// Timer2 Prescale = 1:8			
			 T2_SOURCE_INT ,								// Timer2 Clock Source = Internal
             65535);										// Timer2 Count(PWM Period) = 65535 Cycle
	  
  // Open OC1 = PWM1 Config,Duty(OC1RS),Pulse Start
  OpenOC1(OC_IDLE_STOP &									// Disable OC in IDLE Mode 
 		  OC_TIMER2_SRC & 									// Select Timer2 = Clock Source
          OC_PWM_FAULT_PIN_DISABLE, 						// OC1 = PWM Mode
          
          // Duty Cycle = OCxRS
		  5523,												// PWM Duty Cycle = 5523 = 1.5 mS

          // Pulse Start
          0);												// PWM1(OC1) Pulse Start  		
}

/***********************/
/* Delay Time Function */
/*    1-4294967296     */
/***********************/
void delay_led(unsigned long int count1)
{
  while(count1 > 0) {count1--;}								// Loop Decrease Counter	
}

  