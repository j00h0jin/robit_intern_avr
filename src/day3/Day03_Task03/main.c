/*
 * Day03_Task03.c
 *
 * Created: 2026-07-31 오전 11:11:55
 * Author : hojin
 */ 

#define F_CPU 16000000
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "include/LCD_Text.h"
#include "dynamixel_sdk.h"

unsigned char Uart_getch(void);
void Uart_Putch(unsigned char PutData);
int Read_ADC();


int main(void)
{
	UBRR0L = 16; // 57600
	UBRR0H = 0;
	UCSR0A = 0x20;
	UCSR0B = 0x18;
	UCSR0C = 0x06; // data 8bit
	
		DDRE = 0x02; // E0 입력, E1 출력
	
	ADMUX = 0x40; // 0100 0000 외부의 AVCC 전압 사용
	ADCSRA = 0x87; // 1000 0111 ADC 활성화, 클럭 분주비 128

	SREG = 0x80; // 전역 인터럽트 허용
	
	int targetSpeed = 0; // 목표 속도
	
	lcdInit();
	lcdClear();
	
	
	while (1)
	{
		int recvData = Uart_getch(); // 입력 받기
		for (int i = 48; i < 58; i++) // 0 ~ 9인 경우
		{
			if (recvData == i)
			{
				targetSpeed = (recvData - 47) * 30; // target speed 설정
			}
		}
		
		
	}
}

unsigned char Uart_getch(void)
{
	while(!(UCSR0A & (1<< RXC0)));
	
	return UDR0;
	
}

void Uart_Putch(unsigned char PutData)
{
	while (!(UCSR0A & (1<< UDRE0)));

	UDR0 = PutData;

}

int Read_ADC() {
	ADCSRA |= (1 << ADSC); // 변환 시작 ADSC = 1
	return ADC;
}


