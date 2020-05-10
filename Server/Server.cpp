#include "Server.h"


void Server::RunTimeServer()
{
	if (setUpNet() == false)
	{
		return;
	}

	cout << "Time Server: Wait for clients' requests.\n";
	while (true)
	{
		m_bytesRecv = recvfrom(m_socket, m_recvBuff, 255, 0, &m_client_addr, &m_client_addr_len);
		if (SOCKET_ERROR == m_bytesRecv)
		{
			cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}


		m_recvBuff[1] = '\0'; //add the null-terminating to make it a string
		int messageNumber = m_recvBuff[0];
		cout << "Time Server: Recieved: " << m_bytesRecv << " bytes of \"" << messageNumber << "\" message.\n";

		answerClientsRequest(messageNumber);

		m_bytesSent = sendto(m_socket, (char*)m_sendBuff, (int)strlen(m_sendBuff), 0, (const sockaddr *)&m_client_addr, m_client_addr_len);
		if (SOCKET_ERROR == m_bytesSent)
		{
			cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		cout << "Time Server: Sent: " << m_bytesSent << "\\" << strlen(m_sendBuff) << " bytes of \"" << m_sendBuff << "\" message." <<
			endl << endl;
	}

	cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
}

bool Server::setUpNet()
{
	this->m_client_addr_len = sizeof(m_client_addr);

	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &m_wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
		return false;
	}

	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (INVALID_SOCKET == m_socket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}

	m_serverService.sin_family = AF_INET;
	m_serverService.sin_addr.s_addr = INADDR_ANY;	//inet_addr("127.0.0.1");
	m_serverService.sin_port = htons(TIME_PORT);

	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR *)&m_serverService, sizeof(m_serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(m_socket);
		WSACleanup();
		return false;
	}

	return true;
}

void Server::answerClientsRequest(int messageNumber)
{
	if (messageNumber == 1)
	{
		return getTime();
	}
	else if (messageNumber == 2)
	{
		return getTimeWithoutDate();
	}
	else if (messageNumber == 3)
	{
		return getTimeSinceEpoch();
	}
	else if (messageNumber == 4)
	{
		return getClientToServerDelayEstimation();
	}
	else if (messageNumber == 6)
	{
		return getTimeWithoutDateOrSeconds();
	}
	else if (messageNumber == 7)
	{
		return getYear();
	}
	else if (messageNumber == 8)
	{
		return getMonthAndDay();
	}
	else if (messageNumber == 9)
	{
		return getSecondsSinceBeginingOfMonth();
	}
	else if (messageNumber == 10)
	{
		return getDayOfYear();
	}
	else if (messageNumber == 11)
	{
		return getDaylightSavings();
	}
}

void Server::getTime()
{
	// Get the current time.
	time_t timer;
	time(&timer);
	// Parse the current time to printable string.
	strcpy(m_sendBuff, ctime(&timer));
	m_sendBuff[strlen(m_sendBuff) - 1] = '\0'; //to remove the new-line from the created string
	return;
}

void Server::getTimeWithoutDate()
{
	time_t timer;
	time(&timer);

	struct tm * timeInfo = localtime(&timer);
	string exactTime = std::to_string(timeInfo->tm_hour);
	exactTime.push_back(':');
	exactTime.append(std::to_string(timeInfo->tm_min));
	exactTime.push_back(':');
	exactTime.append(std::to_string(timeInfo->tm_sec));
	exactTime.push_back('\0');

	strToChar(exactTime, m_sendBuff);
}

void Server::getTimeSinceEpoch()
{
	time_t timer;
	time(&timer);

	string sinceEpoch = std::to_string(timer);

	strToChar(sinceEpoch, m_sendBuff);

}

void Server::getClientToServerDelayEstimation()
{
	string tickCount = to_string(GetTickCount());
	strToChar(tickCount, m_sendBuff);
}

void Server::getTimeWithoutDateOrSeconds()
{
	time_t timer;
	time(&timer);

	struct tm * timeInfo = localtime(&timer);
	string Time = std::to_string(timeInfo->tm_hour);
	Time.push_back(':');
	Time.append(std::to_string(timeInfo->tm_min));
	Time.push_back('\0');

	strToChar(Time, m_sendBuff);
}

void Server::getYear()
{
	time_t timer;
	time(&timer);

	struct tm * timeInfo = localtime(&timer);
	string year = std::to_string((timeInfo->tm_year) + 1900);

	strToChar(year, m_sendBuff);
}

void Server::getMonthAndDay()
{
	time_t timer;
	time(&timer);

	struct tm * timeInfo = localtime(&timer);

	translateMonthNumber(timeInfo->tm_mon);
	string dayOfMonth = to_string(timeInfo->tm_mday);

	for (int i = 0, sendBuffLen = strlen(m_sendBuff), dayLength = dayOfMonth.size() + 1; i < dayLength; i++)
	{
		m_sendBuff[sendBuffLen + i] = dayOfMonth[i];
	}
}

void Server::getSecondsSinceBeginingOfMonth()
{
	time_t timer;
	time(&timer);

	struct tm * timeInfo = localtime(&timer);

	int secondsPast = 0;
	secondsPast += (timeInfo->tm_mday - 1) * (24 * 60 * 60) + (timeInfo->tm_hour * 60 * 60) + (timeInfo->tm_min * 60) + (timeInfo->tm_sec);

	string secondPastStr = to_string(secondsPast);

	strToChar(secondPastStr, m_sendBuff);
}

void Server::getDayOfYear()
{
	time_t timer;
	time(&timer);

	struct tm * timeInfo = localtime(&timer);

	string dayOfTheYear = to_string(timeInfo->tm_yday); // not including today witch didn't past yet.
	dayOfTheYear.append(" days have past");

	strToChar(dayOfTheYear, m_sendBuff);
}

void Server::getDaylightSavings()
{
	time_t timer;
	time(&timer);

	struct tm * timeInfo = localtime(&timer);

	if (timeInfo->tm_isdst == 0)
	{
		strcpy(m_sendBuff, "daylight saving: no");
	}
	else
	{
		strcpy(m_sendBuff, "daylight saving: yes");
	}
}

void Server::translateMonthNumber(int monthNumber)
{

	if (monthNumber == 0)
	{
		(strcpy(m_sendBuff, "January "));
		return;
	}
	else if (monthNumber == 1)
	{
		(strcpy(m_sendBuff, "February "));
		return;
	}
	else if (monthNumber == 2)
	{
		(strcpy(m_sendBuff, "March "));
		return;
	}
	else if (monthNumber == 3)
	{
		(strcpy(m_sendBuff, "April "));
		return;
	}
	else if (monthNumber == 4)
	{
		(strcpy(m_sendBuff, "May "));
		return;
	}
	else if (monthNumber == 5)
	{
		(strcpy(m_sendBuff, "June "));
		return;
	}
	else if (monthNumber == 6)
	{
		(strcpy(m_sendBuff, "July "));
		return;
	}
	else if (monthNumber == 7)
	{
		(strcpy(m_sendBuff, "August "));
		return;
	}
	else if (monthNumber == 8)
	{
		(strcpy(m_sendBuff, "September "));
		return;
	}
	else if (monthNumber == 9)
	{
		(strcpy(m_sendBuff, "October "));
		return;
	}
	else if (monthNumber == 10)
	{
		(strcpy(m_sendBuff, "November "));
		return;
	}
	else if (monthNumber == 11)
	{
		(strcpy(m_sendBuff, "December "));
		return;
	}
}

void Server::strToChar(string str, char* des)
{
	for (int i = 0, length = str.size() + 1; i < length; i++)
	{
		des[i] = str[i];
	}
}
