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

void motor_control(int speedduty)//常规模式版
{
	int speednum;
	speednum=speedduty*1500;//脉冲比值需测试
	if(rotatenum-speednum>(brakevalue*rotatenum))// brakevalue<1
		brake();
	else
		motorduty(speedduty,0);
}

void stop(void)//常规模式版
{
	while(rotatenum<500 && rotatenum>-500)//停车实际减速值需测试
	brake();
	motorduty(0,0);
	while(1);
}

void brake(void)//常规模式版
{
	motorduty(0,0);
	while(!(rotatenum>brakelimit||rotatenum<(-brakelimit)));//等待实际速度下降至一定程度
	//FTM_PWM_Duty(FTM0,CH3,0);//btn7971使能关版刹车
	motorduty(0,20);//反转版刹车
	DELAY_MS(5);//减速作用延时
	motorduty(0,0);//恢复滚动，获取实际脉冲值
}

void motorduty(int duty1,int duty2)//常规模式版
{
	FTM_PWM_Duty(FTM0, FTM_CH1, duty1);//野火驱动FTM0，CH3，CH4
	FTM_PWM_Duty(FTM0, FTM_CH2, duty2);//电机与野火驱动红黑线暗示错误，实际红线正极时反转，但不影响编程
} 