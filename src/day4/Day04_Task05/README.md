# Day04_Task05

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.08.04

---

## 1. 개요 (Overview)

본 과제는 ATmega128 마이크로컨트롤러를 활용하여 주요 주변장치(Peripherals)를 제어하고 센서 데이터를 수신/처리하는 시스템을 구현하는 것을 목표로 함.

### 핵심 목표

- ATmega128 레지스터 설정을 통한 주변장치 제어
- 서보모터(SG90) PWM 제어 + 범위 밖 입력 값 예외 처리

---

## 2. 개발 환경 (Environment)

| 항목               | 내용                                            |
| :----------------- | :---------------------------------------------- |
| **MCU**            | ATmega128A (16MHz External Crystal)             |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC        |
| **Flasher Tool**   | USBISP / STK500                                 |
| **언어**           | C Language                                      |
| **주요 부품**      | ATmega128 개발보드, Servo motor                 |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                 [Target Component]
 PE0 (RXD0) / PE1    ----->   UART Serial Communication
 PORTB Pin 7 (PB7)   ----->   Servo motor

```

### 주요 회로 특징

- **전원:** 5V DC 안정화 전원 공급
- **주의사항:** ISP 다운로드 시 SPI 핀 타겟 전원 및 리셋 회로 간섭 주의

---

## 4. 프로젝트 구조 (Directory Structure)

> 구현부(.c), 선언부(.h)만 구조에 표기함.

```text
├── Day04_Task05/
│   └── main.c # 메인 제어 루프 및 시스템 초기화
└── README.md
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

###  예시 (`main.c`)

```c
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
	int isNum = 1; // 숫자인지 아닌지 판별 1이면 숫자
	
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
			if (isdigit((unsigned char)degreeChar[j])==0) // 숫자 판별
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
		
		degree = atoi(degreeChar); // 문자열을 정수형으로 변환
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
		OCR1C = MIN_OCR + (unsigned long)degree*500/180; // degree값대로 모터 회전
		
    }
}
```

---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오

서보모터(SG90) PWM 제어

서보모터는 PORTB7에 연결 (혹은 다른 timer 포트에 연결 가능)

UART 시리얼 통신을 통한 목표 각도 입력

PC 시리얼 터미널에서 목표 각도(0 ~ 180도)를 정수 문자열로 입력

예: 90 입력 시 -> 서보모터 90도 위치로 이동

시스템 초기화 시 원점(0도 또는 90도) 복귀 필수

전원 투입 또는 리셋 시, 서보모터를 지정된 기준 원점으로 안전하게 초기화 후 대기

유효 범위를 벗어난 입력값 예외처리 필수

0도 미만 또는 180도 초과 수치 입력 시 경고 메시지 출력 및 모터 동작 제한

### 동작 사진 / 영상

|                 정면 동작 모습                 |
| :-------------------------------------------: | 
| [동작 영상](https://drive.google.com/file/d/1p7xtRFjm4lPPYBnW3_io236cE5SyzTOm/view?usp=drive_link) | 

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)

본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)의 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool)        | 활용 영역              | 세부 사용 목적 및 내용  |
| :------------------- | :--------------------- | :------------------------------------ |
| **Gemini**           | 개념정리, 디버깅 | Servo모터를 제어하기 위한 개념 설명, timer, counter 1/3에 대한 레지스터 개념 설명, qns주비, 클럭, 주기 관련성 설명, 문자열을 받는 방법, 모터가 안 돌아가는 경우의 수 등  |

### AI 활용 및 검증 원칙

1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
