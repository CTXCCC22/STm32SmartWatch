#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Menu.h"
#include "Timer.h"
#include "Key.h"

int main(void)
{
	OLED_Init();
	OLED_Clear();
	Peripheral_Init();
	Timer_Init();
	int clkflag1;
	
	while (1)
	{
		clkflag1 = First_Page_Clock();
		if(clkflag1 == 1)
		{
			Menu_Page();		//进入菜单页面
		}
		else if(clkflag1 == 2)
		{
			Settings_Page();	//进入设置页面
		}
	}
}

//定时器中断函数，可以复制到使用它的地方
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		Key_Tick();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}