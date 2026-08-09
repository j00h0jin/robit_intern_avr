/*
 * Day06_Task01.c
 *
 * Created: 2026-08-08 오전 3:06:37
 * Author : hojin
 */ 

#define F_CPU 16000000UL

#include "include/LCD_Text.h"
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#define arrSize 4
#define indexIR 6

unsigned int Read_ADC(int i);
void Uart_Putch(unsigned char PutData);


volatile unsigned int ms_count = 0;
volatile unsigned char print_flag = 0;
ISR(TIMER0_OVF_vect) // timer0 interrupt
{
	// 클럭 / 분주비 = 250KHz (16MHz / 64)
	// 1주기 = 4us ( 1 / 250K )
	// 4us * 250 = 1ms
	TCNT0 = 256 - 250; // 250번 count ((256 - 250) ~ 256)
	ms_count++;
	
	if (ms_count >= 1000) // 주기
	{
		ms_count = 0;
		print_flag = 1;
	}
}

int main(void)
{
	UBRR0L = 16; // 57600
	UBRR0H = 0;
	UCSR0A = 0x20; // 송신 상태비트 초기화
	UCSR0B = 0x18;
	UCSR0C = 0x06; // data 8bit
	
	DDRA = 0xFF;
	DDRE = 0x02; // E0 입력, E1 출력
	
	ADMUX = 0x40; // 0100 0000
	ADCSRA = 0x87; // 1000 0111 ADC 활성화, 분주비 128
	
	TCCR0 = (1 << CS02); // 분주비 64
	TCNT0 = 256 - 250;
	TIMSK |= (1 << TOIE0); // timer0 interrupt 활성화
	
	sei(); // 전역 인터럽트 활성화
	
	int moveAvgArr[indexIR][arrSize]; // 이동 평균 필터에 사용할 값 저장
	int moveAvgFilterValue[indexIR]; // 필터링 값
	
	int minMax[indexIR][2]; // 정규화에 필요한 포트별 min, max 값 저장
	
	float normalization[indexIR] = {0};

	for (int i = 0; i < indexIR; i++) // min, max 초기값 설정
	{
		minMax[i][0] = 1024;
		minMax[i][1] = -1;
	}
	
	char tempStr[20];
	char original[] = "Original ";
	char filter[] = "  filter  ";
	char minStr[] = "  min  ";
	char maxStr[] = "    max    ";
	char norm[] = "    norm";
	
	int sum = 0;
	
	lcdInit();
	lcdClear();
	
	for (int i = 0; i < indexIR; i++) // while문 전 평균값을 낼 데이터들을 채움
	{
		for (int j = 0; j < arrSize; j++)
		{
			moveAvgArr[i][j] = Read_ADC(i);
			// min, max 판별
			if(moveAvgArr[i][j] < minMax[i][0])
				minMax[i][0] = moveAvgArr[i][j];
			if(moveAvgArr[i][j] > minMax[i][1])
				minMax[i][1] = moveAvgArr[i][j];
			
		}
	}
	
	_delay_ms(10);
		
    while (1) 
    {
		for (int i = 0; i < indexIR; i++)
		{
			sum = 0;
			for(int j = arrSize - 1; j > 0; j--)
			{
				// 한 칸씩 밀기 a, b, c => a, a, b
				moveAvgArr[i][j] = moveAvgArr[i][j-1];
			}
			// New_value, a, b
			moveAvgArr[i][0] = Read_ADC(i);
			
			// min, max 판별
			if(moveAvgArr[i][0] < minMax[i][0])
			minMax[i][0] = moveAvgArr[i][0];
			if(moveAvgArr[i][0] > minMax[i][1])
			minMax[i][1] = moveAvgArr[i][0];
			
			// sum 구한 후 avg에 넣기
			for (int j = 0; j < arrSize; j++)
			{
				sum += moveAvgArr[i][j];
			}
			moveAvgFilterValue[i] = sum / arrSize;
		}
		
		for (int i = 0; i < indexIR; i++)
		{
			float temp;
			temp = minMax[i][1] - minMax[i][0]; // max - min 저장
			if(temp == 0) // 초기에 max - min이 0인 경우 0으로 나눌 수 없으므로 정규화 값 0으로 설정
			{
				normalization[i] = 0;
				continue;
			}
			// 정규화 공식은 filtering value - min값을 max-min값으로 나누어줌
			// ex) min: 100, max: 600이고 value가 300이면 0.4가 나와야하므로
			// max-min = 500, value - min = 200, 200/500 = 0.4
			// ex) value가 600인 경우 => value - min = 500, 500/500 = 1.0
			// ex) value가 100인 경우 => value - min = 0, 0/100 = 0.0
			normalization[i] = (float)(moveAvgFilterValue[i]-minMax[i][0]) / temp;
		}
		
		PORTA = 0xFF;
		
		for (int i = 0; i < indexIR; i++)
		{
			if(normalization[i] >= 0.80)
			{
				PORTA &= ~(1 << i); // 해당하는 LED 켜기
			}
		}
		
		if(print_flag) // LCD 출력과 UART의 경우 1초마다 갱신되게 하였음
		{
			print_flag = 0;
			
			// LCD 출력
			lcdClear();
			_delay_ms(5);
			
			for (int i = 0; i < 6; i++)
			{
				if(i < 3)
				lcdFloat(0, i*5, normalization[i], 2);
				// lcdNumber(0, i*5, normalization[i]); // 0 5 10
				else
				lcdFloat(1, (i-3)*5, normalization[i], 2);
				// lcdNumber(1, (i-3)*5, normalization[i]); // 0 5 10
			}
			
			// UART 출력
			Uart_Putch('\t');
			for (int i = 0; original[i] != 0; i++)
			{
				Uart_Putch(original[i]);
			}
			Uart_Putch('|');
			for (int i = 0; filter[i] != 0; i++)
			{
				Uart_Putch(filter[i]);
			}
			Uart_Putch('|');
			for (int i = 0; minStr[i] != 0; i++)
			{
				Uart_Putch(minStr[i]);
			}
			Uart_Putch('|');
			for (int i = 0; maxStr[i] != 0; i++)
			{
				Uart_Putch(maxStr[i]);
			}
			Uart_Putch('|');
			for (int i = 0; norm[i] != 0; i++)
			{
				Uart_Putch(norm[i]);
			}
			Uart_Putch('\r');
			Uart_Putch('\n');
			
			for (int i = 0; i < indexIR; i++)
			{
				sprintf(tempStr, "IR %d :\t", i);
				for (int j = 0; tempStr[j] != 0; j++)
				{
					Uart_Putch(tempStr[j]);
				}
				sprintf(tempStr, "%d\t", moveAvgArr[i][0]);
				for (int j = 0; tempStr[j] != 0; j++)
				{
					Uart_Putch(tempStr[j]);
				}
				sprintf(tempStr, "%d\t", moveAvgFilterValue[i]);
				for (int j = 0; tempStr[j] != 0; j++)
				{
					Uart_Putch(tempStr[j]);
				}
				sprintf(tempStr, "%d\t", minMax[i][0]);
				for (int j = 0; tempStr[j] != 0; j++)
				{
					Uart_Putch(tempStr[j]);
				}
				sprintf(tempStr, "%d\t", minMax[i][1]);
				for (int j = 0; tempStr[j] != 0; j++)
				{
					Uart_Putch(tempStr[j]);
				}
				dtostrf(normalization[i], 3, 2, tempStr);
				for (int j = 0; tempStr[j] != 0; j++)
				{
					Uart_Putch(tempStr[j]);
				}
				Uart_Putch('\r');
				Uart_Putch('\n');
			}
			Uart_Putch('\r');
			Uart_Putch('\n');
		}
	}
}


unsigned int Read_ADC(int i) // (i = PF 2-7)
{
	int index = i + 2;
	ADMUX = 0x40 | index; // 해당 index의 ADC 활성화
	_delay_us(3); // ADMUX 적용되는 delay 약 1.3us
	
	ADCSRA |= (1 << ADSC); // 변환 시작 ADSC = 1
	while (ADCSRA & (1 << ADSC))
	;
	
	return ADC;
}

void Uart_Putch(unsigned char PutData)
{
	while (!(UCSR0A & (1 << UDRE0)))
	;

	UDR0 = PutData;
}
