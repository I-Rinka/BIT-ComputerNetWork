#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<iostream>
#include<thread>
#include"../GBN_Core.h"

#define PORT "10488"
#define IP "127.0.0.1"

int main(int argc, char* argv[])
{
	ESCPP_GBN* client = new ESCPP_GBN(IP, PORT);

	const char* str = "Hello world! hahhahaha";
	client->Core_Send(str, strlen(str));

	char buffer[50] = { 0 };
	client->Core_Receive(buffer, 50);
	printf("%s", buffer);
	delete client;
	return 0;
}