# Day03 Task01

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.07.31

---

# I2C 통신의 SCL, SDA에 대한 정보

시리얼 통신 중 하나. 동기적 통신이며, 속도가 빠르지는 않지만 2개의 선으로 통신이 가능하다.

2개의 선으로 통신하기 때문에 TWI(Two Wire Interface)라고도 부른다.


두개의 선은 SDA, SCL인데 SDA는 데이터를 주고 받는 선이고, SCL은 클럭 정보를 제공하는 선이다. 데이터선이 SDA 하나이므로 반이중(half duplex)통신을 할 수밖에 없다.

(반이중통신: 양방향 통신이 가능하지만, 두 장치가 동시에 데이터를 주고받을 수 없고, 번갈아 가며 한쪽씩만 송신하는 통신 방식(ex. 무전기)

모든 디바이스가 SDA, SCL선을 공유, 풀업 저항으로 평소에는 1 상태로 유지되도록 한다.
<img width="781" height="286" alt="image" src="https://github.com/user-attachments/assets/b4b237db-2f26-4900-b803-e80000bbd3d6" />

 
SDA의 데이터는 SCL이 High(1)일 때는 Data가 stable한 상태를 띤다. low상태에서만 SDA의 신호가 바뀐다고 한다.
그러나 START, RE START, STOP 신호에 대해서는 SCL이 high인 상태에서 SDA의 상태를 변경하여 통신의 시작과 끝을 알려주도록 되어있다.

SCL이 high인 상태에서 SDA가 low로 바뀌게 되면 통신이 개시된다는 것을 slave로 알려주게 된다.
반대로 SCL이 high인 상태에서 SDA가 high가 되면 통신이 끝났음을 slave로 알린다. I2C 통신은 START와 STOP신호가 쌍으로 제공되어야 하는데 때로는 STOP을 생략하고 START 신호를 줄 필요가 있다고 한다. 이때 REPEATED START 신호를 사용한다.

I2C에서는 slave마다 가지고 있는 고유의 주소를 이용하여 특정 slave를 선택할 수 있다.
따라서 동일한 I2C버스에 중복된 주소를 가지고 있는 slave가 있으면 안된다. 주소는 7비트 값이다.

I2C통신을 시작하기 위해서 START 신호를 보내고 그 다음 어떤 slave와 통신할 것인지 알려주기 위하여 주소를 보낸다(7비트). I2C통신은 항상 MSB(Most Significant Bit; 최상위 비트를 뜻한다. LSB와 반대)부터 내보내도록 되어있다.
master에서 slave 주소를 내보낼 때 읽기 동작을 할건지, 쓰기 동작을 할것인지 미리 알려주도록 되어있다. 읽기동작은 1을 내보내고 쓰기 동작은 0을 내보내도록 되어있다.

예를 들어서 0x52라는 주소값을 가지는 slave로 읽기동작을 하기 위해서는 0xA5의 데이터가 SDA선으로 나가게 되고 쓰기 동작을 하려면 0xA4를 내보내어야 한다.

0x52의 7비트 값: 101 0010

0xA5의 비트값(8비트) 1010 0101 (101 0010 1)

0xA4의 비트값(8비트) 1010 0100 (101 0010 0)	

선택된 slave에서 주소를 제대로 인식했으면 다음 SCL 클럭에서 Ack 신호를 보내주어야 한다.
제대로 신호를 수신했다는 것을 알려주기 위해서 SDA 신호를 low로 만들어준다.
만약 master에서 주소값을 보낸 후 Ack 신호를 검출하였을 때 그 값이 0이 아니면 slave에서 주소를 제대로 받지 못했다고 판단하게 된다.

<img width="781" height="286" alt="image" src="https://github.com/user-attachments/assets/240f6c68-7cd2-40b7-87a2-c9d55e6c49d6" />

> 해당 사진의 Device Address 부분에서 만약 ACK 신호가 1로 검출되었다면 master는 해당 device가 주소를 전달받지 못했다고 판단)



<img width="781" height="286" alt="image" src="https://github.com/user-attachments/assets/ca9d0d4c-76b6-4e45-9631-3ee365c1be46" />


 
위의 그림은 데이터 통신에 있어서 신호의 변화를 설명한 그림이다.

두번째 줄에 있는 SDA from Transmitter은 데이터를 내보내는 쪽에서의 상태변화를 보여준다.
8비트 데이터를 모두 보낸 다음에는 반드시 SDA 신호를 high상태로 만든다.

3번째의 receiver은 수신쪽에서의 상태 변화를 보여주는 것으로 8비트 데이터를 모두 받으면 SDA신호를 low로 만들어 Ack 신호를 알려준다.
수신측에서 9번째 비트 위치에서 SDA신호를 high로 만들면 더 이상 수신을 하지 않겠다는 의미로 사용된다.

<img width="781" height="286" alt="image" src="https://github.com/user-attachments/assets/2e62ec07-0170-4a10-8225-ff43817851a8" />


 
위 그림은 AVR에 있는 TWI 블록의 구성도이다. 굵은 선의 박스(TWDR, TWBR, TWSR, TWCR, TWAR)로 표시되어 있는 것이 CPU에서 제거 가능한 TWI 레지스터이다.

TWDR = data shift register 읽기/쓰기를 위한 데이터 처리

TWBR = bit rate register TWI의 통신 속도를 조절
> (일반적으로 I2C의 속도는 100kHz를 저속, 400kHz를 고속을 정의한다고 함)  
> (최저 속도가 정해져있지는 않으나 보편적으로 100kHz를 쓰고 이보다 느릴 경우 신호 안정성은 올라가지만 전송 시간이 늘어난다.  
> <img width="907" height="161" alt="image" src="https://github.com/user-attachments/assets/a3dc1bc7-413b-46ec-a936-d126f817f9e4" />


 
TWSR = Status register twi의 제어를 위해 사용

TWCR = control register twi의 제어를 위해 사용

TWAR = address Register slave의 주소값을 써놓은 곳

---

## ATmega128에서의 TWI(I2C) (ATmega128 Datasheet pp.198-229)

• Both Master and Slave Operation Supported

아트메가128이 마스터 또는 다른 마스터의 slave도 할 수 있음

• Device can Operate as Transmitter or Receiver

아트메가128이 transmitter나 receiver 모드를 자율적으로 전환할 수 있다.
(ex. 128이 slave인 경우 master가 준 신호를 receive할 수도 있고, master에게 어떠한 신호 값을 요청하는 신호를 줄 수(transmit)도 있다

• 7-bit Address Space allows up to 128 Different Slave Addresses

7비트 주소 체계를 사용하므로 하나의 버스 라인에 최대 128개(000 0001 ~ 111 1111)의 slave를 둘 수 있다

• Multi-master Arbitration Support

Master가 다수일 때 동시에 통신을 시도할 경우 충돌을 방지하고 우선순위를 정리해주는 Arbitration 기능을 지원
> (버스 안에 master 역할을 할 수 있는 장치가 2개 이상인 경우에서 master 신호를 동시에 2개 이상의 장치에서 보냈을 경우 Arbitration 기능이 하나의 master를 지정하고 나머지 장치는 slave가 되도록 함)

MSB부터 신호를 비교해서 1을 먼저 보낸 master가 slave로 내려가게 된다.(그 이유는 오픈 드레인 구조에서 나옴)

두 master의 신호가 처음부터 끝까지 완전히 동일한 경우는 동일한 명령인 경우이니 상관 없음)


• Up to 400 kHz Data Transfer Speed

400kHz까지 지원

• Slew-rate Limited Output Drivers

출력 신호의 전압 변화율을 제한하여 신호 반사나, 급격한 전압 변화로 인한 고주파 노이즈를 줄여준다.


• Noise Suppression Circuitry Rejects Spikes on Bus Lines

버스 라인에 Spikes를 걸러내는 노이즈 제거 회로(Noise Suppression Circuitry)가 있다.

• Fully Programmable Slave Address with General Call Support

소프트웨어로 슬레이브 주소를 자유롭게 설정할 수 있으며, 버스 상의 모든 슬레이브 장치에 동일한 명령을 담은 메시지를 보내는 제너럴 콜(General Call) 기능을 지원한다.

• Address Recognition Causes Wake-up when AVR is in Sleep Mode

AVR(MCU)이 전력 절약을 위해 슬립(Sleep) 모드에 들어가 있어도, TWI 버스에서 자신의 슬레이브 주소가 호출되면 이를 자동으로 감지하여 MCU를 깨우는(Wake-up) 기능이 있다. (전력 관리 용이)

> C example
> <img width="940" height="1019" alt="image" src="https://github.com/user-attachments/assets/1d52fa5d-1cc5-4a9a-a186-91291871f58e" />

> atmega128의 TWCR  
> <img width="604" height="633" alt="image" src="https://github.com/user-attachments/assets/4b29935d-fbca-4eaa-a234-07ee5a0e1751" />

---

# UART 통신에서 사용되는 232 통신과 485 통신에 대한 장단점과 차이




