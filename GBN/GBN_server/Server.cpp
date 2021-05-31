#include <stdio.h>   
#include <Winsock2.h>   
#include <Ws2tcpip.h>
#include <string.h>
#include<iostream>
#include<stdlib.h>
#include <thread>
#include"../UDP_Core.h"
#include"..\ESCPP.h"

#pragma comment(lib,"Ws2_32.lib")//连接Sockets相关库  

int main()
{

	printf("=========================================================\n");
	printf("                      ESCPP  服务器端\n");
	printf("=========================================================\n");
	
	printf("请输入要监听的端口号:\n");
	int port;
	std::cin >> port;
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 1), &wsaData)) //初始化  
	{
		printf("Winsock无法初始化!\n");
		WSACleanup();
		return 0;
	}

	Init();

	UDP_Socket* sock = new UDP_Socket(port);

	std::thread* th = new std::thread([&]() {Daemon_Thread(sock, "C:\\Users\\I_Rin\\Desktop\\server_recv.docx");});

	int operate = 0;

start:
	printf("请输入操作:\n0:退出	1:发送文件\n");
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
	//取消守护进程 

	WSACleanup();
	return 0;
}