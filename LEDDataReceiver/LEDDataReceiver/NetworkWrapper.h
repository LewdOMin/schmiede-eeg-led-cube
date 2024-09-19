// Written for MMT 2017 - Online Gaming and Cloud Computing at FH Salzburg
// By Bastian Born
#pragma once

#include <windows.h> // meh. but easier than forward declare SOCKET
#include <vector>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")

inline bool InitWinsock()
{
    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        printf("WSAStartup failed with error: %d\n", result);
        return false;
    }

    return true;
}

inline std::vector<SOCKET> WaitForInput(std::vector<SOCKET> waitFor, unsigned long timeout)
{
	FD_SET readSet;
	FD_ZERO(&readSet);

	// init read set with socket we want to wait for
	for (auto socket : waitFor)
	{
		FD_SET(socket, &readSet);
	}

	// build timeout
	timeval t;
	t.tv_sec = 0;
	t.tv_usec = timeout * 1000;

	// wait!
	int total = select(0, &readSet, nullptr, nullptr, &t);

	if (total == SOCKET_ERROR)
	{
		printf("select() error: %d\n", WSAGetLastError());
	}

	// read back set and put into result
	std::vector<SOCKET> triggered;
	for (auto socket : waitFor)
	{
		if (FD_ISSET(socket, &readSet))
		{
			triggered.push_back(socket);
		}
	}

	return triggered;
}

inline void CloseSocket(SOCKET socket)
{
	closesocket(socket);
}

inline unsigned long BytesAvailable(SOCKET socket)
{
	unsigned long bytes;
	ioctlsocket(socket, FIONREAD, &bytes);
	return bytes;
}


inline int UdpRead(SOCKET socket, char* fromIpAddr, unsigned short* fromPort, char* outBuffer, unsigned long length)
{
	sockaddr_in remoteAddr;
	int remoteAddrSize = sizeof(remoteAddr);
	int read = recvfrom(socket, outBuffer, length, 0, (sockaddr*)&remoteAddr, &remoteAddrSize);
	
	if (read == SOCKET_ERROR)
	{
        const int error = WSAGetLastError();
	    
		printf("Recv error: %d\n", error);
		CloseSocket(socket);
	    
		return -1; // Closed with error
	}

	// Write remote address
	*fromPort = ntohs(remoteAddr.sin_port);
	inet_ntop(AF_INET, &(remoteAddr.sin_addr), fromIpAddr, 16);
	
	return read;
}

inline bool UdpWrite(SOCKET socket, const char* toIpAddr, unsigned short toPort, const char* buffer, unsigned long length)
{
	sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(toPort);
	inet_pton(AF_INET, toIpAddr, &(remoteAddr.sin_addr));

	if (sendto(socket, buffer, length, 0, (sockaddr*)&remoteAddr, sizeof(remoteAddr)) == SOCKET_ERROR)
	{
		printf("Send error: %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

inline SOCKET CreateUdpSocket(unsigned short port)
{
	sockaddr_in args;
	args.sin_family = AF_INET;
	args.sin_addr.s_addr = INADDR_ANY;
	args.sin_port = htons(port);

	SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if (udpSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %d\n", WSAGetLastError());
		return INVALID_SOCKET;
	}

	if (bind(udpSocket, (sockaddr*)&args, sizeof(args)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d\n", WSAGetLastError());
		closesocket(udpSocket);
		return INVALID_SOCKET;
	}

	return udpSocket;
}

inline bool TcpRead(SOCKET socket, char* outBuffer, unsigned long length)
{
	int left = length;
	while(left > 0)
	{
		int read = recv(socket, outBuffer, left, 0);
		
		if (read == SOCKET_ERROR)
		{
            const int error = WSAGetLastError();
	    
		    printf("Recv error: %d\n", error);
			CloseSocket(socket);
			return false; // Closed with error
		}
	
		outBuffer += read;
		left -= read;
	}
	
	return true;
}

inline bool TcpWrite(SOCKET socket, const char* buffer, unsigned long length)
{
	if (send(socket, buffer, length, 0) == SOCKET_ERROR)
	{
		printf("Send error: %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

inline SOCKET OpenServer(unsigned short port)
{
	sockaddr_in serverArgs;
	serverArgs.sin_family = AF_INET;
	serverArgs.sin_addr.s_addr = INADDR_ANY;
	serverArgs.sin_port = htons(port);

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (serverSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %d\n", WSAGetLastError());
		return NULL;
	}

	if (bind(serverSocket, (sockaddr*)&serverArgs, sizeof(serverArgs)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d\n", WSAGetLastError());
		closesocket(serverSocket);
		return NULL;
	}

	if (listen(serverSocket, 10) == SOCKET_ERROR)
	{
		printf("Listen failed with error code : %d\n", WSAGetLastError());	
		closesocket(serverSocket);
		return NULL;
	}

	return serverSocket;
}

inline SOCKET AcceptConnection(SOCKET serverSocket, sockaddr* clientAddr, int* clientAddrSize)
{
	SOCKET newSocket = accept(serverSocket, clientAddr, clientAddrSize);
	
	if (newSocket == INVALID_SOCKET)
	{
		printf("Accept failed with error: %d\n", WSAGetLastError());
		return NULL;
	}
	
	return newSocket;
}

inline SOCKET Connect(const char* ipAddr, unsigned short port)
{
	sockaddr_in clientArgs;
	clientArgs.sin_family = AF_INET;
	clientArgs.sin_port = htons(port);
	inet_pton(AF_INET, ipAddr, &(clientArgs.sin_addr));

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %d\n", WSAGetLastError());
		return NULL;
	}
	
	int result = connect(clientSocket, (sockaddr*)&clientArgs, sizeof (clientArgs));
    if (result == SOCKET_ERROR)
	{
        printf("Connect function failed with error: %d\n", WSAGetLastError());
        CloseSocket(clientSocket);
        return NULL;
    }

	return clientSocket;
}
