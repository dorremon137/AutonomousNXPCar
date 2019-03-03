#ifndef PWM_H_
#define PWM_H_

void SetDutyCycle(float DutyCycle, unsigned int Frequency, int dir);
void SetServoDutyCycle(double DutyCycle);
void InitPWM(void);
void PWM_ISR(void);

#endif /* PWM_H_ */
