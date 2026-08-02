/*
 * Day03_Task03.c
 *
 * Created: 2026-07-31 오전 11:11:55
 * Author : hojin
 */ 

#define F_CPU 16000000
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "include/LCD_Text.h"

static const unsigned short crc_table[256] = {
	0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
	0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
	0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
	0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
	0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
	0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
	0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
	0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
	0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
	0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
	0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
	0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
	0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
	0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
	0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
	0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
	0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
	0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
	0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
	0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
	0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
	0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
	0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
	0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
	0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
	0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
	0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
	0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
	0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
	0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
	0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
	0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
};

unsigned char Uart_getch(void);
void Uart_Putch(unsigned char PutData);
int Read_ADC();
unsigned short update_crc(unsigned short crc_accum, unsigned char *data_blk_ptr, unsigned short data_blk_size);
void dxl_write_data(unsigned short address, unsigned char *p_data, unsigned short data_len);

int main(void)
{
	UBRR0L = 16; // 57600
	UBRR0H = 0;
	UCSR0A = 0x20;
	UCSR0B = 0x18;
	UCSR0C = 0x06; // data 8bit
	
	DDRE = 0x06; // E0 입력, E1 출력, E2 출력 0000 0110
	
	ADMUX = 0x40; // 0100 0000 외부의 AVCC 전압 사용
	ADCSRA = 0x87; // 1000 0111 ADC 활성화, 클럭 분주비 128

	SREG = 0x80; // 전역 인터럽트 허용
	
	unsigned int op_mode = 3; // position control mode
	unsigned int torque_on = 1; // 1 = on
	unsigned int targetLocation = 0; // 0 ~ 4095
	unsigned int prevLocation = 0;
	unsigned int targetSpeed = 150; // 목표 속도: 0~32767
	
	int potentiometerValue = 0; // 가변저항 값 : 0~1023
	
	lcdInit();
	lcdClear();
	
	dxl_write_data(11, (unsigned char *)&op_mode, sizeof(op_mode)); // 11 = operating mode
	dxl_write_data(64, (unsigned char *)&torque_on, sizeof(torque_on)); // 64 = torque enable
	_delay_ms(20);
	
	while (1)
	{
		if(UCSR0A & (1<<RXC0)) // UDR0 = 1 => 수신 버퍼에 아직 안 읽은 데이터가 존재하는 상태 
		{
			int recvData = UDR0; // 값 받기
			for (int i = 48; i < 58; i++) // 0 ~ 9인 경우
			{
				if (recvData == i)
				{
					targetSpeed = (recvData - 47) * 30; // target speed 설정, 0~9, 0~300(목표 속도)
					dxl_write_data(112, (unsigned char *)&targetSpeed, sizeof(targetSpeed)); // 112 = profile velocity
					break;
				}
			}
		}
		potentiometerValue = Read_ADC(); // 가변저항 값
		targetLocation = (unsigned int)(potentiometerValue) * 4; // 0~1023, 0~4095
		if (abs((int)targetLocation - (int)prevLocation) > 8)
		{
			dxl_write_data(116, (unsigned char *)&targetLocation, sizeof(targetLocation)); // 116 = goal position
			prevLocation = targetLocation;
		}
		
		lcdString(0, 0, "vel: ");
		lcdString(0, 8, "    ");
		lcdNumber(0, 8, (int)targetSpeed);
		lcdString(0, 8, "    ");
		lcdString(1, 0, "pos: ");
		lcdNumber(1, 8, (int)targetLocation);
		
		_delay_ms(20);
	}
}

unsigned char Uart_getch(void)
{
	while(!(UCSR0A & (1<< RXC0)));
	
	return UDR0;
	
}

void Uart_Putch(unsigned char PutData)
{
	while (!(UCSR0A & (1<< UDRE0)));

	UDR0 = PutData;

}

int Read_ADC() {
	ADCSRA |= (1 << ADSC); // 변환 시작 ADSC = 1
	return ADC;
}

// https://emanual.robotis.com/docs/kr/dxl/crc/ 에서 가져옴
// crc 계산 예제
// CRC = update_crc(0, TxPacket , 12);  // 12 = 5 + Packet Length(7)
// CRC_L = (CRC & 0x00FF);              // 0x00FF & 연산 후 하위비트만 남게 됨
// CRC_H = (CRC>>8) & 0x00FF;           // 비트를 밀고 연산하면 상위비트만 남게 됨
// crc accum: '0'으로 설정
unsigned short update_crc(unsigned short crc_accum, unsigned char *data_blk_ptr, unsigned short data_blk_size)
{
	unsigned short i, j;

	for(j = 0; j < data_blk_size; j++)
	{
		i = ((unsigned short)(crc_accum >> 8) ^ data_blk_ptr[j]) & 0xFF;
		crc_accum = (crc_accum << 8) ^ crc_table[i];
	}

	return crc_accum;
}

// data 길이는 가변이기 때문에 포인터 값으로 받기
void dxl_write_data(unsigned short address, unsigned char *p_data, unsigned short data_len) {
	unsigned char packet[32]; // 패킷 배열
	
	// length (=Instruction(1), Parameter(2+data_len), CRC(2) 필드의 Byte Size를 나타내는 필드)
	unsigned short length = data_len + 5; // inst(1) + (address(2) + Data_Len) + CRC(2) 
	unsigned short packet_len_before_crc = length + 5; // header(4) + id(1) + length
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
	packet[7] = 0x03; // Write 명령 0x03
	packet[8] = (unsigned char)(address & 0xFF);
	packet[9] = (unsigned char)((address >> 8) & 0xFF); // [8] ~ [9] address

	// 데이터(param의 주솟값 이후) 넣기
	for (unsigned short i = 0; i < data_len; i++) {
		packet[10 + i] = p_data[i];
	}

	// crc 넣기
	unsigned short crc = update_crc(0, packet, packet_len_before_crc);
	packet[packet_len_before_crc]     = (unsigned char)(crc & 0xFF);         // crc 하위비트
	packet[packet_len_before_crc + 1] = (unsigned char)((crc >> 8) & 0xFF);  // crc 상위비트

	// MAX485 송신 모드
	PORTE |= (1 << PE2); // PE2 = HIGH(=송신 모드)

	// UART로 transmit
	for (unsigned short i = 0; i < total_len; i++) {
		Uart_Putch(packet[i]);
	}

	while (!(UCSR0A & (1 << TXC0))); // 송신 데이터가 모두 전송되고 UDRE0의 송신 버퍼에 새로운 송신 데이터가 write되지 않은 상태
	
	UCSR0A |= (1 << TXC0); // TXC0 플래그 클리어

	// MAX485 수신 모드
	PORTE &= ~(1 << PE2); // PE2 = LOW(=수신 모드)
}
