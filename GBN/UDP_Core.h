#pragma once
#include <Winsock2.h>
#include<Ws2tcpip.h>
#include<Windows.h>
#pragma comment(lib,"Ws2_32.lib")
class UDP_Socket
{
public:
	UDP_Socket(int port);
	UDP_Socket(const char* target_ip, int port);
	~UDP_Socket();

	int ReciveFrom(char* buffer, int buffer_size)
	{
		//WaitForSingleObject(this->socket_rw_mutex, 20);
		int rt_val = recvfrom(rt_socket, buffer, buffer_size, 0, (struct sockaddr*)&opposite_addr, &addr_size);
		//ReleaseMutex(this->socket_rw_mutex);
		return rt_val;
	}
	int SendTo(char* buffer, int buffer_size)
	{
		//WaitForSingleObject(this->socket_rw_mutex, 20);
		int rt_val = sendto(rt_socket, buffer, buffer_size, 0, (struct sockaddr*)&opposite_addr, addr_size);
		//ReleaseMutex(this->socket_rw_mutex);
		return rt_val;
	}

private:
	struct sockaddr_in opposite_addr;//�ͻ��˵�ַ��ؽṹ��  
	SOCKET rt_socket;
	WSADATA wsaData;
	int addr_size = sizeof(opposite_addr);

	HANDLE socket_rw_mutex;
};

UDP_Socket::UDP_Socket(int port)
{
	this->socket_rw_mutex = CreateMutex(NULL, FALSE, NULL);
	struct sockaddr_in local;//������ַ��ؽṹ��  
	local.sin_family = AF_INET;
	local.sin_port = htons(port); ///�����˿�   
	local.sin_addr.s_addr = INADDR_ANY; ///����   
	rt_socket = socket(AF_INET, SOCK_DGRAM, 0);
	bind(rt_socket, (struct sockaddr*)&local, sizeof(local));//��SOCKET���˲��ؼ�  
}

UDP_Socket::UDP_Socket(const char* target_ip, int port)
{
	this->socket_rw_mutex = CreateMutex(NULL, FALSE, NULL);
	opposite_addr.sin_family = AF_INET;
	opposite_addr.sin_port = htons(port); ///server�ļ����˿�   
	InetPtonA(AF_INET, target_ip, &opposite_addr.sin_addr.s_addr);
	rt_socket = socket(AF_INET, SOCK_DGRAM, 0);
}

UDP_Socket::~UDP_Socket()
{
	CloseHandle(this->socket_rw_mutex);

	closesocket(rt_socket);
}
