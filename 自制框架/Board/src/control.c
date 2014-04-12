#include "common.h"
#include "include.h"

extern long int rotatenum;
extern float brakevalue;
extern long int brakelimit;

void brake(void);
void motorduty(int duty1,int duty2);

void servo_control(int angle)
{
	uint16 servoduty;
	if(angle>60)
	angle=60;
	if(angle<-60)
	angle=-60;
	servoduty=(int)(1500+angle*500/90);
	FTM_PWM_Duty(FTM1,FTM_CH0,servoduty);
}

void motor_control(int speedduty)//����ģʽ��
{
	int speednum;
	speednum=speedduty*1500;//�����ֵ�����
	if(rotatenum-speednum>(brakevalue*rotatenum))// brakevalue<1
		brake();
	else
		motorduty(speedduty,0);
}

void stop(void)//����ģʽ��
{
	while(rotatenum<500 && rotatenum>-500)//ͣ��ʵ�ʼ���ֵ�����
	brake();
	motorduty(0,0);
	while(1);
}

void brake(void)//����ģʽ��
{
	motorduty(0,0);
	while(!(rotatenum>brakelimit||rotatenum<(-brakelimit)));//�ȴ�ʵ���ٶ��½���һ���̶�
	//FTM_PWM_Duty(FTM0,CH3,0);//btn7971ʹ�ܹذ�ɲ��
	motorduty(0,20);//��ת��ɲ��
	DELAY_MS(5);//����������ʱ
	motorduty(0,0);//�ָ���������ȡʵ������ֵ
}

void motorduty(int duty1,int duty2)//����ģʽ��
{
	FTM_PWM_Duty(FTM0, FTM_CH1, duty1);//Ұ������FTM0��CH3��CH4
	FTM_PWM_Duty(FTM0, FTM_CH2, duty2);//�����Ұ����������߰�ʾ����ʵ�ʺ�������ʱ��ת������Ӱ����
} 