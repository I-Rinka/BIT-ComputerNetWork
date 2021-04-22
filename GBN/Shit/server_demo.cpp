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
	ESCPP_GBN* server = new ESCPP_GBN(PORT);


	while (1)
	{
		char buffer[21] = { 0 };
		int len = server->Core_Receive(buffer, 20);
		printf("%s", buffer);

		const char* str = "Hello Client, I am a Server! ";
		server->Core_Send(str, strlen(str));
	}

	delete server;
	return 0;
}