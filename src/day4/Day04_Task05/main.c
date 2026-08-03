/*
 * Day04_Task05.c
 *
 * Created: 2026-07-31 오전 11:16:11
 * Author : hojin
 */ 

#define F_CPU 16000000UL
#define MIN_OCR 125  // 0.5/0.004 = 125 (0도)
#define MAX_OCR 625  // 2.5/0.004 = 125*5 = 625 (180도)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <ctype.h>

unsigned char Uart_getch(void);
void Uart_Putch(unsigned char PutData);

// sg90
// 목표 주파수 50Hz
// 목표 주기 20ms(1/50s)
// high 0.5ms -> 0도
// 1.5ms -> 90도
// 2.5ms -> 180도
int main(void)
{
	UBRR0L = 16; // 57600
	UBRR0H = 0;
	UCSR0A = 0x20; // 송신 상태비트 초기화
	UCSR0B = 0x18; // RX TX 활성화
	UCSR0C = 0x06; // data 8bit
	
	DDRB |= (1 << PB7); // PE7 출력
	
	// timer 1 -> PB
	TCCR1A = (1 << COM1C1) | (1 << WGM11); // Non-Inverting mode(듀티비에 따라 high가 늘어남), Fast PWM(TOP: ICR)
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10); // 011 분주비 = 64

	// 16MHz / 분주비 64 = 250000Hz, 1주기 0.004ms
	ICR1 = 5000 - 1; // Top = 0~4999(5000개 카운트) 0.004 x 5000 = 20ms
	
	char degreeChar[10];
	char warning[] = "warning!";
	
	int degree = 0;
	int i = 0;
	int isNum = 1;
	
	OCR1C =MIN_OCR; // reset시 0도 설정
	_delay_ms(100);

	
    while (1) 
    {
		i = 0;
		while (1)
	    {
			if(i == 9) // 문자열 배열 오버플로우 방지
			{
				degreeChar[i] = '\0';
				break;
			}
		    char c = Uart_getch();
		    
		    if (c =='\r' || c =='\n')
		    {
			    degreeChar[i] = '\0'; // 엔터 시 문자열 끝 지정
			    break;
		    }
		    
		    if (i < sizeof(degreeChar) - 1)
		    {
			    degreeChar[i++] = c;
		    }
	    }
		if (i == 0) // 입력된 값이 없을 때
			continue;
		
		isNum = 1;
		i = 0;
		for (int j = 0; degreeChar[j] != '\0'; j++)
		{
			if (isdigit((unsigned char)degreeChar[j])==0)
			{
				isNum = 0; // 숫자가 아닌 문자가 섞임
				break;
			}
		}
		
		if(isNum == 0) // 숫자가 아닌 경우 warning 출력 후 continue
		{
			Uart_Putch(' ');
			while(warning[i] != '\0')
			{
				Uart_Putch(warning[i]);
				i++;
			}
			Uart_Putch(' ');
			i = 0;
			continue;
		}
		
		degree = atoi(degreeChar);
		i = 0;
		if(degree > 180 || degree < 0) // 0~180 외의 값일 때 warning 출력
		{
			Uart_Putch(' ');
			while(warning[i] != '\0')
			{
				Uart_Putch(warning[i]);
				i++;
			}
			Uart_Putch(' ');
			i = 0;
			continue; // 모터 제어하지 않고 continue
		}
		// 125 ~ 625
		OCR1C = MIN_OCR + (unsigned long)degree*500/180;
		
    }
}

unsigned char Uart_getch(void)
{
	while (!(UCSR0A & (1 << RXC0)))
	;

	return UDR0;
}


void Uart_Putch(unsigned char PutData)
{
	while (!(UCSR0A & (1 << UDRE0)))
	;

	UDR0 = PutData;
}