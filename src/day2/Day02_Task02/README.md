
# Day02 Task02

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.07.30

---

## 1. 개요 (Overview)
본 과제는 ATmega128A 마이크로컨트롤러를 활용하여 주요 주변장치(Peripherals)를 제어하고 센서 데이터를 수신/처리하는 시스템을 구현하는 것을 목표로 함.

### 핵심 목표
* ATmega128A 레지스터 설정을 통한 Potentiometer값 ADC 수행, I2C를 통한 LCD 제어

---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드, LCD, Potentiometer |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128A]                            [Target Component]
 PORTF PF0                       ----->   Potentiometer
 PORTD Pin 0, 1 (PIND0, PIND1)   ----->   LCD(I2C)
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

### 가변저항 값 읽어오기 예시 (`main.c`)
```c
#include "LCD_Text.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int Read_ADC() {
	ADCSRA |= (1 << ADSC); // 변환 시작 ADSC = 1 // ADSC 비트가 켜지면 ADC 기능 수행
	return ADC;
}

int main(void)
{
	int r;
	float v;
	ADMUX = 0x40; // 0100 0000 외부의 AVCC 전압 사용(PF0)
	ADCSRA = 0x87; // 1000 0111 ADC 활성화, 클럭 분주비 128
.
.
.
}
```
<img width="613" height="252" alt="image" src="https://github.com/user-attachments/assets/0541182e-f6c8-4423-a0c5-433f24a3d3ac" /><img width="345" height="170" alt="image" src="https://github.com/user-attachments/assets/9a6831ca-defd-4e70-a145-b2966d98cab2" />



---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
1. 가변 저항 값에 따라 LED를 이동시킨다.
2. ADC값을 LCD에 띄우기.
3. ADC값을 계산해서 현재 가변저항의
 전압 값을 LCD에 표시.
- 이름 이니셜 표기할 것

### 동작 사진 / 영상

| 정면 동작 모습 | 
| :---: | 
| [동작 영상](https://drive.google.com/file/d/1Ya7Mv3Qy5gi0vabb7k1WjlcRavthSn4t/view?usp=drive_link) | 

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)의 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Gemini** | 개념 정리 & 모듈 설계 | - I2C제어를 위한 라이브러리, I2C 개념 정리, I2C와 직결의 차이점 |

### AI 활용 및 검증 원칙
1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
