#pragma once
#include"Frame.h"
#include <windows.h>
class Error_emulator
{
public:
	Error_emulator();
	~Error_emulator();

	int GetErrorCounter() {
		return error_counter;
	}

	void TryMakeFrameBitsError(Frame& f)
	{
		if (error_counter % ErroRate == 0)
		{
			*(f.head + rand() % f.total_len) <<= 2;
		}
		error_counter++;
	}
private:
	int error_counter;
};



Error_emulator::Error_emulator()
{
}

Error_emulator::~Error_emulator()
{
}