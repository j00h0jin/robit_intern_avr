# Day05 Task03

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.08.04

---

# TIMER 1/3, PWM Register 설정에 대한 설명

## timer 1/3 register(16bit Timer/Counter) (ATmega128 Datasheet pp.133-143)

### TCCRnA, TCCRnB, TCCRnC (n = 1, 3) (Timer Counter Control Register A B C)

<img width="532" height="152" alt="image" src="https://github.com/user-attachments/assets/17cb04cd-9209-47a5-b778-4159bc2c2649" />

### TCCRnA - 채널 A, B, C의 비교 출력 모드 설정 / 파형 발생 모드 설정(TCCRnB 비트와 조합)

### BIT 7:2 - 비교 출력 모드 설정 비트 (7:6 COMnA, 5:4 COMnB, 3:2 COMnC)

타이머 카운터 레지스터(TCNT)와 출력 비교 레지스터(OCR)가 일치할 때  COM 비트 설정에 따라 OC 핀의 동작을 결정한다.
OC핀을 출력으로 사용할 때에는 DDRx를 출력으로 설정해두어야 한다.

TCNT와 OCR은 하단에 후술되어있다.

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

### ICRnH(High), ICRnL(Low) - Input Capture Register

<img width="492" height="164" alt="image" src="https://github.com/user-attachments/assets/057985ba-8220-450f-812c-1f449424e35d" />


코드로 시간을 확인하면 코드 실행 시간이나 인터럽트 때문에 오차가 발생하는데 입력 캡쳐를 사용하면 입력 캡쳐 레지스터(ICRn)에 즉시 저장함

16비트 타이머기 때문에 8비트씩 나누어서 상위(high)비트와 하위(Low)비트로 나누어서 저장한다.

Fast PWM 모드에서 타이머의 TOP 값으로 설정될 수 있다.


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

### TCNTn(L, H) - Timer Counter Register

<img width="607" height="200" alt="image" src="https://github.com/user-attachments/assets/31906937-6471-40c3-8af6-c8288d2234b3" />

타이머 카운터의 현재 카운트 값이 실시간으로 저장되는 레지스터이고 위에서 L, H가 나뉘었듯 16비트를 상위비트와 하위비트로 나누어서 저장한다.

---

### OCRn(A, B, C)(L, H) - Output Compare Register

<img width="589" height="294" alt="image" src="https://github.com/user-attachments/assets/76b8836f-3860-433b-88d9-0c602efafaec" />

내가 원하는 목표 카운트 값을 설정해두는 레지스터이다. MAX값보다 낮은 값에서 Compare Match를 원하는 경우 해당 레지스터에 TOP을 설정해둘 수 있다. 듀티비 제어용도로는 OCR만이 가능하기 때문에 PWM 모드에서는 ICR 레지스터를 TOP으로 사용하고 OCR 레지스터는 듀티비 조절용으로 사용하는 것이 일반적이다. (ICRn에는 전압을 켜고 끌 출력 핀과 비교(Compare) 하드웨어가 없기 때문에 듀티비 제어가 불가능하다)

*듀티비: 한 주기 동안에 전기가 들어와 있는 시간의 비율을 듀티비라고 한다.

> ex) ICR을 1000으로 설정(주기는 1000이 됨), OCR을 600으로 설정하면 듀티비는 60%(600/1000)이 된다.  

> Accessing 16-bit Registers (16비트 레지스터 접근 원리)    
> Atmega에서는 16비트의 값을 상위와 하위 비트로 나누어서 읽는데 하위 비트를 읽는 중 상위 비트가 변경되는 경우 엉뚱한 값을 참조하게 된다. 그렇기 때문에 Atmega에서 내부적으로 하위 비트를 읽는 동안에 상위 비트를 TEMP라는 레지스터에 저장해놓게 된다.

---

### SFIOR - Special Function IO Register

<img width="595" height="83" alt="image" src="https://github.com/user-attachments/assets/2517df30-9982-403c-a476-6cf33bfd5dea" />

AVR의 특수 기능들을 모아놓은 레지스터이다.

### ​​BIT 7, 1, 0 - 타이머 동기화 및 분주기(Prescaler) 제어

> BIT 7 - TSM(Timer/Counter Synchronization Mode)
> 여러 개의 타이머를 정확히 동시에 카운팅을 시작시킬 때 사용   
> BIT 1 - PSR2(Prescaler Reset Timer/Counter2)
> 1을 쓰면 Timer 2의 분주기 회로를 리셋한다.   
> BIT 0 - PSR310 (Prescaler Reset Timer/Counter 3, 1, 0)
> 1을 쓰면 Timer 0, 1, 3이 공유하는 분주기 회로를 리셋한다.

### ​​BIT 3 - ACME(Analog Comparator Multiplexer Enable)

아날로그 비교기의 - 단자로 ADC핀을 입력할 수 있게 확장한다.

### ​​BIT 2 - PUD(Pull-up Disable)

비트에 1 입력 시 MCU의 모든 GPIO 핀에 내장된 내부 풀업 저항을 일괄적으로 끈다.

---

### TIMSK, ETIMSK - (Extended)Interrupt Mask Register

<img width="582" height="81" alt="image" src="https://github.com/user-attachments/assets/65027ec8-24e7-4ad4-a4c3-6256fdeec6fb" />

<img width="584" height="83" alt="image" src="https://github.com/user-attachments/assets/1280287b-626b-43a9-8b31-0e5e16ba834e" />

각 비트에 해당하는 인터럽트를 허용할지 말지 결정하는 레지스터이다. (1이면 허용)

TOIE(Timer Overflow Interrupt Enable) - 타이머 오버플로우 인터럽트

OCIE(Output Compare Match Interrupt Enable) - 출력 비교 일치 인터럽트(카운터(TCNT) 값과 레지스터(OCR) 값이 일치하는 순간)

TICIE(Timer Input Capture Interrupt Enable) - 입력 캡쳐 인터럽트 허용 비트

---

### TIFR, ETIFR - (Extended)Interrupt Flag Register

<img width="581" height="82" alt="image" src="https://github.com/user-attachments/assets/42694a98-79c7-4163-a9cb-d88d493b4390" />

<img width="586" height="86" alt="image" src="https://github.com/user-attachments/assets/673bd93a-b6c8-4069-bf7d-54607bf1cc30" />

인터럽트 발생 조건이 충족되었을 때 켜지는(Flag) 레지스터이다.

TOV(Timer Overflow Flag) - 타이머 오버플로우가 발생하면 1

OCF(Output Compare Flag) - OCR값이 일치하여 이벤트가 일어나면 1

ICF(Input Capture Flag) - 입력 캡쳐 핀에 엣지가 감지되어 ICR에 값이 복사되면 1

플래그를 감지한 뒤 직접 끄려면 (W1C: Write 1 Clear) 1을 써서 클리어해야 한다. (0을 써서 지우는 경우 clear 찰나에 다른 flag가 켜지는 순간 해당 flag까지 지워버릴 수 있기 때문이다.)
