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

#define BUFFER_SIZE 1000


std::deque<Frame*> data_queue;

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
Semaphore Empty(5);

void P_Full()
{
	Full.Wait();
}
void V_Full()
{
	Full.Signal();
}
void P_Empty()
{
	Empty.Wait();
}
void V_Empty()
{
	Empty.Signal();
}

char memBuffer[5][1000];

void RecvThread()
{
	Frame* f;
	int now = -1;
	int now2 = 0;
	char buffer[1000] = { 0 };
	while (true)
	{
		//来源
		memset(buffer, 0, 1000);
		//scanf("%s", buffer);
		for (int i = 0; i < 100; i++)
		{
			now = (now + 1) % 10;
			memset(memBuffer[now % 5], 0, 1000);
			f = new Frame(memBuffer[now % 5]);
			// 后面全都要用托管的
			f->InitFrameStruct(Frame::frame, now, strlen(f->GetDataAddr()) + 1);
			if (rand() % 2 == 0)
			{
				f->AppendOPCode(Frame::ack);
			}

			if (rand() % 2 == 0)
			{
				*(f->head + rand() % f->total_len) <<= 2;
				// *(f->head + random() % f->total_len) <<= 2;
			}

			if (f->GetLabel() == now2 && f->VerifyCRC())
			{
				//是frame，同时也满足要求时，pushback
				P_Empty();
				data_queue.push_back(f);
				V_Full();
				now2 = (now2 + 1) % 10;
			}
			//失序的帧直接无视？
		}
	}
}

Frame GetFrame(char* buffer)
{
	P_Full();
	Frame f(buffer);
	if (buffer != NULL)
	{
		memcpy(f.GetDataAddr(), data_queue.front()->GetDataAddr(), data_queue.front()->GetDataLen());
	}
	f.InitFrameStruct(data_queue.front()->GetOPCode(), data_queue.front()->GetLabel(), data_queue.front()->GetDataLen());
	delete data_queue.front();
	data_queue.pop_front();//不知道这样写会不会有问题
	V_Empty();
	return f;
}

void Daemon_Thread(UDP_Socket* sock, const char* file_default_path)
{
	while (true)
	{
		errno_t err;
		FILE* fd = NULL;
		char buffer[BUFFER_SIZE];
		Frame f(buffer);
		int fst = 0;
		int read_len = 0;
		const char* file_name = file_default_path;
		while (true)
		{
			read_len = sock->ReciveFrom(f);
			f.InitDataLen(read_len);

			if (read_len > 0)
			{
				//尽量在这里设计个东西，让能进入下面环节的全是通过校验的帧
				switch (f.GetOPCode())
				{
				case Frame::frame:

					printf("接收文件中，大小:%d...\n", f.GetDataLen());
					if (fst == 0)
					{
						err = fopen_s(&fd, file_name, "wb+");
						fst = 1;
					}

					//在这里加一个

					if (fd == NULL)
					{
						printf("File Open Error\n");
					}
					else
					{
						fwrite(f.GetDataAddr(), sizeof(char), f.GetDataLen(), fd);
					}

					//结束
					if (read_len < BUFFER_SIZE)
					{
						//break;
						printf("File Get OK\n");
						if (fd != NULL)
						{
							fclose(fd);
						}
						fst = 0;
					}

					break;

				case Frame::ack:
					break;
				case Frame::exit:
					break;
				default:
					break;
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
	char buffer[BUFFER_SIZE];

	Frame f(buffer);

	int max_payload_len = BUFFER_SIZE - Frame::header_byte_len - 2;

	char file_name[200];
	printf("请输入要传输的文件路径:\n");
	std::cin >> file_name;
	//scanf_s("%s", file_name, 199);
	//fgets(file_name, 199, stdin);

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
		int read_len = fread(f.GetDataAddr(), sizeof(char), max_payload_len, stream);
		f.InitFrameStruct(Frame::frame, 1, read_len);
		if (sock->SendTo(f) != SOCKET_ERROR)
		{

		}
		else
		{
			printf("Error Occurs\n");
		}
		if (read_len < max_payload_len)
		{
			break;
		}
		Sleep(10);
	}

	fclose(stream);
}
