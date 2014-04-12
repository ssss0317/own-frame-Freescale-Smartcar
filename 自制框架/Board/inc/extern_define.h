#ifndef _EXTERN_DEFINE_H_
#define _EXTERN_DEFINE_H_

/*
 *自定义PID结构定义
*/
typedef struct 
{
	float Kp;
	float Kd;
	int16 e0;
	int16 e1;
	int16 e2;

}ServoPidStruct;


typedef struct 
{
	float Kp;
	float Ki;
	float Kd;
	int32 e0;
	int32 e1;
	int32 e2;
	int32 duty;
	int32 lastduty;
	int32 Speed;

}MotorPidStruct;

#endif