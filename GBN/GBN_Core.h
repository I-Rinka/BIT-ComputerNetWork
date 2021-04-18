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
#endif // !_GBN_CORE_