/*
 * Day02_Task02.c
 *
 * Created: 2026-07-29 오전 10:47:33
 * Author : hojin
 */ 
#define F_CPU 16000000
#include "LCD_Text.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int Read_ADC() {
	ADCSRA |= (1 << ADSC); // 변환 시작 ADSC = 1
	return ADC;
}


int main(void)
{
	int r;
	float v;
	ADMUX = 0x40; // 0100 0000 외부의 AVCC 전압 사용
	ADCSRA = 0x87; // 1000 0111 ADC 활성화, 클럭 분주비 128

	lcdInit();
	lcdClear();

	while (1)
	{
		r=Read_ADC();
		v=(float)r*5/1024;
		lcdString(0,0,"Jhj"); // row col txt
		lcdNumber(1,0,r); // row col num
		lcdFloat(1,6,v,1); // row col num decimal
		lcdString(1,9,"V");
	}
}

