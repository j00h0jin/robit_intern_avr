# Day06_Task02

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.08.09

---

## 1. 개요 (Overview)
본 과제는 ATmega128 마이크로컨트롤러를 활용하여 주요 주변장치(Peripherals)를 제어하고 센서 데이터를 수신/처리하는 시스템을 구현하는 것을 목표로 함.

### 핵심 목표
* ATmega128 레지스터 설정을 통한 주변장치 제어
* 모터 드라이버를 통한 모터 제어
* 12V 전원으로 시스템 전력 공급

---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드, 모터 드라이버(L298N), 레귤레이터, 모터 2개 |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                  [Target Component]
 PORTB (PB0-3, PB5-6) ----->   L298N
```

### 주요 회로 특징
* **전원:** 5V DC 안정화 전원 공급
* **주의사항:** ISP 다운로드 시 SPI 핀 타겟 전원 및 리셋 회로 간섭 주의

---

## 4. 프로젝트 구조 (Directory Structure)
> 구현부(.c), 선언부(.h)만 구조에 표기함.
```text
├── Day06_Task02/
│   └── main.c # 메인 제어 루프 및 시스템 초기화
└── README.md
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 모터 구동 코드 예시 (`main.c`)
```c
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// PB0, 1 모터 1 방향 제어(10 전진, 01 후진)
// PB2, 3 모터 2 방향 제어
// PB5, 6 모터 1 속도, 모터 2 속도
#define motor1Forward()  {PORTB |= (1<<PB0); PORTB &= ~(1<<PB1);}
#define motor1Stop() {PORTB &= ~((1<<PB0) | (1<<PB1)); }
#define motor2Forward()  {PORTB |= (1<<PB2); PORTB &= ~(1<<PB3);}
#define motor2Stop() {PORTB &= ~((1<<PB2) | (1<<PB3));}

void set_speed(unsigned char speed_1, unsigned char speed_2);

int main(void)
{
	DDRB = 0x6F; // 0110 1111 (PB6, 5, 3, 2, 1, 0)
	// non-inverting mode A B, Fast PWM, 8-bit mode(WGM), 분주비 64(CS)
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);

	while (1)
	{
		// 전진
		motor1Forward();
		motor2Forward();
		set_speed(200, 200);
		_delay_ms(4000);
		// 정지
		motor1Stop();
		motor2Stop();
		set_speed(0, 0);
		_delay_ms(1000);
	}
	return 0;
}

void set_speed(unsigned char speed_1, unsigned char speed_2)
{
	OCR1A = speed_1;
	OCR1B = speed_2;
	// timer 1번 사용했으므로 OCR1에 속도 지정
	// 모터 속도 (0 ~ 255)
}
```

---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
모터 2개 동작시키기

### 동작 사진 / 영상

| 정면 동작 모습 |
| :---: |
| [작동 영상](https://drive.google.com/file/d/1XPs_Bo_FtwRTFKa4a_NbOXGrMwUFvmA7/view?usp=drive_link) |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)의 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Gemini** | 개념 정리 | L298N으로 모터 드라이버 제어를 위한 정보 정리 |

### AI 활용 및 검증 원칙
1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
