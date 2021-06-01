#pragma once
#ifndef _GBN_CORE_
#define _GBN_CORE_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include<iostream>
#include <vector>
#include <queue>
#include <thread>
#pragma comment(lib,"ws2_32.lib")


class ESCPP_GBN
{

public:
	static const int crc_divisor = 0x7D;//They live in 7D so I try to make them happy :)
	static const int packet_length = 500;//in byte
	static const int header_length = 6;//in byte
	static const int window_size = 5;
	static const int expire_timer = 10;// in ms

	ESCPP_GBN(const char* target_ip, const char* port);
	ESCPP_GBN(const char* port);
	~ESCPP_GBN();

	int Send(const char* buffer, int send_length);
	
	// Because user have to make the receive buffer larger than the packet_lenth (if not the memory will be corrupted), so we always assume the max lenght is packet_length.
	// It returns the actual length recived from socket. If it returns -1, some error occurs. If it returns 0, the recive should be stop.
	int Recv(char* buffer);

	std::thread* send_thread;
	std::thread* recv_thread;
	std::thread* timer_thread;

private:
	void CoreInitializer();

	bool window_moved = true;
	bool stop_recv = false;

	// This will become a private function
	/* char* message is the message buffer you want to send. int send_length is the message length you want to send, it should never exceed the length of this->packet_lenth.
	 The function returns -1 means error occurs. Returns 0 means successfully sended */
	int Core_Send(const char* message, int send_length);

	// This will become a private function
	/* char* message is the message buffer you want to send. int buffer_length is the maxium of message in order to prevent boundary exceed.
	 The function returns data length that the socket received */
	int Core_Receive(char* message, int buffer_length);

	bool is_passive = false;// Server is passive connection.
	WSADATA wsaData;
	SOCKET connectSocket = INVALID_SOCKET;
	SOCKADDR_IN clnt_addr;
	//client side

	std::deque<unsigned long long>time_stamp_queue;
	unsigned long long timer_time_stamp;

	std::deque<char*>send_queue;
	std::queue<char*>void_queue;
	bool ack[ESCPP_GBN::window_size] = { false };
	
	char* recv_buffer = NULL;

	int send_frame_number = 0;// This maximum size should be window_size*2
	int recv_require_frame_number = 0;
	
	char* send_frame_next = NULL;

	int iResult;//socket connection result

	HANDLE send_mutex;
	HANDLE send_queue_mutex;
	HANDLE send_queue_full;
	HANDLE send_queue_empty;
	HANDLE recv_buffer_semaphore;// I think using semaphore would be better, maybe future the recv_buffer can using queue to store multiple message
	HANDLE recv_buffer_mutex;

	void AppendFrameNumber(char* buffer, int frame_number);
	int GetFrameNumber(const char* buffer);

	void AppendCRCNumber(char* buffer);//It automaticlly general a checksum (but passes the void CRC filed) and then append the number to the CRC filed.
	bool CheckSum(char*buffer);//CheckSum will jump over the CRC code field, but it will utilize length from the length field.


	void AppendLengthNumber(char* buffer, int length_number);
	int GetLengthNumber(const char* buffer);


	void RecvThread();
	void SendThread();
	void TimerThread();

	//time out: only send has a timer. When the timer expires, sender would resend all the frames in the queue.

};

#endif // !_GBN_CORE_