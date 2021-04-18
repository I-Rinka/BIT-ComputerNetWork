#pragma once
#ifndef _GBN_CORE_
#define _GBN_CORE_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include<iostream>
#include<thread>
#pragma comment(lib,"ws2_32.lib")
class ESCPP_GBN
{
public:
	ESCPP_GBN(const char* target_ip, const char* port);
	ESCPP_GBN(const char* port);
	~ESCPP_GBN();

	int packet_lenth = 500;//byte

	// This will become a private function
	/* char* message is the message buffer you want to send. int send_length is the message length you want to send, it should never exceed the length of this->packet_lenth.
	 The function returns -1 means error occurs. Returns 0 means successfully sended */
	int Core_Send(const char* message, int send_length);

	// This will become a private function
	/* char* message is the message buffer you want to send. int buffer_length is the maxium of message in order to prevent boundary exceed.
	 The function returns data length that the socket received */
	int Core_Receive(char* message, int buffer_length);


private:
	int crc_divisor = 0x7D;//They live in 7D so I try to make them happy :)
	bool is_passive = false;// Server is passive connection.
	WSADATA wsaData;
	SOCKET connectSocket = INVALID_SOCKET;
	SOCKADDR_IN clnt_addr;
	//client side


	int iResult;//socket connection result
};

ESCPP_GBN::ESCPP_GBN(const char* port)
{
	this->is_passive = true; // This is a Server
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		//some throw?
	}

	//this->connectSocket 
	SOCKET fd_socket;
	fd_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	this->connectSocket = fd_socket;

	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(port));
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	//bind socket
	bind(this->connectSocket, (SOCKADDR*)&server_addr, sizeof(SOCKADDR));

}
ESCPP_GBN::ESCPP_GBN(const char* target_ip, const char* port)
{
	struct addrinfo* result = NULL, * ptr = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		//some throw?
	}

	//initialize UDP information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	// Resolve the server address and port
	iResult = getaddrinfo(target_ip, port, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		//some throw?
	}

	// Attempt to connect to an address until one succeeds. In fact, as ptr is a link list, so it can also connect to multiple server if it necessary 
	for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		this->connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (this->connectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			//some throw?
		}

		freeaddrinfo(result);
		// Connect to server.
		iResult = connect(this->connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(this->connectSocket);
			this->connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

}
ESCPP_GBN::~ESCPP_GBN()
{
	if (!this->is_passive)
	{
		iResult = shutdown(this->connectSocket, SD_SEND);// Client will shutdown the socket connection
	}

	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		//some throw?
	}
	closesocket(this->connectSocket);
	WSACleanup();
}

int ESCPP_GBN::Core_Send(const char* message, int send_length)
{
	int iSendResult = -1;
	if (send_length > this->packet_lenth)
	{
		send_length = packet_lenth;
	}
	if (this->is_passive)
	{
		int	clnt_addr_size = sizeof(this->clnt_addr);
		iResult = sendto(this->connectSocket, message, packet_lenth, 0, (SOCKADDR*)&clnt_addr, clnt_addr_size);
	}
	else
	{
		iSendResult = send(this->connectSocket, message, send_length, 0);
	}
	return iSendResult;
}

int ESCPP_GBN::Core_Receive(char* message, int buffer_length)
{
	int iResult = -1;
	if (this->is_passive)
	{
		// Server is passively connected, so it gets the data from the server
		int	clnt_addr_size = sizeof(this->clnt_addr);
		iResult = recvfrom(this->connectSocket, message, buffer_length, 0, (SOCKADDR*)&clnt_addr, &clnt_addr_size);
	}
	else
	{
		iResult = recv(this->connectSocket, message, buffer_length, 0);

	}
	return iResult;
}

#endif // !_GBN_CORE_