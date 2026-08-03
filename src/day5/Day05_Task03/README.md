# Day05 Task03

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.08.00

---

# TIMER 1/3, PWM Register 설정에 대한 설명

## timer 1/3 register(16bit Timer/Counter) (ATmega128 Datashhet pp.133-143)

### TCCRnA, TCCRnB, TCCRnC (n = 1, 3) (Timer Counter Control Register A B C)

<img width="532" height="152" alt="image" src="https://github.com/user-attachments/assets/17cb04cd-9209-47a5-b778-4159bc2c2649" />

### TCCRnA - 채널 A, B, C의 비교 출력 모드 설정 / 파형 발생 모드 설정(TCCRnB 비트와 조합)

### BIT 7:2 - 비교 출력 모드 설정 비트 (7:6 COMnA, 5:4 COMnB, 3:2 COMnC)

타이머 카운터 레지스터(TCNT)와 출력 비교 레지스터(OCR)가 일치할 때  COM 비트 설정에 따라 OC 핀의 동작을 결정한다.
OC핀을 출력으로 사용할 때에는 DDRx를 출력으로 설정해두어야 한다.

일반 모드 or CTC 모드일 때

<img width="387" height="161" alt="image" src="https://github.com/user-attachments/assets/5df6ad1d-b310-4828-a09d-561ad32ae066" />


fast PWM 모드일 때

<img width="386" height="222" alt="image" src="https://github.com/user-attachments/assets/fc336188-aa24-4834-a958-f13db2f5bdb8" />


Phase Correct PWM 모드와 Phase and Frequency Correct PWM 모드일 때

<img width="397" height="244" alt="image" src="https://github.com/user-attachments/assets/d2690f59-a670-4c19-9f53-870a609fd166" />



### BIT 1:0 - Waveform Generation Mode

<img width="541" height="332" alt="image" src="https://github.com/user-attachments/assets/6f5796f2-19a6-4d52-b243-74a827fc9bde" />

TCCRnB에 있는 WGMn3:WGMn2와 TCCRnA에 있는 WGMn1:0을 합쳐서 4개의 비트로 사용

Waveform Generation Mode: 타이머/카운터가 카운트를 어떤 방식으로 올리고 내릴지, 언제 리셋할지 결정하는 동작 모드 설정

Normal / CTC / Fast PWM / Phase Correct PWM 중 원하는 모드에 맞는 비트를 설정

---

<img width="499" height="145" alt="image" src="https://github.com/user-attachments/assets/2a6d1053-a892-4f8d-aab5-4058f26ba868" />

### TCCRnB - 입력 캡쳐 설정 / 분주비 설정

### BIT 7 - ICNCn (Input Capture Noise Canceler n)

입력 캡쳐 각각 단자 ICn 으로 입력되는 캡쳐 신호를 위한 노이즈 제어 회로가 작동하도록 설정(단, 노이즈 회로가 동작하는 경우 4 시스템 클럭만큼 입력 캡쳐 신호가 지연된다)

NCn = 1 노이즈 제거 회로가 작동

INCn = 0 노이즈 제거 회로가 작동되지 않음

입력 캡쳐: 외부에서 들어오는 신호의 타이밍을 기록하는 기능

### ICRnH(High), ICRnL(Low)

<img width="492" height="164" alt="image" src="https://github.com/user-attachments/assets/057985ba-8220-450f-812c-1f449424e35d" />


코드로 시간을 확인하면 코드 실행 시간이나 인터럽트 때문에 오차가 발생하는데 입력 캡쳐를 사용하면 입력 캡쳐 레지스터(ICRn)에 즉시 저장함

16비트 타이머기 때문에 8비트씩 나누어서 상위(high)비트와 하위(Low)비트로 나누어서 저장한다.

### BIT 6 - ICESn (Input Capture Edge Select)

외부 핀으로 들어오는 신호를 어떤 Edge에서 타이머 값을 ICRn에 복사할지 결정하는 비트

0일 때 falling edge(하강 엣지)에서 복사

1일 때 rising edge(상승 엣지)에서 복사

### ​​BIT 5 - 사용되지 않음 (Reserved)

### BIT 4:3 - 파형 발생 모드 비트 (WGM : Waveform Generate Mode)

TCCRnA에서 설명(위에)

### BIT 2:0 - 클럭 선택 비트 (CK : Clock Select)

<img width="454" height="202" alt="image" src="https://github.com/user-attachments/assets/bc071e09-3f1d-4a5f-9d1a-fc1bd1c2c142" />

분주기(Prescaler)를 설정하는 비트이다.

메인 클럭 주파수(Atmega128의 경우 16000000Hz)를 분주기로 나누어서 timer/counter 속도를 느리게 해준다.

원하는 시간 단위를 만들기 위해 메인 클럭 주파수를 분주기로 나누어 조정한다.

---

<img width="600" height="176" alt="image" src="https://github.com/user-attachments/assets/c241f65b-bfb6-4b97-a876-945b333355ab" />

### TCCRnC - 강제 출력 비교 설정

### ​​BIT 7:5 강제 출력 비교 설정 (A, B, C)

타이머 카운터(TCNT)가 OCR과 같아질 때까지 기다리지 않고 강제로 Compare Match 이벤트를 발생시키게 하는 모드이다.

해당하는 비트를 1로 설정하면 강제로 Compare Match 이벤트를 발생시킨다.

---


TCNT1H, TCNT1L

OCR1AH, OCR1AL, OCR1BH, OCR1BL, OCR1CH, OCR1CL

SFIOR

TIMSK, ETIMSK, TIFR, ETIFR
