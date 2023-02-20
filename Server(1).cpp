#include "Pack_unpack.h"
#include "Exchange.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

using namespace std;

/*int main() {
	CExchange_udp srv(54000, 54001);
	srv.open();

	char buff[1024];
	int size = 0;
	while(size == 0) {
		srv.recv(buff, &size);
	}
	Message msg1 = unpack((uint8_t*) buff);
	srv.close();

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
	return 0;
}*/

vector<string> split(const string& input, const string& delimeter) {
	// passing -1 as the submatch index parameter performs splitting
	regex re(delimeter);
	sregex_token_iterator first{ input.begin(), input.end(), re, -1 };
	sregex_token_iterator last;

	vector<string> vec(first, last);

	return vec;
}

int main()
{

	string adr_file = "cfg.txt";

	ifstream file;
	file.open(adr_file);
	if (!file.is_open())
	{
		cout << "Error of opening file!" << endl;
	}
	else {
		cout << "file is open!" << endl;
	}

	string str;
	bool bTypeFound = false;
	int conections[15][5];
	int j = 0;
	
	while (!file.eof())
	{
		getline(file, str);
		if (str.find(',') != string::npos)
		{
			if (!bTypeFound)
			{
				continue;
			}

			vector<string> vec = split(str, ",");
			vector<string> ipStr = split(vec[0], "\\.");
			int ip[4];
			if (ipStr.size() != 4)
			{
				cout << "Error: invalid IP address" << endl;
				continue;
			}

			bool bSuccess = true;
			for (int i = 0; i <= 3; i++)
			{
				ip[i] = atoi(ipStr[i].c_str());
				if (ip[i] < 0 || ip[i] > 255)
				{
					cout << "Error: invalid IP address" << endl;
					bSuccess = false;
					break;
				}
				cout << ipStr[i] << "; " << endl;
			}

			if (!bSuccess)
			{
				continue;
			}

			int port = atoi(vec[1].c_str());
			if (port < 0 || port > 65535)
			{
				cout << "Error: invalid port" << endl;
				continue;
			}

			cout << port << endl;

			for (int i = 0; i <= 3; ++i) {
				conections[j][i] = ip[i];
			}

			conections[j][4] = port;

			j++;
		} 
		else
		{
			if (str != "udp")
			{
				cout << "Error: invalid protocol" << endl;

				bTypeFound = false;
				continue;
			}

			bTypeFound = true;

			cout << str << endl;
		}
	}
	file.close();

	system("pause");
	return 0;

}
