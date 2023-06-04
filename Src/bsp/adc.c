#include "adc.h"


ADC_HandleTypeDef hadc2;

/* ADC2 init function */
void ADC2_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.GainCompensation = 0;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc2.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(adcHandle->Instance==ADC2)
  {
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
    PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* ADC2 clock enable */
    __HAL_RCC_ADC12_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC2 GPIO Configuration
    PA4     ------> ADC2_IN17
    PA5     ------> ADC2_IN13
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC2)
  {
    __HAL_RCC_ADC12_CLK_DISABLE();

    /**ADC2 GPIO Configuration
    PA4     ------> ADC2_IN17
    PA5     ------> ADC2_IN13
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_5);

  }
}


uint16_t get_AO1(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.Channel = ADC_CHANNEL_17;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
	
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);
	
	uint16_t adc;

	HAL_ADC_Start(&hadc2);
	adc = HAL_ADC_GetValue(&hadc2);
	
	return adc;
}

uint16_t get_KEY(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
	
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);
	
	uint16_t adc;

	HAL_ADC_Start(&hadc2);
	adc = HAL_ADC_GetValue(&hadc2);
	
	return adc;
}


uint16_t Read_Btn(void)
{
	uint8_t i = 0, j = 0;
	uint16_t Btn_Buff[50], temp;

	for(i=0; i<50; i++)
	{
		Btn_Buff[i] = get_KEY();
		//HAL_Delay(1);
	}
	
	for(i=0; i<25; i++)
	{
		for(j=0; j<50-i-1; j++)
		{
			if(Btn_Buff[j+1] < Btn_Buff[j])
			{
				temp = Btn_Buff[j+1];
				Btn_Buff[j+1] = Btn_Buff[j];
				Btn_Buff[j] = temp;
			}
		}
	}
	
	return ((Btn_Buff[25-1] + Btn_Buff[25])/2);
}



uint8_t Read_Key(void)
{
	uint16_t Btn_Temp;
	Btn_Temp = Read_Btn();
	
	if(Btn_Temp < 0x0fff/14)
		return 1;
	
	else if((Btn_Temp > 1*0x0fff/14) && (Btn_Temp < 3*0x0fff/14))
		return 2;

	else if((Btn_Temp > 3*0x0fff/14) && (Btn_Temp < 5*0x0fff/14))
		return 3;

	else if((Btn_Temp > 5*0x0fff/14) && (Btn_Temp < 7*0x0fff/14))
		return 4;

	else if((Btn_Temp > 7*0x0fff/14) && (Btn_Temp < 9*0x0fff/14))
		return 5;

	else if((Btn_Temp > 9*0x0fff/14) && (Btn_Temp < 11*0x0fff/14))
		return 6;

	else if((Btn_Temp > 11*0x0fff/14) && (Btn_Temp < 13*0x0fff/14))
		return 7;

	else if((Btn_Temp > 13*0x0fff/14) && (Btn_Temp < 0x0f6f))
		return 8;	
	
	else
		return 0;
}


