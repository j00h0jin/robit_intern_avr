# Day02 Task01

## ATmega128 데이터 시트 ADC 파트 공부(pp.230-245)

---


## ATmega128 ADC 목차

<img width="517" height="287" alt="image" src="https://github.com/user-attachments/assets/d87d8d35-f0bf-42b6-b137-309319955617" />

---

## 10-bit Resolution(10비트 해상도)

ATmega128은 기본적으로 10비트 해상도를 지원한다. 입력된 아날로그 전압을 2^(10) = 1024 단계로 쪼개어 디지털 값으로 변환한다. 1024단계로 쪼개기 때문에 5V 기준 전압이 약 4.883mV(5V/1024)의 값이 변할 때마다 디지털 값이 1씩 증감한다.

## 0.5 [LSB](https://blog.naver.com/ansdbtls4067/220886567257) Integral Non-linearity

LSB는 Least Significant Bit의 약자이고, 디지털 비트 중 최하위 비트를 뜻한다. ADC가 전압을 측정할 때 발생하는 비선형([Non-linearity](https://blog.naver.com/bringblingme/221560547684)) 오차가 0.5 LSB 이하다. (5V기준 약 1 LSB = 4.883mV, 따라서 약 0.5LSB = 2.4415mV)

## ±2 LSB Absolute Accuracy

절대적인 정확도의 최대 오차는 +-2 LSB이다. (5V 기준 최대 +- 9.77mV가 최대 오차 범위)

## 13 - 260 µs Conversion Time

ADC 과정을 거치는 시간은 13μsec ~ 260μsec이다. (μ = 10^(-6))

> 13μs -> 클럭을 높였을 때   
> 104μs -> 125kHz 클럭 사용 시   
> 260μs -> 가장 느리게 설정할 때   
>> exception: ADC를 켜고 가장 처음에 측정할 때는 13주기가 아니라 25주기가 걸린다.


## Up to 76.9 kSPS (Up to 15 kSPS at Maximum Resolution)

kSPS: kilo Samples Per Second의 약자, 초당 몇k의 전압을 샘플링(측정)하는지

10비트 해상도를 손실 없이 측정할 수 있는 최대 속도는 초당 15000번(15kSPS)이다.

정밀도가 흔들리는 것을 감수하고 ADC 클럭을 한계까지 높이는 경우 초당 76900번(76.9kSPS)까지 가능하다.


## 8 Multiplexed Single Ended Input Channels

Single Ended: GND를 바닥에 대고 선 1개로 전압을 재는 일반적인 측정 방식

ADC 기능을 하는 엔진이 ATmega128에는 하나이지만 Multiplexer(ADMUX)를 통해 8개까지 입력받을 수 있다.



## 7 Differential Input Channels(차동 입력 채널)

차동 입력 채널: GND(=0V)가 기준이 아닌, 두 핀 사이의 전압 차이만 측정하는 방식

ADC1 핀을 (-)로 정해두고 나머지 핀을 (+)로 설정하여 총 7개의 차동 조합을 만들 수 있다.

외부에서 전자기 노이즈가 들어올 때 보통 똑같은 값의 노이즈가 들어오는데 이때 두 핀의 전압 차이를 이용하면 노이즈는 고려되지 않으므로 제거되는 효과를 볼 수 있다고 한다.


## 2 Differential Input Channels with Optional Gain of 10x and 200x

위 차동 입력 채널 중 ADC1, ADC0 조합과 ADC3, ADC2 조합 2곳에서는 신호를 10배 또는 200배로 증폭해서 읽을 수 있다.

대신 노이즈도 같이 증폭되기 떄문에 실제 정밀도는 10비트에서 8비트/7비트로 떨어진다.


## Optional Left Adjustment for ADC Result Readout

10비트 변환 결과를 8비트 상자에 나눠 담을 때 비트를 왼쪽으로 밀어서 정렬할 수 있는 옵션 존재

10비트가 아닌 8비트의 환경만 사용해도 충분한 경우 이용할 수 있다.


## 0 - VCC ADC Input Voltage Range

Input은 0~VCC(거의 5V)의 범위의 전압만 입력받을 수 있다.


## Selectable 2.56 V ADC Reference Voltage

기준 전압(Reference Voltage): ADC로 들어온 전압을 측정할 때  1023(최고점)의 기준

내부의 2.56V 전압 선택 시 1 LSB = (2.56/1024 = 2.5mV)로 정밀도가 향상된다.

단, 2.56V 선택 시 5V를 연결하면 안된다. (2.56V 회로가 망가짐)


## Free Running or Single Conversion Mode

Free Running: 시작하면 하드웨어가 알아서 무한 반복 측정한다.

Single Conversion: 개발자가 코드에서 명령을 내릴 때 1번 측정 후 정지한다.

상황에 따라 Free running과 Single Conversion 중 적절한 모드를 선택하면 된다.


## Interrupt on ADC Conversion Complete

ADC가 아날로그 값을 10비트 숫자로 변환하는 데에는 위에도 기재했지만 13μsec ~ 260μsec가 걸리는데 16MHz의 CPU 입장에서는 그동안 다른 연산을 처리할 수 있는 긴 시간이다.

따라서 그 시간동안 CPU가 대기하지 않고 다른 연산을 수행하고, ADC 연산이 끝나면 결과값을 Interrupt하여 자원을 효율적으로 관리할 수 있다.


## Sleep Mode Noise Canceler

CPU 내부의 트랜지스터가 16MHz 속도로 on/off 될 때 전원선과 칩 내부로 미세한 고주파 스위칭 노이즈가 발생한다. 하지만 ADC 변환 과정 순간에 Noise Canceler 상태에 들어가게 되면 CPU를 Sleep(정지)하여 ADC에 노이즈가 흘러가는 것을 방지해줄 수 있다.

> 노이즈 캔슬링에 도움되는 것들(?)
> + 아날로그 신호 경로는 짧게 유지하기
> + AVCC핀은 VCC 전압에 연결
> + ADC 전환 중에 ADC 핀들을 디지털 출력 핀으로 사용하지 않기


