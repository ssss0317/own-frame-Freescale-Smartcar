#ifndef __CONTROLCAL__
#define __CONTROLCAL__

typedef struct 
{
	float Kp;
	float Kd;
	s16 e0;
	s16 e1;
	s16 e2;

}ServoPidStruct;


typedef struct 
{
	float Kp;
	float Ki;
	float Kd;
	s32 e0;
	s32 e1;
	s32 e2;
	s32 Sum;
	s32 DefaultSum;
	s32 Speed;

}MotorPidStruct;
extern float SpeedPid_P;
extern float SpeedPid_I;
extern float SpeedPid_D;
extern float ServoPID_P;
extern float ServoPID_D;

extern ServoPidStruct ServoPID;
extern MotorPidStruct MotorPID;

int servo_angle_cal(int error);
int motor_speed_cal(int error);

#endif