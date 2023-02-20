#include "Pack_unpack.h"
#include "Exchange.h"
#include "Structs.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <ctime>
#include <chrono>
#include <sstream>

using namespace std;

class Map {
	struct Pair {
		char addr[3];
		CExchange* comp;
	};

public:
	void Add(char addr[3], CExchange* comp)
	{
		if (count < 15)
		{
			map[count].addr[0] = addr[0];
			map[count].addr[1] = addr[1];
			map[count].addr[2] = addr[2];
			map[count].comp = comp;
			count++;
		}
	}

	CExchange* Find(char addr[3])
	{
		for (int i = 0; i < count; i++)
		{
			if (map[i].addr[1] == addr[1] & map[i].addr[2] == addr[2] & map[i].addr[3] == addr[3])
			{
				return map[count].comp;
			}
		}
		return 0;
	}

	void Clear()
	{
		for (int i = 0; i < count; i++)
		{
			delete map[count].comp;
		}

		count = 0;
	}


	Pair map[15];
	unsigned int count = 0;
};

class Meneger {
private:

	// Функция для разделения строки
	vector<string> split(string input, string delimeter) {

		regex re(delimeter);
		sregex_token_iterator first{ input.begin(), input.end(), re, -1 };
		sregex_token_iterator last;

		vector<string> vec(first, last);

		return vec;
	}

	Map map;

	int connections[15][10];
	int numb_connections = 0;

public:
	void Read_Config(string config) // Функция для считывания из файла конфигурации
	{
		string adr_file = config;

		ifstream file;
		file.open(adr_file);
		if (!file.is_open()) // Проверка открыт ли фаил
		{
			cout << "Error of opening file!" << endl;
		}
		else {
			cout << "file is open!" << endl;
		}

		string str;
		bool bTypeFound = false;

		while (!file.eof())
		{
			getline(file, str);
			if (str.find(',') != string::npos)
			{
				if (!bTypeFound)  // Если udp не найдена переходим к следующей строке
				{
					continue;
				}

				vector<string> vec = split(str, ","); // разделение строки на номер IP и номер порта

				vector<string> ipMngr = split(vec[0], "\\."); // разделение IP по точкам
				int ipM[4];
				if (ipMngr.size() != 4)  // проверка правильно ли введён IP
				{
					cout << "Error: invalid IP address" << endl;
					continue;
				}

				bool bSuccess = true;
				for (int i = 0; i <= 3; i++)
				{
					ipM[i] = atoi(ipMngr[i].c_str()); // Конвертирование из фармата string в int
					if (ipM[i] < 0 || ipM[i] > 255) // проверка правильно ли введён IP
					{
						cout << "Error: invalid IP address" << endl;
						bSuccess = false;
						break;
					}
				}

				if (!bSuccess) // Если IP неверный переходим к следующей строке
				{
					continue;
				}

				int portMngr = atoi(vec[1].c_str()); // Конвертирование из фармата string в int
				if (portMngr < 0 || portMngr > 65535) // проверка правильно ли введён порт
				{
					cout << "Error: invalid port" << endl;
					continue;
				}

				for (int i = 0; i <= 3; ++i) {
					connections[numb_connections][i] = ipM[i];  // Запись в масив IP и порта
				}

				connections[numb_connections][4] = portMngr;

				vector<string> ipCmpnt = split(vec[2], "\\."); 
				int ipC[4];
				if (ipCmpnt.size() != 4)
				{
					cout << "Error: invalid IP address" << endl;
					continue;
				}

				bSuccess = true;
				for (int i = 0; i <= 3; i++)
				{
					ipC[i] = atoi(ipCmpnt[i].c_str());
					if (ipC[i] < 0 || ipC[i] > 255)
					{
						cout << "Error: invalid IP address" << endl;
						bSuccess = false;
						break;
					}
				}

				if (!bSuccess)
				{
					continue;
				}

				int portCmpnt = atoi(vec[3].c_str());
				if (portCmpnt < 0 || portCmpnt  > 65535)
				{
					cout << "Error: invalid port" << endl;
					continue;
				}

				for (int i = 5; i <= 8; ++i) {
					connections[numb_connections][i] = ipC[i];
				}

				connections[numb_connections][9] = portCmpnt;

				numb_connections++; // счётчик строк
			}
			else
			{
				if (str != "udp")
				{
					cout << "Error: invalid protocol" << endl;

					bTypeFound = false; // Если udp не найден переходим к следующей строчке
					continue;
				}

				bTypeFound = true;
			}
		}
		file.close();

	}

	// Функция открытия соединения с компонентами
	void Open_All_Connections()
	{
		for (int i = 0; i <= numb_connections; i++)
		{
			IP_Port info;
			memcpy(&info, &connections[i], sizeof(info)); // записываем в структуру информацию о соединении

			CExchange* comp = new CExchange_udp(info);
			comp->open();

			char buff[1024];
			int size = 0;

			while (true)
			{
				if (comp->recv(buff, &size) == 0)
				{
					Message msg1 = unpack((uint8_t*)buff); // Распаковка
					if (msg1.h.type == TYPE_CMP)
					{
						char addr[3];
						addr[0] == msg1.h.addr_from_subsyst;  // запись адреса отправителя
						addr[1] == msg1.h.addr_from_NODE;
						addr[2] == msg1.h.addr_from_cmp;

						map.Add(addr, comp); //добавление адреса в словарь

						msg1.h.confirm = 1;
						uint8_t* data = pack(msg1);
						comp->send((char*)data, size);
						break;
					}
				}
				Sleep(100);
			}
		}
	}

	void Routing()
	{
		for (int i = 0; i <= map.count; i++)
		{
			CExchange* Comp = map.map[i].comp;

			char buff[1024];
			int size = 0;

			if (Comp->recv(buff, &size) == 0)
			{
				Message msg1 = unpack((uint8_t*)buff);
				char addr[3];
				addr[0] == msg1.h.addr_to_subsyst;
				addr[1] == msg1.h.addr_to_NODE;
				addr[2] == msg1.h.addr_to_cmp;

				uint8_t* data = pack(msg1);
				if (map.Find(addr) != 0)
				{
					map.Find(addr)->send((char*)data, size);

					Log(msg1);
				}
			}
		}
	}

	void Close()
	{
		for (int i = 0; i <= map.count; i++)
		{
			map.map[i].comp->close();
		}
		map.Clear();
	}

	void Log(Message msg1)
	{
		using namespace std::chrono;

		system_clock::time_point ts = system_clock::now();
		long long millis = duration_cast<milliseconds>(ts.time_since_epoch()).count();

		time_t seconds = millis / 1000;
		tm date;
		localtime_s(&date, &seconds);

		std::ostringstream timestamp;

		char timeBuf[32];
		strftime(timeBuf, 32, "%d.%m.%Y %H.%M.%S", &date);

		timestamp << timeBuf << "." << millis % 1000;

		string str = timestamp.str();

		ofstream file;
		file.open("log.txt");
		file << "[" << str << "]: " << msg1.h.addr_from_subsyst << " -> " << msg1.h.addr_from_NODE << "; " << endl
			<< msg1.h.addr_from_cmp << " -> " << msg1.h.addr_to_subsyst << "; " << endl
			<< msg1.h.addr_to_NODE << " -> " << msg1.h.addr_to_cmp << "; " << endl
			<< msg1.h.type << endl;
	}

};