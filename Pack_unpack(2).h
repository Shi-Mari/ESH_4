#include <iostream>
#include <stdio.h>
#include <stdint.h>
using namespace std;

struct Header {
	uint8_t addr_from_subsyst;
	uint8_t addr_from_NODE;
	uint8_t addr_from_cmp;
	uint8_t addr_to_subsyst;
	uint8_t addr_to_NODE;
	uint8_t addr_to_cmp;
	uint8_t type;
	uint16_t length;
	uint8_t priority;
	uint8_t confirm;
	uint16_t crc;
};

struct Message {
	Header h;
	uint8_t* inf;
};

// Генератор контрольной суммы
uint16_t GetCrc16(uint8_t* inf, uint32_t size)
{
	uint16_t crc = 0x0;

	if (inf)
	{
		while (size--)
		{
			crc ^= *inf++ << 8;

			for (uint32_t i = 0; i < 8; ++i)
			{
				crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
			}
		}
	}

	return crc;
}

// Запаковка заголовка и сообщения
uint8_t* pack(Message msg) {

	uint8_t* heading = new uint8_t[9 + msg.h.length];

	msg.h.crc = GetCrc16(msg.inf, msg.h.length);

	heading[0] = 42;
	heading[1] = (msg.h.addr_from_subsyst & 0b00001111) | ((msg.h.addr_from_NODE & 0b00001111) << 4);
	heading[2] = (msg.h.addr_from_cmp & 0b00001111) | ((msg.h.addr_to_subsyst & 0b00001111) << 4);
	heading[3] = (msg.h.addr_to_NODE & 0b00001111) | ((msg.h.addr_to_cmp & 0b00001111) << 4);

	heading[4] = (msg.h.type & 0b00111111) | ((msg.h.length & 0b0000000011) << 6);
	heading[5] = ((msg.h.length & 0b1111111100) >> 2);
	heading[6] = (msg.h.priority & 0b00000011) | ((msg.h.confirm & 0b00000011) << 2) | ((msg.h.crc & 0b00001111) << 4);
	heading[7] = (msg.h.crc & 0b0000111111110000) >> 4;
	heading[8] = (msg.h.crc & 0b1111000000000000) >> 12;

	memcpy(heading + 9, msg.inf, msg.h.length);

	return heading;

}

// Распаковка заголовка и сообщения
Message unpack(uint8_t* heading) {

	Message h_unpack{};

	if (heading[0] == 42) {

		h_unpack.h.addr_from_subsyst = heading[1] & 0b00001111;
		h_unpack.h.addr_from_NODE = (heading[1] & 0b11110000) >> 4;
		h_unpack.h.addr_from_cmp = heading[2] & 0b00001111;
		h_unpack.h.addr_to_subsyst = (heading[2] & 0b11110000) >> 4;
		h_unpack.h.addr_to_NODE = heading[3] & 0b00001111;
		h_unpack.h.addr_to_cmp = (heading[3] & 0b11110000) >> 4;

		h_unpack.h.type = heading[4] & 0b00111111;
		h_unpack.h.length = (uint16_t(heading[4] & 0b11000000) >> 6) | (uint16_t(heading[5] & 0b11111111) << 2);
		h_unpack.h.priority = heading[6] & 0b00000011;
		h_unpack.h.confirm = (heading[6] & 0b00001100) >> 2;
		h_unpack.h.crc = (uint16_t(heading[6] & 0b11110000) >> 4) | (uint16_t(heading[7] & 0b11111111) << 4) | (uint16_t(heading[8] & 0b00001111) << 12);

		h_unpack.inf = new uint8_t[h_unpack.h.length];
		memcpy(h_unpack.inf, heading + 9, h_unpack.h.length);

		printf("OK\n");
	}
	else {
		printf("ERROR\n");

	}

	return h_unpack;

}

/*
int main() {

	Header h_pack;

	h_pack.addr_from_subsyst = 1;
	h_pack.addr_from_NODE = 2;
	h_pack.addr_from_cmp = 3;
	h_pack.addr_to_subsyst = 4;
	h_pack.addr_to_NODE = 5;
	h_pack.addr_to_cmp = 6;

	h_pack.type = 7;
	h_pack.length = 4;
	h_pack.priority = 1;
	h_pack.confirm = 2;

	Message msg;
	float velocity = 2;
	msg = { h_pack, (uint8_t*)&velocity };

	uint8_t* packet = pack(msg);


	Message msg1 = unpack(packet);

	delete[] packet;

	printf
	(
		"addr_from_subsyst = %d\n"
		"addr_from_NODE = %d\n"
		"addr_from_cmp = %d\n"
		"addr_to_subsyst = %d\n"
		"addr_to_NODE = %d\n"
		"addr_to_cmp = %d\n"
		"type = %d\n"
		"length = %d\n"
		"priority = %d\n"
		"confirm = %d\n"
		"crc = %d\n"
		"velocity = %f\n",

		msg1.h.addr_from_subsyst,
		msg1.h.addr_from_NODE,
		msg1.h.addr_from_cmp,
		msg1.h.addr_to_subsyst,
		msg1.h.addr_to_NODE,
		msg1.h.addr_to_cmp,
		msg1.h.type,
		msg1.h.length,
		msg1.h.priority,
		msg1.h.confirm,
		msg1.h.crc,
		*(float*)msg1.inf
	);

	system("pause");
}*/
