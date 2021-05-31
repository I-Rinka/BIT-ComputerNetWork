#pragma once
#include"UDP_Core.h"
#include<stdio.h>

#define BUFFER_SIZE 1000

void Daemon_Thread(UDP_Socket* sock, const char* file_default_path)
{
	while (true)
	{
		errno_t err;
		FILE* fd = NULL;
		char buffer[1000];
		int fst = 0;
		int read_len = 0;
		const char* file_name = file_default_path;
		while (true)
		{
			read_len = sock->ReciveFrom(buffer, sizeof(buffer));
			Sleep(100);
			if (read_len > 0)
			{
				if (fst == 0)
				{
					err = fopen_s(&fd, file_name, "wb+");
					fst = 1;
				}

				if (fd == NULL)
				{
					printf("File Open Error\n");
				}
				else
				{
					fwrite(buffer, sizeof(char), read_len, fd);
				}

				if (read_len < sizeof(buffer))
				{
					//break;
					printf("File Get OK\n");
					if (fd != NULL)
					{
						fclose(fd);
					}
					fst = 0;
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
	char file_name[200];
	printf("请输入要传输的文件路径:\n");
	scanf_s("%s", file_name, 199);
	//fgets(file_name, 199, stdin);

	errno_t err;
	FILE* stream;
	err = fopen_s(&stream, file_name, "rb");


	if (stream == NULL)
	{
		return;
	}

	while (true)
	{
		int read_len = fread(buffer, sizeof(char), sizeof(buffer), stream);
		Sleep(100);
		if (sock->SendTo(buffer, read_len) != SOCKET_ERROR)
		{

		}
		else
		{
			//Sleep(10);
			printf("Error Occurs\n");
			//break;
		}
		if (read_len < sizeof(buffer))
		{
			break;
		}
	}

	fclose(stream);
}