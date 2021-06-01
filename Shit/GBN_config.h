#pragma once
#ifndef _GBN_CONFIG_H_
#define _GBN_CONFIG_H_
namespace GBN_ESCPP_CONFIG {
	static const char CRC_Divisor = 0x7D;
	static const char* UDP_Port = "8888";
	static const char* IP_Addr = "127.0.0.1";
	static const short Data_Size = 1024;
	static const int Error_Rate = 0;
	static const int Lost_Rate = 0;
	static const int Send_Window_Size = 4;
	static const int Init_Seq_No = 0;
	static const int Time_Out = 10;//in ms
}
#endif // !_GBN_CONFIG_H_
