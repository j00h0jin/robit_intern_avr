# Day03_Task03

> **광운대학교 로봇학부**  
> **작성자:** 주호진  
> **제출일:** 2026.08.02

---

## 1. 개요 (Overview)

본 과제는 ATmega128 마이크로컨트롤러를 활용하여 주요 주변장치(Peripherals)를 제어하고 센서 데이터를 수신/처리하는 시스템을 구현하는 것을 목표로 함.

### 핵심 목표

- ATmega128 레지스터 설정을 통한 주변장치 제어
- 센서 및 외부 모듈과의 통신 (USART) 및 데이터 처리
- mx485를 통한 다이나믹셀 제어(다이나믹셀 프로토콜 2.0 사용)

---

## 2. 개발 환경 (Environment)

| 항목               | 내용                                            |
| :----------------- | :---------------------------------------------- |
| **MCU**            | ATmega128A (16MHz External Crystal)             |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC        |
| **Flasher Tool**   | USBISP / STK500                                 |
| **언어**           | C Language                                      |
| **주요 부품**      | ATmega128 개발보드, LCD, Potentiometer, mx-64, mx485   |

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                              [Target Component]
 PE0 (RXD0) / PE1                ----->   UART Serial Communication with mx485
 PORTD Pin 0, 1 (PIND0, PIND1)   ----->   LCD(I2C)
 PORTF PF0                       ----->   Potentiometer
[mx485]
 Pin 6, 7                        ----->   Dynamixel
```

### 주요 회로 특징

- **전원:** 5V DC 안정화 전원 공급
- **주의사항:** ISP 다운로드 시 SPI 핀 타겟 전원 및 리셋 회로 간섭 주의, dynamixel의 ID와 baud rate 확인

---

## 4. 프로젝트 구조 (Directory Structure)

> 구현부(.c), 선언부(.h)만 구조에 표기함.

```text
├── Day03_Task03/
│   ├──  main.c # 메인 제어 루프 및 시스템 초기화
│   └──  LCD_Text.c # LCD 제어 함수 파일
├── include/
│   └── LCD_Text.h # LCD 제어 헤더 파일
└── README.md
```

---

## 5. 핵심 코드 및 레지스터 설정 (Key Implementation)

### 다이나믹셀 Instruction Packet 예시 (`main.c`)

```c
// data 길이는 가변이기 때문에 포인터 값으로 받기
void dxl_write_data(unsigned short address, unsigned char *p_data, unsigned short data_len)
{
    unsigned char packet[32]; // 패킷 배열

    // length (=Instruction(1), Parameter(2+data_len), CRC(2) 필드의 Byte Size를 나타내는 필드)
    unsigned short length = data_len + 5;                 // inst(1) + (address(2) + Data_Len) + CRC(2)
    unsigned short packet_len_before_crc = length + 5;    // header(4) + id(1) + length
    unsigned short total_len = packet_len_before_crc + 2; // + crc 2바이트

    // 패킷 헤더
    // https://emanual.robotis.com/docs/kr/dxl/protocol2/
    // 2. Instruction Packet 참조
    packet[0] = 0xFF;
    packet[1] = 0xFF;
    packet[2] = 0xFD;
    packet[3] = 0x00; // [0] ~ [3] Protocol 2.0 Header
    packet[4] = 0x01; // ID = 1
    packet[5] = (unsigned char)(length & 0xFF);
    packet[6] = (unsigned char)((length >> 8) & 0xFF); // [5] ~ [6] length
    packet[7] = 0x03;                                  // Write 명령 0x03
    packet[8] = (unsigned char)(address & 0xFF);
    packet[9] = (unsigned char)((address >> 8) & 0xFF); // [8] ~ [9] address

    // 데이터(param의 주솟값 이후) 넣기
    for (unsigned short i = 0; i < data_len; i++)
    {
        packet[10 + i] = p_data[i];
    }

    // crc 넣기
    unsigned short crc = update_crc(0, packet, packet_len_before_crc);
    packet[packet_len_before_crc] = (unsigned char)(crc & 0xFF);            // crc 하위비트
    packet[packet_len_before_crc + 1] = (unsigned char)((crc >> 8) & 0xFF); // crc 상위비트

    // MAX485 송신 모드
    PORTE |= (1 << PE2); // PE2 = HIGH(=송신 모드)
    _delay_ms(20);

    // UART로 transmit
    for (unsigned short i = 0; i < total_len; i++)
    {
        Uart_Putch(packet[i]);
    }

    while (!(UCSR0A & (1 << TXC0)))
        ; // 송신 데이터가 모두 전송되고 UDRE0의 송신 버퍼에 새로운 송신 데이터가 write되지 않은 상태

    UCSR0A |= (1 << TXC0); // TXC0 플래그 클리어

    _delay_ms(100);

    // MAX485 수신 모드
    PORTE &= ~(1 << PE2); // PE2 = LOW(=수신 모드)
}

// https://docs.robotis.com/ko/docs/dxl/model_reference/mx_series/mx-64-2/ 에서 원하는 동작과 data값 확인 가능

```

---

## 6. 동작 설명 및 결과 (Results)

### 동작 시나리오

가변저항 값에 따라 Dynamixel 목표 위치 설정 (0~1023)

PC에서 전송 받은 값(0~9)에 따라서 Dynamixel 목표 속도 설정

(*Hint. PC에서 받은 0 - 9값이 0 - 300 값에 대응하도록 목표속도 설정)

Dynamixel 목표 속도 (첫 번째 줄)와 목표 위치 (두 번째 줄) LCD에 표시





### 동작 사진 / 영상

|                 정면 동작 모습                 |  
| :--------------------------------------------: | 
| [동작 영상](https://drive.google.com/file/d/1koG8_DheqYDZZBQfv49opYXSWe0qBL8m/view?usp=drive_link) |

---

## 7. AI 툴 활용 명시 (AI Tools Declaration)

본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)의 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool)        |     활용 영역              | 세부 사용 목적 및 내용        |
| :--------------- | :--------------------------- | --------------------- |
| Gemini, ChatGPT |  개념 정리, 디버깅  | 다이나믹셀 프로토콜 2.0에 대해 쉽게 설명, CRC가 뭔지?, CRC 쓰는 이유, 패킷 내용을 쉽게 설명, 파라미터 내의 주소값 순서, mx-64를 동작하기 위한 자료, 패킷 내 length 쉽게 설명, 패킷 파라미터 길이가 다른 경우 해결법, Operating Mode 디폴트 값이 3인데 따로 선언을 해줘야 하는지, 토크가 걸렸는지 안걸렸는지 확인하는 방법, 다이나믹셀 토크 안 걸리는 이유, 패킷이 정상적으로 receive되는데 다이나믹셀이 작동하지 않는 이유, 시리얼에 뜨는데 이게 pc로만 오고 있어서 작동을 안 하는 건지?, mx485가 죽었는지 확인하는 방법, 회로상 결함이 없어보이는데 혹시 추가로 확인할 수 있는 방법, 작동이 안되는 경우의 수 전부 제시 후 소 등...  |

### AI 활용 및 검증 원칙

1. **코드 검증:** AI가 생성한 레지스터 설정 및 함수 코드는 데이터시트(ATmega128 Datasheet)와 비교 검증한 후 실제 오실로스코프/시리얼 모니터링을 거쳐 직접 수정 및 테스트하였습니다.
2. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구(디버깅, 문서화)로만 활용하였습니다.
