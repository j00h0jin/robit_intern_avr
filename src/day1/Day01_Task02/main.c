/*
 * Day01_Task02.c
 *
 * Created: 2026-07-29 오전 10:46:37
 * Author : hojin
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

ISR(INT2_vect) // ISR(INTX_vect) {}  << 인터럽트 지정
{
	_delay_ms(15);
	
	if((PIND & (1<<PIND2))!=0)
	{
		EIFR = ( 1<< INTF2);
		return;	
	}
	
	for (int i = 0; i < 8; i++)
	{
		PORTA = ~(1<<i); // 포트A의 i번째'만' 1
		_delay_ms(100);
	}
}

ISR(INT3_vect)
{
	_delay_ms(15);
		
	if((PIND & (1<<PIND3))!=0)
	{
		EIFR = ( 1<< INTF3);
		return;
	}
		
	for (int i = 7; 0 <= i; i--)
	{
		PORTA = ~(1<<i);
		_delay_ms(100);
		
	}
}

int main(void)
{
    /* Replace with your application code */
	DDRA = 0xFF;
	DDRD = 0x00; // D2 -> SW 1번, D3 -> 2번
	DDRE = 0x00; // E4 -> 3번, E5 -> 4번 (INT2, 3, 4, 5 순)
	
	PORTD |= (1 << PIND2) | (1 << PIND3);
	PORTE |= (1 << PINE4) | (1 << PINE5);
	
	EICRA |= (1 << ISC21) | (1 << ISC31); // INT0~3 -> EICRA, 4~7 -> B
	
	EIMSK = (1 << INT2) | (1 << INT3); // 개별 인터럽트 허용
	
	sei(); // 인터럽트 활성화
	
    while (1) 
    {
			if(((PINE & (1 << PINE4)) == 0) && ((PINE & (1 << PINE5)) == 0))
			{
				PORTA = ~0xFF;
			}
			else if((PINE & (1 << PINE4)) == 0) 
			{
				PORTA = ~0xF0; // 1111 0000
			}
			else if((PINE & (1 << PINE5)) == 0)
			{
				PORTA = ~0x0F; // 1111 0000
			}
			else
			{
				PORTA = 0x00;
				_delay_ms(500);

				PORTA = 0xFF;
				_delay_ms(500);
			}
	}
}

