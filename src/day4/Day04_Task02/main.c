/*
 * Day04_Task02.c
 *
 * Created: 2026-07-31 오전 11:14:32
 * Author : hojin
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "include/LCD_Text.h"

unsigned int count8ms = 0;
unsigned int year =0;
unsigned int month =0;
unsigned int day =0;
unsigned int hour =0;
unsigned int min =0;
unsigned int sec =0;
unsigned int mSec =0;

int current = 0;
int isFlow = 0;

unsigned int Read_ADC();
unsigned int getDay(unsigned int year, unsigned int month);

ISR(INT2_vect) // ISR(INTX_vect) {}  << 인터럽트 지정
{
	_delay_ms(15);
	
	if((PIND & (1<<PIND2))!=0)
	{
		EIFR = ( 1<< INTF2);
		return;
	}
	
	current += 1; // 버튼을 누르면 다음 시간으로 넘어감
}

ISR(INT3_vect) // ISR(INTX_vect) {}  << 인터럽트 지정
{
	_delay_ms(15);
	
	if((PIND & (1<<PIND3))!=0)
	{
		EIFR = ( 1<< INTF3);
		return;
	}
	if(current > 6)
		isFlow = 1; // isFlow = 1이면 타임 인터럽트
}

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 131;

	if(isFlow == 0)
	return;

	count8ms++;
	mSec += 8;

	if(count8ms >= 125|| mSec >=1000)
	{
		count8ms = 0;
		mSec = 0;
		sec++;

		if(sec >= 60)
		{
			sec = 0;
			min++;

			if(min >= 60)
			{
				min = 0;
				hour++;

				if(hour >= 24)
				{
					hour = 0;
					day++;

					if(day > getDay(year, month))
					{
						day = 1;
						month++;

						if(month > 12)
						{
							month = 1;
							year++;
						}
					}
				}
			}
		}
	}
}


int main(void)
{
	
	DDRD = 0x00; // D2 -> SW 1번, D3 -> 2번
	
	EICRA |= (1 << ISC21) | (1 << ISC31);
	EIMSK |= (1 << INT2) | (1 << INT3); // 인터럽트 활성화
	
	TCNT0 = 131;
	TCCR0 = 0x07;
	TIMSK = 0x01;
	
	ADMUX = 0x40;  // 0100 0000 외부의 AVCC 전압 사용
	ADCSRA = 0x87; // 1000 0111 ADC 활성화, 클럭 분주비 128
	
	SREG = 0x80;
	
	lcdInit();
	lcdClear();

	
	unsigned int potentiometerValue = 0; // 가변저항 값 : 0~1023

	
    while (1) 
    {
		potentiometerValue = Read_ADC();
		switch(current) { // current 값에 따라 시간 값 수정(가변저항 값을 범위에 맞게 저장)
			case 0:
			year = 2000 + ((unsigned long)potentiometerValue * 100 / 1024);
			lcdNumber(0,0,year);
			break;
			case 1:
			month = 1 + ((unsigned long)potentiometerValue * 12 / 1024);
			lcdNumber(0,4,month);
			break;
			case 2:
			day = 1 + ((unsigned long)potentiometerValue * getDay(year, month) / 1024);
			lcdNumber(0,6,day);
			break;
			case 3: 
			hour = (unsigned long)potentiometerValue * 24 / 1024;
			lcdNumber(1,0,hour);
			break;
			case 4: 
			min = (unsigned long)potentiometerValue * 60 / 1024;
			lcdString(1, 2, ":");
			lcdNumber(1,3,min);
			break;
			case 5: 
			sec = (unsigned long)potentiometerValue * 60 / 1024;
			lcdString(1, 5, ":");
			lcdNumber(1,6,sec);
			break;
			case 6:
			mSec = (unsigned long)potentiometerValue * 1000 / 1024;
			lcdString(1, 8, ".");
			lcdNumber(1,9,mSec/10);
			break;
			default:
			lcdNumber(0,0,year);
			lcdNumber(0,4,month);
			lcdNumber(0,6,day);
			lcdNumber(1,0,hour);
			lcdString(1, 2, ":");
			lcdNumber(1,3,min);
			lcdString(1, 5, ":");
			lcdNumber(1,6,sec);
			lcdString(1, 8, ".");
			lcdNumber(1,9,mSec/10);
			break;
		}
    }
}

unsigned int Read_ADC()
{
	ADCSRA |= (1 << ADSC); // 변환 시작 ADSC = 1
	return ADC;
}

// 윤년, 월에 따른 day를 return
// 윤년인 경우 2월 판별, 그 외에는 30일과 31일을 나눔
// 31 28(29) 31 30 31 30 31 31 30 31 30 31
unsigned int getDay(unsigned int year, unsigned int month) {
	if(month == 2) {
		// 윤년 판별식
	if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
		return 29;
	else
		return 28;
	}
	if (month == 4 ||month == 6 || month == 9 || month == 11)
		return 30;
	
	return 31;
}
	