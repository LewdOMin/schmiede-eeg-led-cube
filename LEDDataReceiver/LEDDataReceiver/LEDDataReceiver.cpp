#include <format>
#include <iostream>
#include <string>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>

#include "NetworkWrapper.h"

const uint32_t PORT = 6969;
const size_t PACKET_SIZE = 1; // actually 16384

SOCKET server_socket;
char client_ip[16];

int recv();

int main(int argc, char* argv[])
{
    
    
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in args{AF_INET, htons(PORT)};
    args.sin_addr.s_addr = INADDR_ANY;
    int result = bind(server_socket, (sockaddr*)&args, sizeof(args));

    if (result == SOCKET_ERROR)
    {
        std::cout << "Error" << std::endl;
        closesocket(server_socket);
        return -1;
    }

    return recv();
}

int recv()
{
    while (true)
    {
        char from_ip[16];
        uint16_t from_port;
        char* buffer = new char[PACKET_SIZE];
        sockaddr_in remoteAddr;
        int remoteAddrSize = sizeof(remoteAddr);
        int read = recvfrom(server_socket, buffer, PACKET_SIZE, 0, (sockaddr*)&remoteAddr, &remoteAddrSize);
	
        if (read == SOCKET_ERROR)
        {
            const int error = WSAGetLastError();
	    
            printf("Recv error: %d\n", error);
            closesocket(server_socket);
	    
            return -1; // Closed with error
        }

        std::string msg = std::format("Received {0} bytes from {1}:{2}", read, from_ip, from_port);
        std::cout << msg << std::endl;
    }
}
