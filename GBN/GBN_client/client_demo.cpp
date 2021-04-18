
/*uecho_client_win.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include<iostream>
#include<thread>
#pragma comment(lib,"ws2_32.lib")
#define DEFAULT_BUFLEN 30

#define DEFAULT_PORT "10488"
#define IP "127.0.0.1"

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;
	const char* sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Validate the parameters
	if (argc != 2) {
		printf("usage: %s server-name\n", argv[0]);
		return 1;
	}

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	// Resolve the server address and port
	iResult = getaddrinfo(IP, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	char buffer2[DEFAULT_BUFLEN];
	std::thread th([&] {while (1)
	{
		memset(buffer2, 0, DEFAULT_BUFLEN);
		scanf_s("%s", buffer2, DEFAULT_BUFLEN - 1);
		send(ConnectSocket, buffer2, DEFAULT_BUFLEN, 0);
	}}
	);


	char buffer[DEFAULT_BUFLEN];

	// Send an initial buffer
	while (1)
	{
		memset(buffer, 0, DEFAULT_BUFLEN);
		int len = 4;
		recv(ConnectSocket, buffer, len, 0);
		std::cout << "message from server:" << buffer << std::endl;
		send(ConnectSocket, "get", 4, 0);
		//printf("message from server:%s\n", buffer);
	}
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive until the peer closes the connection
	//do {

	//    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	//    if (iResult > 0)
	//        printf("Bytes received: %d\n", iResult);
	//    else if (iResult == 0)
	//        printf("Connection closed\n");
	//    else
	//        printf("recv failed with error: %d\n", WSAGetLastError());

	//} while (iResult > 0);

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}