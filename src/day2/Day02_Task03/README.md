
# Day02 Task03

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.07.30

---

## 1. 개요 (Overview)
본 과제는 ATmega128A 마이크로컨트롤러를 활용하여 주요 주변장치(Peripherals)를 제어하고 센서 데이터를 수신/처리하는 시스템을 구현하는 것을 목표로 함.

### 핵심 목표
* ATmega128A 레지스터 설정을 통한 Switch 기능 할당, I2C를 통한 LCD 제어

---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드, LCD, Switch |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128A]                     [Target Component]
 PORTD Pin 0, 1 (PIND0, PIND1)   ----->   LCD(I2C)
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
├── Day02_Task02/
│   ├── main.c # 메인 제어 루프 및 시스템 초기화
│   └── LCD_Text.c # LCD제어 라이브러리 및 함수
├── include/
│   └── LCD_Text.h # LCD 제어 함수 선언 헤더 파일
└── README.md
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 스위치를 통한 계산기 구현 예시 (`main.c`)
```c
#include "LCD_Text.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void)
{
	// D2 -> SW 1번, D3 -> 2번, E4 -> 3번, E5 -> 4번
	DDRD = 0x00; // 입력 설정 PD, PE
	DDRE = 0x00; 

	PORTD |= (1 << PIND2) | (1 << PIND3); // 풀업 설정
	PORTE |= (1 << PINE4) | (1 << PINE5);

	int a = 1, b = 1; // A, B
	int  c = 0, symbol = 0; // 기호 switch에 사용
	float answer = 0; // 답

	lcdInit();
	lcdClear(); // lcd 초기 설정 후 clear

    while (1) 
    {
		lcdString(0,0,"Jhj");
		if(!(PIND & (1<<PIND2))) // 1번 A 증가
		{
			a += 1;
			_delay_ms(300); // 채터링 방지
		}
		if(!(PIND & (1<<PIND3))) // 2번 기호 변경
		{
			c += 1;
			symbol = c % 4; 
			_delay_ms(300);
		}
		if(!(PINE & (1<<PINE4))) // 3번 B 증가
		{
			b += 1;
			_delay_ms(300);
		}
		if(!(PINE & (1<<PINE5))) // 4번을 누르면 답이 보임
		{
			switch(symbol) // 스위치 문에 따라 현재 기호에 맞는 answer 지정
			{
				case 0:
				answer = a + b;
				break;
				case 1:
				answer = a - b;
				break;
				case 2:
				answer = a * b;
				break;
				case 3:
				answer = (float)a / b;
				break;
				default:
				break;
			}
			lcdString(1,7,"=");
			lcdFloat(1,8,answer,1);
			_delay_ms(50);
		}
		else // 누르지 않았을 때 빈 화면으로 덮기
		{
			lcdString(1,7,"      ");
		}
			
		lcdNumber(1,0,a);
		switch(symbol) // symbol(c%/)값에 따라 기호 출력
		{
			case 0:
				lcdString(1,3,"+");
				answer = a + b;
				break;
			case 1:
				lcdString(1,3,"-");
				answer = a - b;
				break;
			case 2:
				lcdString(1,3,"*");
				answer = a * b;
				break;
			case 3:
				lcdString(1,3,"/");
				answer = a / b;
				break;
			default:
				lcdString(1,3," ");
				break;			
		}
		lcdNumber(1,4,b);
    }
}
```




---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
덧셈, 뺄셈, 곱셈, 나눗셈이 가능한 계산기 만들어 값을 LCD에 띄운다.
(ex. A + B = C의 형태)
<조건>
1. 첫번째 스위치를 누를 때마다 A의 값이 1씩 오른다. (초기값 A = 1)
2. 두번째 스위치를 누를 때마다 산술 연산자가 바뀐다. (순서는 ‘+’ ‘-‘ ‘*’ ‘/’
순)
3. 세번째 스위치를 누를 때마다 B의 값이 1씩 오른다. (초기값 B = 1)
4. 네번째 스위치를 누르면 연산을 하여 값을 LCD에 띄운다.
※ Note! ‘A + B = C’ 의 값은 LCD의 1번째 줄에 다 띄울 수 있도록 한다.

### 동작 사진 / 영상

| 정면 동작 모습 | 
| :---: | 
| [동작 영상](https://drive.google.com/file/d/1R8rZVie_DkWiZ-e0INbt_Q19qn_yNwvi/view?usp=drive_link) | 

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)의 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| - | - | -  |

### AI 활용 및 검증 원칙
1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
