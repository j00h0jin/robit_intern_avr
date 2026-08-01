/*
 * Day03_Task02.c
 *
 * Created: 2026-07-31 오전 11:11:28
 * Author : hojin
 */ 

#define F_CPU 16000000
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

unsigned char Uart_getch(void);
void Uart_Putch(unsigned char PutData);

char reset[] = "RESET";

ISR(INT2_vect) // ISR(INTX_vect) {}  << 인터럽트 지정
{
	_delay_ms(15); // 채터링 방지
	if((PIND & (1<<PIND2))!=0)
	{
		EIFR = ( 1<< INTF2);
		return;
	}
	
	PORTA = 0xFF; // 다 끄기 1111 1111
	for (int i = 0; i < 5; i++)
	{
		Uart_Putch(reset[i]);
	}
}


int main(void)
{
	UBRR0L = 16; // 57600
	UBRR0H = 0;
	UCSR0A = 0x20;
	UCSR0B = 0x18;
	UCSR0C = 0x06; // data 8bit

	DDRA = 0xFF; // LED 출력으로 사용
	DDRE = 0x02; // E0 입력, E1 출력
	DDRD = 0x00; // D2 -> SW 1번
	
	PORTA = 0xFF; // 1111 1111 (전부 OFF)
	EICRA |= (1 << ISC21);
	EIMSK = (1 << INT2); // 개별 인터럽트 허용
	
	SREG = 0x80; // 전역 인터럽트 허용	
	
	char error[] = "ERROR";
	char ledOn[] = " LED on";
	char left[] = "left";
	char right[] = "right";
	
	int isCheck = 0; // 메모리 아끼기 위한 로직
	// isCheck = 1이라면 이미 위에서 확인한 경우이므로 8, 9 조건문 확인 skip
		
    while (1) 
    {
		int recvData = Uart_getch(); // 입력 받기
		for (int i = 48; i < 56; i++) // 0 ~ 7인 경우
		{
			if (recvData == i)
			{
				PORTA = ~(1<<(i - 48));
				
				Uart_Putch((char)i); // 보여줄 때는 0~7이어야 하므로 다시 char
				for (int j = 0; j < 7; j++)
				{
					Uart_Putch(ledOn[j]);
				}
				isCheck = 1;
				continue;
			}
		}
		if (isCheck == 1) // 0 ~ 7에서 이미 확인된 경우 continue
		{
			isCheck = 0;
			continue;
		}
		switch(recvData) // 
		{
			case 56:
			if(PORTA & (1 << 0)) { // 0000 0001 << 0 = 0000 0001
				PORTA = PORTA >> 1; // 비트 밀기
				PORTA |= 0x80; // 끝 비트 0 설정(1000 0000 OR 연산, 0부분은 그대로 끝에만 1로 바뀜)
			}
			else
			{
				PORTA = PORTA >> 1;
			} // 비트 밀기
			for (int i = 0; i < 4; i++)
			{
				Uart_Putch(left[i]);
			}
			break;
			
			case 57:
			if(PORTA & (1 << 7)) { // 0000 0001 << 0 = 0000 0001
				PORTA = PORTA << 1; // 비트 밀기
				PORTA |= 0x01; // 끝 비트 0 설정(0000 0001 OR 연산, 0부분은 그대로 끝에만 1로 바뀜)
			}
			else
			{
				PORTA = PORTA << 1;
			}
			for (int i = 0; i < 5; i++)
			{
				Uart_Putch(right[i]);
			}
			break;
			
			default: // 0 ~ 9 외의 값은 모두 error가 뜨게 됨
			for (int i = 0; i < 5; i++)
			{
				Uart_Putch(error[i]);
			}
			break;
		}
    }
}

unsigned char Uart_getch(void)
{
	while(!(UCSR0A & (1<< RXC0)));
	
	return UDR0;
	// 숫자 0 ~ 9 -> ASCII 48 ~ 57
	// return값을 int형으로 변환 후 조건 만들기
	
}

void Uart_Putch(unsigned char PutData)
{
	while (!(UCSR0A & (1<< UDRE0)));

	UDR0 = PutData;

}

