/*
 * Day04_Task03.c
 *
 * Created: 2026-07-31 오전 11:15:29
 * Author : hojin
 */ 

#define F_CPU 16000000UL


#define A 0.008271
#define B 939.6
#define C -3.398
#define D 17.339


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

int Read_ADC();
unsigned char Uart_getch(void);
void Uart_Putch(unsigned char PutData);

int main(void)
{
	UBRR0L = 16; // 57600
	UBRR0H = 0;
	UCSR0A = 0x20; // 송신 상태비트 초기화
	UCSR0B = 0x18;
	UCSR0C = 0x06; // data 8bit
	
	DDRE = 0x02; // E0 입력, E1 출력
		
	int psdADC = 0, i = 0;
	float voltage = 0;
	float distance = 0;
	
	char psdChar[20];
	
	ADMUX = 0x41; // 0100 0001 ADC1번
	ADCSRA = 0x87; // 1000 0111 ADC 활성화, 클럭 분주비 128
	
    while (1) 
    {
		i = 0;
		psdADC = Read_ADC();
		// 20cm를 기준으로 전압(ADC값)이 올랐다 떨어지는데
		// 유의미한 데이터를 얻기 위해서는 20cm 이후의 공식이 적용되는 부분만 걸러줘야 함
		// 따라서 20cm의 전압 고점인 2.7V에서(넉넉잡아 2.6V로 계산하였음)
		// ADC를 역산해주면 1023x2.6/5 = 531.96이 나옴
		// 531.96을 넘어가는 값은 20cm 안쪽으로 판단하고 값을 출력하지 않고 continue시킴
		// 2.6으로 계산했기 때문에 20cm가 조금 넘는 구간부터 측정됨
		if(psdADC > 532) 
			continue;
		voltage = (float)psdADC * 5 / 1023;
		// https://blog.naver.com/ann_arbor/221627224574 해당 공식 참고
		distance = (A +B*voltage) / (1 + C*voltage + D*voltage*voltage);
		dtostrf(distance, 5, 1, psdChar);
		// sprintf(psdChar, "%d", psdADC);
		
		while(psdChar[i] != '\0')
		{
			Uart_Putch(psdChar[i]);
			i++;
		}
		Uart_Putch('c');
		Uart_Putch('m');
		Uart_Putch(' ');
		psdChar[0] = '\0';
		_delay_ms(500);
    }
}

int Read_ADC() {
	ADCSRA |= (1 << ADSC); // 변환 시작 ADSC = 1
	while (ADCSRA & (1 << ADSC)) {}
	
	return ADC;
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


