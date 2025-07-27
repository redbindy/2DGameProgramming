#include "Net.h"

Net::Net()
	: mServerSocket(0)
	, mClientSocket(0)
	, mClientIP{ '\0', }
	, mClientPort(0)
{
	WSADATA wsaData;
	int errCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (errCode != 0)
	{
		std::cerr << "WSAStartup" << std::endl;

		DebugBreak();
	}
}

Net::~Net()
{
	if (mClientSocket != 0)
	{
		closesocket(mClientSocket);
	}

	if (mServerSocket != 0)
	{
		closesocket(mServerSocket);
	}

	WSACleanup();
}

void Net::CreateServer()
{
	mServerSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mServerSocket == INVALID_SOCKET)
	{
		std::cerr << "socket - server" << std::endl;

		DebugBreak();
	}

	sockaddr_in serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(12345);

	int errCode = bind(mServerSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
	if (errCode == SOCKET_ERROR)
	{
		std::cerr << "bind" << std::endl;
	}

	u_long nonBlockingMode = 1;
	ioctlsocket(mServerSocket, FIONBIO, &nonBlockingMode);
}

void Net::CreateClient()
{
	mClientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mClientSocket == INVALID_SOCKET)
	{
		std::cout << "socket - client" << std::endl;

		DebugBreak();
	}

	sockaddr_in serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(23456);

	int errCode = inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	if (errCode != 1)
	{
		std::cout << "inet_pton" << std::endl;
	}

	u_long nonBlockingMode = 1;
	ioctlsocket(mClientSocket, FIONBIO, &nonBlockingMode);
}

bool Net::TrySend(const char* pData, const int len) const
{
	sockaddr_in sockAddr;
	ZeroMemory(&sockAddr, sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;

	if (mServerSocket != 0)
	{
		if (mClientPort == 0)
		{
			return false;
		}

		sockAddr.sin_port = htons(mClientPort);
		inet_pton(AF_INET, mClientIP, &sockAddr.sin_addr);

		sendto(mServerSocket, pData, len, 0, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr));
	}
	else if (mClientSocket != 0)
	{
		sockAddr.sin_port = htons(12345);
		inet_pton(AF_INET, "127.0.0.1", &sockAddr.sin_addr);

		sendto(mClientSocket, pData, len, 0, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr));
	}

	return true;
}

bool Net::TryReceive(char* pBuffer, const int bufferSize, int& outLen)
{
	sockaddr_in sockAddr;
	ZeroMemory(&sockAddr, sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;

	int addrSize = sizeof(sockAddr);
	if (mServerSocket != 0)
	{
		outLen = recvfrom(mServerSocket, pBuffer, bufferSize, 0, reinterpret_cast<sockaddr*>(&sockAddr), &addrSize);

		mClientPort = ntohs(sockAddr.sin_port);
		inet_ntop(AF_INET, &sockAddr.sin_addr, mClientIP, INET_ADDRSTRLEN);
	}
	else if (mClientSocket != 0)
	{
		outLen = recvfrom(mClientSocket, pBuffer, bufferSize, 0, reinterpret_cast<sockaddr*>(&sockAddr), &addrSize);
	}

	if (outLen == -1)
	{
		return false;
	}

	return true;
}