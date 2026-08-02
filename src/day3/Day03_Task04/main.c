/*
 * Day03_Task04.c
 *
 * Created: 2026-07-31 오전 11:13:37
 * Author : hojin
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define BIT_DELAY 104 // 1 / 9600 = 104.xx(us) 오차범위 어느정도 허용
#define TX_PIN PD3 // PD3 핀 사용

/*
비트 뱅잉
하드웨어 없이 소프트웨어를 사용하여 구현
Start Bit: 전송 데이터 패킷의 시작 부분을 알린다. 이 신호를 통해 RX에서 데이터 수신을 시작한다
Data Bits: Start Bit와 Stop Bit 사이에 존재한다. 최대 9bit까지 데이터 통신이 가능하다.
Parity Bit: 선택적으로 사용 가능한 Bit로, 대표적으로 Odd Parity(홀수 패리티)와 Even Parity(짝수 패리티)를 사용한다. - Odd Parity: 데이터 비트의 총합이 홀수일 경우 오류가 없는 것으로 판단한다. - Even Parity: 데이터 비트의 총합이 짝수일 경우 오류가 없는 것으로 판단한다.
Stop Bit: 전송 데이터 패킷의 끝을 정의한다. 1bit 또는 2bit를 사용하며, 1bit 사용시에는 논리값1, 2bit 사용시에는 논리값 0, 1을 순서대로 사용한다.

데이터 8비트 사용 시 start bit + 8 bit + stop bit = 10bit / 패리티 비트 사용 X
*/
void transmit(unsigned char data);

int main(void)
{
	// 출력
	DDRD |= (1 << TX_PIN);
	
	// 기본 high, low 인식 시 start로 인식
	PORTD |= (1 << TX_PIN);
	_delay_ms(10);
	
	char helloWolrd[] = "HelloWorld!";

	while (1)
	{
		for (int i = 0; i < 11; i++)
		{
			transmit(helloWolrd[i]);
		}
		
		_delay_ms(1000);
	}
}

void transmit(unsigned char data) {
	// 전송을 시작할 때는 low비트를 1비트 시간만큼 출력 start
	PORTD &= ~(1 << TX_PIN); // low
	_delay_us(BIT_DELAY); // 비트마다 딜레이 필요

	// 데이터 비트 (8bit) lsb부터 보냄(최소 단위 비트)
	// 해당 비트만 1인 비트와 &연산을 하면 해당 비트 값만 제외하고 0
	// 해당 비트는 0이면 0, 1이면 1
	for (int i = 0; i < 8; i++)
	{
		if (data & (1 << i))
		{
			PORTD |= (1 << TX_PIN);  
		}
		else
		{
			PORTD &= ~(1 << TX_PIN); 
		}
		_delay_us(BIT_DELAY);
	}

	// 전송을 끝마칠 때는 high비트를 1비트 시간만큼 출력 end
	PORTD |= (1 << TX_PIN); // high
	_delay_us(BIT_DELAY);
}