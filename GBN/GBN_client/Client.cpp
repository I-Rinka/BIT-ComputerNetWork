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

int GetMsg(UDP_Socket* sock)
{
	Sleep(1000);
	char buffer[1001];
	while (true)
	{
		int rd = (int)sock->ReciveFrom(buffer, sizeof(buffer) - 1);
		if (rd > 0)
		{
			buffer[rd - 1] = 0;
			printf("%s\n", buffer);
		}
		//buffer[rd] = 0;
	}
	return 0;
}


int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 1), &wsaData)) //初始化  
	{
		printf("Winsock无法初始化!\n");
		WSACleanup();
		return 0;
	}
	printf("客户端开始创建SOCKET。\n");

	UDP_Socket* sock = new UDP_Socket("127.0.0.1", 23333);

	const char* file_name = "C:\\Users\\I_Rin\\Desktop\\RISCV.docx";
	//std::thread* th = new std::thread([&]() {PutFile(file_name, client);});
	char buffer[1001];
	std::thread* th = new std::thread([&]() {GetFile("C:\\Users\\I_Rin\\Desktop\\test_tmp.md", sock);});

	while (true)
	{
		getchar();
		PutFile(file_name, sock);
	}


	//const char* file_name2 = "C:\\Users\\I_Rin\\Desktop\\test_temp.md";
	//GetFile(file_name2, client);
	th->join();

	WSACleanup();
	return 0;
}