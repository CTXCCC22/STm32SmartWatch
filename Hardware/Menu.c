/*
此代码是用于STM32F10x系列微控制器的外设初始化和时钟显示功能的实现。它包括对RTC、按键、LED和OLED显示屏的初始化，
以及在OLED上显示当前时间和日期的功能。用户可以通过按键在菜单和设置选项之间切换，并确认选择。
*/
#include "stm32f10x.h"                  // Device header
#include "MyRtc.h"
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "LED.h"

uint8_t KeyNum;
 

void Peripheral_Init(void)
{
	MyRTC_Init();
	Key_Init();
	LED_Init();
}

/*--------------首页时钟----------------*/

void Show_Clock_UI(void)
{
	MyRTC_ReadTime();
	OLED_Printf(0,0,OLED_6X8,"%d-%d-%d",(int)MyRTC_Time[0],(int)MyRTC_Time[1],(int)MyRTC_Time[2]);
	OLED_Printf(16,16,OLED_12X24,"%02d:%02d:%d",(int)MyRTC_Time[3],(int)MyRTC_Time[4],(int)MyRTC_Time[5]);
	OLED_ShowString(0,48,"菜单",OLED_8X16);
	OLED_ShowString(96,48,"设置",OLED_8X16);

}

int clkflag = 1;

int First_Page_Clock(void)
{
	while(1)
	{
		KeyNum = Key_GetNum();
		if(KeyNum == 1)//上一项
		{
			clkflag--;
			if(clkflag<=0)
			{
				clkflag = 2;
			} 
		}
		else if(KeyNum == 2)//下一项
		{
			clkflag++;
			if(clkflag>=3)
			{
				clkflag = 1;
			} 
		}
		else if(KeyNum == 3)//确认
		{
			OLED_Clear();
			OLED_Update();
			return clkflag;
		}
		switch(clkflag)
		{
			case 1:
				Show_Clock_UI();
				OLED_ReverseArea(0,48,32,16);
				OLED_Update();
				break;
			case 2:
				Show_Clock_UI();
				OLED_ReverseArea(96,48,32,16);
				OLED_Update();
				break;
		}
		
	}
}

/*--------------菜单页面----------------*/
// 菜单选项名称（使用ASCII英文，OLED字模库中无对应汉字字模）
char *menu_items[] = {"1.StopWatch", "2.Alarm", "3.BackLight", "4.About"};
#define MENU_COUNT (sizeof(menu_items) / sizeof(menu_items[0]))

int Menu_Page(void)
{
	uint8_t menu_index = 0;
	uint8_t i;
	OLED_Clear();
	OLED_Update();
	
	while(1)
	{
		KeyNum = Key_GetNum();
		if(KeyNum == 1)	    // 上一项
		{
			if(menu_index > 0) menu_index--;
			else menu_index = MENU_COUNT - 1;
		}
		else if(KeyNum == 2)// 下一项
		{
			if(menu_index < MENU_COUNT - 1) menu_index++;
			else menu_index = 0;
		}
		else if(KeyNum == 3)// 确认进入
		{
			OLED_Clear();
			OLED_Update();
			return menu_index;
		}
		// 显示菜单列表
		OLED_Clear();
		OLED_ShowString(0, 0, "-- Menu --", OLED_8X16);
		for(i = 0; i < MENU_COUNT; i++)
		{
			OLED_ShowString(8, 18 + i * 12, menu_items[i], OLED_6X8);
		}
		// 反显选中项
		OLED_ReverseArea(0, 18 + menu_index * 12, 96, 12);
		OLED_Update();
	}
}

/*--------------设置页面----------------*/
void Settings_Page(void)
{
	// 设置：用按键调整时间
	// 调整顺序：年 -> 月 -> 日 -> 时 -> 分 -> 秒
	uint8_t set_index = 0;		// 0~5 对应年月日时分秒
	int16_t temp_time[6];
	uint8_t i;
	
	// 用当前 RTC 时间初始化
	MyRTC_ReadTime();
	for(i = 0; i < 6; i++)
	{
		temp_time[i] = MyRTC_Time[i];
	}
	OLED_Clear();
	OLED_Update();
	
	while(1)
	{
		// 数值边界检查
		if(temp_time[0] < 2020) temp_time[0] = 2099;
		if(temp_time[0] > 2099) temp_time[0] = 2020;
		if(temp_time[1] < 1)    temp_time[1] = 12;
		if(temp_time[1] > 12)   temp_time[1] = 1;
		if(temp_time[2] < 1)    temp_time[2] = 31;
		if(temp_time[2] > 31)   temp_time[2] = 1;
		if(temp_time[3] < 0)    temp_time[3] = 23;
		if(temp_time[3] > 23)   temp_time[3] = 0;
		if(temp_time[4] < 0)    temp_time[4] = 59;
		if(temp_time[4] > 59)   temp_time[4] = 0;
		if(temp_time[5] < 0)    temp_time[5] = 59;
		if(temp_time[5] > 59)   temp_time[5] = 0;
		
		KeyNum = Key_GetNum();
		if(KeyNum == 1)			// 数值减
		{
			temp_time[set_index]--;
		}
		else if(KeyNum == 2)	// 数值增
		{
			temp_time[set_index]++;
		}
		else if(KeyNum == 3)	// 下一项（切换调整项）
		{
			set_index++;
			if(set_index >= 6)
			{
				// 所有项调整完毕，保存并退出
				for(i = 0; i < 6; i++)
				{
					MyRTC_Time[i] = temp_time[i];
				}
				MyRTC_SetTime();
				OLED_Clear();
				OLED_Update();
				return;
			}
		}
		//显示设置界面
		OLED_Clear();
		//显示标题和当前调整的时间
		OLED_ShowString(0, 0, "-Time Set-", OLED_8X16);
		OLED_Printf(0, 16, OLED_8X16, "%04d", (int)temp_time[0]);
		//显示年月日时分秒
		OLED_ShowString(40, 16, "/", OLED_8X16);
		OLED_Printf(48, 16, OLED_8X16, "%02d", (int)temp_time[1]);
		//显示分隔符
		OLED_ShowString(72, 16, "/", OLED_8X16);
		OLED_Printf(80, 16, OLED_8X16, "%02d", (int)temp_time[2]);
		//显示时分秒
		OLED_Printf(16, 36, OLED_8X16, "%02d:%02d:%02d", (int)temp_time[3], (int)temp_time[4], (int)temp_time[5]);
		OLED_Printf(8, 52, OLED_6X8, "Key1:- Key2:+ Key3:OK(%d/6)", set_index + 1);
		OLED_Update();
	}
}	
