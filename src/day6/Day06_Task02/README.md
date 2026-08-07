# Day06_Task02

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.08.00

---

## 1. 개요 (Overview)
본 과제는 ATmega128 마이크로컨트롤러를 활용하여 주요 주변장치(Peripherals)를 제어하고 센서 데이터를 수신/처리하는 시스템을 구현하는 것을 목표로 함.

### 핵심 목표
* ATmega128 레지스터 설정을 통한 주변장치 제어
* 모터 드라이버를 통한 모터 제어
* 레귤레이터를 통해 12V 전원으로 시스템 전력 공급

---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드, 모터 드라이버, 레귤레이터, 모터 2개 |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                 [Target Component]
 PORTA (PA0 ~ PA7)   ----->   8-Bit LED
 PORTB Pin 4 (PB4)   ----->   PWM Motor Control (OC0)
 PE0 (RXD0) / PE1    ----->   UART Serial Communication
 ADC0 (PF0)          ----->   Analog Sensor Input
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
├── include/
│   ├── timer.h
│   ├── uart.h
│   └── adc.h
└── README.md
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 타이머/카운터 및 PWM 초기화 예시 (`timer.c`)
```c
asdasd
```

---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오
모터 2개 동작시키기

### 동작 사진 / 영상

| 정면 동작 모습 |
| :---: |
| [작동 영상]() |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)의 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Gemini** |  |  |

### AI 활용 및 검증 원칙
1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
