# Day04 Task01

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.08.03

---

### timer, counter 기본 개념

<img width="740" height="357" alt="image" src="https://github.com/user-attachments/assets/11ebb8ae-467b-4153-99ad-69f0d5cbb395" />

BOTTOM : 타이머/카운터가 가질 수 있는 최솟값
MAX : 타이머/카운터가 가질 수 있는 최댓값
TOP : 타이머/카운터가 도달할 수 있는 최댓값(설정값)

업 카운터 : 카운터 값이 항상 증가하는 것

(예 : 0x00 -> 0x01 -> 0x02 -> 0x03 .....)

다운 카운터 : 카운터 값이 항상 감소하는 것
​
(예 : 0xFF -> 0xFE -> 0xFD .....)

업 다운 카운터 : 카운터 값이 증가하다가 감소하는 것

(예 : 0x00 -> 0x01 -> 0x02 -> ... -> 0xFF -> 0xFE -> 0xFD....)


---

# TIMER 0/2에 대한 정보 (사용법, 정의, 원리)

### Timer/counter 0/2

범용 목적의 단일 채널 8비트 timer/counter이다

특징

> 단일 채널 카운터  
> CTC 모드(clear timer on compare match)  
> 글리치가 없는 pc pwm모드  
> 주파수 발생기  
> 외부 이벤트 카운터  
> 10비트 클럭 분주기  
> 오버플로우 인터럽트(TOV0)와 비교 일치 인터럽트(OCF0)  

---

### timer 0/2 인터럽트
<img width="940" height="339" alt="image" src="https://github.com/user-attachments/assets/afdb8f1e-dfce-4a50-820a-68b7a53edfc9" />

COMP OCRn과 TCTNn이 같으면 OCFn이 set(TIMSK의 플래그)

OVF(타임 오버플로우 인터럽트)

TCNTn값이 overflow가 되면 인터럽트 발생

---

### Timer, counter 0/2 register
<img width="940" height="135" alt="image" src="https://github.com/user-attachments/assets/a04bdc14-06f3-45e2-a03f-b9fb3225faf5" />

<img width="940" height="137" alt="image" src="https://github.com/user-attachments/assets/a56531ff-db46-4385-9d8c-85be271ea22b" />

TCCR0/2

해당 레지스터는

> 강제 출력 비교  
> 파형 발생 모드  
> 비교 일치 출력 모드  
> 클럭 소스 입력  

의 기능을 가짐

### BIT 7 - 강제 출력 비교 비트 (FOC : Force Output Compare) 

PWM 모드가 아닌 경우에만 유효하며, 이 비트를 세트하면 즉시, 강제로 OC 단자에 출력 비교가 일치된 것과 동일한 출력을 내보낸다. 그러나 인터럽트를 발생하지 않으며, 특별한 경우가 아니면 0으로 설정한다.

### BIT 6,3 - 파형 발생 모드 비트 (WGM : Waveform Ganerate Mode)

비트 설정에 따른 파형 발생 모드를 선택

파형 발생 모드로는

> 일반 모드  
> CTC 모드  
> 고속 PWM 모드  
> 위상 교정 PWM (PC PWM)

4가지가 있고, 아래와 같이 설정할 수 있다.

<img width="645" height="184" alt="image" src="https://github.com/user-attachments/assets/9b30d623-a362-43a0-a243-7fa7ba05582c" />


## (1) 일반 모드 (Normal Mode)  

가장 간단한 카운터로써 계수 증가는 BOTTOM -> MAX 순으로 단순하게 증가하는 업 카운터로만 동작이 된다.

인터럽트는 TCNT 값이 MAX -> BOTTOM이 될 때 인터럽트가 발생하며 외부 펄스 입력을 세는 단순한 카운터 용도에 적합하다.


## (2) CTC 모드 (Clear Timer on Compare Match Mode) 

CTC 모드는 원하는 시간 간격으로 주기적인 인터럽트를 발생시키는 데 적합하다.

카운터가 BOTTOM에서 설정된 OCR 값과 같아지면 0으로 클리어되면서 인터럽트가 발생한다.


## (3) 고속 PWM 모드 (Fast Pulse Width Modulation Mode) 

고속 PWM 모드는 고속으로 PWM 파형을 발생시키는데 적합하다.

계수는 BOTTOM -> MAX 순으로 단순하게 증가하는 방향으로만 반복 수행한다.


## (4) PC PWM 모드 (Phase Correct  PWM mode)

PC PWM 모드는 높은 분해능의 PWM 출력 파형을 발생하는데 유용하다.

해당 모드는 주파수를 절반이 되는 대신 분해능이 2배인 16비트로 높아진다.

(*분해능: 샘플링 간격이 좁아진다. 시간의 해상도(?)가 좋아짐)
<img width="473" height="326" alt="image" src="https://github.com/user-attachments/assets/3a37e177-4d84-4cbd-8b2e-6bee9913bbf8" />

### BIT 5,4 - 비교 일치 출력 모드 비트 (COM : Compare Match Output Mode)

COM 비트 설정에 따라 OC 핀의 동작이 결정된다.
OC핀으로 출력을 원하는 경우는 해당 포트의 데이터 출력 방향을 DDRx에서 출력으로 설정해야 한다.

<img width="602" height="672" alt="image" src="https://github.com/user-attachments/assets/cee97e61-bb98-4fee-9dfa-ec4a8544081e" />

BIT 2,1,0 - 클럭 선택 비트 (CS : Clock Select)
비트에 따라서 타이머/카운터의 클럭을 무엇으로 받을 지 결정한다.

> timer, counter 2 설정
<img width="598" height="329" alt="image" src="https://github.com/user-attachments/assets/eae31261-c01d-435b-9231-583c4ee59d4a" />

> timer, counter 0 설정
<img width="600" height="331" alt="image" src="https://github.com/user-attachments/assets/7416eb0b-833d-4784-a1c2-262babab86e4" />

--- 
<img width="940" height="137" alt="image" src="https://github.com/user-attachments/assets/c2881d71-18f0-48ba-8f61-e4ced4870658" />
<img width="940" height="140" alt="image" src="https://github.com/user-attachments/assets/8e31b25c-fa0d-48fd-9260-4cac6f3ff1fd" />

TCNTn

타이머/카운터의 카운터 값을 저장하고 있는 레지스터

동작 중에 수정하게 되면 사용자의 의도와 다르게 TCNT 값과 OCR 값을 비교하여 출력 신호를 발생할 수 있다. (COMP 인터럽트)

---

<img width="940" height="139" alt="image" src="https://github.com/user-attachments/assets/73818ea0-d29b-43e0-bc39-1fbfd3c16143" />

<img width="940" height="140" alt="image" src="https://github.com/user-attachments/assets/9592fcbe-439a-4292-bc97-50577cfefb49" />

OCRn

타이머/카운터 레지스터 TCNT 값과 비교하여 OC2 단자에 출력을 발생하기 위한 8비트 값을 저장하는 레지스터

사용자가 동작 도중에 값을 수정한다 하더라도 바로 바뀌지 않고 TOP 혹은 BOTTOM 에 도달했을 때 바뀌게 된다.



---

<img width="940" height="141" alt="image" src="https://github.com/user-attachments/assets/d10f9fe1-730f-44d4-9a34-def4fc5e9dc8" />

ASSR (0만 해당)

외부 클럭에 의하여 비동기 모드로 시작하는 경우에 관련된 기능을 수행한다.

BIT [7:4] - 사용되지 않음 (Reserved) 

### BIT 3 - AS0 (Asynchronous Timer/Counter) 

타이머/카운터 0의 클럭 소스를 선택하는 비트

AS=0 동기적

AS=1 비동기적


### BIT 2 - TCN0UB (Timer/Counter 0 Update Busy) 

타이머/카운터 0 이 비동기적으로 동작하고 있는 경우 TCNT0 에 새로운 값이 써질 때 TCN0UB = 1로 세트되며 쓰기가 완료되면 TCN0UB = 0 으로 클리어 된다.


### BIT 1 - OCR0UB (Output Compare Register 0 Update Busy) 

타이머/카운터 0 이 비동기적으로 동작하고 있는 경우 **OCR0** 에 새로운 값이 써질 때 OCR0UB = 1로 세트되며 쓰기가 완료되면 OCR0UB = 0 으로 클리어 된다.


### BIT 0 - TCR0UB (Timer/Counter Control Register 0 Update Busy) 

타이머/카운터 0 이 비동기적으로 동작하고 있는 경우 **TCCR0** 에 새로운 값이 써질 때 TCR0UB = 1로 세트되며 쓰기가 완료되면 TCR0UB = 0 으로 클리어 된다.

---

<img width="940" height="137" alt="image" src="https://github.com/user-attachments/assets/5aa5dfcd-a3d3-482f-8cb8-33c288aa60db" />

TIMSK(Interrupt Mask Register)

인터럽트를 개별적으로 허용하는 기능을 수행하는 레지스터이다.

---

<img width="940" height="137" alt="image" src="https://github.com/user-attachments/assets/03153555-adb2-4537-8045-9540bd4ac643" />

TIFR(Interrupt Flag Register)

인터럽트 플래그가 저장되는 레지스터이다.
