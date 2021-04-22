#include<stdio.h>
#include<stdlib.h>
#include"frame.h"
int main()
{
	char* buffer = (char*)malloc(Frame::buffer_recommand_size);
	char* temp_buffer = (char*)calloc(Frame::buffer_recommand_size, 1);

	for (int i = 0; i < 10; i++)
	{
		fgets(temp_buffer, Frame::buffer_recommand_size, stdin);//这fgets会读入末尾的空格

		Frame* frame = new Frame(buffer, i * 5 % 9, temp_buffer, strlen(temp_buffer) + 1);//永远注意strlen结尾有个\0！！！所以帧的长度要+1

		printf("frame number: %d, frame size: %d,%x\n", frame->GetFrameNumber(), (int)frame->GetByteCount(), frame->GetByteCount());

		for (int i = 0; i < frame->GetByteCount(); i++)
		{
			temp_buffer[i] = *(frame->PeekContent(NULL) + i);
		}
		printf("%s", temp_buffer);

		char op_buffer[4];//head是3
		Frame *op_frame;
		op_frame = new Frame(op_buffer, i % 3 + 1, i * 5 % 9);

		printf("this frame:%d ", i * 5 % 9);

		if (i % 3 == 0)
		{
			printf("ACK");
		}
		else if (i % 3 == 1)
		{
			printf("Resend");
		}
		else
		{
			printf("Exit");
		}

		Frame frame_saved(op_buffer);

		printf(" frame number:%d op_code:%d op_frame:%d,%x\n", frame_saved.GetFrameNumber(),frame_saved.GetOpCode() ,frame_saved.GetOpFrame(), frame_saved.GetOpFrame());

	}
}