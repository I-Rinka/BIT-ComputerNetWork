#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include<iostream>
#include<thread>
#pragma comment(lib,"ws2_32.lib")
#define BUF_SIZE 30

void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
	WSADATA wsa_data;
	SOCKET serv_sock;
	char message[BUF_SIZE];
	int len;
	int clnt_addr_size;

	SOCKADDR_IN serv_addr, clnt_addr;
	if (argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		printf("WSAStartup() error!");
	}

	serv_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (serv_sock == INVALID_SOCKET)
	{
		printf("UDP socket create error");
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (SOCKADDR*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)//the bind is waiting for client's connection?
	{

		printf("bind() error");
	}

	char buffer[BUF_SIZE];
	std::thread th([&] {while (1)
	{
		memset(buffer, 0, BUF_SIZE);
		scanf_s("%s", buffer, BUF_SIZE - 1);
		sendto(serv_sock, buffer, strlen(buffer), 0, (SOCKADDR*)&clnt_addr, sizeof(clnt_addr));
	}}
	);

	while (1)
	{
		memset(message, 0, BUF_SIZE);
		clnt_addr_size = sizeof(clnt_addr);
		len = recvfrom(serv_sock, message, BUF_SIZE, 0, (SOCKADDR*)&clnt_addr, &clnt_addr_size);
		//	send(serv_sock, buffer, BUF_SIZE, 0);
		std::cout<<"message from client:%s"<< message<<std::endl;
		sendto(serv_sock, "send", 4, 0, (SOCKADDR*)&clnt_addr, sizeof(clnt_addr));
	}
	//th.join();
	closesocket(serv_sock);
	WSACleanup();
	return 0;
}
void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
}