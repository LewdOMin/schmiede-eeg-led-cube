#include <format>
#include <iostream>
#include <string>
#include <thread>
#include <WinSock2.h>

#include "NetworkWrapper.h"

const uint32_t PORT = 6969;
const size_t PACKET_SIZE = 1; // actually 16384

SOCKET server_socket;
char client_ip[16];

void recv();

int main(int argc, char* argv[])
{
    InitWinsock();
    
    server_socket = CreateUdpSocket(PORT);

    recv();
    
    return 0;
}

void recv()
{
    while (true)
    {
        char from_ip[16];
        uint16_t from_port;
        char* buffer = new char[PACKET_SIZE];
        auto result = UdpRead(server_socket, from_ip, &from_port, buffer, PACKET_SIZE);

        std::string msg = std::format("Received {0} bytes from {1}:{2}", result, from_ip, from_port);
        std::cout << msg << std::endl;
    }
}
