#include <stdio.h>   
#include <Winsock2.h>   
#include <Ws2tcpip.h>
#include <string.h>
#include<iostream>
#include<stdlib.h>
#include <thread>
#include"..\UDP_Socket.h"
#include"..\ESCPP.h"

#pragma comment(lib,"Ws2_32.lib")//����Sockets��ؿ�  

extern Semaphore isConnect;

int main()
{

	printf("=========================================================\n");
	printf("                      ESCPP  ��������\n");
	printf("=========================================================\n");

	printf("������Ҫ�����Ķ˿ں�:\n");
	int port;
	std::cin >> port;
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 1), &wsaData)) //��ʼ��  
	{
		printf("Winsock�޷���ʼ��!\n");
		WSACleanup();
		return 0;
	}

	Init();

	UDP_Socket* sock = new UDP_Socket(port);

	std::thread* th = new std::thread([&]() {Daemon_Thread(sock, "server_rcv.mp4");});
	
	int operate = 0;

	isConnect.Wait();

start:
	printf("�пͻ�������\n���������:\n0:�˳�	1:�����ļ�\n");
	std::cin >> operate;
	switch (operate)
	{
	case 0:
		break;
	case 1:
		Mode_SendFile(sock);
		goto start;
	default:
		break;
	}


	WSACleanup();
	return 0;
}