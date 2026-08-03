/*
 * Day04_Task04.c
 *
 * Created: 2026-07-31 오전 11:15:48
 * Author : hojin
 */ 

#define F_CPU 16000000UL

#define A 0.008271
#define B 939.6
#define C -3.398
#define D 17.339
#define arrSize 4

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

int Read_ADC();
void Uart_Putch(unsigned char PutData);

int main(void)
{
	UBRR0L = 16; // 57600
	UBRR0H = 0;
	UCSR0A = 0x20; // 송신 상태비트 초기화
	UCSR0B = 0x18;
	UCSR0C = 0x06; // data 8bit
	
	DDRE = 0x02; // E0 입력, E1 출력
	
	ADMUX = 0x41; // 0100 0001 ADC1번
	ADCSRA = 0x87; // 1000 0111 ADC 활성화, 클럭 분주비 128
	
	int psdADC = 0, i = 0;
	float voltage = 0;
	float distance = 0;
	
	char psdChar[20];
	char raw[] = "RAW: ";
	char filtered[] = "FILTERED: ";
	char dist[] = "DISTANCE: ";
	
	int moveAvgArr[arrSize];
	
	int sum, avg;
	
	for (int j = 0; j < arrSize; j++) // while문 전에 평균값을 낼 데이터들을 채움
	{
		moveAvgArr[j] = Read_ADC();
	}

	
	while (1)
	{
		i = 0, sum = 0;
		psdADC = Read_ADC();
		// 20cm를 기준으로 전압(ADC값)이 올랐다 떨어지는데
		// 유의미한 데이터를 얻기 위해서는 20cm 이후의 공식이 적용되는 부분만 걸러줘야 함
		// 따라서 20cm의 전압 고점인 2.7V에서(넉넉잡아 2.6V로 계산하였음)
		// ADC를 역산해주면 1023x2.6/5 = 531.96이 나옴
		// 531.96을 넘어가는 값은 20cm 안쪽으로 판단하고 값을 출력하지 않고 continue시킴
		// 2.6으로 계산했기 때문에 20cm가 조금 넘는 구간부터 측정됨
		if(psdADC > 532)
		continue;

		sprintf(psdChar, "%d", psdADC);
		
		while(raw[i] != '\0')
		{
			Uart_Putch(raw[i]);
			i++;
		}
		i = 0;
		while(psdChar[i] != '\0')
		{
			Uart_Putch(psdChar[i]);
			i++;
		}
		i = 0;
		Uart_Putch(' ');
		Uart_Putch('|');
		Uart_Putch(' ');
		
		// 이동 평균 필터
		// 최근의 연속된 데이터 값들의 평균을 구하는 방법
		// 새로운 값이 들어오면 가장 오래된 값을 버림
		// 최근 arrSize개 값으로 필터링하기
		
		for(int j = arrSize - 1; j > 0; j--)
		{
			moveAvgArr[j] = moveAvgArr[j-1];
			// ex a b c를 a a b로 밀려면
			// c = b;
			// b = a;
		}
		moveAvgArr[0]=psdADC; // 맨 앞에 가장 최근 값
		for (int j = 0; j < arrSize; j++) // sum
		{
			sum += moveAvgArr[j];
		}
		avg = sum/ arrSize; // sum / size = 평균
		sprintf(psdChar, "%d", avg);
		while(filtered[i] != '\0')
		{
			Uart_Putch(filtered[i]);
			i++;
		}
		i = 0;
		while(psdChar[i] != '\0')
		{
			Uart_Putch(psdChar[i]);
			i++;
		}
		i = 0;
		Uart_Putch(' ');
		Uart_Putch('|');
		Uart_Putch(' ');
		
		voltage = (float)avg * 5 / 1023;
		// https://blog.naver.com/ann_arbor/221627224574 해당 공식 참고
		distance = (A +B*voltage) / (1 + C*voltage + D*voltage*voltage);
		dtostrf(distance, 5, 1, psdChar);
		
		while(dist[i] != '\0')
		{
			Uart_Putch(dist[i]);
			i++;
		}
		i = 0;
		while(psdChar[i] != '\0')
		{
			Uart_Putch(psdChar[i]);
			i++;
		}
		i = 0;
		Uart_Putch('c');
		Uart_Putch('m');
		Uart_Putch('\r'); 
		Uart_Putch('\n');
		psdChar[0] = '\0';
		_delay_ms(500);
	}
}

int Read_ADC() {
	ADCSRA |= (1 << ADSC); // 변환 시작 ADSC = 1
	while (ADCSRA & (1 << ADSC)) {}
	
	return ADC;
}

void Uart_Putch(unsigned char PutData)
{
	while (!(UCSR0A & (1 << UDRE0)))
	;

	UDR0 = PutData;
}


