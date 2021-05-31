#include <Winsock2.h>   
#include <stdio.h>
#include <thread>
#include<iostream>
#include"..\UDP_Core.h"
#include"../ESCPP.h"
#pragma warning(disable:4996)
#pragma comment(lib,"Ws2_32.lib")//����Sockets��ؿ�  

int GetMsg(UDP_Socket* sock)
{
	char buffer[1001];
	while (true)
	{
		int rd = (int)sock->ReciveFrom(buffer, sizeof(buffer) - 1);
		if (rd > 0)
		{
			buffer[rd] = 0;
			printf("%s\n", buffer);
		}
		//buffer[rd] = 0;
	}
	return 0;
}



int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 1), &wsaData)) //��ʼ��  
	{
		printf("Winsock�޷���ʼ��!\n");
		WSACleanup();
		return 1;
	}
	printf("��������ʼ����SOCKET��\n");

	char buffer[1000];

	UDP_Socket* sock = new UDP_Socket(23333);

	const char* file_name = "C:\\Users\\I_Rin\\Desktop\\RISCV_temp.docx";

	//std::thread* th = new std::thread([&]() {GetFile(file_name, sock);});
	std::thread* th = new std::thread([&]() {GetFile(file_name, sock);});

	while (true)
	{
		getchar();
		PutFile("C:\\Users\\I_Rin\\Desktop\\test.md", sock);
	}
	//const char* file_name2 = "C:\\Users\\I_Rin\\Desktop\\test.md";
	//PutFile(file_name2, sock);
	th->join();


	WSACleanup();
}