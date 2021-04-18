#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<iostream>
#include<thread>
#include"../GBN_Core.h"

#define PORT "10488"
#define IP "127.0.0.1"

ESCPP_GBN* GBN;

int Send(const char* buffer, int length)
{
	char len[4] = { 0 };
	char* cursor = (char*)&length;
	for (int i = 0; i < 4; i++)
	{
		len[i] = *(cursor + i);
	}
	printf("send size: %d", length);
	//GBN->Core_Send(len, 4);
	//return GBN->Core_Send(buffer, length);
	return 0;
}
int Receive(char* buffer, int length)
{
	char len[5] = { 0 };
	//GBN->Core_Receive(len, 4);
	int size = 0;
	char* cursor = (char*)&size;
	for (int i = 0; i < 4; i++)
	{
		 *(cursor + i)= len[i];
	}
	printf("receive size: %d",size);
	//return GBN->Core_Receive(buffer,length);
	return 1;
}

int main(int argc, char* argv[])
{
	GBN = new ESCPP_GBN(PORT);

	while (1)
	{
		char buffer[21] = { 0 };
		Receive(buffer, 20);
		printf("%s", buffer);
		const char* str = "Hello Client, I am a Server! ";
		Send(str, strlen(str));
	}

	delete GBN;
	return 0;
}