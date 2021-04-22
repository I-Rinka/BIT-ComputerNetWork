#pragma once
void InvertUint8(unsigned char* origin, unsigned char* dest)
{
    unsigned char a = *origin;
    unsigned char one = 0x01;
    unsigned char ans = 0;
    for (int i = 0; i < 8; i++)
    {
        unsigned char tmp = a & one;
        ans = ans | (tmp << (7 - i));
        a >>= 1;
    }
    *dest = ans;
}
void InvertUint16(unsigned short* origin, unsigned short* dest)
{
    unsigned short a = *origin;
    unsigned short one = 0x0001;
    unsigned short ans = 0;
    for (int i = 0; i < 16; i++)
    {
        unsigned short tmp = a & one;
        ans = ans | (tmp << (15 - i));
        a >>= 1;
    }
    *dest = ans;
}
unsigned short CRC16_CCITT(unsigned char* data, unsigned int data_len)
{
    unsigned short ans = 0x0000;
    unsigned short CRC_poly = 0x1021;
    unsigned char cursor = 0;

    while (data_len--)
    {
        cursor = *(data++);
        InvertUint8(&cursor, &cursor);
        ans ^= (cursor << 8);
        for (int i = 0; i < 8; i++)
        {
            if (ans & 0x8000)
                ans = (ans << 1) ^ CRC_poly;
            else
                ans = ans << 1;
        }
    }
    InvertUint16(&ans, &ans);
    return (ans);
}
bool VerifyCRC_CCITT(unsigned char* data, unsigned int data_len)
{
    unsigned short ans = 0x0000;
    unsigned short CRC_poly = 0x1021;
    unsigned char cursor = 0;

    while (data_len--)
    {
        cursor = *(data++);
        InvertUint8(&cursor, &cursor);
        ans ^= (cursor << 8);
        for (int i = 0; i < 8; i++)
        {
            if (ans & 0x8000)
                ans = (ans << 1) ^ CRC_poly;
            else
                ans = ans << 1;
        }
    }
    InvertUint16(&ans, &ans);
    if (!ans)
    {
        return true;
    }
    return false;
}

class Frame
{
private:
    int header_byte_len = 4;
    int opcode_byte_len = header_byte_len / 2;

public:
    enum OP
    {
        frame,
        ack,
        exit,
        resend //保留值
    };

    int data_len = 0;
    int total_len = 0;
    char* head;
    unsigned short mark = 0;
    void AppendLabel(unsigned short label)
    {
        unsigned short* lp = (unsigned short*)(head + opcode_byte_len);
        *(lp) = label;
    }
    unsigned short GetLabel()
    {
        //小端法
        unsigned short* lp = (unsigned short*)(head + opcode_byte_len);
        return (*lp);
    }
    void AppendOPCode(int OP)
    {
        unsigned short* lp = (unsigned short*)(head);
        *lp = (unsigned short)OP;
    }
    unsigned short GetOPCode()
    {
        unsigned short* lp = (unsigned short*)(head);
        return *lp;
    }
    char* GetDataAddr()
    {
        return head + this->header_byte_len;
    }
    void AppendCRC16()
    {
        unsigned short CRC_num = CRC16_CCITT((unsigned char*)this->head, header_byte_len + data_len);
        *(unsigned short*)(this->head + header_byte_len + data_len) = CRC_num;
    }
    bool VerifyCRC()
    {
        return VerifyCRC_CCITT((unsigned char*)this->head, header_byte_len + data_len + 2);
    }
    unsigned short GetCRCNum()
    {
        return *((unsigned short*)(head + data_len + header_byte_len));
    }
    int PutDataLen(int frame_len)
    {
        total_len = frame_len;
        data_len = (frame_len - header_byte_len - 2); //去头和去尾
        return data_len;
    }

    void InitFrameStruct(int OP_code, int frame_label, int data_len)
    {
        AppendOPCode(OP_code);
        if (OP_code == frame)
        {
            AppendLabel(frame_label);
            this->total_len = data_len + 2 + this->header_byte_len;
            this->data_len = data_len;
        }
        else if (OP_code == ack)
        {
            AppendLabel(frame_label);
            this->total_len = this->header_byte_len + 2;
            this->data_len = 0;
        }
        else if (OP_code == exit)
        {
            this->total_len = this->header_byte_len + 2;
            AppendLabel(0);
            this->data_len = 0;
        }
        else if (OP_code == resend)
        {
            AppendLabel(frame_label);
        }
        AppendCRC16();
    }

    Frame(char* buffer);
    Frame(char* buffer, int data_len);
    ~Frame();
};

Frame::Frame(char* buffer)
{
    this->head = buffer;
}

Frame::Frame(char* buffer, int data_len)
{
    this->data_len = data_len;
    this->head = buffer;
}

Frame::~Frame()
{
}