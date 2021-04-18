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
	client->Core_Send("Hello!",strlen("Hello!"));
	delete client;
	return 0;
}