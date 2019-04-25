#include "F8L10_TASK.h"
#include "usart.h"
#include "string.h"
#include "systick.h"
#include "timer.h"
#include "F8L10_LORA.h"
#include "stdlib.h"

uint8_t F8L10D_IntoATMode(char *cal_str);

TASK_RECV_BUFFER_T TASKRecvBuf ;
TASK_RECV_MESSAGE_QUEUE_NODE_T TASKRecvTempNode ;
static uint32_t F8L10D_LocalTime = 0;
uint8_t F8L10D_TaskID = 1;
uint8_t F8L10D_TX_Flag = 0;
typedef void (*F8L10D_Config)(char * tx_str,char * cal_str,uint32_t timeout,uint8_t retry,uint8_t *task_id);

F8L10D_TASK_NAME  AT_TASK_NAME;

F8L10D_TASK  F8L10D_Task[max_task_size] = 
{
		{1,2000,max_retry_times,"+++","OK",F8L10D_IntoATMode},
		{2,2000,max_retry_times,"AT+DEI?\r\n","+DEI",F8L10D_IntoATMode},//查询设备序列号
		{3,2000,max_retry_times,"AT+AEI?\r\n","+AEI",F8L10D_IntoATMode},//查询应用序列号
		{4,2000,max_retry_times,"AT+AKY?\r\n","+AKY",F8L10D_IntoATMode},//查询应用密钥
		{5,2000,max_retry_times,"AT+SAV\r\n","OK",F8L10D_IntoATMode},//保存设置
		{6,2000,max_retry_times,"AT+SRS\r\n","OK",F8L10D_IntoATMode},//重启
		
		
	//{6,2000,max_retry_times,"AT+MOD=1\r\n","OK",F8L10D_IntoATMode},//AT模式设置		
	//{6,2000,max_retry_times,"AT+JON\r\n","OK",F8L10D_IntoATMode},//加网		
//		{2,1000,max_retry_times,},
//    {3,1000,max_retry_times,},
//		{4,1000,max_retry_times,},
		
};

void init_timer(uint16_t arr,uint16_t period,uint16_t pulse_red,uint16_t pulse_green,uint16_t pulse_blue);

char *strnstr(const char *s1, const char *s2, uint32_t len)  
{  
    uint32_t l2;  
  
    l2 = strlen(s2);  
    if (!l2)  
        return (char *)s1;  
    while (len >= l2) {  
        len--;  
        if (!memcmp(s1, s2, l2))  
            return (char *)s1;  
        s1++;  
    }  
    return NULL;  
}

//16进制发送数据
void F8L10D_SendHEX(uint8_t* string)
{
		const char *Hex_Start = "AT+TXA=";
		const char *Hex_end = "\r\n";
	  char *HexData = (char *)malloc(strlen(Hex_Start) + strlen(Hex_end) + strlen((char*)string));
	  sprintf(HexData, "%s%s%s", Hex_Start, string , Hex_end);
	  usart_sendstring(USART3,(uint8_t *)HexData,strlen(HexData));//发送指定AT16进制消息
}
//模块信息回调函数
uint8_t 	F8L10D_IntoATMode(char *cal_str)
{
		const char *DELIM= (const char*)cal_str;//获取校验字符
		const int DELIM_LEN = strlen(DELIM);//获取校验字符串长度
		char *start = NULL;
	  unsigned int  startlen = 0 ;
	  startlen = TASKRecvBuf.msgLength ;//获取当前缓存长度
	  start = strnstr((const char *)&TASKRecvBuf.buffer,DELIM,startlen);//在目前缓存内判断是否有校验字符串数据
		if(start != NULL)
				return 1;
		else 
				return 0;
}


int receiveByteFromF8L10D_LORA(enum BOARD_USART_TYPE usart_no,unsigned char recv)
{
		if( TASKRecvBuf.msgLength<sizeof(TASKRecvBuf.buffer) )
		{
				TASKRecvBuf.buffer[TASKRecvBuf.msgLength] = recv ;
				TASKRecvBuf.msgLength++ ;
				TASKRecvBuf.lastDataAppendingTime = local_ticktime();
		}
		return 0;
}	

void clearF8L10DBuf(void)
{
		TASKRecvBuf.msgLength = 0x00;
	  memset(TASKRecvBuf.buffer,0,sizeof(TASKRecvBuf));
	  TASKRecvBuf.lastDataAppendingTime = 0x00;
}
//复位四信模块
void F8L10D_ResetFun(void)
{
		F8L10_LORA_RST();//超时 超次数 复位
		clearF8L10DBuf();//清除缓存
		F8L10D_TX_Flag = 0;//发送标志清零
		F8L10D_TaskID = 1;//进程ID置1
}

//void F8L10D_ConfigVerify(char * cal_str)
//{
//		const char *DELIM= (const char*)cal_str;
//		const int DELIM_LEN = strlen(DELIM);
//		char *start = NULL;
//	  unsigned int  startlen = 0 ;
//	  startlen = TASKRecvBuf.msgLength ;
//	  start = strnstr((const char *)&TASKRecvBuf.buffer,DELIM,startlen);
//	  if(start != NULL)
//		{
//			  memset(TASKRecvBuf.buffer,0,TASKRecvBuf.msgLength);
//			  TASKRecvBuf.msgLength  = 0x00;
//		}
//	  
//}

//根据task_id执行配置操作
void F8L10D_Context(F8L10D_TASK *F8L10D_task)
{
		uint8_t  i;
	  /*遍历查询任务*/
		for(i = 0;i < max_task_size;i++)
		{
				if(F8L10D_task[i].pid == F8L10D_TaskID)//有任务需要进行
				{
						if(!F8L10D_TX_Flag)//未发送过信息
						{
								usart_sendstring(USART3,(uint8_t *)F8L10D_task[i].tx_str,strlen(F8L10D_task[i].tx_str));//发送指定AT命令  
							  init_timer(64,1000,000,000,500);//
							  Delay_ms(50);
							  F8L10D_LocalTime =  local_ticktime();	
                F8L10D_TX_Flag = 1;							
						}
						else
						{
								if(F8L10D_task[i].task_callback(F8L10D_task[i].cal_str)) //检测指定任务 数据接收校验正确
								{
									  init_timer(64,1000,000,500,000);//green
									  Delay_ms(50);
										F8L10D_TaskID++;//进入下一任务
									  F8L10D_TX_Flag = 0;//发送标志置零
									  F8L10D_task[i].retry = max_retry_times;//恢复该阶段最大失败尝试次数
									  clearF8L10DBuf();//清除接收缓存
								}
								else //接收数据校验失败
								{
									  /*判断是否超时*/							   
										if(timeout(F8L10D_LocalTime,F8L10D_task[i].max_timeout)) //超时
										{
											  init_timer(64,1000,500,000,000);//red
							          Delay_ms(50);
												if(F8L10D_task[i].retry)//判断最大尝试次数未结束
												{
														usart_sendstring(USART3,(uint8_t *)F8L10D_task[i].tx_str,strlen(F8L10D_task[i].tx_str));//重复发送指定AT命令
													  F8L10D_LocalTime =  local_ticktime();
													  F8L10D_task[i].retry --;//最大尝试次数衰减
												}
												else //最大尝试次数结束
												{
													
														F8L10D_ResetFun();
													  F8L10D_task[i].retry = max_retry_times;//恢复最大失败尝试次数
													  
												}
										}
								}
						}
				}
		}	
}

void F8L10D_Test(void)
{
		F8L10D_Context(F8L10D_Task);		
}





