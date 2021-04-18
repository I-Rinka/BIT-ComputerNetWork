#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include<iostream>
#include<thread>
#include"GBN_Core.h"
#pragma comment(lib,"ws2_32.lib")


inline int ESCPP_GBN::Send(const char* buffer, int send_length)
{
	return 0;
}
inline int ESCPP_GBN::Recv(char* buffer)
{
	return 0;
}


//pay attention: the frame_number should never exceed 2^8
void ESCPP_GBN::AppendFrameNumber(char* buffer, int frame_number)
{
	*buffer = (short)frame_number;
}

int ESCPP_GBN::GetFrameNumber(const char* buffer)
{
	return (int)*buffer;
}

void ESCPP_GBN::AppendCRCNumber(char* buffer)
{
}

bool ESCPP_GBN::CheckSum(char* buffer)
{
	return true;
}

void ESCPP_GBN::AppendLengthNumber(char* buffer, int length_number)
{
	char* len = buffer + 2;
	char* cursor = (char*)&length_number;
	for (int i = 0; i < 4; i++)
	{
		len[i] = *(cursor + i);
	}
}

int ESCPP_GBN::GetLengthNumber(const char* buffer)
{
	const char* len = buffer + 2;
	int size = 0;
	char* int_cursor = (char*)&size;
	for (int i = 0; i < 4; i++)
	{
		*(int_cursor + i) = len[i];
	}
	return size;
	return 0;
}

//需要思考一下发送队列和发送操作的锁应该如何施加
void ESCPP_GBN::RecvThread()
{
	char header[6];
	this->Core_Receive(header, 6);
	int frame_number = this->GetFrameNumber(header);
	if (frame_number == -1)
	{
		if (CheckSum(header))
		{
			int op_number = header[2];
			if (op_number == 1)
			{
				//顺利接收，对应帧置ack，同时将窗口疯狂后移
				this->ack[header[3] % window_size] = true;//对应帧号置ack
				for (int i = 0; i < this->send_queue.size(); i++)
				{
					int ack_frame = GetFrameNumber(send_queue.front()) % window_size;
					if (ack[ack_frame])
					{
						//WaitForSingleObject(this->send_queue_mutex, INFINITE);//申请修改发送队列
						this->Core_Send(send_queue.front(), this->GetLengthNumber(send_queue.front()));
						void_queue.push(this->send_queue.front());
						send_queue.pop_front();

						ack[ack_frame] = false;//窗口已后移，ack置false

						//ReleaseSemaphore(this->send_queue_empty, 1, NULL);//先释放一个empty再释放队列
						ReleaseMutex(this->send_queue_mutex);
					}
					else
					{
						break;//如果最前头的帧没有ack，那么窗口不移动
					}
				}
			}
			else if (op_number == 2)
			{
				//重发指定帧
				WaitForSingleObject(this->send_mutex, INFINITE);

				//推掉指定帧前的所有帧，并重发指定帧
				int frame_to_send = header[3] % window_size;
				for (int i = 0; i < this->send_queue.size(); i++)
				{
					//WaitForSingleObject(this->send_queue_mutex, INFINITE);//申请修改发送队列
					if (GetFrameNumber(send_queue.front())%window_size==frame_to_send)
					{
						this->Core_Send(send_queue.front(), this->GetLengthNumber(send_queue.front()));
						break;
					}
					void_queue.push(this->send_queue.front());
					send_queue.pop_front();

					ReleaseSemaphore(this->send_queue_empty,1, NULL);
					//ReleaseMutex(this->send_queue_mutex);
				}

				ReleaseMutex(this->send_mutex);
			}
			else if (op_number == 3)
			{
				//退出线程
				exit(0);
			}
			else
			{
				//重发窗口内的所有帧，不修改内容感觉可以不加锁
				for (int i = 0; i < this->send_queue.size(); i++)
				{
					WaitForSingleObject(this->send_mutex, INFINITE);
					this->Core_Send(send_queue.at(i), this->GetLengthNumber(send_queue.at(i)));
					ReleaseMutex(this->send_mutex);
					
				}
			}
		}
		//操作码如果不对的话,我们也不知道它想干啥,直接无视就行了. 因为如果我方迟迟不收到ack,timer到期后可全发送一遍现有帧
	}
	else
	{
		if (frame_number == this->recv_require_frame)
		{
			// 返回一个ACK,然后把收到的内容给到buffer,并返还给用户.
		}
		else
		{
			// 要求重传
		}
	}
}

void ESCPP_GBN::SendThread()
{
}

inline void ESCPP_GBN::CoreInitializer()
{
	this->send_mutex = CreateMutex(NULL, FALSE, NULL);
	this->send_queue_mutex= CreateMutex(NULL, FALSE, NULL);
	this->send_queue_empty = CreateSemaphore(NULL, window_size, window_size, NULL);
	this->send_queue_full = CreateSemaphore(NULL, 0, window_size, NULL);
	this->recv_buffer_mutex = CreateMutex(NULL, FALSE, NULL);

	//initialize memory
	for (int i = 0; i < window_size; i++)
	{
		char* temp = (char*)malloc(packet_length + header_length);
		void_queue.push(temp);
	}

	this->recv_buffer = (char*)malloc(packet_length + header_length);

	this->recv_thread = new std::thread([this]() {RecvThread();});
	this->recv_thread->detach();

	this->send_thread = new std::thread([this]() {SendThread();});
	this->send_thread->detach();
}

// Don't care about what I have wrote here (unless it crashes at there
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

	this->CoreInitializer();

}
// Don't care about what I have wrote here (unless it crashes at there
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
	this->CoreInitializer();
}
// Don't care about what I have wrote here (unless it crashes at there
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

	delete this->recv_thread;

	WaitForSingleObject(this->send_mutex, 1000);
	TerminateThread(this->send_thread->native_handle(), 0);
	delete this->send_thread;

	CloseHandle(this->send_mutex);
	CloseHandle(this->recv_buffer_mutex);
	CloseHandle(this->send_queue_full);
	CloseHandle(this->send_queue_empty);
}

// Don't care about what I have wrote here (unless it crashes at there
int ESCPP_GBN::Core_Send(const char* message, int send_length)
{
	int iSendResult = -1;
	if (send_length > this->packet_length)
	{
		send_length = packet_length;
	}
	if (this->is_passive)
	{
		int	clnt_addr_size = sizeof(this->clnt_addr);
		iResult = sendto(this->connectSocket, message, packet_length, 0, (SOCKADDR*)&clnt_addr, clnt_addr_size);
	}
	else
	{
		iSendResult = send(this->connectSocket, message, send_length, 0);
	}
	return iSendResult;
}

// Don't care about what I have wrote here (unless it crashes at there
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