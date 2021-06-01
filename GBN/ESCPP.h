#pragma once
#include"UDP_Core.h"
#include "Frame.h"
#include <stdio.h>
#include <deque>
#include <string.h>
#include <stdlib.h>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <string>
#include<time.h>

#define WINDOW_SIZE 5
#define LABEL_MAX_SIZE (WINDOW_SIZE*2)
char memBuffer[5][BUFFER_SIZE];

int SEND_LABEL = 0;

std::mutex WindowMutex;

Frame SendWindow[WINDOW_SIZE];
bool isACK[WINDOW_SIZE] = { true };
char SendBuffer[WINDOW_SIZE][BUFFER_SIZE];
clock_t timeStamp[WINDOW_SIZE];

std::deque<Frame*> data_queue;


void Init()
{

	for (int i = 0; i < WINDOW_SIZE; i++)
	{
		SendWindow[i].head = SendBuffer[i];
		isACK[i] = true;
	}

}
class Semaphore
{
public:
	explicit Semaphore(int count = 0) : count_(count)
	{
	}

	void Signal()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		++count_;
		cv_.notify_one();
	}

	void Wait()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		cv_.wait(lock, [=]
			{ return count_ > 0; });
		--count_;
	}

private:
	std::mutex mutex_;
	std::condition_variable cv_;
	int count_;
};
Semaphore Full(0);
Semaphore Empty(WINDOW_SIZE);

void P_Empty()
{
	Empty.Wait();
}
void V_Empty()
{
	Empty.Signal();
}

void Timer_Thread(UDP_Socket* sock)
{
	while (true)
	{
		Sleep(1000);
		WindowMutex.lock();
		for (int i = 0; i < WINDOW_SIZE; i++)
		{
			if (isACK[i] == false)
			{
				printf("��ʧ֡%d�ش�\n", SendWindow[i].GetLabel());
				sock->SendTo(SendWindow[i]);
			}
		}
		WindowMutex.unlock();
	}
}

void Daemon_Thread(UDP_Socket* sock, const char* file_default_path)
{
	Init();
	std::thread* th = new std::thread([&]() {Timer_Thread(sock);});
	while (true)
	{
		errno_t err;
		FILE* fd = NULL;
		char buffer[BUFFER_SIZE];
		Frame f(buffer);
		int file_first = 0;
		int LABEL_expected = 0;
		int read_len = 0;
		const char* file_name = file_default_path;
		while (true)
		{
			read_len = sock->ReciveFrom(f);
			f.InitDataLen(read_len);

			if (read_len > 0)
			{
				//������������Ƹ����������ܽ������滷�ڵ�ȫ��ͨ��У���֡

				if (f.VerifyCRC())
				{

					switch (f.GetOPCode())
					{
					case Frame::frame:
						if (f.GetLabel() == LABEL_expected)
						{
							//printf("�����ļ��У���С:%d...\n", f.GetDataLen());
							if (file_first == 0)
							{
								err = fopen_s(&fd, file_name, "wb+");
								file_first = 1;
							}

							//�������һ��

							if (fd == NULL)
							{
								printf("File Open Error\n");
							}
							else
							{
								fwrite(f.GetDataAddr(), sizeof(char), f.GetDataLen(), fd);

								//printf("����ACK֡..\n");
								//ACK
								char ack[10];
								Frame f(ack);
								f.InitFrameStruct(Frame::ack, LABEL_expected, 0);
								sock->SendTo(f);
							}

							//����
							if (read_len < BUFFER_SIZE)
							{
								//break;
								printf("File Get OK\n");
								if (fd != NULL)
								{
									fclose(fd);
								}
								file_first = 0;
							}

							LABEL_expected = (LABEL_expected + 1) % LABEL_MAX_SIZE; //�κ�ͨ��У���֡������ɹ�
						}
						else
						{
							printf("ʧ��֡������\n");
						}

						break;

					case Frame::ack:
						WindowMutex.lock();
						//printf("֡%dAck\n", f.GetLabel());
						isACK[f.GetLabel() % WINDOW_SIZE] = true;//����������һ��ȡ��֮ǰ��ACK�Ĺ���
						for (int i = 0; i < f.GetLabel() % WINDOW_SIZE; i++)
						{
							if (timeStamp[i] < timeStamp[f.GetLabel() % WINDOW_SIZE]) //ʱ���С�ģ�ȫ��Ҳ���ӵ���
							{
								isACK[i] = true;
								V_Empty();
							}
						}
						WindowMutex.unlock();
						V_Empty();
						break;
					case Frame::exit:
						break;
					default:
						break;
					}
				}
				else
				{
					printf("����֡������\n");
				}
			}
			else
			{

			}
		}
	}
}


void Mode_SendFile(UDP_Socket* sock)
{
	int max_payload_len = BUFFER_SIZE - Frame::header_byte_len - 2;

	char file_name[200];
	printf("������Ҫ������ļ�·��:\n");
	std::cin >> file_name;

	errno_t err;
	FILE* stream;
	file_name[199] = 0;
	err = fopen_s(&stream, file_name, "rb");

	if (stream == NULL)
	{
		printf("File Do not Exsist!\n");
		return;
	}

	while (true)
	{
		P_Empty();
		WindowMutex.lock();
		Frame& f = SendWindow[SEND_LABEL % WINDOW_SIZE];
		int read_len = fread(f.GetDataAddr(), sizeof(char), max_payload_len, stream);
		isACK[SEND_LABEL % WINDOW_SIZE] = false;
		timeStamp[SEND_LABEL % WINDOW_SIZE] = clock();
		f.InitFrameStruct(Frame::frame, SEND_LABEL, read_len);
		WindowMutex.unlock();


		if (sock->SendTo(f) != SOCKET_ERROR)
		{
			//printf("����֡:%d\n", f.GetLabel());
			SEND_LABEL = (SEND_LABEL + 1) % LABEL_MAX_SIZE;
		}
		else
		{
			printf("Error Occurs\n");
		}
		if (read_len < max_payload_len)
		{
			break;
		}
	}

	Init();

	fclose(stream);
}
