#ifndef 	_F8L10_TASK_H
#define 	_F8L10_TASK_H
#include  "usart.h"
#include 	<stdio.h>
#include 	"stm32f10x.h"

#define MAX_TASK_BUFFER_SIZE (512)
#define MAX_TASK_RECV_BUFFER_SIZE (128)
/*四信串口数据接收缓存*/
#define max_task_size   16
#define max_retry_times   3
#define max_str_length   64
typedef struct {
	unsigned short	msgLength;
	char						buffer[MAX_TASK_BUFFER_SIZE];
	uint32_t				lastDataAppendingTime;
} TASK_RECV_BUFFER_T;

typedef struct {
	uint16_t     	msgLength;
	char					buffer[MAX_TASK_RECV_BUFFER_SIZE];
} TASK_RECV_MESSAGE_QUEUE_NODE_T;


typedef struct
{
		uint8_t 	pid;
	  uint32_t 	max_timeout;
	  uint8_t 	retry;
	  char 			* tx_str;
		char 			* cal_str;
	  uint8_t   (*task_callback)(char * cal_str);

}F8L10D_TASK;

typedef enum
{
		INTO_AT_MODE = 0x00,
		
}F8L10D_TASK_NAME;
//void F8L10D_Context(char * tx_str,char * cal_str,F8L10D_TASK *F8L10D_task);
void F8L10D_Context(F8L10D_TASK *F8L10D_task);

char *strnstr(const char *s1, const char *s2, uint32_t len);

//16进制发送数据
void F8L10D_SendHEX(uint8_t* string);
uint8_t F8L10D_IntoATMode(char *cal_str);

int receiveByteFromF8L10D_LORA(enum BOARD_USART_TYPE usart_no,unsigned char recv);

void clearF8L10DBuf(void);

void F8L10D_ResetFun(void);

//根据task_id执行配置操作
void F8L10D_Context(F8L10D_TASK *F8L10D_task);

void F8L10D_Test(void);


#endif
