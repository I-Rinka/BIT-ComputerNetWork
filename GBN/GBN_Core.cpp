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
	int crc_divisor = 0x7D;//they live in 7D so I try to make them happy :)
	WSADATA wsaData;
	SOCKET connectSocket = INVALID_SOCKET;

	//client side
	struct addrinfo* result = NULL, * ptr = NULL, hints;

	int iResult;//socket connection result
};

ESCPP_GBN::ESCPP_GBN(const char* port)
{
	WSADATA wsa_data;
	SOCKET serv_sock;

	int clnt_addr_size;

	SOCKADDR_IN serv_addr, clnt_addr;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		//some throw?
	}


	//initialize UDP information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, port, &hints, &result);//It is similar to client side, but the server is passively connected, so the ip address is NULL.
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
	}

	SOCKET listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);//The result contains the infomation such as ip or port that socket need which is from the getaddrinfo(), and getaddrinfo() stores the socket's protocol infomation.
	//this->connectSocket

	if (listenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
	}

	iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(this->connectSocket);
		WSACleanup();
	}

	freeaddrinfo(result);// After binding, we do not need the result any more.


	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		//some throw?
	}

	// Accept a client socket
	this->connectSocket = accept(listenSocket, NULL, NULL); // In our recent implementation, there is only one client per server. So we implements it in this way.
	// However, if it is necessary in future, we can add some queue for the connect socket so the connectSocket can connect multiple server
	if (this->connectSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(this->connectSocket);
		WSACleanup();
		//some throw?
	}

	closesocket(listenSocket);// The server socket will never be used if we get the information of clients

	// Then we can use recv() or send as ususal.

	WSACleanup();
}
ESCPP_GBN::ESCPP_GBN(const char* target_ip, const char* port)
{
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
		WSACleanup();
		//some throw?
	}

	// Attempt to connect to an address until one succeeds. In fact, as ptr is a link list, so it can also connect to multiple server if it necessary 
	for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		this->connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (this->connectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
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
	iResult = shutdown(this->connectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(this->connectSocket);
		WSACleanup();
		//some throw?
	}
	closesocket(this->connectSocket);
	WSACleanup();
}

int ESCPP_GBN::Core_Send(const char* message, int send_length)
{
	if (send_length > this->packet_lenth)
	{
		send_length = packet_lenth;
	}
	int iSendResult = send(this->connectSocket, message, send_length, 0);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		/*	closesocket(this->connectSocket);
			WSACleanup();*/
		return -1;
	}
	return 0;
}

int ESCPP_GBN::Core_Receive(char* message, int buffer_length)
{
	int iResult = recv(this->connectSocket, message, buffer_length, 0);
	return iResult;
}
