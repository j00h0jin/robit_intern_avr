# Day03_Task04

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.08.02

---

## 1. 개요 (Overview)

본 과제는 ATmega128 마이크로컨트롤러를 활용하여 주요 주변장치(Peripherals)를 제어하고 센서 데이터를 수신/처리하는 시스템을 구현하는 것을 목표로 함.

### 핵심 목표

- ATmega128 레지스터 설정을 통한 주변장치 제어
- USART 레지스터를 사용하지 않고 USART 통신

---

## 2. 개발 환경 (Environment)

| 항목               | 내용                                            |
| :----------------- | :---------------------------------------------- |
| **MCU**            | ATmega128A (16MHz External Crystal)             |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC        |
| **Flasher Tool**   | USBISP / STK500                                 |
| **언어**           | C Language                                      |
| **주요 부품**      | ATmega128 개발보드   |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                              [Target Component]
 PD2 (RXD0) / PD3                ----->   UART Serial Communication
```

### 주요 회로 특징

- **전원:** 5V DC 안정화 전원 공급
- **주의사항:** ISP 다운로드 시 SPI 핀 타겟 전원 및 리셋 회로 간섭 주의

---

## 4. 프로젝트 구조 (Directory Structure)

> 구현부(.c), 선언부(.h)만 구조에 표기함.

```text
├── Day03_Task04/
│   └── main.c # 메인 제어 루프 및 시스템 초기화
└── README.md
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 비트 뱅잉 예시 (`main.c`)

```c
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define BIT_DELAY 104 // 1 / 9600 = 104.xx(us) 오차범위 어느정도 허용
#define TX_PIN PD3 // PD3 핀 사용

/*
비트 뱅잉
하드웨어 없이 소프트웨어를 사용하여 구현
Start Bit: 전송 데이터 패킷의 시작 부분을 알린다. 이 신호를 통해 RX에서 데이터 수신을 시작한다
Data Bits: Start Bit와 Stop Bit 사이에 존재한다. 최대 9bit까지 데이터 통신이 가능하다.
Parity Bit: 선택적으로 사용 가능한 Bit로, 대표적으로 Odd Parity(홀수 패리티)와 Even Parity(짝수 패리티)를 사용한다. - Odd Parity: 데이터 비트의 총합이 홀수일 경우 오류가 없는 것으로 판단한다. - Even Parity: 데이터 비트의 총합이 짝수일 경우 오류가 없는 것으로 판단한다.
Stop Bit: 전송 데이터 패킷의 끝을 정의한다. 1bit 또는 2bit를 사용하며, 1bit 사용시에는 논리값1, 2bit 사용시에는 논리값 0, 1을 순서대로 사용한다.

데이터 8비트 사용 시 start bit + 8 bit + stop bit = 10bit / 패리티 비트 사용 X
*/
void transmit(unsigned char data);

int main(void)
{
	// 출력
	DDRD |= (1 << TX_PIN);
	
	// 기본 high, low 인식 시 start로 인식
	PORTD |= (1 << TX_PIN);
	_delay_ms(10);
	
	char helloWolrd[] = "HelloWorld!";

	while (1)
	{
		for (int i = 0; i < 11; i++)
		{
			transmit(helloWolrd[i]);
		}
		
		_delay_ms(1000);
	}
}

void transmit(unsigned char data) {
	// 전송을 시작할 때는 low비트를 1비트 시간만큼 출력 start
	PORTD &= ~(1 << TX_PIN); // low
	_delay_us(BIT_DELAY); // 비트마다 딜레이 필요

	// 데이터 비트 (8bit) lsb부터 보냄(최소 단위 비트)
	// 해당 비트만 1인 비트와 &연산을 하면 해당 비트 값만 제외하고 0
	// 해당 비트는 0이면 0, 1이면 1
	for (int i = 0; i < 8; i++)
	{
		if (data & (1 << i))
		{
			PORTD |= (1 << TX_PIN);  
		}
		else
		{
			PORTD &= ~(1 << TX_PIN); 
		}
		_delay_us(BIT_DELAY);
	}

	// 전송을 끝마칠 때는 high비트를 1비트 시간만큼 출력 end
	PORTD |= (1 << TX_PIN); // high
	_delay_us(BIT_DELAY);
}

```

---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오

UART 관련 레지스터 사용하지 않고 PORTD와 DDRD만을 사용하여 uart 데이터 전송 가능하게 하기

“HelloWorld!” 1초마다 보내기







### 동작 사진 / 영상

|                 정면 동작 모습                 |  
| :--------------------------------------------: | 
| [동작 영상](https://drive.google.com/file/d/17DFO2kJkxy1npNCU43cUEoL55sgbxAoP/view?usp=drive_link) |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)

본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)의 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool)        |     활용 영역              | 세부 사용 목적 및 내용        |
| :--------------- | :--------------------------- | --------------------- |
| Gemini |  개념 정리, 디버깅  | 비트 뱅잉에 대해 쉽게 설명, UART에서의 비트 뱅잉, USART1번 칸에 스위치가 연결되어 있는데 그냥 사용해도 문제가 없는지?  |

### AI 활용 및 검증 원칙

1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
