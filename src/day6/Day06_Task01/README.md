# Day06_Task01

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.08.09

---

## 1. 개요 (Overview)
본 과제는 ATmega128 마이크로컨트롤러를 활용하여 주요 주변장치(Peripherals)를 제어하고 센서 데이터를 수신/처리하는 시스템을 구현하는 것을 목표로 함.

### 핵심 목표
* ATmega128 레지스터 설정을 통한 주변장치 제어
* USART를 통한 IR 센서값 정규화
* LED, LCD를 사용하여 IR 센서 상태 출력

---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드, IR센서, LCD, LED |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                 [Target Component]
 PORTA (PA0 ~ PA7)               ----->   8-Bit LED
 PORTD (PD0 - PD1)               ----->   LCD(I2C)
 PORTE (PE0 - PE1)               ----->   UART Serial Communication
 PORTF (PF2 - PF7)               ----->   IR Sensor
```

### 주요 회로 특징
* **전원:** 5V DC 안정화 전원 공급
* **주의사항:** ISP 다운로드 시 SPI 핀 타겟 전원 및 리셋 회로 간섭 주의

---

## 4. 프로젝트 구조 (Directory Structure)
> 구현부(.c), 선언부(.h)만 구조에 표기함.
```text
├── Day06_Task01/
│   ├── main.c     # 메인 제어 루프 및 시스템 초기화
│   └── LCD_Text.c # LCD 제어 함수 파일
├── include/
│   └── LCD_Text.h # LCD 제어 헤더 파일
└── README.md
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### IR ADC값 읽어오기 및 정규화 과정 예시 (`main.c`)
```c
#define F_CPU 16000000UL

#include "include/LCD_Text.h"
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#define arrSize 4
#define indexIR 6

unsigned int Read_ADC(int i);

volatile unsigned int ms_count = 0;
volatile unsigned char print_flag = 0;
ISR(TIMER0_OVF_vect) // timer0 interrupt
{
	// 클럭 / 분주비 = 250KHz (16MHz / 64)
	// 1주기 = 4us ( 1 / 250K )
	// 4us * 250 = 1ms
	TCNT0 = 256 - 250; // 250번 count ((256 - 250) ~ 256)
	ms_count++;
	
	if (ms_count >= 1000) // 주기
	{
		ms_count = 0;
		print_flag = 1;
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
int moveAvgArr[indexIR][arrSize]; // 이동 평균 필터에 사용할 값 저장
int moveAvgFilterValue[indexIR]; // 필터링 값
	
int minMax[indexIR][2]; // 정규화에 필요한 포트별 min, max 값 저장
	
float normalization[indexIR] = {0};
.
.
.
while (1) 
{
for (int i = 0; i < indexIR; i++)
		{
			sum = 0;
			for(int j = arrSize - 1; j > 0; j--)
			{
				// 한 칸씩 밀기 a, b, c => a, a, b
				moveAvgArr[i][j] = moveAvgArr[i][j-1];
			}
			// New_value, a, b
			moveAvgArr[i][0] = Read_ADC(i);
			
			// min, max 판별
			if(moveAvgArr[i][0] < minMax[i][0])
			minMax[i][0] = moveAvgArr[i][0];
			if(moveAvgArr[i][0] > minMax[i][1])
			minMax[i][1] = moveAvgArr[i][0];
			
			// sum 구한 후 avg에 넣기
			for (int j = 0; j < arrSize; j++)
			{
				sum += moveAvgArr[i][j];
			}
			moveAvgFilterValue[i] = sum / arrSize;
		}
		
		for (int i = 0; i < indexIR; i++)
		{
			float temp;
			temp = minMax[i][1] - minMax[i][0]; // max - min 저장
			if(temp == 0) // 초기에 max - min이 0인 경우 0으로 나눌 수 없으므로 정규화 값 0으로 설정
			{
				normalization[i] = 0;
				continue;
			}
			// 정규화 공식은 filtering value - min값을 max-min값으로 나누어줌
			// ex) min: 100, max: 600이고 value가 300이면 0.4가 나와야하므로
			// max-min = 500, value - min = 200, 200/500 = 0.4
			// ex) value가 600인 경우 => value - min = 500, 500/500 = 1.0
			// ex) value가 100인 경우 => value - min = 0, 0/100 = 0.0
			normalization[i] = (float)(moveAvgFilterValue[i]-minMax[i][0]) / temp;
		}
		
		PORTA = 0xFF;
		
		for (int i = 0; i < indexIR; i++)
		{
			if(normalization[i] >= 0.80)
			{
				PORTA &= ~(1 << i); // 해당하는 LED 켜기
			}
		}

  if(print_flag) // LCD 출력과 UART의 경우 1초마다 갱신되게 하였음
  {
  .
  . // LCD 및 UART 출력
  .
  }
}
}

unsigned int Read_ADC(int i) // (i = PF 2-7)
{
	int index = i + 2;
	ADMUX = 0x40 | index; // 해당 index의 ADC 활성화
	_delay_us(3); // ADMUX 적용되는 delay 약 1.3us
	
	ADCSRA |= (1 << ADSC); // 변환 시작 ADSC = 1
	while (ADCSRA & (1 << ADSC))
	;
	
	return ADC;
}
```

---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
IR센서값을 이용하여 USART를 통해 터미널에 각각의 값을 띄운다.

(IR센서 8개의 원본 값, 필터를 적용한 값, min값, max값, 정규화 값(min 0.00 ~ max 1.00)

<img width="987" height="424" alt="image" src="https://github.com/user-attachments/assets/bc7cac9a-c266-4855-886e-422b2d285e3c" />

IR센서를 filter -> 정규화 한 값이 0.8 이상일 경우 같은 번호의 LED를 켜고 그 이하일 경우 끈다.

LCD에 IR센서 8개의 정규화 된 값을 띄운다.

### 동작 사진 / 영상

| 정면 동작 모습 |
| :---: |
| [작동 영상](https://drive.google.com/file/d/1UFM6ivjVSQsOC62rp9AArAQopDWS_TGk/view?usp=drive_link) |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)의 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Gemini** | 개념정리, 디버깅 | MUX값 재설정 시 걸리는(적용되는) 시간, IR센서 값에 노이즈 끼는 경우의 수 |

### AI 활용 및 검증 원칙
1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
