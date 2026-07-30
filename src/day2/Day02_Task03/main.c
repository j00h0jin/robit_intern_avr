/*
 * Day02_Task03.c
 *
 * Created: 2026-07-29 오전 10:48:02
 * Author : hojin
 */ 

#define F_CPU 16000000
#include "LCD_Text.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void)
{
	// D2 -> SW 1번, D3 -> 2번, E4 -> 3번, E5 -> 4번
	DDRD = 0x00; // 입력 설정 PD, PE
	DDRE = 0x00; 

	PORTD |= (1 << PIND2) | (1 << PIND3); // 풀업 설정
	PORTE |= (1 << PINE4) | (1 << PINE5);

	int a = 1, b = 1; // A, B
	int  c = 0, symbol = 0; // 기호 switch
	float answer = 0; // 답

	lcdInit();
	lcdClear(); // lcd 초기 설정 후 clear

    while (1) 
    {
		lcdString(0,0,"Jhj");
		if(!(PIND & (1<<PIND2))) // 1번 A 증가
		{
			a += 1;
			_delay_ms(300);
		}
		if(!(PIND & (1<<PIND3))) // 2번 기호 변경
		{
			c += 1;
			symbol = c % 4; 
			_delay_ms(300);
		}
		if(!(PINE & (1<<PINE4))) // 3번 B 증가
		{
			b += 1;
			_delay_ms(300);
		}
		if(!(PINE & (1<<PINE5))) // 4번을 누르면 답이 보임
		{
			switch(symbol) // 스위치 문에 따라 현재 기호에 맞는 answer 지정
			{
				case 0:
				answer = a + b;
				break;
				case 1:
				answer = a - b;
				break;
				case 2:
				answer = a * b;
				break;
				case 3:
				answer = (float)a / b;
				break;
				default:
				break;
			}
			lcdString(1,7,"=");
			lcdFloat(1,8,answer,1);
			_delay_ms(50);
		}
		else // 누르지 않았을 때 빈 화면으로 덮기
		{
			lcdString(1,7,"      ");
		}
			
		lcdNumber(1,0,a);
		switch(symbol)
		{
			case 0:
				lcdString(1,3,"+");
				answer = a + b;
				break;
			case 1:
				lcdString(1,3,"-");
				answer = a - b;
				break;
			case 2:
				lcdString(1,3,"*");
				answer = a * b;
				break;
			case 3:
				lcdString(1,3,"/");
				answer = a / b;
				break;
			default:
				lcdString(1,3," ");
				break;			
		}
		lcdNumber(1,4,b);
    }
}

