/*
 * Day01_Task03.c
 *
 * Created: 2026-07-29 오전 10:45:06
 * Author : hojin
 */

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

ISR(INT2_vect)
{
	_delay_ms(15); // 채터링 문제가 있어 채터링 방지 코드 삽입
	if((PIND & (1<<PIND2))!=0)
	{
		EIFR = ( 1<< INTF2);
		return;
	}
	
	PORTA = ~0x07; // 0000 0111
	
	for (int i = 0; i < 13; i++)
	{
		_delay_ms(100);
		
		if(PORTA & (1 << 7)) { // 0000 0001 << 7 = 1000 0000
			PORTA = PORTA << 1;
			PORTA |= 0x01;
		}
		else
		{
			PORTA = PORTA << 1;
		}
	}
}

ISR(INT3_vect)
{
	_delay_ms(15); // 채터링 문제가 있어 채터링 방지 코드 삽입
	if((PIND & (1<<PIND3))!=0)
	{
		EIFR = ( 1<< INTF3);
		return;
	}
	
	PORTA = ~0xE0; // 1110 0000
	
	for (int i = 0; i < 13; i++)
	{
		_delay_ms(100);
		
		if(PORTA & (1 << 0)) { // 0000 0001
			PORTA = PORTA >> 1;
			PORTA |= 0x80;
		}
		else
		{
			PORTA = PORTA >> 1;
		}
	}
}

ISR(INT4_vect)
{
	_delay_ms(15);
	if((PINE & (1<<PINE4))!=0)
	{
		EIFR = ( 1<< INTF4);
		return;
	}
	
	PORTA = 0xFF;
	for (int i = 0; i < 8; i++)
	{
		PORTA = ~(1<<i);
		_delay_ms(100);
	}
	for (int i = 7; 0 <= i; i--)
	{
		PORTA = ~(1<<i);
		_delay_ms(100);
	}

}

ISR(INT5_vect)
{
	_delay_ms(15);
	if((PINE & (1<<PINE5))!=0)
	{
		EIFR = ( 1<< INTF5);
		return;
	}
	
	PORTA = 0xFF;
}

int main(void)
{
	DDRA = 0xFF;
	DDRD = 0x00; // D2 -> SW 1번, D3 -> 2번
	DDRE = 0x00; // E4 -> 3번, E5 -> 4번 (INT2, 3, 4, 5 순)
	
	PORTD |= (1 << PIND2) | (1 << PIND3);
	
	
	EICRA |= (1 << ISC21) | (1 << ISC31); // INT0~3 -> EICRA, 4~7 -> B
	EICRB |= (1 << ISC41) | (1 << ISC51);
	
	EIMSK = (1 << INT2) | (1 << INT3) | (1 << INT4) | (1 << INT5); // 개별 인터럽트 허용
	
	sei(); // 인터럽트 활성화
	
	
	while (1)
	{
			PORTA = 0xFF;
			_delay_ms(500);
			
			for (int i = 0; i < 256; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					if((PINA & (1 << j))!=0)
					{
						PORTA |= (1 << j);
					}
					else
					{
						PORTA == PORTA << 1;
						
					}
					_delay_ms(500);
				}
				
			}
	
	}
}