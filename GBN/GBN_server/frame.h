#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"GBN_config.h"
#ifndef _FRAME_H
#define _FRAME_H


class Frame
{
public:
	static const short crc_divisor = GBN_ESCPP_CONFIG::CRC_Divisor;
	static const short content_max_length = GBN_ESCPP_CONFIG::Data_Size;
	static const short head_length = 3;//in byte
	static const int buffer_recommand_size = head_length + content_max_length;

	Frame(char* saved_buffer);
	Frame(char* buffer, short frame_number, const char* content, short content_size);
	//数据帧  buffer最好是Data_Size+content_max_length+1
	//操作帧
	Frame(char* buffer, short op_code, short op_frame);
	~Frame();
	static char GetFrameNumber(char* buffer_head);
	static short GetByteCount(char* buffer_head);
	static char GetOpCode(char* buffer_head);
	static char GetOpFrame(char* buffer_head);

	short GetByteCount();
	char GetFrameNumber();
	char GetOpCode();
	char GetOpFrame();
	void CopyContentTo(char* destination);
	bool CheckSum();
	char* frame_buffer;
	const char* PeekContent(int* content_size);
private:
	//base+content_length即为CRC需要加上去的第一个元素
	void AppendCRC(char CRC_divisor, int content_length);
};


inline Frame::Frame(char* saved_buffer)
{
	this->frame_buffer = saved_buffer;
}

inline Frame::Frame(char* buffer, short frame_number, const char* content, short content_size)
{
	this->frame_buffer = buffer;
	//short len = (short)strlen(content)+1;//strlen是不包括0的，所以这个字节要手动加上
	//因为不光是字符串，还有可能是二进制数据，所以不能用strlen
	short len = content_size;
	if (len > this->content_max_length)
	{
		len = content_max_length;
	}

	memcpy(buffer + head_length, content, len);//不要使用strcpy

	buffer[0] = (char)frame_number;

	short* lp_byte_count = (short*)(buffer + 1);
	*lp_byte_count = len;

	//复制内容
	AppendCRC(this->crc_divisor, this->head_length+len);
}

inline Frame::Frame(char* buffer, short op_code, short op_frame)
{
	this->frame_buffer = buffer;
	buffer[0] = -1;
	buffer[1] = (char)op_code;
	buffer[2] = (char)op_frame;

	//复制内容
	AppendCRC(this->crc_divisor, this->head_length);
}

Frame::~Frame()
{
}

char Frame::GetFrameNumber(char* buffer_head)
{
	return buffer_head[0];
}

short Frame::GetByteCount(char* buffer_head)
{
	short* lp_byte_count = (short*)(buffer_head + 1);
	return *lp_byte_count;
}

char Frame::GetOpCode(char* buffer_head)
{
	return buffer_head[1];
}

char Frame::GetOpFrame(char* buffer_head)
{
	return buffer_head[2];
}

short Frame::GetByteCount()
{
	return GetByteCount(this->frame_buffer);
}

char Frame::GetFrameNumber()
{
	return GetFrameNumber(this->frame_buffer);
}

char Frame::GetOpCode()
{
	return GetOpCode(this->frame_buffer);
}

char Frame::GetOpFrame()
{
	return GetOpFrame(this->frame_buffer);
}

void Frame::CopyContentTo(char* destination)
{
}

bool Frame::CheckSum()
{
	return false;
}

const char* Frame::PeekContent(int* content_size)
{
	if (content_size != NULL)
	{
		*content_size = this->GetByteCount(this->frame_buffer);
	}
	return this->frame_buffer + head_length;
}

void Frame::AppendCRC(char CRC_divisor, int content_length)
{
}

#endif // !_FRAME_H
