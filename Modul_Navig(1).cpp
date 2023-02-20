#include "Modul_Navig.h"

void NavigComponent::timer_work()
{
	Header data;

	data.addr_from_subsyst = addr_component.subsyst;
	data.addr_from_NODE = addr_component.NODE;
	data.addr_from_cmp = addr_component.cmp;
	data.addr_to_subsyst = addr_manager.subsyst;
	data.addr_to_NODE = addr_manager.NODE;
	data.addr_to_cmp = addr_manager.cmp;

	data.type = NAVIG_GET;
	data.length = sizeof(SNavigGet);
	data.priority = 2;
	data.confirm = 0;

	SNavigGet navigData;
	navigData.x = x;
	navigData.y = y;
	navigData.vx = vx_prev;
	navigData.vy = vy_prev;
	navigData.ax = ax;
	navigData.ay = ay;

	Message novig;
	novig = { data, (uint8_t*)&navigData };

	uint8_t* packet = pack(novig);

	excInterface->send((char*)packet, sizeof(SNavigGet) + 9);
}

int32_t NavigComponent::run(Message* s)
{
	float vx = ((float*)s->inf)[0];
	float vy = ((float*)s->inf)[1];
	float dt = ((float*)s->inf)[2];
	x = x + vx * dt;
	y = y + vy * dt;
	ax = vx / dt - vx_prev / dt;
	ay = vy / dt - vy_prev / dt;
	vx_prev = vx;
	vy_prev = vy;

	return 0;
}

int NavigComponent::init()
{
	Header data;
		
	data.addr_from_subsyst = addr_component.subsyst;
	data.addr_from_NODE = addr_component.NODE;
	data.addr_from_cmp = addr_component.cmp;
	data.addr_to_subsyst = addr_manager.subsyst;
	data.addr_to_NODE = addr_manager.NODE;
	data.addr_to_cmp = addr_manager.cmp;

	data.type = TYPE_CMP;
	data.length = 0;
	data.priority = 1;
	data.confirm = 1;

	Message msg;
	msg = {data};

	uint8_t* packet = pack(msg);

	excInterface->open();
	excInterface->send((char*)packet, 9);

	char data_1[1024];
	int size = 0;
	
	char type;
	do
	{
		excInterface->recv(data_1, &size);
		Message msg1 = unpack((uint8_t*)data_1);
		char type = msg1.h.type;
	} while (!(type == TYPE_CMP));

	return 0;
}
