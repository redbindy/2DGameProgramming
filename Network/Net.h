#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <cctype>

#define SERVER false
#define CLIENT true

class Net final
{
public:
	Net();
	virtual ~Net();

	void CreateServer();
	void CreateClient();
	bool TrySend(const char* pData, const int len) const;
	bool TryReceive(char* pBuffer, const int bufferSize, int& outLen);

private:
	SOCKET mServerSocket;
	SOCKET mClientSocket;

	char mClientIP[INET_ADDRSTRLEN];
	unsigned short mClientPort;
};