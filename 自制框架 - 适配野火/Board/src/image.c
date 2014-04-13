 /*!
  *     COPYRIGHT NOTICE
  *     Copyright (c) 2013,野火科技
  *     All rights reserved.
  *     技术讨论：野火初学论坛 http://www.chuxue123.com
  *
  *     除注明出处外，以下所有内容版权均属野火科技所有，未经允许，不得用于商业用途，
  *     修改内容时必须保留野火科技的版权声明。
  *
  * @file       main.c
  * @brief      野火K60 平台主程序
  * @author     野火科技
  * @version    v5.0
  * @date       2013-08-28
  */

#include "common.h"
#include "include.h"

#define Array_Size   30
#define BIN_MAX 0xd0

//定义存储接收CCD图像的数组
extern uint8 CCD_BUFF[TSL1401_MAX*3][TSL1401_SIZE];

void abs_diff(uint8 *dst,uint8 *src,uint16 len/*,uint8 * maxval*/,uint8 * avgval);
void maxvar(uint8 *buf,uint16 len,uint8  maxval);
// //用于记录最大差分值的点

//组合数组函数定义
void List_Array(uint8 *Array_list,uint8 LineKey);

uint8 SmallPoint = 0;
uint8 BigPoint = 0;
uint8 MiddPoint = 0;
uint8 def_diff_fazhi = 15;//差分最大值的预设值

//定义的三个用于存放边界点的数组
uint8 Array_Small[Array_Size] = {0};
uint8 Array_Big[Array_Size] = {0};
uint8 Array_Midd[Array_Size] = {0};


uint8  avg[TSL1401_SIZE];
extern uint8 time;
void CalculationExposureTime();//动态计算曝光时间


/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       野火 线性CCD 测试实验
                修改 PIT0 的定时时间即可修改曝光时间
 */
int  get_img_error(void)
{
    /**/
    Site_t site1={0,0};                         //显示图像左上角位置
    Site_t site_left={0,70};                     //用于显示差分最大值位置
    Site_t site_right={100,70};
    Site_t site_midd={50,70};
    
    Size_t imgsize={TSL1401_SIZE,1};            //图像大小
    Size_t size={TSL1401_SIZE,30};              //显示区域大小
   
    
    
   
    /**/
        //uint8  max[TSL1401_SIZE];
    	//uint8  avg[TSL1401_SIZE];

        //LCD 显示图像
        LCD_Img_gray_Z(site1,size,(uint8 *)&CCD_BUFF[0],imgsize);
       // LCD_Img_gray_Z(site2,size,(uint8 *)&CCD_BUFF[1],imgsize);


        //LCD 显示波形图                                    蓝线
        LCD_wave_display(site1,size,(uint8 *)&CCD_BUFF[0],BIN_MAX,BLUE );
       // LCD_wave_display(site2,size,(uint8 *)&CCD_BUFF[1],BIN_MAX,BLUE ); 

        //求波形差分
        abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],TSL1401_SIZE/*,&max[1]*/,&avg[0]);
       // abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],TSL1401_SIZE,&max[1],&avg[1]);
        
        CalculationExposureTime();//计算下一次曝光时间
        
        //LCD 显示差分图  
        //LCD_wave_display(site1,size,(uint8 *)&CCD_BUFF[TSL1401_MAX+0],max[0],GREEN);        
        //
        
        //显示三个数
        LCD_num_BC(site_left, SmallPoint,3,FCOLOUR,BCOLOUR);
        LCD_num_BC(site_right,BigPoint,3,FCOLOUR,BCOLOUR);
        LCD_num_BC(site_midd, MiddPoint,3,FCOLOUR,BCOLOUR);
        //
        
        //将得到的三条线放入对应数组
        List_Array(Array_Small,SmallPoint);
        List_Array(Array_Big,BigPoint);
        List_Array(Array_Midd,MiddPoint);

	return (MiddPoint-64);

}


//****************************计算动态曝光时间*************************//


void CalculationExposureTime()
{
    if(*avg > 128)
    {
        time--;
    }
    else if(*avg < 128)
    {
        time++;
    }
    else
      ;
    if(time<=1)
      time =1;
    else if(time>=20)
      time=20;
}


void maxvar(uint8 *buf,uint16 len,uint8  maxval)
{
    while(len--)
    {
        if(buf[len] > maxval)
        {
            buf[len]= maxval;
        }
    }

}



/*!
 *  @brief      计算差分
 *  @since      v5.0
 *  @note       野火差分法补充说明：差分最大值maxval 和 差分平均值avgval 这两个
                参数是为了便于定义确定阈值而加入的，可删除。差分平均值，一般返回结果
                都非常小，因此顶层用不上，建议删掉（此处保留是为了给大家验证）
 */
void abs_diff(uint8 *dst,uint8 *src,uint16 len/*,uint8 * maxval*/,uint8 * avgval)
{
    int8 tmp;
    
    uint8 max = 0;
    uint8 max2 = 0;
    uint32 sum = 0;
   // uint32 sum2 = 0;
    uint16 lentmp = 0;
    uint16 lentmp2 = 64;
    while(lentmp < 63)                                  //得到左侧最大差分值           
    {
        sum += src[lentmp];//将所有灰度值放入sum中，这里只有0~62
        tmp =src[lentmp+2] - src[lentmp];
        if(tmp > max )
        {
            SmallPoint = lentmp; 
            max = tmp;
        }
        //sum += src[lentmp];
        *dst = tmp;
        dst++;
        lentmp++;
    }
    if(max > def_diff_fazhi)                          //判断最大差分值，如果大于预设值直接返回
    {;}
    else
    {
      SmallPoint = 0;
      /***********************************************
      //sum = sum / 63;
      if((src[0]+src[1])/2 < 128)                                  
      {
        SmallPoint = 63;
      }
      else
      {
        SmallPoint = 0;
      }
      *******************************************************/
    }
    
    
    while(lentmp2 < 127)                             //得到右侧最大差分值  
    {
        sum +=src[lentmp2];//将所有灰度值放入sum中，这里只有64~125
        tmp =src[lentmp2-2] - src[lentmp2];
        if(tmp > max2 )
        {
            BigPoint = lentmp2; 
            max2 = tmp;
        }
        //sum2 += src[lentmp2];
        dst++;
        lentmp2++;
    }
    if(max2 > def_diff_fazhi)                     //判断最大差分值，如果大于预设值直接返回       
    {;}
    else
    {
      BigPoint = 127;
      /************************************
      //sum2 = sum2 / 63;
      if((src[126]+src[127])/2 < 128)             
      {
        BigPoint = 64;
      }
      else
      {
        BigPoint = 127;
      }
      *************************************/
    }
    *dst = 0;               // 最后一个 点配置为 0
    
    // *maxval = max;           // 返回最大绝对差值
    *avgval =(uint8)(sum/126);  //只有126个像素值的平均，用于调节动态曝光
    MiddPoint = (SmallPoint + BigPoint)/2; 
    printf("%d",MiddPoint);
}


//将得到的三条线放入数组的函数
void List_Array(uint8 *Array_list,uint8 LineKey)
{
    uint8 i = 0;
    for(i = 0;i < Array_Size-1;i++)
    {
         Array_list[i]=Array_list[i+1];
    }
    Array_list[i] = LineKey;
}


