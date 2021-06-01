#pragma once
#ifndef _CONFIG_H_
#define _CONFIG_H_
#define DataSize 32768
#define ErroRate 11
#define LostRate 11
#define SWSize 5
#define InitSeqNo 0
#define Timeout 50


#define WINDOW_SIZE SWSize
#define LABEL_MAX_SIZE (WINDOW_SIZE*2)
#define MAX_FRAME DataSize
//#define UDPPORT 0488 学号后四位没办法，小组成员都是0开头


#endif // !_CONFIG_H_

