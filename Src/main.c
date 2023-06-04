#include "main.h"
#include "rcc.h"
#include "led.h"
#include "adc.h"
#include "lcd.h"
#include "uart.h"
#include "i2c_hal.h"


void led_proc(void);
void key_proc(void);
void lcd_proc(void);
void adc_proc(void);

struct s_stuff
{
	uint8_t number;
	float price;
	float height;
	float total;
}stuff[3];


//for delay
__IO uint32_t uwTick_led, uwTick_lcd, uwTick_key, uwTick_lcd, uwTick_adc;

//for led
uint8_t led;
uint32_t count_led;

//for key
uint8_t key_val, key_old, key_down, key_up;
uint32_t count_jia, count_jian;

//for lcd 
uint8_t string_lcd[21];
uint8_t lcd_index;
uint8_t	which_para;

//for adc
float RP5;

//for eeprom
uint8_t eeprom_W[8] = {'a', 2, 2, 3, 0, 0, 0, 0};
uint8_t eeprom_R[8];

//for uart
char uart_buf[50];

//for task
uint8_t which_stuff;  //0,1,2
uint8_t select_flag;
uint8_t setting_times;
float price_1;
float price_2;
float price_3;

int main(void)
{
  HAL_Init();
	//HAL_Delay(1);
  SystemClock_Config();
	
	Led_Init();
	ADC2_Init();
	
	MX_USART1_UART_Init();
	
	LCD_Init();
	LCD_SetTextColor(White);
	LCD_SetBackColor(Black);
	LCD_Clear(Black);
	
	stuff[0].number = 1;
	stuff[0].price = 0.2;
	stuff[0].height = 8.0;
	stuff[0].total = 1.6;
	
	stuff[1].number = 2;
	stuff[1].price = 0.2;
	stuff[1].height = 8.0;
	stuff[1].total = 1.6;
		
	stuff[2].number = 3;
	stuff[2].price = 0.3;
	stuff[2].height = 8.0;	
	stuff[2].total = 2.4;

	price_1 = stuff[0].price;
	price_2 = stuff[1].price;
	price_3 = stuff[2].price;
	
	I2CInit();
	eeprom_read(eeprom_R, 0, 8);
	if(eeprom_R[0] != 'a')
	{
		eeprom_write(eeprom_W, 0 ,8);
	}
	
	else
	{
		stuff[0].price = 0.1*eeprom_R[1] + 0.01*eeprom_R[4] ;
		stuff[1].price = 0.1*eeprom_R[2] + 0.01*eeprom_R[5] ;
		stuff[2].price = 0.1*eeprom_R[3] + 0.01*eeprom_R[6] ;
		price_1 = stuff[0].price;
		price_2 = stuff[1].price;
		price_3 = stuff[2].price;
		setting_times = eeprom_R[7];
	}
	
	
  while (1)
  {
		led_proc();
		key_proc();
		lcd_proc();
		adc_proc();
  }

}

void adc_proc(void)
{
	if(uwTick - uwTick_adc < 50)	return;
	uwTick_adc = uwTick;
	
	if(select_flag ==1)
	{
		HAL_Delay(1);
		get_AO1();
		HAL_Delay(1);
		
		RP5 = get_AO1()*0.0008056640625;
		stuff[which_stuff].height = RP5*3.0303030303;	
	}
}



void key_proc(void)
{
	if(uwTick - uwTick_key < 50)	return;
	uwTick_key = uwTick;
	
	HAL_Delay(1);
	get_KEY();
	HAL_Delay(1);
	
	key_val = Read_Key();
	key_down = key_val & (key_val ^ key_old);
	key_up  = ~key_val & (key_val ^ key_old);
	key_old = key_val;
	
	if(key_down == 1)
	{
		lcd_index ^= 1;
		which_para = 0;
		LCD_Clear(Black);
		LCD_SetBackColor(Black);
		
		
		if(lcd_index == 0)
		{
			if((stuff[0].price == price_1) && (stuff[1].price == price_2) && (stuff[2].price == price_3))
			{
			
			}
			else 
			{
				stuff[0].price = price_1;
				stuff[1].price = price_2;
				stuff[2].price = price_3;
				setting_times++;
			}

			
			eeprom_W[0] = (uint8_t)(10*stuff[0].price);
			eeprom_W[3] = (uint16_t)(100*stuff[0].price)%10;
			eeprom_W[1] = (uint8_t)(10*stuff[1].price);
			eeprom_W[4] = (uint16_t)(100*stuff[1].price)%10;
			eeprom_W[2] = (uint8_t)(10*stuff[2].price);
			eeprom_W[5] = (uint16_t)(100*stuff[2].price)%10;
			eeprom_W[6] = setting_times;
			eeprom_write(eeprom_W, 1, 7);		
			
			sprintf(uart_buf, "U.W.1:%.2f\r\nU.W.2:%.2f\r\nU.W.3:%.2f\r\n",stuff[0].price, stuff[1].price, stuff[2].price);
			HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, strlen(uart_buf), 50);
		}
	}
		
	else if(key_down == 2)
	{
		if(lcd_index != 0)
		{
			switch(which_para)
			{
				case 0:	if(price_1 < 10)	price_1+=0.01;	break;
				case 1:	if(price_2 < 10)	price_2+=0.01;	break;
				case 2:	if(price_3 < 10)	price_3+=0.01;	break;
			}	
		}
	}

	else if(key_down == 3)
	{
		if(lcd_index != 0)
		{
			switch(which_para)
			{
				case 0:	if(price_1 >= 0.001)	price_1-=0.01;	if(price_1<0)	price_1 = -price_1;	break;
				case 1:	if(price_2 >= 0.001)	price_2-=0.01;	if(price_2<0)	price_2 = -price_2;	break;
				case 2:	if(price_3 >= 0.001)	price_3-=0.01;	if(price_3<0)	price_3 = -price_3;	break;
			}			
		}
	}
	
	else if(key_down == 4)
	{
		which_para++;
		if(which_para >= 3)
			which_para = 0;
	}
	
	else if(key_down == 5)
	{
		select_flag = 1;
		which_stuff = 0;
	}		
	
	else if(key_down == 6)
	{
		select_flag = 1;
		which_stuff = 1;
	}		
	
	else if(key_down == 7)
	{
		select_flag = 1;
		which_stuff = 2;
	}		
	
	else if(key_down == 8)
	{
		if(select_flag ==1)
		{
			stuff[which_stuff].total = stuff[which_stuff].height * stuff[which_stuff].price;
			select_flag = 0;
			
			sprintf(uart_buf, "U.W.%d:%.2f\r\nG.W:%.2f\r\nTotal:%.2f\r\n", (which_stuff+1), stuff[which_stuff].price, stuff[which_stuff].height, stuff[which_stuff].total);
			HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, strlen(uart_buf), 50);
		}
	}	
	
	switch(Read_Key())
	{
		case 2:	
			count_jia++;	
			if(count_jia >= 17)	
			{
				switch(which_para)
				{
					case 0:	if(price_1 < 10)	price_1+=0.01;	break;
					case 1:	if(price_2 < 10)	price_2+=0.01;	break;
					case 2:	if(price_3 < 10)	price_3+=0.01;	break;
				}	
			}
			break;
			
		case 3:	
			count_jian++;
			if(count_jian >= 17)	
			{
				switch(which_para)
				{
					case 0:	if(price_1 >= 0.001)	price_1-=0.01;	if(price_1<0)	price_1 = -price_1;	break;
					case 1:	if(price_2 >= 0.001)	price_2-=0.01;	if(price_2<0)	price_2 = -price_2;	break;
					case 2:	if(price_3 >= 0.001)	price_3-=0.01;	if(price_3<0)	price_3 = -price_3;	break;
				}			
			}
			break;
				
		case 0:	count_jia=0;	count_jian=0;	break;
	}
	
}


void led_proc(void)
{
	if(uwTick - uwTick_led < 400)	return;
	uwTick_led = uwTick;
	
	//led ^= 0x01;
	if(select_flag) //0.8s
	{
		count_led++;
		if(count_led%2 != 0)
			led ^= 0x01;
	}
	
	else if(lcd_index == 1)
		led ^= 0x01;
	
	else 
		led = 0x00;
	
	led_disp(led);
}



void lcd_proc(void)
{
	if(uwTick - uwTick_lcd < 50)	return;
	uwTick_lcd = uwTick;
	
	if(lcd_index == 0)
	{
		sprintf((char *)string_lcd, "      cheng_zhong");
		LCD_DisplayStringLine(Line0, string_lcd);
		
		sprintf((char *)string_lcd, "bian_hao: %d", stuff[which_stuff].number);
		LCD_DisplayStringLine(Line2, string_lcd);

		sprintf((char *)string_lcd, "dan_jia : %.2f", stuff[which_stuff].price);
		LCD_DisplayStringLine(Line4, string_lcd);		
	
		sprintf((char *)string_lcd, "zhong_li: %.2f", stuff[which_stuff].height);
		LCD_DisplayStringLine(Line6, string_lcd);

		sprintf((char *)string_lcd, "zong_jia: %.2f", stuff[which_stuff].total);
		LCD_DisplayStringLine(Line8, string_lcd);		
	}
	
	else
	{
		LCD_SetBackColor(Black);
		sprintf((char *)string_lcd, "      Para_Setting");
		LCD_DisplayStringLine(Line0, string_lcd);
		
		if(which_para == 0)
			LCD_SetBackColor(Green);
		else
			LCD_SetBackColor(Black);
		sprintf((char *)string_lcd, "stuff_1_price: %.2f", price_1);
		LCD_DisplayStringLine(Line2, string_lcd);

		if(which_para == 1)
			LCD_SetBackColor(Green);
		else
			LCD_SetBackColor(Black);
		sprintf((char *)string_lcd, "stuff_1_price: %.2f", price_2);
		LCD_DisplayStringLine(Line4, string_lcd);		
	
		if(which_para == 2)
			LCD_SetBackColor(Green);
		else
			LCD_SetBackColor(Black);
		sprintf((char *)string_lcd, "stuff_1_price: %.2f", price_3);
		LCD_DisplayStringLine(Line6, string_lcd);	
		
		LCD_SetBackColor(Black);
		sprintf((char *)string_lcd, "    setting_times:%d  ", setting_times);
		LCD_DisplayStringLine(Line9, string_lcd);	
	}
}





void Error_Handler(void)
{

  __disable_irq();
  while (1)
  {
  }
}

