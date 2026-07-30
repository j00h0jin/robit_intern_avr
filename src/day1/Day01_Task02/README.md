
# Day01 Task02

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.07.30

---

## 1. 개요 (Overview)
본 과제는 ATmega128A 마이크로컨트롤러를 활용하여 주요 주변장치(Peripherals)를 제어하고 센서 데이터를 수신/처리하는 시스템을 구현하는 것을 목표로 함.

### 핵심 목표
* ATmega128A 레지스터 설정을 통한 8-bit LED 제어

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
├── Day01_Task02/
│   └── main.c # 메인 제어 루프 및 시스템 초기화
└── README.md
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 인터럽 예시 (`main.c`)
```c
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

ISR(INT2_vect) // ISR(INTX_vect) {}  << 인터럽트 지정
{
	_delay_ms(15); // 채터링 방지 코드
	
	if((PIND & (1<<PIND2))!=0)
	{
		EIFR = ( 1<< INTF2);
		return;	
	}
	
	for (int i = 0; i < 8; i++)
	{
		PORTA = ~(1<<i); // 포트A의 i번째'만' 1인 비트를 PORTA에 저장
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
		PORTA = ~(1<<i); // 위 로직 동일
		_delay_ms(100);
		
	}
}
```

---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
1. 0.5s 마다 모든 LED 깜빡이기
_delay_ms() 함수를 사용할것
2. SW1이 눌리면 4~7 LED 켜기
3. SW2이 눌리면 0~3 LED 켜기
4. 둘 다 눌리면 모두 켜기
5. INT3 발생시 LED 좌측 이동
6. INT4 발생시 LED 우측 이동

### 동작 사진 / 영상

| 정면 동작 모습 | 
| :---: | 
| [동작 영상](https://drive.google.com/file/d/1OTVlsBYQZYYMvsRXXIyHOOPH9vhQXzbZ/view?usp=drive_link) | 

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)의 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Gemini** | 개념 정리 & 모듈 설계 | - ATmega128 입출력 설정하는 법, 인터럽트 선언 방법 |

### AI 활용 및 검증 원칙
1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
