# Day04_Task04

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.08.03

---

## 1. 개요 (Overview)

본 과제는 ATmega128 마이크로컨트롤러를 활용하여 주요 주변장치(Peripherals)를 제어하고 센서 데이터를 수신/처리하는 시스템을 구현하는 것을 목표로 함.

### 핵심 목표

- ATmega128 레지스터 설정을 통한 주변장치 제어
- PSD의 ADC값을 cm로 변환
- ADC의 값을 이동 평균 필터를 사용하여 노이즈 필터링

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
├── Day04_Task04/
│   └──  main.c # 메인 제어 루프 및 시스템 초기화
└── README.md
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 이동 평균 필터 로직 예시 (`main.c`)

이동평균 필터를 적용한 부분만 표시하였음(Day04_Task03 코드를 기반으로 했기 때문)

```c
#define F_CPU 16000000UL


#define A 0.008271
#define B 939.6
#define C -3.398
#define D 17.339
#define arrSize 4

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
.
.
.
		
int psdADC = 0, i = 0;
	float voltage = 0;
	float distance = 0;
	
	char psdChar[20];
	char raw[] = "RAW: ";
	char filtered[] = "FILTERED: ";
	char dist[] = "DISTANCE: ";
	
	int moveAvgArr[arrSize];
	
	int sum, avg;

	for (int j = 0; j < arrSize; j++) // while문 전에 평균값을 낼 데이터들을 채움
	{
		moveAvgArr[j] = Read_ADC();
	}

.
.
.
	
   	while (1)
	{
		i = 0, sum = 0;
  .
  .
  .
		
		// 이동 평균 필터
		// 최근의 연속된 데이터 값들의 평균을 구하는 방법
		// 새로운 값이 들어오면 가장 오래된 값을 버림
		// 최근 arrSize개 값으로 필터링하기
		
		for(int j = arrSize - 1; j > 0; j--)
		{
			moveAvgArr[j] = moveAvgArr[j-1];
			// ex a b c를 a a b로 밀려면
			// c = b;
			// b = a;
		}
		moveAvgArr[0]=psdADC; // 맨 앞에 가장 최근 값
		for (int j = 0; j < arrSize; j++) // sum
		{
			sum += moveAvgArr[j];
		}
		avg = sum/ arrSize; // sum / size = 평균
		sprintf(psdChar, "%d", avg);
		while(filtered[i] != '\0')
		{
			Uart_Putch(filtered[i]);
			i++;
		}
		i = 0;
		while(psdChar[i] != '\0')
		{
			Uart_Putch(psdChar[i]);
			i++;
		}
		i = 0;
		Uart_Putch(' ');
		Uart_Putch('|');
		Uart_Putch(' ');
		
		voltage = (float)avg * 5 / 1023;
		// https://blog.naver.com/ann_arbor/221627224574 해당 공식 참고
		distance = (A +B*voltage) / (1 + C*voltage + D*voltage*voltage);
		dtostrf(distance, 5, 1, psdChar);
		
		while(dist[i] != '\0')
		{
			Uart_Putch(dist[i]);
			i++;
		}
		i = 0;
		while(psdChar[i] != '\0')
		{
			Uart_Putch(psdChar[i]);
			i++;
		}
		i = 0;
		Uart_Putch('c');
		Uart_Putch('m');
		Uart_Putch('\r'); 
		Uart_Putch('\n');
		psdChar[0] = '\0';
		_delay_ms(500);
	}
}

```


---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오

PSD로부터 받은 원시 데이터(raw) 및 필터링 데이터 동시 출력

필터링 미적용 데이터 (raw)와 이동 평균 필터 혹은 다른 필터를 적용한 데이터 (Filtered ADC)을 UART로 동시에 출력

(예: RAW: 412 | FILTERED: 405 | DISTANCE: 15.2cm)

필터링 관련 보고서 작성 (사용한 필터의 개념, 특징, 필터 미적용 값과 필터 적용한 값의 변화 등)
> 보고서는 최하단에 작성하였다.

### 동작 사진 / 영상

|                 정면 동작 모습                 |
| :--------------------------------------------: |
| [동작 영상](https://drive.google.com/file/d/1wazyi7UMdYLYez8l2y1k0ou6GlitUJ7v/view?usp=drive_link) |


---

## 7. AI 툴 활용 명시 (AI Tools Declaration)

본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)의 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool)        | 활용 영역              | 세부 사용 목적 및 내용 |
| :------------------- | :--------------------- | :-------------------- |
| **Gemini**           | 개념 정리 | 이동 평균 필터 개념 쉽게 설명 |

### AI 활용 및 검증 원칙

1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.

----------

### 필터링 관련 보고서 (사용한 필터의 개념, 특징, 필터 미적용 값과 필터 적용한 값의 변화 등)

#### 사용한 필터: 이동 평균 필터

##### 이동 평균 필터란?

<img width="560" height="505" alt="image" src="https://github.com/user-attachments/assets/27bc9b23-dd3a-450c-891f-042b084fd33d" />

일단 데이터들의 평균을 측정하게 되면 noise값을 잡아주는 효과가 있다. 하지만 위의 예시는 측정값이 일정할 때에는 문제가 없으나 측정값이 변화하는 경우에 문제가 생긴다.

<img width="561" height="424" alt="image" src="https://github.com/user-attachments/assets/a5290cf0-a8c1-4429-b50a-2fe5e30fc81a" />

위의 사진처럼 측정값이 변화하는 경우 평균을 누적해서 쌓게 되면 측정값과 평균값의 오차율이 점점 커지게 된다.

이러한 단점을 해소하기 위해 이동 평균 필터라는 것을 사용한다. 이동 평균 필터는 평균을 낼 데이터의 사이즈를 정한 뒤 그 사이즈를 넘어가는 데이터가 들어오면 가장 최근 값을 넣고, 가장 오래된 값은 버려서 평균을 측정한다. 자료구조의 queue와 같은 구조라고 보면 될 것 같다.

<img width="557" height="506" alt="image" src="https://github.com/user-attachments/assets/619102a7-d882-4697-a28e-703e844d9942" />

해당 로직을 적용해주면 가장 최신값에서 설정해둔 사이즈만큼의 값만 평균을 내어주니 측정값과 오차율의 괴리가 줄어드는 것을 볼 수 있다.


#### 필터 미적용 값과 필터 적용한 값의 변화

[동작 영상](https://drive.google.com/file/d/1wazyi7UMdYLYez8l2y1k0ou6GlitUJ7v/view?usp=drive_link)을 보게되면 동일한 거리에서 값이 튀어서 noise가 발생할 때 보정해주는 효과가 있다.

<img width="236" height="286" alt="image" src="https://github.com/user-attachments/assets/9a94bb24-4802-481e-b4c4-fa1feff69c1e" />

위의 사진을 보면 측정값이 동일한 경우에서 noise 때문에 512로 측정된 값이 생겼다. 이동 평균 필터를 적용했기 때문에 cm로 변환한 값이 크게 흔들리지 않고 안정적으로 출력되는 것을 볼 수 있다.

그러나 거리가 짧은 시간 내에 확 바뀌는 경우에는 거리가 한번에 갱신되지 않고 점차적으로 올라가거나 줄어드는 모습을 볼 수 있다. 아래 사진은 25cm 부근에서 40cm부근까지, 그리고 다시 25cm 부근까지 빠르게 움직였을 때의 값이다. 해당 코드에서는 이동 평균에 사용된 사이즈가 4개이기 때문에 이전 3개의 값에 영향을 받아 빨간색으로 박스친 부분이 순간적으로 변하지 않고 완만하게 변화하는 것을 볼 수 있다.

<img width="225" height="206" alt="image" src="https://github.com/user-attachments/assets/a3900934-d4c2-405d-b007-04f5c0ac3719" />

이러한 점은 이동 평균 필터의 값에 새로운 정보들이 들어오기를 기다리거나, 더 빠르게 값이 변화하는 것을 원한다면 이동 평균 필터의 사이즈를 줄이거나 값을 측정하는 주기를 줄이면 해결될 것으로 보인다.
