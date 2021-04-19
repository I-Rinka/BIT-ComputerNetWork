#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<iostream>
#include<thread>
#include"../GBN_Core.h"

#define PORT "10488"
#define IP "127.0.0.1"

ESCPP_GBN* GBN;


int main(int argc, char* argv[])
{
	GBN = new ESCPP_GBN(PORT);
	char buffer[ESCPP_GBN::packet_length];

	std::thread th([&]() {
		char send_buffer[ESCPP_GBN::packet_length] = { 0 };
		while(true)
		{
			scanf_s("%s", buffer, ESCPP_GBN::packet_length);
			GBN->Send(send_buffer,strlen(send_buffer));
		}
		});

	while (1)
	{
		int len=GBN->Recv(buffer);
		printf("%s", buffer);
	}

	delete GBN;
	return 0;
}