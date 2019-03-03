#ifndef CAMERA_H_
#define CAMERA_H_

int *findEdge(void);
void ADC0_IRQHandler(void);
void FTM2_IRQHandler(void);
void PIT0_IRQHandler(void);
void init_FTM2(void);
void init_PIT(void);
void init_GPIO(void);
void init_ADC0(void);
void init_camera(void);

#endif /* CAMERA_H_ */
