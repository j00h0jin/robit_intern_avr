# Day04_Task02

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.08.03

---

## 1. 개요 (Overview)

본 과제는 ATmega128 마이크로컨트롤러를 활용하여 주요 주변장치(Peripherals)를 제어하고 센서 데이터를 수신/처리하는 시스템을 구현하는 것을 목표로 함.

### 핵심 목표

- ATmega128 레지스터 설정을 통한 주변장치 제어
- timer, counter, switch를 활용한 날짜 구현

---

## 2. 개발 환경 (Environment)

| 항목               | 내용                                            |
| :----------------- | :---------------------------------------------- |
| **MCU**            | ATmega128A (16MHz External Crystal)             |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC        |
| **Flasher Tool**   | USBISP / STK500                                 |
| **언어**           | C Language                                      |
| **주요 부품**      | ATmega128 개발보드, LCD, Switch, Potentiometer |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                             [Target Component]
 PORTD Pin 0, 1 (PIND0, PIND1)   ----->   LCD(I2C)
 PORTD Pin 2, 3 (PIND2, PIND3)   ----->   Switch 1, 2
 PORTF PF0                       ----->   Potentiometer
```

### 주요 회로 특징

- **전원:** 5V DC 안정화 전원 공급
- **주의사항:** ISP 다운로드 시 SPI 핀 타겟 전원 및 리셋 회로 간섭 주의

---

## 4. 프로젝트 구조 (Directory Structure)

> 구현부(.c), 선언부(.h)만 구조에 표기함.

```text
├── Day04_Task02/
│   ├──  main.c # 메인 제어 루프 및 시스템 초기화
│   └──  LCD_Text.c # LCD 제어 함수 파일
├── include/
│   └── LCD_Text.h # LCD 제어 헤더 파일
└── README.md
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 시간 작동 예시 (`main.c`)

```c
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "include/LCD_Text.h"
.
.
.
int isFlow = 0;
.
.
.
// 각 시간의 단위를 넘어가면 다음 단위 +1 자신은 0으로 초기화
ISR(TIMER0_OVF_vect)
{
	TCNT0 = 131;

	if(isFlow == 0)
	return;

	count8ms++;
	mSec += 8;

	if(count8ms >= 125|| mSec >=1000)
	{
		count8ms = 0;
		mSec = 0;
		sec++;

		if(sec >= 60)
		{
			sec = 0;
			min++;

			if(min >= 60)
			{
				min = 0;
				hour++;

				if(hour >= 24)
				{
					hour = 0;
					day++;

					if(day > getDay(year, month))
					{
						day = 1;
						month++;

						if(month > 12)
						{
							month = 1;
							year++;
						}
					}
				}
			}
		}
	}
}


int main(void)
{
.
.
.
while (1) 
    {
		potentiometerValue = Read_ADC();
		switch(current) {
.
.
.
   // 출력
			default:
			lcdNumber(0,0,year);
			lcdNumber(0,4,month);
			lcdNumber(0,6,day);
			lcdNumber(1,0,hour);
			lcdString(1, 2, ":");
			lcdNumber(1,3,min);
			lcdString(1, 5, ":");
			lcdNumber(1,6,sec);
			lcdString(1, 8, ".");
			lcdNumber(1,9,mSec/10);
			break;
   }
}

```

### day 추출 예시 (`main.c`)

```c
// 윤년, 월에 따른 day를 return
// 윤년인 경우 2월 판별, 그 외에는 30일과 31일을 나눔
// 31 28(29) 31 30 31 30 31 31 30 31 30 31
unsigned int getDay(unsigned int year, unsigned int month) {
	if(month == 2) { // 2월의 경우
		// 윤년 판별식
	if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
		return 29;
	else
		return 28;
	}
	if (month == 4 ||month == 6 || month == 9 || month == 11)
		return 30;
	
	return 31;
}
```

---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오

 달력, 시계 만들기
 가변저항 값과 SW1을 이용해 날짜, 시간 세팅
   (가변저항 값에 따라 연도 세팅 -> 스위치로 확정, 월 세팅 -> 스위치로 확정…)

 LCD에 연-월-일, 시-분-초-밀리초 형태로 출력
     (ex : 190722 10:50:48.34)

 SW2 누르면 시간 흐르기 시작

날짜나 시간 등 예외처리 필수

### 동작 사진 / 영상

|                 정면 동작 모습                 |
| :--------------------------------------------: |
| [동작 영상](https://drive.google.com/file/d/19gLojNIkGKV4M6Xzzet9RSAV6HJX8s64/view?usp=drive_link) |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)

본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)의 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool)        | 활용 영역              | 세부 사용 목적 및 내용 |
| :------------------- | :--------------------- | :-------------------- |
| **Gemini**           | 개념 정리, 디버깅 | ppt timer 예제 동작 원리 설명, 시간이 흐른 뒤 출력이 안되는 이유 등 |

### AI 활용 및 검증 원칙

1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
