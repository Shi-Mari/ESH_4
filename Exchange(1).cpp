#include "Exchange.h"

CExchange_udp::CExchange_udp(IP_Port _info)
{
	info = _info;
}

int CExchange_udp::open()
{

	//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 2);  // используемая версия библиотеки для сокетов 2.2
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		return WSAGetLastError();
	}

	Connection = socket(AF_INET, SOCK_DGRAM, 0);

	sockaddr_in serverHint;
	serverHint.sin_addr.S_un.S_un_b.s_b1 = info.ipM[0];
	serverHint.sin_addr.S_un.S_un_b.s_b2 = info.ipM[1];
	serverHint.sin_addr.S_un.S_un_b.s_b3 = info.ipM[2];
	serverHint.sin_addr.S_un.S_un_b.s_b4 = info.ipM[3];
	serverHint.sin_family = AF_INET; // Задаём формат адреса IPv4
	serverHint.sin_port = htons(info.portM); // Задаём порт

	// Привязка сокита к порту
	if (bind(Connection, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	return 0;
}

int CExchange_udp::recv(char* data, int* size)
{
	unsigned long Sizebuffer = 0;
	// Проверка есть ли в сокете информация для чтения
	if (ioctlsocket(Connection, FIONREAD, &Sizebuffer) != 0) {
		return WSAGetLastError();
	}
	if (Sizebuffer == 0) {
		return -1;
	}

	sockaddr_in client; // Создадим переменную для инф. о клиенте
	int clientLength = sizeof(client); 

	char buf[1024];

	int bytesIn = recvfrom(Connection, buf, 1024, 0, (sockaddr*)&client, &clientLength); // Возвращает длину полученных данных
	if (bytesIn == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}

	memcpy(data, buf, bytesIn); // Передаём buf, в data чтобы получать данные вне функции
	*size = bytesIn;
	return 0;
}

int CExchange_udp::send(char* data, int size)
{
	sockaddr_in server;
	server.sin_family = AF_INET; // Задаём формат адреса IPv4
	server.sin_port = htons(info.portC); // Задаём порт
	// Задаём IP адресс
	server.sin_addr.S_un.S_un_b.s_b1 = info.ipC[0];
	server.sin_addr.S_un.S_un_b.s_b2 = info.ipC[1];
	server.sin_addr.S_un.S_un_b.s_b3 = info.ipC[2];
	server.sin_addr.S_un.S_un_b.s_b4 = info.ipC[3];
	int sendOk = sendto(Connection, data, size, 0, (sockaddr*)&server, sizeof(server)); // Отправляем сообщение
	if (sendOk != 0) {
		return WSAGetLastError();
	}
	return 0;
}

int CExchange_udp::close()
{
	// Закрываем сокет
	if (closesocket(Connection) != 0) {
		return WSAGetLastError();
	};
	// Закрываем библиотеку winsock
	if (WSACleanup() != 0) {
		return WSAGetLastError();
	};
	return 0;
}