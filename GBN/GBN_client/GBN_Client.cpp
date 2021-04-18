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
	GBN->Core_Send(len, 4);
	return GBN->Core_Send(buffer, length);
}
int Receive(char* buffer, int length)
{
	char len[5] = { 0 };
	GBN->Core_Receive(len, 4);
	int size = 0;
	char* cursor = (char*)&size;
	for (int i = 0; i < 4; i++)
	{
		*(cursor + i) = len[i];
	}
	printf("receive size: %d", size);
	return GBN->Core_Receive(buffer, length);
}
int main(int argc, char* argv[])
{
	GBN = new ESCPP_GBN(IP, PORT);

	//const char* str = "Hello world! hahhahaha";

	while (1)
	{
		char buffer[50] = { 0 };
		scanf_s("%s", buffer,50);
		Send(buffer, strlen(buffer));


		memset(buffer, 0, 50);
		Receive(buffer, 50);
		printf("%s", buffer);

	}
	delete GBN;
	return 0;
}