#include "common.h"
#include "include.h"

#include "image.h"
#include "control.h"
#include "controlcal.h"

#define motor_en_io PTC24
#undef FTM1_PRECISON
#define FTM1_PRECISON 10000u//������ƾ���

#if 0
#define Motor_Hz    (50)//����ģʽ�£�Ƶ��50
#else
#define Motor_Hz    (20*1000)//����ģʽ�£�Ƶ��20k
#endif

int startline=0;
int image_error;
long int rotatenum=0;
int servo_angle;
int expect_speed_duty;
int brakevalue��0.5;//brakevalue<1��ɲ���б���ֵ��������ʵ���������������ֵ��ʵ��������ɲ��
long int brakelimit=ʵ�ʵ���ʱ������;//ʵ�ʵ���ʱ�������������
uint highduty=10;//���ռ�ձ�
uint lowduty=3;//���ռ�ձ�
float A=0.5;//������Ʋ�����duty=(-A)*duty*duty+highspeed

//���PID
float SpeedPid_P = 1;//80
float SpeedPid_I =0;//20
float SpeedPid_D = 0;//10.0

//���λ��ʽPD//
float ServoPID_P = 1;//40
float ServoPID_D = 0;//20

//�������ʽPD//
//float ServoPID_P = 53;
//float ServoPID_D = 3;

uint8 CCD_BUFF[TSL1401_MAX*3][TSL1401_SIZE];//����洢����CCDͼ�������


void Motor_init();
void PIT0_IRQHandler();//CCD�ɼ��ж�
void PIT1_IRQHandler();//�������ɼ��ж�
uint8 time��6;//�ع�ʱ�������

void main
{
	DisableInterrupts;

	uart_init(UART4,19200);

	tsl1401_set_addrs(TSL1401_MAX,(uint8 *)&CCD_BUFF[0],(uint8 *)&CCD_BUFF[1],(uint8 *)&CCD_BUFF[2]);//��ʼ�� ����CCD
	tsl1401_init(time);//��ʼ�� ����CCD �������ж�ʱ��Ϊtime

	//tsl1401_led_en(TSL1401_MAX);               //���������
	//tsl1401_led_dis(TSL1401_MAX);             //�رղ����

	FTM_PWM_init(FTM1,CH0,100,1500);//��ʼ�����
	Motor_init;//��ʼ�����
	FTM_QUAD_Init(FTM2);//��ʼ����������
	
	pit_init_ms(PIT1,7);//��������ʱ��ʼ��

	set_vector_handler(PIT0_VECTORn,PIT0_IRQHandler);//����CCD��ʱ�ж�����
	set_vector_handler(PIT1_VECTORn,PIT1_IRQHandler);//�����������ʱ�ж�����
	
	//NVIC_SetPriorityGrouping(1);//�������ȼ����飬PriorityGroup = 1��ʾ 1bit ��ռ���ȼ�  ��3bit �����ȼ� ��2����ռ���ȼ���
	//NVIC_SetPriority(PIT0_IRQn,0);//����CCD���ȼ���priority��ΧΪ0 ~ 15�� prioԽ�ͣ������ȼ�Խ�� ��4bit��
	//NVIC_SetPriority(PIT1_IRQn,1);//���ñ��������ȼ�
	//�������ȼ���Ҫע��arm_math.h �ļ��ĵ�һ��
	//#define __CMSIS_GENERIC /* disable NVIC and Systick functions */

	enable_irq(PIT0_IRQn);//��CCD��ʱ�ж�
	enable_irq(PIT1_IRQn);//����������ʱ�ж�
	
	EnableInterrupts;
	DELAY_MS(500);//��ʱ����

	//DELAY_MS(3000);//������ǰ������λ
	//motorcontrol(10);//10%ռ�ձ�����
	//DELAY_MS(2000);//���������
	
	while(1)
	{
		tsl1401_get_img();
		mode_get();
		if(startline==1)
		stop();
		image_error=get_img_error();
		servo_angle=servo_angle_cal(image_error);
		expect_speed_duty=motor_speed_cal(imageerror);
		servo_control(servo_angle);
		motor_control(expect_speed_duty);
		DELAY_MS(5);
	}
	
}

void PIT0_IRQHandler(void)
{
	tsl1401_time_isr();
	PIT_Flag_Clear(PIT0);
}

void PIT1_IRQHandler(void)
{
	rotatenum=FTM_QUAD_get(FTM2);
	FTM_QUAD_clean(FTM2);
	PIT_Flag_Clear(PIT1);
}

void Motorinit(void)
{
	FTM_PWM_init(FTM0, CH1, Motor_Hz,,0);
	FTM_PWM_init(FTM0, CH2, Motor_Hz,,0);
	FTM_PWM_init(FTM0, CH3, Motor_Hz,,0);
��