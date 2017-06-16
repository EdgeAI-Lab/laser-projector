#include "data_handle.h"
#include "stm32f10x.h"
#include "queue.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>
#include "mcp4728.h"
#include "enable.h"
#include "flash.h"

extern CycQueue *q;

volatile uint8_t offset = 0;
volatile uint8_t rx_index = 0;
uint8_t rx_frame_len = 0;
uint8_t frame_data_len = 0;
uint8_t uart_rx_buf[255] = {0};
uint8_t data_handle_buf[50] = {0};
/*
 *
 * 将串口接收到的数据分为一帧一帧的
 *
 */
void frame_data_prase(void)
{
	if( (rx_index < 255) && (CycQueueIsEmpty(q) == 0 ) ) // CycQueueIsEmpty() return true when queue is empty.
	{

		uint8_t ch = CycQueueOut(q);
		uart_rx_buf[rx_index++] = ch;
		//printf("%x ",ch);
		
	}
	
	if(rx_index <= PROTOCOL_FIXED_LENGTH )
	{
		return;
	}
	
	while((rx_index - offset) >  PROTOCOL_FIXED_LENGTH)
	{
 
			if(uart_rx_buf[offset + HEAD_FIRST] != 0x5A)
			{
			  offset ++;
			  continue;
			}
			
			if(uart_rx_buf[offset + HEAD_SECOND] != 0xA5)
			{
			  offset ++;
			  continue;
			}      

			rx_frame_len = uart_rx_buf[offset + FRAME_LENGTH];
			
			// 
			frame_data_len = rx_frame_len - PROTOCOL_FIXED_LENGTH;
			
			//
			if( (rx_frame_len > 249) && (rx_frame_len == 0) )
			{
				offset += 2;
				continue;
			}

			if(( rx_index < (rx_frame_len + offset) ) || ( (rx_frame_len + offset) < 1))
			{
				break;
			}
			
			uint8_t recvSum = uart_rx_buf[offset + rx_frame_len - 1];
			//printf("recvSum: %x\n",recvSum);
			uint8_t sum = get_check_sum((unsigned char *)&uart_rx_buf[offset],rx_frame_len - 1);
			//printf("sum: %x\n",sum);
			
			//
			if(sum != recvSum)
			{
			  //printf("check sum fail\n");
			  //
			  offset += 2;
			  continue;
			}
			

			memcpy(data_handle_buf,&uart_rx_buf[offset],rx_frame_len);
			
			cmd_handle();
			
			//printf("one frame parse over\n");

			offset += rx_frame_len;
			
			//printf("offset: %d\n",offset);
			
	}//end while
		
		rx_index -= offset;
		  
		//printf("rx_in: %d\n",rx_in);
		
		if(rx_index > 0)
		{
			memcpy(uart_rx_buf,&uart_rx_buf[offset],rx_index);
		}

		
		offset = 0;
		rx_frame_len = 0;
	
}

/*
 * 命令处理
 *
 */
void cmd_handle(void)
{

	switch(data_handle_buf[CMD])
	{
	
		case READ:
			
			read_cmd_handle();
		
		break;
		
		case WRITE:
			
			write_cmd_handle();
		
		break;
		
		default:
			
		break;
		
	}

}


void read_cmd_handle(void)
{
	uint8_t voltage[8] = {0};
	
	switch(data_handle_buf[DEVICE_ENDPOINT])
	{
		
		case 0x19:
			mcp4728_ReadRegEEPROM(voltage);
	
			data_upload(0x19, voltage, 8);
		
		break;
	}
}

void write_cmd_handle(void)
{

	uint8_t voltage[8] = {0};
	
	switch(data_handle_buf[DEVICE_ENDPOINT])
	{
		case RED_LIGHT_ENDPOINT:
		
			if(data_handle_buf[DATA] == 0x30) //attrID set DAC
			{
				single_write(0x00,(float)(data_handle_buf[DATA+2]*100.0 + data_handle_buf[DATA+3])/100.0);
				mcp4728_ReadRegEEPROM(voltage);
	
				data_upload(0x19, voltage, 8);
			}
			
			if(data_handle_buf[DATA] == 0x20) //attrID switch
			{
				if(data_handle_buf[DATA+2] == 0x01)
				{
					RED_ENABLE();
					save_enable_state(RED_CHANNEL,TRUE);
					
				}
				else if(data_handle_buf[DATA+2] == 0x00)
				{
					RED_DISABLE();
					save_enable_state(RED_CHANNEL,FALSE);
				}
				
			}
		
		break;
		
		case GREEN_LIGHT_ENDPOINT:
		
			if(data_handle_buf[DATA] == 0x30) // set DAC
			{
				single_write(0x02,(float)(data_handle_buf[DATA+2]*100 + data_handle_buf[DATA+3])/100.0);
				mcp4728_ReadRegEEPROM(voltage);
	
				data_upload(0x19, voltage, 8);
			}
			
			if(data_handle_buf[DATA] == 0x20)
			{
				if(data_handle_buf[DATA+2] == 0x01)
				{
					GREEN_ENABLE();
					save_enable_state(GREEN_CHANNEL,TRUE);
				}
				else if(data_handle_buf[DATA+2] == 0x00)
				{
					GREEN_DISABLE();
					save_enable_state(GREEN_CHANNEL,FALSE);
				}
			}
		
		break;
			
		case BLUE_LIGHT_ENDPOINT:
		
			if(data_handle_buf[DATA] == 0x30) // set DAC
			{
				single_write(0x04,(float)(data_handle_buf[DATA+2]*100 + data_handle_buf[DATA+3])/100.0);
				mcp4728_ReadRegEEPROM(voltage);
	
				data_upload(0x19, voltage, 8);
			}
			
			if(data_handle_buf[DATA] == 0x20)
			{
				if(data_handle_buf[DATA+2] == 0x01)
				{
					BLUE_ENABLE();
					save_enable_state(BLUE_CHANNEL,TRUE);
				}
				else if(data_handle_buf[DATA+2] == 0x00)
				{
					BLUE_DISABLE();
					save_enable_state(BLUE_CHANNEL,FALSE);
				}
			}
		
		break;
				
		case GREEN532_LIGHT_ENDPOINT:
		
			if(data_handle_buf[DATA] == 0x30) // set DAC
			{
				single_write(0x06,(float)(data_handle_buf[DATA+2]*100 + data_handle_buf[DATA+3])/100.0);
				mcp4728_ReadRegEEPROM(voltage);
	
				data_upload(0x19, voltage, 8);
			}
			
			if(data_handle_buf[DATA] == 0x20)
			{
				if(data_handle_buf[DATA+2] == 0x01)
				{
					GREEN532_ENABLE();
					save_enable_state(GREEN532_CHANNEL,TRUE);
				}
				else if(data_handle_buf[DATA+2] == 0x00)
				{
					GREEN532_DISABLE();
					save_enable_state(GREEN532_CHANNEL,FALSE);
				}
			}
		
		break;
		
		case ALL_LIGHT_ENABLE_ENDPOINT:
		
			if(data_handle_buf[DATA] == 0x01)
			{
				ALL_POWER_ENABLE();
			}
			else if(data_handle_buf[DATA] == 0x00)
			{
				ALL_POWER_DISABLE();
			}
			
		
		break;
		
		
	}
}


/*
 * send ack
 *
 */
void send_ack(void)
{


	
}

