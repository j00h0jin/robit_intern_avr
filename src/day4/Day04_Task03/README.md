# Day04_Task03

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.08.03

---

## 1. 개요 (Overview)

본 과제는 ATmega128 마이크로컨트롤러를 활용하여 주요 주변장치(Peripherals)를 제어하고 센서 데이터를 수신/처리하는 시스템을 구현하는 것을 목표로 함.

### 핵심 목표

- ATmega128 레지스터 설정을 통한 주변장치 제어
- PSD의 ADC값을 cm로 변환

---

## 2. 개발 환경 (Environment)

| 항목               | 내용                                            |
| :----------------- | :---------------------------------------------- |
| **MCU**            | ATmega128A (16MHz External Crystal)             |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC        |
| **Flasher Tool**   | USBISP / STK500                                 |
| **언어**           | C Language                                      |
| **주요 부품**      | ATmega128 개발보드, UART, PSD |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                             [Target Component]
 PE0 (RXD0) / PE1                ----->   UART Serial Communication
 PORTF PF1                       ----->   PSD
```

### 주요 회로 특징

- **전원:** 5V DC 안정화 전원 공급
- **주의사항:** ISP 다운로드 시 SPI 핀 타겟 전원 및 리셋 회로 간섭 주의

---

## 4. 프로젝트 구조 (Directory Structure)

> 구현부(.c), 선언부(.h)만 구조에 표기함.

```text
├── Day04_Task03/
│   ├──  main.c # 메인 제어 루프 및 시스템 초기화
└── README.md
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 시간 작동 예시 (`main.c`)

PSD에서 받아오는 ADC값을 cm로 변환하기 위한 정보는 [여기](https://blog.naver.com/ann_arbor/221627224574)를 참고하였다.

샤프 2y0a02 센서의 측정범위는 20~150cm이고, 20cm 이전에는 adc값이 일정하지 않으므로 20cm 이전에 발생하는 불규칙적인 값들은 continue처리해주었다.

```c
#define F_CPU 16000000UL


#define A 0.008271
#define B 939.6
#define C -3.398
#define D 17.339


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
	UBRR0L = 16; // 57600
	UBRR0H = 0;
	UCSR0A = 0x20; // 송신 상태비트 초기화
	UCSR0B = 0x18;
	UCSR0C = 0x06; // data 8bit
	
	DDRE = 0x02; // E0 입력, E1 출력
		
	int psdADC = 0, i = 0;
	float voltage = 0;
	float distance = 0;
	
	char psdChar[20];
	
	ADMUX = 0x41; // 0100 0001 ADC1번
	ADCSRA = 0x87; // 1000 0111 ADC 활성화, 클럭 분주비 128
	
    while (1) 
    {
		i = 0;
		psdADC = Read_ADC();
		// 20cm를 기준으로 전압(ADC값)이 올랐다 떨어지는데
		// 유의미한 데이터를 얻기 위해서는 20cm 이후의 공식이 적용되는 부분만 걸러줘야 함
		// 따라서 20cm의 전압 고점인 2.7V에서(넉넉잡아 2.6V로 계산하였음)
		// ADC를 역산해주면 1023x2.6/5 = 531.96이 나옴
		// 531.96을 넘어가는 값은 20cm 안쪽으로 판단하고 값을 출력하지 않고 continue시킴
		// 2.6으로 계산했기 때문에 20cm가 조금 넘는 구간부터 측정됨
		if(psdADC > 532) 
			continue;
		voltage = (float)psdADC * 5 / 1023;
		// https://blog.naver.com/ann_arbor/221627224574 해당 공식 참고
		distance = (A +B*voltage) / (1 + C*voltage + D*voltage*voltage);
		dtostrf(distance, 5, 1, psdChar);
		// sprintf(psdChar, "%d", psdADC);
		
		while(psdChar[i] != '\0')
		{
			Uart_Putch(psdChar[i]);
			i++;
		}
		Uart_Putch('c');
		Uart_Putch('m');
		Uart_Putch(' ');
		psdChar[0] = '\0';
		_delay_ms(500);
    }
}

```


---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오

PORTF(ADC) 이용해서 PSD 센서 값 읽기

ADC 변환 값을 센서 특성에 맞는 거리 (cm) 단위로 환산

계산된 거리 데이터를 UART로 PC 시리널 터미널에서 출력

측정 주기 설정 및 비정상 센서 데이터 예외처리 필수

### 동작 사진 / 영상

|                 정면 동작 모습                 |
| :--------------------------------------------: |
| [동작 영상](https://drive.google.com/file/d/1OrT7VlcoCsus9QoZrx43QftGYJ9iamZu/view?usp=drive_link) |
| [비정상 센서 데이터 예외처리](https://drive.google.com/file/d/1nGGR3H2DJUeGItnNomLxYzGYAkYtJKgs/view?usp=drive_link) |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)

본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)의 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool)        | 활용 영역              | 세부 사용 목적 및 내용 |
| :------------------- | :--------------------- | :-------------------- |
| **Gemini**           | 디버깅 | 이상한 값이 뜨는 이유(디버깅), PSD값이 출렁이는 원인 |

### AI 활용 및 검증 원칙

1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
