#pragma once

#include "Exchange.h"
#include "Structs.h"
#include <stdio.h>
#include "Pack_unpack.h"

class BaseComponent
{
protected:
	int32_t update_time_millis;
	SAddress addr_component;
	SAddress addr_manager;

	CExchange* excInterface;

	FILE* log_file;
	bool need_update_loop_info;

	bool is_update_time(const long int delay_millisec = 500);

	virtual void timer_work() {};
	virtual int32_t run(Message* s) = 0;

	inline void close_log_file() { fclose(log_file); }

public:
	BaseComponent(SAddress _addr_component, SAddress _addr_manager, CExchange* intf_manager, FILE* log_file, const int32_t _update_time_millis = 50);

	virtual ~BaseComponent() {};

	virtual int init() = 0;
	int32_t start();
};