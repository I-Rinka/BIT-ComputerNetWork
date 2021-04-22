#pragma once
#include"UDP_Core.h"
#include<stdio.h>
int GetFile(const char* file_name, UDP_Socket* sock)
{
	while (true)
	{
		errno_t err;
		FILE* fd=NULL;
		char buffer[1000];
		int fst = 0;
		int read_len = 0;
		while (true)
		{
			read_len = sock->ReciveFrom(buffer, sizeof(buffer));
			if (read_len > 0)
			{
				if (fst == 0)
				{
					err = fopen_s(&fd, file_name, "wb+");
					fst = 1;
					if (fd == NULL)
					{
						return -1;
					}
				}
				else
				{
					fwrite(buffer, sizeof(char), read_len, fd);
				}
				if (read_len < sizeof(buffer))
				{
					break;
				}
			}
			else
			{

			}
		}
		printf("File Get OK");
		fclose(fd);
	}
	return 0;
}

int PutFile(const char* file_name, UDP_Socket* sock)
{
	char buffer[1000];

	errno_t err;
	FILE* stream;
	err = fopen_s(&stream, file_name, "rb");

	if (stream == NULL)
	{
		return -1;
	}

	while (true)
	{
		int read_len = fread(buffer, sizeof(char), sizeof(buffer), stream);
		if (sock->SendTo(buffer, read_len) != SOCKET_ERROR)
		{
			//printf("file send...\n");
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