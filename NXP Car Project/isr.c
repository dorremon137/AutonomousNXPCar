/*
 * isr.c
 */

#include "isr.h"
#include "MK64F12.h"
#include <stdio.h>
#include "uart.h"
#include "main.h"
//variables global to the IRQ handlers which dictates if timer is enabled &	timer counter
int timer = 0;
int timer2 = 0;
int timer_counter= 0;
int switch2 = 0;
char temp[100]; 
int checkOff1 = 0;
int checkOff2 = 0;
int checkOff3 = 0;
void PDB0_IRQHandler(void){ //For PDB timer
	
	//Clear interrupt in register PDB0_SC
	PDB0_SC &= ~PDB_SC_PDBIF_MASK;

	//toggle the output state for LED1
	if (timer) {
		if(status == wait) {
			//blink red
			GPIOB_PTOR = (1 << 22); 	//Red on
	} else if (status == scan) {
		  if (((GPIOB_PDIR&(1 << 22)) == 0) && (checkOff1 == 0)) {
					GPIOB_PDOR |= (1 << 22);  //Red off
				  checkOff1 = 1;
			}
			checkOff1 = 1;
		  //blink yellow
			GPIOB_PTOR = (1 << 22); 	//Red on
			GPIOE_PTOR = (1 << 26);		//Green on
	} else if (status == ready) {
			if (((GPIOB_PDIR&(1 << 22)) == 0) && ((GPIOE_PDIR&(1 << 26)) == 0) && (checkOff2 == 0)) {
					GPIOB_PDOR |= (1 << 22);  //Red off
				  GPIOE_PDOR |= (1 << 26);  //Green off
				  checkOff2 = 1;
			}
			checkOff2 = 1;
		  //blink green
			GPIOE_PTOR = (1 << 26);		//Green on
	} else if (status == run) {
      if (((GPIOE_PDIR&(1 << 26)) == 0) && (checkOff3 == 0)) {
					GPIOE_PDOR |= (1 << 26);  //Green off
				  checkOff3 = 1;
			}
			checkOff3 = 1;		
		
		  //blink white
			GPIOE_PTOR = (1 << 26);		//Green on
			GPIOB_PTOR = (1 << 21);		//Blue on
			GPIOB_PTOR = (1 << 22);		//Red on
	}
}
	return;
}
	
//void FTM0_IRQHandler(void){ //For FTM timer
//	
//	//clear interrupt in the register FTM0_SC
//	FTM0_SC &= ~FTM_SC_TOF_MASK;
//	
//	//check if switch2 has set to the local variable
//	if(timer2){
//		
//		 //increment the local variable counter
//		 timer_counter++;
//	} 
//	return;
//}
//	
void PORTC_IRQHandler(void){ //For switch 2
	
	//clear the interrupt
	PORTC_PCR6 |= PORT_PCR_ISF_MASK;
	
	//if timer is enable
	if (timer == 1) {
			PDB0_SC &= ~PDB_SC_PDBEN_MASK;
			PDB0_SC |= PDB_SC_LDOK_MASK;
		 //disable timer
		 timer = 0;
		
	//else
	} else {
		
		 //enable the timer
		 timer = 1;
		
		//start it with a trigger
		PDB0_SC |= PDB_SC_PDBEN_MASK;
		PDB0_SC |= PDB_SC_SWTRIG_MASK;
		PDB0_SC |= PDB_SC_LDOK_MASK;
	}
	return;
}
	
//void PORTC_IRQHandler(void){ //For switch 2
//	
//	//clear the interrupt
//	PORTC_PCR6 |= PORT_PCR_ISF_MASK;
//	
//	//if switch2 is pressed 
//	if ((GPIOC_PDIR&(1 << 6)) == 0){
//		
//       FTM0_MODE &=~ (FTM_MODE_FTMEN_MASK);
//	   //reset the FlexTimer
//       FTM0_CNT = FTM_CNT_COUNT(0x0000);
//	   FTM0_SC &= ~(FTM_SC_TOF_MASK);

//       FTM0_MODE |= (FTM_MODE_FTMEN_MASK);
//		
//	   //reset the timer counter
//       timer_counter = 0;
//       timer2 = 1;

//		
//       //turn on the blue LED while the button is pressed
//	   GPIOB_PDOR &= ~(1 << 21);
//		
//	}else if(timer2 == 1) {
//		 //reset the local variable to affect the timer2 function
//		 timer2 = 0;
//		
//		 //turn off the blue LED while button is up
//		 GPIOB_PDOR |= (1 << 21);	
//		
//		 //print the result
//		 sprintf(temp,"Button held for %d milliseconds!\r\n",timer_counter);
//     uart_put(temp);
//	 }
//	return;
//}
