
# Day01 Task03

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.07.30

---

## 1. 개요 (Overview)
본 과제는 ATmega128A 마이크로컨트롤러를 활용하여 주요 주변장치(Peripherals)를 제어하고 센서 데이터를 수신/처리하는 시스템을 구현하는 것을 목표로 함.

### 핵심 목표
* ATmega128A 레지스터 설정을 통한 8-bit LED 제어, 2진 카운터 구현

---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드, LED, Switch |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128A]                     [Target Component]
 PORTA (PA0 ~ PA7)               ----->   8-Bit LED
 PORTD Pin 2, 3 (PIND2, PIND3)   ----->   Switch 1, 2
 PORTE Pin 4, 5 (PINE4, PINE5)   ----->   Switch 3, 4
```

### 주요 회로 특징
* **전원:** 5V DC 안정화 전원 공급
* **주의사항:** ISP 다운로드 시 SPI 핀 타겟 전원 및 리셋 회로 간섭 주의

---

## 4. 프로젝트 구조 (Directory Structure)
> 구현부(.c), 선언부(.h)만 구조에 표기함.
```text
├── Day01_Task03/
│   └── main.c # 메인 제어 루프 및 시스템 초기화
└── README.md
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 인터럽트 및 이진 카운터 예시 (`main.c`)
```c
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int i = 0;

ISR(INT2_vect)
{
	_delay_ms(15); // 채터링 문제가 있어 채터링 방지 코드 삽입
	if((PIND & (1<<PIND2))!=0)
	{
		EIFR = ( 1<< INTF2);
		return;
	}
	
	PORTA = ~0x07; // 0000 0111
	
	for (int i = 0; i < 13; i++) // 2바퀴 (8비트 x 2 = 16, 16 - 3(LED칸수))
	{
		_delay_ms(100);
		
		if(PORTA & (1 << 7)) { // 0000 0001 << 7 = 1000 0000
			PORTA = PORTA << 1; // 비트 밀기
			PORTA |= 0x01; // 끝 비트 0 설정(0000 0001 OR 연산, 0부분은 그대로 끝에만 1로 바뀜)
		}
		else
		{
			PORTA = PORTA << 1;
		}
	}
}
```
```c
int i = 0; // 전역 변수

int main(void) {
.
.
.

while (1)
	{
		PORTA = ~i; // 풀업 상태이기 때문에 0 -> LED 켜짐
		_delay_ms(200);
		if(i == 255) // 1111 1111이면 i=0으로 초기화
		{
			i = 0;
			continue;
		}
		i++;
	}
}
```

---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
1. 0.1s 마다 LED 2진 카운터
2. INT0 발생시 LED 3개씩 우측 이동 X 2
3. INT1 발생시 LED 3개씩 좌측 이동 X 2
4. INT2 발생시 LED1개 좌측 이동 후 우측 이동
5. INT3 발생시 2진 카운터 초기화

### 동작 사진 / 영상

| 정면 동작 모습 | 
| :---: | 
| [동작 영상](https://drive.google.com/file/d/1OTVlsBYQZYYMvsRXXIyHOOPH9vhQXzbZ/view?usp=drive_link) | 

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)의 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Gemini** | 개념 정리 & 모듈 설계 | - ATmega128 특정 비트 수를 읽어오는 방법의 논리, 이진 카운터를 다른 방법으로 구현할 수 있는 방법이 있는지 |

### AI 활용 및 검증 원칙
1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
