#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
using namespace std;

#pragma warning(disable: 4996)

struct IP_Port
{
	int ipM[4];
	int portM;
	int ipC[4];
	int portC;
};

class CExchange {
public:
	virtual int open() = 0;
	virtual int recv(char* data, int* size) = 0;
	virtual int send(char* data, int size) = 0;
	virtual int close() = 0;
};

class CExchange_udp : public CExchange {
public:
	CExchange_udp(IP_Port _info);

	virtual int open() override;
	virtual int recv(char* data, int* size) override;
	virtual int send(char* data, int size) override;
	virtual int close() override;

	SOCKET Connection;
	IP_Port info;
};
