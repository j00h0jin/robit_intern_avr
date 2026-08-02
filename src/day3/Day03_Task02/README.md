# ATmega128 과제 및 프로젝트 템플릿

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.08.02

---

## 1. 개요 (Overview)

본 과제는 ATmega128 마이크로컨트롤러를 활용하여 주요 주변장치(Peripherals)를 제어하고 센서 데이터를 수신/처리하는 시스템을 구현하는 것을 목표로 함.

### 핵심 목표

- ATmega128 레지스터 설정을 통한 주변장치 제어
- 센서 및 외부 모듈과의 통신 (USART) 및 데이터 처리

---

## 2. 개발 환경 (Environment)

| 항목               | 내용                                            |
| :----------------- | :---------------------------------------------- |
| **MCU**            | ATmega128A (16MHz External Crystal)             |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC        |
| **Flasher Tool**   | USBISP / STK500                                 |
| **언어**           | C Language                                      |
| **주요 부품**      | ATmega128 개발보드, LED, Switch   |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                 [Target Component]
 PORTA (PA0 ~ PA7)   ----->   8-Bit LED
 PE0 (RXD0) / PE1    ----->   UART Serial Communication
 PORTD Pin 2, 3 (PIND2, PIND3)   ----->   Switch 1, 2
 PORTE Pin 4, 5 (PINE4, PINE5)   ----->   Switch 3, 4
```

### 주요 회로 특징

- **전원:** 5V DC 안정화 전원 공급
- **주의사항:** ISP 다운로드 시 SPI 핀 타겟 전원 및 리셋 회로 간섭 주의

---

## 4. 프로젝트 구조 (Directory Structure)

> 구현부(.c), 선언부(.h)만 구조에 표기함.

```text
├── Day03_Task02/
│   └──  main.c # 메인 제어 루프 및 시스템 초기화
└── README.md
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### LED 제어 예시 (`main.c`)

```c
#define F_CPU 16000000
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
.
.
.
// 초기화는 인터럽트로 구현하였다
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
.
.
.
int main(void)
{
.
.
.
	// 숫자 0 ~ 9 -> ASCII 48 ~ 57
	// return값을 int형으로 변환 후 조건 만들기
int isCheck = 0; // 메모리 아끼기 위한 로직
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
   // 이전 과제에서 구현한 로직을 가져옴(비트 밀기)
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
			
			default: // 0 ~ 9 외의 값은 모두 error가 뜨게 됨 (예외 처리)
			for (int i = 0; i < 5; i++)
			{
				Uart_Putch(error[i]);
			}
			break;
		}
    }
}
```

---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오

SW1 누르면 모든 상태 초기화 후, “RESET” 문자열 PC로 전송

PC에서 0~7 숫자 입력 시 해당 번호의 LED 켜기 (예외 처리 필요, PC로 “n LED on” 문자열 전송

PC에서 8 입력 시, LED 왼쪽으로 이동, PC에 “LEFT” 문자열 전송

PC에서 9 입력 시, LED 오른쪽으로 이동, PC에 “RIGHT” 문자열 전송


### 동작 사진 / 영상

|                 정면 동작 모습                 |            사진            |
| :--------------------------------------------: | :--------------------------------------------: |
| [동작 영상](https://drive.google.com/file/d/1H3uD0gSduU_Jz6HAiHeQQM6kcHJRlRZG/view?usp=drive_link) | [예외 처리 시](https://drive.google.com/file/d/1lt75J9R-BDelXVo5m4SAOnNPo6hllUj2/view?usp=drive_link) |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)

본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)의 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool)        | 활용 영역              | 세부 사용 목적 및 내용        |
| :--------------- | :---------------- | --- |
| X | - | -|

### AI 활용 및 검증 원칙

1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
