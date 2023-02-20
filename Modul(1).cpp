#include "Modul.h"
#include <stdint.h>
#include <chrono>

bool BaseComponent::is_update_time(const long int delay_millisec)
{
	using namespace std::chrono;

	system_clock::time_point ts = system_clock::now();
	long long millis = duration_cast<milliseconds>(ts.time_since_epoch()).count();

	if (millis >= update_time_millis) {

		update_time_millis += delay_millisec;
		return TRUE;
	}
	return FALSE;
}

BaseComponent::BaseComponent(SAddress _addr_component, SAddress _addr_manager, CExchange* intf_manager, FILE* _log_file, const int32_t _update_time_millis)
{
	addr_component = _addr_component;
	addr_manager = _addr_manager;
	excInterface = intf_manager;
	log_file = _log_file;
	update_time_millis = _update_time_millis;
}

int32_t BaseComponent::start()
{
		char buff[1024];
		int size = 0;

		if (excInterface->recv(buff, &size) == 0)
		{
			Message s = unpack((uint8_t*)buff);
		}
		int res = this->run(s);
		if (s)
			delete s;
		return res;
}
