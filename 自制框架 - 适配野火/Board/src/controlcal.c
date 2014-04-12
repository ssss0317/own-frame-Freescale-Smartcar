#include "common.h"
#include "include.h"

//#define H a*MotorPD.duty-K*(1-a)*MotorPID.e1

//#define a
//#define K

extern uint8 highduty;
extern uint8 lowduty;
extern float A;

//电机PID
extern float SpeedPid_P;//80
extern float SpeedPid_I;//20
extern float SpeedPid_D;//10

//舵机PD//
extern float ServoPID_P;//40
extern float ServoPID_D;//20

extern ServoPidStruct ServoPID;
extern MotorPidStruct MotorPID;

int servo_angle_cal(int error)
{
	int angle=0; 
	ServoPID.Kp=ServoPID_P;//52.0
	ServoPID.Kd=ServoPID_D;//15.0
	ServoPID.e0 = error;
	angle=(int16)(ServoPID.Kp*ServoPID.e0 + ServoPID.Kd*(ServoPID.e1-ServoPID.e2));
	ServoPID.e2 = ServoPID.e1;
	ServoPID.e1 = ServoPID.e0;
        //printf("angle=%d\n",angle);
	return angle;
}

int motor_speed_cal(int error)
{
	int duty;	
	duty=(int)((-A)*error*error+highduty);
	if(duty<lowduty)
	duty=lowduty;
	return duty;

	//普通增量式PID
	/*
	MotorPID.e2=MotorPID.e1;
	MotorPID.e1=MotorPID.e0;
	MotorPID.e0=duty-(rotatenum/32);//实测脉冲比值,需测试
        
	MotorPID.Kp = SpeedPid_P;//12.0;   
	MotorPID.Ki = SpeedPid_I;//0.5;    
	MotorPID.Kd = SpeedPid_D;//7.0;
	MotorPID.lastduty=(int32)(MotorPID.Kp*(MotorPID.e0-MotorPID.e1)+MotorPID.Ki*MotorPID.e0
	+MotorPID.Kd*(MotorPID.e0-2.0*MotorPID.e1+MotorPID.e2));
        
	MotorPID.duty+=MotorPID.lastduty;
        
	if(MotorPID.duty>highduty)
	MotorPID.duty=highduty;
  	if(MotorPID.duty<lowduty)
	MotorPID.duty=lowduty;
	return MotorPID.duty;
        */

	//微分先行，不完全微分pid//正在做不完全微分，黑皮上的是位置式
	/*
        MotorPID.e2=MotorPID.e1;
	MotorPID.e1=MotorPID.e0;
	MotorPID.e0=duty-(rotatenum/32);//实测脉冲比值,需测试
        
	MotorPID.Kp = SpeedPid_P;//12.0;   
	MotorPID.Ki = SpeedPid_I;//0.5;    
	MotorPID.Kd = SpeedPid_D;//7.0;
	MotorPID.deltaduty=(int32)(MotorPID.Kp*(MotorPID.e0-MotorPID.e1)+MotorPID.Ki*MotorPID.e0
        +(MotorPID.Kd*(1-a)*MotorPID.e0+H));
        
	MotorPID.duty+=MotorPID.deltaduty;
        
	if(MotorPID.duty>highduty)
	MotorPID.duty=highduty;
  	if(MotorPID.duty<lowduty)
	MotorPID.duty=lowduty;
	return MotorPID.duty;
	*/
}