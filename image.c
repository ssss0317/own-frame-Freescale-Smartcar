 /*!
  *     COPYRIGHT NOTICE
  *     Copyright (c) 2013,Ұ��Ƽ�
  *     All rights reserved.
  *     �������ۣ�Ұ���ѧ��̳ http://www.chuxue123.com
  *
  *     ��ע�������⣬�����������ݰ�Ȩ����Ұ��Ƽ����У�δ������������������ҵ��;��
  *     �޸�����ʱ���뱣��Ұ��Ƽ��İ�Ȩ������
  *
  * @file       main.c
  * @brief      Ұ��K60 ƽ̨������
  * @author     Ұ��Ƽ�
  * @version    v5.0
  * @date       2013-08-28
  */

#include "common.h"
#include "include.h"

#define Array_Size   30

//����洢����CCDͼ�������
extern uint8 CCD_BUFF[TSL1401_MAX*3][TSL1401_SIZE];

void abs_diff(uint8 *dst,uint8 *src,uint16 len,uint8 * maxval,uint8 * avgval);
void maxvar(uint8 *buf,uint16 len,uint8  maxval);
// //���ڼ�¼�����ֵ�ĵ�

//������麯������
void List_Array(uint8 *Array_list,uint8 LineKey);

extern uint8 SmallPoint = 0;
extern uint8 BigPoint = 0;
extern uint8 MiddPoint = 0;
extern uint8 def_diff_fazhi = 10;//������ֵ��Ԥ��ֵ

//������������ڴ�ű߽�������
extern uint8 Array_Small[Array_Size] = 0;
extern uint8 Array_Big[Array_Size] = 0;
extern uint8 Array_Midd[Array_Size] = 0;



/*!
 *  @brief      main����
 *  @since      v5.0
 *  @note       Ұ�� ����CCD ����ʵ��
                �޸� PIT0 �Ķ�ʱʱ�伴���޸��ع�ʱ��
 */
int  get_img_error(void)
{
    	uint8  max[TSL1401_SIZE];
    	uint8  avg[TSL1401_SIZE];


        //���β��
        abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],TSL1401_SIZE/*,&max[1],&avg[1]*/);
       // abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],TSL1401_SIZE,&max[1],&avg[1]);

        
        //���õ��������߷����Ӧ����
        List_Array(Array_Small,SmallPoint);
        List_Array(Array_Big,BigPoint);
        List_Array(Array_Midd,MiddPoint);

	return (MiddPoint-64);

    }
}


/*
	
*/

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
 *  @brief      �����־���ֵ
 *  @since      v5.0
 *  @note       Ұ���ַ�����˵����������ֵmaxval �� ���ƽ��ֵavgval ������
                ������Ϊ�˱��ڶ���ȷ����ֵ������ģ���ɾ�������ƽ��ֵ��һ�㷵�ؽ��
                ���ǳ�С����˶����ò��ϣ�����ɾ�����˴�������Ϊ�˸������֤��
 */
void abs_diff(uint8 *dst,uint8 *src,uint16 len/*,uint8 * maxval,uint8 * avgval*/)
{
    int8 tmp;
    
    uint8 max = 0;
    uint8 max2 = 0;
    //uint32 sum = 0;
   // uint32 sum2 = 0;
    uint16 lentmp = 0;
    uint16 lentmp2 = 64;
    while(lentmp < 64)                                  //�õ���������ֵ           
    {
        tmp =src[lentmp+1] - src[lentmp];
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
    if(max > def_diff_fazhi)                          //�ж������ֵ���������Ԥ��ֱֵ�ӷ���
    {;}
    else
    {
      //sum = sum / 63;
      if((src[0]+src[1])/2 < 128)                                  
      {
        SmallPoint = 63;
      }
      else
      {
        SmallPoint = 0;
      }
    }
    
    
    while(lentmp2 < 127)                             //�õ��Ҳ������ֵ  
    {
        tmp =src[lentmp2] - src[lentmp2+1];
        if(tmp > max2 )
        {
            BigPoint = lentmp2; 
            max2 = tmp;
        }
        //sum2 += src[lentmp2];
        dst++;
        lentmp2++;
    }
    if(max2 > def_diff_fazhi)                     //�ж������ֵ���������Ԥ��ֱֵ�ӷ���       
    {;}
    else
    {
      //sum2 = sum2 / 63;
      if((src[126]+src[127])/2 < 128)             
      {
        BigPoint = 64;
      }
      else
      {
        BigPoint = 127;
      }
    }
    *dst = 0;               // ���һ�� ������Ϊ 0
    
    // *maxval = max;           // ���������Բ�ֵ
    // *avgval = (uint8)(sum/(len-1));  //ǰ len -1 ������ƽ��ֵ
    MiddPoint = (SmallPoint + BigPoint)/2;   
}


//���õ��������߷�������ĺ���
void List_Array(uint8 *Array_list,uint8 LineKey)
{
    uint8 i = 0;
    for(i = 0;i < Array_Size-1;i++)
    {
         Array_list[i]=Array_list[i+1];
    }
    Array_list[i] = LineKey;
}

