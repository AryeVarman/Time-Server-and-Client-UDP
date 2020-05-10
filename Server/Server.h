#ifndef __Server_H
#define __Server_H
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include<string>
#include<windows.h>
using namespace std;


#define TIME_PORT	27015

class Server
{
public:

	void RunTimeServer();

private:

	bool setUpNet();
	void answerClientsRequest(int messageNumber);
	void getTime();
	void getTimeWithoutDate();
	void getTimeSinceEpoch();
	void getClientToServerDelayEstimation();
	void getTimeWithoutDateOrSeconds();
	void getYear();
	void getMonthAndDay();
	void getSecondsSinceBeginingOfMonth();
	void getDayOfYear();
	void getDaylightSavings();
	void translateMonthNumber(int monthNumber);
	void strToChar(string str, char* des);


private:
	WSAData m_wsaData;
	SOCKET m_socket;
	sockaddr_in m_serverService;
	sockaddr m_client_addr;
	int m_client_addr_len;
	int m_bytesSent = 0;
	int m_bytesRecv = 0;
	char m_sendBuff[255];
	char m_recvBuff[2];
};

#endif // !__Server_H