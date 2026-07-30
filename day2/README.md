# Day02 Task01

#### 데이터 시트 ADC 파트 번역하여 공부(pp.230-245)

---


##### ATmega128 ADC 목차

<img width="517" height="287" alt="image" src="https://github.com/user-attachments/assets/d87d8d35-f0bf-42b6-b137-309319955617" />

---

##### 10-bit Resolution(10비트 해상도)

ATmega128은 기본적으로 10비트 해상도를 지원한다. 입력된 아날로그 전압을 2^(10) = 1024 단계로 쪼개어 디지털 값으로 변환한다. 1024단계로 쪼개기 때문에 5V 기준 전압이 약 4.883mV(5V/1024)의 값이 변할 때마다 디지털 값이 1씩 증감한다.

##### 0.5 [LSB](https://blog.naver.com/ansdbtls4067/220886567257) Integral Non-linearity

LSB는 Least Significant Bit의 약자이고, 디지털 비트 중 최하위 비트를 뜻한다. ADC가 전압을 측정할 때 발생하는 비선형([Non-linearity](https://blog.naver.com/bringblingme/221560547684)) 오차가 0.5 LSB 이하다. (5V기준 약 1 LSB = 4.883mV, 따라서 약 0.5LSB = 2.4415mV)

##### ±2 LSB Absolute Accuracy

절대적인 정확도의 최대 오차는 +-2 LSB이다. (5V 기준 최대 +- 9.77mV가 최대 오차 범위)

##### 13 - 260 µs Conversion Time

ADC 과정을 거치는 시간은 13마이크로sec ~ 260마이크로sec이다. (마이크로 = 10^(-6))

##### Up to 76.9 kSPS (Up to 15 kSPS at Maximum Resolution)



##### 8 Multiplexed Single Ended Input Channels



##### 7 Differential Input Channels



##### 2 Differential Input Channels with Optional Gain of 10x and 200x



##### Optional Left Adjustment for ADC Result Readout



##### 0 - VCC ADC Input Voltage Range



##### Selectable 2.56 V ADC Reference Voltage



##### Free Running or Single Conversion Mode



##### Interrupt on ADC Conversion Complete



##### Sleep Mode Noise Canceler


