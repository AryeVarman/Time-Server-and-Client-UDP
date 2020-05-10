#ifndef __Client_H
#define __Client_H
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string.h>
#include <string>
#include<windows.h>
using namespace std;

#define TIME_PORT	27015


class Client
{
public:
	void RunTimeClient();

	
private:
	bool setUpNet();
	void getClientToServerDelayEstimation();
	void measreRTT();
	void choseClientOption();
	void printMenu();
	double avgOfIntArray(int * arr, int size);

	
private:
	WSAData m_wsaData;
	SOCKET m_connSocket;
	sockaddr_in m_server;
	int m_bytesSent = 0;
	int m_bytesRecv = 0;
	char m_sendBuff[2];
	char m_recvBuff[255];
};

#endif //!__Client_H
