#include "stm32f10x.h"                  // Device header
#include "Delay.h"

uint8_t Key_Num;

void Key_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//将PB1和PB11引脚初始化为上拉输入
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}

/**
  * 函    数：按键获取键码
  * 参    数：无
  * 返 回 值：按下按键的键码值，范围：0~2，返回0代表没有按键按下
  * 注意事项：此函数是阻塞式操作，当按键按住不放时，函数会卡住，直到按键松手
  */
uint8_t Key_GetNum(void)
{
	uint8_t temp;
	if(Key_Num)
	{
		temp = Key_Num;
		Key_Num = 0;
		return temp;
	}
	else
	{
		return 0;
	}
}

/**
  * 函    数：按键获取状态
  * 参    数：无
  * 返 回 值：按下按键的键码值，范围：0~2，返回0代表没有按键按下
  * 注意事项：此函数是非阻塞式操作，当按键按住不放时，函数不会卡住，会持续返回按键状态
  */
uint8_t Key_GetState(void)
{
	
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)	//读PB1输入寄存器的状态，如果为0，则代表按键1按下
	{
		return 1;										//置键码为1
	}
	
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)	//读PB11输入寄存器的状态，如果为0，则代表按键2按下
	{
		return 2;										//置键码为2
	}
	
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)	//读PB11输入寄存器的状态，如果为0，则代表按键2按下
	{
		return 3;										//置键码为2
	}
	else
	{
		return 0;
	}
	
}

/**
  * 函    数：按键扫描函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数是非阻塞式操作，当按键按住不放时，函数不会卡住，会持续返回按键状态
  */
void Key_Tick(void)
{
	static uint8_t Count;
	static uint8_t CurrentState,PreState;
	Count++;
	if(Count >= 20)
	{
		Count = 0;
		PreState = CurrentState;
		CurrentState = Key_GetState();
		if(PreState != 0 && CurrentState == 0)
		{
			Key_Num = PreState;
		}	
	}
}


