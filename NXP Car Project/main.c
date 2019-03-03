#include "MK64F12.h"										// Device header
#include "pwm.h"
#include "uart.h"
#include "camera_FTM.h"
#include "stdio.h"
#include "isrInit.h"
#include "math.h"
void initialize(void);
void drive(int center_left_edge_index, int center_right_edge_index);
void drivev2();
float PID(float Vdes);
int wait = 0;
int scan = 1;
int ready = 2;
int run = 3;
int stop = 4;
int count = 0;
int status = 0;
int centerLeft = 0;
int centerRight = 0;
int *centerPos;
static float error_n1 = 0.0;
static float error_n2=0.0;
float error;
float kp = 0.9;
float ki = 0.0;
float kd = 0.0;
float left = 6.25;
float right = 9.50;
float stright = 7.85;
int speed = 90;
int center_left_edge_index = 26; //centerLeft; //32
int center_right_edge_index = 101; //centerRight; //97
int new_left_edge_index;
int new_right_edge_index;
int right_edge_difference;
int left_edge_difference;
int center_range;
int new_center_difference;
double duty_difference;
double duty;
double oldduty = 0.0;
int *currentPos;
double mid;
char str[100];
float servo;
float oldServo = 0.0;
float middle_center = 63.5;
float middlearray[10] = {65.5,65.5,65.5,65.5,65.5,65.5,65.5,65.5,65.5,65.5};
int i;
int checker = 0;
double speedact = 0.0;
int turn = 0;
int alternate = 0;
int turnleft = 0;
int turnright = 0;
double wheel = 0.0;
int main(){
	//initalize all components
	initialize(); 
    SetDutyCycle(0,10000,0);
    SetDutyCycle(0,10000,1);
	SetServoDutyCycle(stright);
	//wait for sw2 is pressed to begin the program
	status = wait;
	while(((GPIOC_PDIR&(1 << 6)) != 0)){};
	
	
	status = run; 	 
	for(;;){
		drivev2();
	}
}

void drivev2(){
    //scan the current position
    currentPos = findEdge();
	float current_center = (currentPos[1]+currentPos[0])/2.0;


    if (current_center > middle_center){
		//turn right
		turn = 1;
		turnright = 1;
		servo = stright+(0.9)*stright*(current_center-middle_center)/middle_center;
		//servo = stright+(0.15)*stright*(2*current_center-middle_center)/middle_center;
		//error = current_center - middle_center;
		//servo = stright  + kp*(error-error_n1)+ki*((error-error_n1)/2.0)+kd*(error-(2.0*error_n1+error_n2));
		if (servo > right){
			servo = right;
		}
	} else if (current_center<middle_center){
		//turn left
		turn = 1;
		turnleft = 1;
		servo = stright-(0.80)*stright*((middle_center-current_center)/middle_center);

		if (servo < left){
			servo = left;
		}
	}

	SetServoDutyCycle(servo);
	if (servo < 7.05 || servo > 8.625) {
    //handle tight turns
		speedact = speed*(0.55);
		alternate = !alternate;
		if (turnleft){
			SetDutyCycle(30,10000,1);
			SetDutyCycle(speedact+30,10000,0);
		} else if (turnright) {
			SetDutyCycle(speedact+30,10000,1);
			SetDutyCycle(30,10000,0);
		}
		turnleft = 0;
		turnright = 0;
		
	} else if (servo <7.4575 || servo > 8.24) {
    //handle soft turns
		speedact = speed*(0.75 );
		SetDutyCycle(speedact,10000,0);
		SetDutyCycle(speedact,10000,1);
	} else {
        //go stright
		SetDutyCycle(speed,10000,0);
//			SetDutyCycle(speed,10000,1);
	}

  oldServo = servo;
  error_n2 = error_n1;
  error_n1 = error;
	//sprintf(str,"current Speed: %f\n\r",speedact);
	//uart_put(str);
	
}

//void drive(int center_left_edge_index, int center_right_edge_index){
//	currentPos = findEdge();
//		
//	//sprintf(str,"Left Edge:%i Right Edge:%i\n\r", currentPos[0],currentPos[1]);
//	//uart_put(str);
//	center_range = center_right_edge_index - center_left_edge_index;
//	//currentPos = findEdge();
//	new_left_edge_index = currentPos[0];
//	new_right_edge_index = currentPos[1];
//	new_center_difference = new_right_edge_index - new_left_edge_index;
//	mid = new_center_difference/2.0;
//	// Right Edge
//	right_edge_difference = center_right_edge_index - new_right_edge_index;
//	// Left Edge
//	left_edge_difference = new_left_edge_index - center_left_edge_index;
//	if (oldduty == 0.0){
//		oldduty = stright;
//	}
//	// new duty
//	duty_difference = 1-((double)new_center_difference)/((double)center_range);
//	error = duty_difference;
//	// Right Edge: If Old - New >= 5, turn left
//	if (right_edge_difference >= 3) {
//		// turn left
//		//duty = stright - (stright*duty_difference);
//		duty = stright - stright*(kp*(error-error_n1)+ki*(error-error_n1)/2.0+kd*(error-2.0*error_n1+error_n2));
//		//sprintf(str,"duty: %f\r\nold duty: %f\f\rerror: %f\r\nerror_n1= %f\r\nerror_n2: %f\r\n\r\n",duty,oldduty,error,error_n1,error_n2);
//		//uart_put(str);
//		//duty = stright - kp*(64 + mid);
//		if (duty < left){
//			duty = left;
//			SetServoDutyCycle(duty);
//			//lower right motor
//			SetDutyCycle((speed-5)*(3.0/5.0),10000,1);
//			SetDutyCycle((speed+5)*(3.0/5.0),10000,0);
//					
//		} else {
//			SetServoDutyCycle(duty);
//			SetDutyCycle((speed-5),10000,1);
//			SetDutyCycle((speed+5),10000,0);
//		}	
//	}
//	
//	// Left Edge: If New - Old >=5, turn right
//	if (left_edge_difference >= 3) {
//		// turn right
//		//duty = stright + (stright*duty_difference);
//		duty = stright + stright*(kp*(error-error_n1)+ki*(error-error_n1)/2.0+kd*(error-2.0*error_n1+error_n2));
//		//sprintf(str,"duty: %f\r\nold duty: %f\f\rerror: %f\r\nerror_n1= %f\r\nerror_n2: %f\r\n\r\n",duty,oldduty,error,error_n1,error_n2);
//		//uart_put(str);
//		//duty = stright + kp*(64 - mid);
//		if (duty > right){
//			duty = right;
//			SetServoDutyCycle(duty);
//			//lower right motor
//			SetDutyCycle(speed*(3.0/5.0),10000,1);
//			SetDutyCycle(speed*(3.0/5.0),10000,0);
//		} else {
//			SetServoDutyCycle(duty);
//			SetDutyCycle(speed,10000,1);
//			SetDutyCycle(speed,10000,0);
//		}
//	}
//	// Else, Straight
//	if ((right_edge_difference < 3) && (left_edge_difference < 3)) {
//		SetServoDutyCycle(stright);
//		SetDutyCycle(speed,10000,1);
//		SetDutyCycle(speed+5,10000,0);
//	}
//	if (status == stop) {
//		SetDutyCycle(0,10000,1);
//		SetDutyCycle(0,10000,0);
//		SetServoDutyCycle(stright);
//	}
//	oldduty = duty;
//	error_n2 = error_n1;
//	error_n1 = error;
	
	
//	else {
//		SetServoDutyCycle(stright);
//		SetDutyCycle(speed,10000,1);
//		SetDutyCycle(speed+5,10000,0);
//	}


//float PID(float servoDes){
//  servoAct = servo_n1;
//  error = ServoDes - ServoAct;
//	servo = servo_n1+kp*(error-error_n1)+ki*((error-error_n1)/2.0)+kd*(error-(2.0*error_n1+error_n2));
//	servo_n1 = speed;
//	error_n2 = error_n1;
//	error_n1 = error;
//	return servo;
// }

 
void initialize(){
	
	//initialize the UART
	uart_init();
	
	//initialize the DC motors and Servo motor
	InitPWM();
	
	//initialize the camera
	init_camera();
	
	//initialize LED
	initISR();
}
