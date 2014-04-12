/*野火硬件适配版*/
#include "common.h"
#include "include.h"

#include "image.h"
#include "control.h"
#include "controlcal.h"

//#define motor_en_io PTE26
#define MOTOR1_EN_IO   PTD15
#define MOTOR2_EN_IO   PTA19//开启时激光测速灯会开

//#undef FTM1_PRECISON
//#define FTM1_PRECISON 10000u//舵机控制精度

#if 0
#define Motor_Hz    (50)//滑行模式下，频率50
#else
#define Motor_Hz    (20*1000)//常规模式下，频率20k
#endif

//int startline=0;
volatile int image_error;
extern volatile long int rotatenum=0;
int servo_angle;
int expect_speed_duty;
float brakevalue=0.5;//brakevalue<1，刹车判别阈值，期望与实际脉冲数差大于阈值倍实际脉冲则刹车
long int brakelimit=32;//实际低速时脉冲数，需测试
uint8 highduty=10;//最高占空比
uint8 lowduty=3;//最低占空比
float A=0;//电机控制参数，duty=(-A)*error*error+highspeed

//电机PID
float SpeedPid_P = 1;//80
float SpeedPid_I =0;//20
float SpeedPid_D = 0;//10.0

//舵机位置式PD//
float ServoPID_P = 1.5;//40
float ServoPID_D = 0;//20

//舵机增量式PD//
//float ServoPID_P = 53;
//float ServoPID_D = 3;

ServoPidStruct ServoPID;

MotorPidStruct MotorPID={
.e2=0,
.e1=0,
.e0=0,
};


uint8 CCD_BUFF[TSL1401_MAX*3][TSL1401_SIZE];//定义存储接收CCD图像的数组
uint8 time=12;//曝光时间初定义


void Motor_init();
void PIT0_IRQHandler();//CCD采集中断
void PIT1_IRQHandler();//编码器采集中断




void main()
{
	DisableInterrupts;
        
        MotorPID.duty=lowduty;

	//uart_init(UART4,19200);

	tsl1401_set_addrs(TSL1401_MAX,(uint8 *)&CCD_BUFF[0],(uint8 *)&CCD_BUFF[1]);//初始化 线性CCD
	tsl1401_init(time);//初始化 线性CCD ，配置中断时间为time

    /*LCD显示*/
    LCD_init();                                 //LCD初始化
    //

	tsl1401_led_en(TSL1401_MAX);               //开启补光灯
	//tsl1401_led_dis(TSL1401_MAX);             //关闭补光灯

	FTM_PWM_init(FTM1,FTM_CH0,100,10000-1500);//初始化舵机
	//Motor_init();//初始化电机
	FTM_QUAD_Init(FTM2);//初始化正交解码
	
	pit_init_ms(PIT1,10);//编码器定时初始化

	set_vector_handler(PIT0_VECTORn,PIT0_IRQHandler);//定义CCD定时中断向量
	set_vector_handler(PIT1_VECTORn,PIT1_IRQHandler);//定义编码器定时中断向量
	
	//NVIC_SetPriorityGrouping(1);//设置优先级分组，PriorityGroup = 1表示 1bit 抢占优先级  ，3bit 亚优先级 （2个抢占优先级）
	//NVIC_SetPriority(PIT0_IRQn,0);//配置CCD优先级，priority范围为0 ~ 15， prio越低，则优先级越高 （4bit）
	//NVIC_SetPriority(PIT1_IRQn,1);//配置编码器优先级
	//配置优先级需要注释arm_math.h 文件的第一行
	//#define __CMSIS_GENERIC /* disable NVIC and Systick functions */

	enable_irq(PIT0_IRQn);//开CCD定时中断
	enable_irq(PIT1_IRQn);//开编码器定时中断
	
	EnableInterrupts;
	DELAY_MS(500);//延时启动

	//DELAY_MS(3000);//起跑线前待机就位
	//motorcontrol(10);//10%占空比启动
	//DELAY_MS(2000);//冲过起跑线
	
	while(1)
	{
		tsl1401_get_img();
		//mode_get();
		//if(startline==1)
		//stop();
		image_error=get_img_error();
		servo_angle=servo_angle_cal(image_error);
		expect_speed_duty=motor_speed_cal(image_error);
		//servo_control(servo_angle);
		//motor_control(expect_speed_duty);//CCD慢速手动测试
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

void Motor_init(void)
{
        gpio_init(MOTOR1_EN_IO,GPO,HIGH);//初始化BTN INH
        gpio_init(MOTOR2_EN_IO,GPO,HIGH);
	FTM_PWM_init(FTM0, FTM_CH3, Motor_Hz,100);//野火使用CH3，CH4，同时激光灯开
	FTM_PWM_init(FTM0, FTM_CH4, Motor_Hz,100);
	//FTM_PWM_init(FTM0, FTM_CH3, Motor_Hz,0);
}