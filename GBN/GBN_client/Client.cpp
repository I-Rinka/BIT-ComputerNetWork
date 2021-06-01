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

int main()
{
	printf("=========================================================\n");
	printf("                      ESCPP  �ͻ���\n");
	printf("=========================================================\n");

	char ip[40]="127.0.0.1";

	//printf("������Ҫ���ӵķ�������ַ:\n");
	//fgets(ip, 39, stdin);

	printf("������Ŀ��˿ں�:\n");
	int port;
	std::cin >> port;
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 1), &wsaData)) //��ʼ��  
	{
		printf("Winsock�޷���ʼ��!\n");
		WSACleanup();
		return 0;
	}

	UDP_Socket* sock = new UDP_Socket(ip, port);

	int iMode = 0; 
	ioctlsocket(sock->rt_socket, FIONBIO, (u_long FAR*) & iMode);//��������

	char tmp_buffer[10];
	Frame connect_frame;
	connect_frame.head = tmp_buffer;
	connect_frame.InitFrameStruct(Frame::connect, 1, 0);
	sock->SendTo(connect_frame);
	
	std::thread* th = new std::thread([&]() {Daemon_Thread(sock, "client_recv.mp4");});

	int operate = 0;

start:
	printf("���������:\n0:�˳�	1:�����ļ�\n");
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
	//ȡ���ػ����� 

	WSACleanup();
	return 0;
}