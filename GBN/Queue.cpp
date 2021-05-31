#include "Frame.h"
#include <deque>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <string>
#include <unistd.h>

std::deque<Frame *> data_queue;

class Semaphore
{
public:
    explicit Semaphore(int count = 0) : count_(count)
    {
    }

    void Signal()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        ++count_;
        cv_.notify_one();
    }

    void Wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [=]
                 { return count_ > 0; });
        --count_;
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
};
Semaphore Full(0);
Semaphore Empty(5);

void P_Full()
{
    Full.Wait();
}
void V_Full()
{
    Full.Signal();
}
void P_Empty()
{
    Empty.Wait();
}
void V_Empty()
{
    Empty.Signal();
}

char memBuffer[5][1000];

void RecvThread()
{
    Frame *f;
    int now = -1;
    int now2 = 0;
    char buffer[1000] = {0};
    while (true)
    {
        //来源
        memset(buffer, 0, 1000);
        scanf("%s", buffer);
        for (int i = 0; i < 100; i++)
        {
            now = (now + 1) % 10;
            memset(memBuffer[now % 5], 0, 1000);
            f = new Frame(memBuffer[now % 5]);
            // 后面全都要用托管的
            sprintf(f->GetDataAddr(), "%d%s", now, buffer);
            f->InitFrameStruct(Frame::frame, now, strlen(f->GetDataAddr()) + 1);
            if (random() % 2 == 0)
            {
                f->AppendOPCode(Frame::ack);
            }

            if (random() % 2 == 0)
            {
                *(f->head + random() % f->total_len) <<= 2;
                // *(f->head + random() % f->total_len) <<= 2;
            }

            if (f->GetLabel() == now2 && f->VerifyCRC())
            {
                //是frame，同时也满足要求时，pushback
                P_Empty();
                data_queue.push_back(f);
                V_Full();
                now2 = (now2 + 1) % 10;
            }
            //失序的帧直接无视？
        }
    }
}

Frame GetFrame()
{
    Frame *f;
    char *buffer = (char *)malloc(1000);
    while (true)
    {
        usleep(50000);
        P_Full();
        f = data_queue.front();
        memcpy(buffer, f->GetDataAddr(), f->GetDataLen());
        data_queue.pop_front();
        V_Empty();
        printf("data:%s,size:%d,CRC:%d,Hex:0x", buffer, f->GetDataLen(), f->GetCRCNum());
        for (int i = 0; i < f->GetDataLen() + 4; i++)
        {
            printf("%x", *(f->head + i));
        }
        printf("\n");

        delete f;
    }
}

int main(int argc, char const *argv[])
{
    std::thread *th = new std::thread([]()
                                      { RecvThread(); });
    GetFrame();
    return 0;
}
