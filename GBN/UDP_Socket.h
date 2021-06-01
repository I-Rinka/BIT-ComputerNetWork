#pragma once
#include <Winsock2.h>
#include<Ws2tcpip.h>
#include<Windows.h>
#include"Frame.h"
#pragma comment(lib,"Ws2_32.lib")
class UDP_Socket
{
public:
	UDP_Socket(int port);
	UDP_Socket(const char* target_ip, int port);
	~UDP_Socket();

	int ReciveFrom(char* buffer, int buffer_size)
	{
		int rt_val = recvfrom(rt_socket, buffer, buffer_size, 0, (struct sockaddr*)&opposite_addr, &addr_size);
		return rt_val;
	}
	int ReciveFrom(Frame& f)
	{
		return this->ReciveFrom(f.head, MAX_FRAME);
	}
	int SendTo(char* buffer, int buffer_size)
	{
		int rt_val = sendto(rt_socket, buffer, buffer_size, 0, (struct sockaddr*)&opposite_addr, addr_size);
		return rt_val;
	}
	int SendTo(Frame& f)
	{
		return this->SendTo(f.head, f.total_len);
	}

private:
	struct sockaddr_in opposite_addr;//�ͻ��˵�ַ��ؽṹ��  
	SOCKET rt_socket;
	WSADATA wsaData;
	int addr_size = sizeof(opposite_addr);

};

UDP_Socket::UDP_Socket(int port)
{
	struct sockaddr_in local;//������ַ��ؽṹ��  
	local.sin_family = AF_INET;
	local.sin_port = htons(port); ///�����˿�   
	local.sin_addr.s_addr = INADDR_ANY; ///����   
	rt_socket = socket(AF_INET, SOCK_DGRAM, 0);
	bind(rt_socket, (struct sockaddr*)&local, sizeof(local));//��SOCKET
}

UDP_Socket::UDP_Socket(const char* target_ip, int port)
{
	opposite_addr.sin_family = AF_INET;
	opposite_addr.sin_port = htons(port); ///server�ļ����˿�   
	InetPtonA(AF_INET, target_ip, &opposite_addr.sin_addr.s_addr);
	rt_socket = socket(AF_INET, SOCK_DGRAM, 0);
}

UDP_Socket::~UDP_Socket()
{
	closesocket(rt_socket);
}
