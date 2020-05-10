#include "Client.h"

void Client::RunTimeClient()
{
	if (setUpNet() == false)
	{
		return;
	}

	while (m_sendBuff[0] != 12)
	{

		choseClientOption();
		int messageNumber = m_sendBuff[0];

		if (m_sendBuff[0] == 12) continue;
		if (m_sendBuff[0] == 4)
		{
			getClientToServerDelayEstimation();
			continue;
		}
		if (m_sendBuff[0] == 5)
		{
			measreRTT();
			continue;
		}

		m_bytesSent = sendto(m_connSocket, m_sendBuff, (int)strlen(m_sendBuff), 0, (const sockaddr *)&m_server, sizeof(m_server));
		if (SOCKET_ERROR == m_bytesSent)
		{
			cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(m_connSocket);
			WSACleanup();
			return;
		}
		cout << "Time Client: Sent: " << m_bytesSent << "/" << strlen(m_sendBuff) << " bytes of \"" << messageNumber << "\" message.\n";

		m_bytesRecv = recv(m_connSocket, m_recvBuff, 255, 0);
		if (SOCKET_ERROR == m_bytesRecv)
		{
			cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
			closesocket(m_connSocket);
			WSACleanup();
			return;
		}

		m_recvBuff[m_bytesRecv] = '\0'; //add the null-terminating to make it a string
		cout << "Time Client: Recieved: " << m_bytesRecv << " bytes of \"" << m_recvBuff << "\" message." << endl << endl;
	}

	cout << "Time Client: Closing Connection.\n";
	closesocket(m_connSocket);
}

bool Client::setUpNet()
{
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &m_wsaData))
	{
		cout << "Time Client: Error at WSAStartup()\n";
		return false;
	}

	m_connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_connSocket)
	{
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}

	m_server.sin_family = AF_INET;
	m_server.sin_addr.s_addr = inet_addr("127.0.0.1");
	m_server.sin_port = htons(TIME_PORT);

	return true;
}

void Client::getClientToServerDelayEstimation()
{
	int diffrenceTimesArray[99];
	int size = 99;
	int prevTime, currTime;

	for (int i = 0; i < 100; i++)
	{
		m_bytesSent = sendto(m_connSocket, m_sendBuff, (int)strlen(m_sendBuff), 0, (const sockaddr *)&m_server, sizeof(m_server));
		if (SOCKET_ERROR == m_bytesSent)
		{
			cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(m_connSocket);
			WSACleanup();
			m_sendBuff[0] = 12;
			m_sendBuff[1] = '\0';
			return;
		}
	}
	cout << "Time Client Sent 100 '4' messages" << endl;

	m_bytesRecv = recv(m_connSocket, m_recvBuff, 255, 0);
	m_recvBuff[m_bytesRecv] = '\0';
	if (SOCKET_ERROR == m_bytesRecv)
	{
		cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(m_connSocket);
		WSACleanup();
		m_sendBuff[0] = 12;
		m_sendBuff[1] = '\0';
		return;
	}

	prevTime = stoi(m_recvBuff);

	for (int i = 0; i < 99; i++)
	{
		m_bytesRecv = recv(m_connSocket, m_recvBuff, 255, 0);
		m_recvBuff[m_bytesRecv] = '\0';

		if (SOCKET_ERROR == m_bytesRecv)
		{
			cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
			closesocket(m_connSocket);
			WSACleanup();
			m_sendBuff[0] = 12;
			m_sendBuff[1] = '\0';
			return;
		}

		currTime = stoi(m_recvBuff);

		diffrenceTimesArray[i] = currTime - prevTime;

		prevTime = currTime;
	}

	double avgDifference = avgOfIntArray(diffrenceTimesArray, size);
	cout << "the client to server delay estimation is: " << avgDifference << " MS" << endl << endl;
}


void Client::measreRTT()
{
	int RTTArray[100];
	int size = 100;
	int firstLocalTimeMeasure, secondLocalTimeMeasure;
	m_sendBuff[0] = 1;

	for (int i = 0; i < 100; i++)
	{
		m_bytesSent = sendto(m_connSocket, m_sendBuff, (int)strlen(m_sendBuff), 0, (const sockaddr *)&m_server, sizeof(m_server));
		firstLocalTimeMeasure = GetTickCount();
		if (SOCKET_ERROR == m_bytesSent)
		{
			cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(m_connSocket);
			WSACleanup();
			m_sendBuff[0] = 12;
			m_sendBuff[1] = '\0';
			return;
		}

		m_bytesRecv = recv(m_connSocket, m_recvBuff, 255, 0);
		secondLocalTimeMeasure = GetTickCount();
		if (SOCKET_ERROR == m_bytesRecv)
		{
			cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
			closesocket(m_connSocket);
			WSACleanup();
			m_sendBuff[0] = 12;
			m_sendBuff[1] = '\0';
			return;
		}

		RTTArray[i] = secondLocalTimeMeasure - firstLocalTimeMeasure;
	}
	double avgRTT = avgOfIntArray(RTTArray, size);
	cout << "the client to server RTT average is: " << avgRTT << " MS" << endl << endl;
}


void Client::choseClientOption()
{
	char chosenAct[10];
	do
	{
		printMenu();
		cin >> chosenAct;
		cout << endl;

		if (strcmp(chosenAct, "1") == false)
		{
			m_sendBuff[0] = 1;
			m_sendBuff[1] = '\0';
			return;
		}
		else if (strcmp(chosenAct, "2") == false)
		{
			m_sendBuff[0] = 2;
			m_sendBuff[1] = '\0';
			return;
		}
		else if (strcmp(chosenAct, "3") == false)
		{
			m_sendBuff[0] = 3;
			m_sendBuff[1] = '\0';
			return;
		}
		else if (strcmp(chosenAct, "4") == false)
		{
			m_sendBuff[0] = 4;
			m_sendBuff[1] = '\0';

			return;
		}
		else if (strcmp(chosenAct, "5") == false)
		{
			m_sendBuff[0] = 5;
			m_sendBuff[1] = '\0';
			return;
		}
		else if (strcmp(chosenAct, "6") == false)
		{
			m_sendBuff[0] = 6;
			m_sendBuff[1] = '\0';
			return;
		}
		else if (strcmp(chosenAct, "7") == false)
		{
			m_sendBuff[0] = 7;
			m_sendBuff[1] = '\0';
			return;
		}
		else if (strcmp(chosenAct, "8") == false)
		{
			m_sendBuff[0] = 8;
			m_sendBuff[1] = '\0';
			return;
		}
		else if (strcmp(chosenAct, "9") == false)
		{
			m_sendBuff[0] = 9;
			m_sendBuff[1] = '\0';
			return;
		}
		else if (strcmp(chosenAct, "10") == false)
		{
			m_sendBuff[0] = 10;
			m_sendBuff[1] = '\0';
			return;
		}
		else if (strcmp(chosenAct, "11") == false)
		{
			m_sendBuff[0] = 11;
			m_sendBuff[1] = '\0';
			return;
		}
		else if (strcmp(chosenAct, "12") == false)
		{
			m_sendBuff[0] = 12;
			m_sendBuff[1] = '\0';
			return;
		}
		else
		{
			cout << endl << "invalid option, please try again" << endl;
		}

	} while (true);
}

void Client::printMenu()
{
	cout << "please choose the number of the wanted activity:" << endl <<
		"1) get date and time" << endl <<
		"2) get time without date" << endl <<
		"3) get time since epoch" << endl <<
		"4) get client to server delay estimation" << endl <<
		"5) measure RTT" << endl <<
		"6) get hours and minutes" << endl <<
		"7) get year" << endl <<
		"8) get month and day" << endl <<
		"9) get seconds since begining of month" << endl <<
		"10) get day of the year" << endl <<
		"11) get daylight saving" << endl <<
		"12) exit " << endl;
}

double Client::avgOfIntArray(int * arr, int size)
{
	double avg = 0;

	for (int i = 0; i < size; i++)
	{
		avg += (arr[i] / 100.0);
	}
	return avg;
}
