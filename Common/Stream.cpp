#include "stdafx.h"

using namespace std;

Stream::Stream()
{
    _readOnly = false;
}

Stream::~Stream()
{
}

bool Stream::IsAtEnd() const
{
    return GetPosition() >= Size();
}

bool Stream::IsReadOnly() const
{
    return _readOnly;
}

vector<byte> Stream::ReadBytes(int size)
{
    vector<byte> result(size);
    ReadBytes(result.data(), size);
    return result;
}

char Stream::ReadChar()
{
    char c;
    ReadStruct(c);
    return c;
}

wchar_t Stream::ReadWChar()
{
    wchar_t c;
    ReadStruct(c);
    return c;
}

byte Stream::ReadByte()
{
    byte uc;
    ReadStruct(uc);
    return uc;
}

short Stream::ReadInt16()
{
    short s;
    ReadStruct(s);
    return s;
}

ushort Stream::ReadUInt16()
{
    ushort us;
    ReadStruct(us);
    return us;
}

int Stream::ReadInt32()
{
    int i;
    ReadStruct(i);
    return i;
}

uint Stream::ReadUInt32()
{
    uint ui;
    ReadStruct(ui);
    return ui;
}

int Stream::ReadVarInt32()
{
    int result = ReadByte();
    if ((result & 0x80) == 0)
        return result;

    result = ((result & 0x7F) << 8) | ReadByte();
    if ((result & 0x4000) == 0)
        return result;

    byte uc1 = ReadByte();
    byte uc0 = ReadByte();
    result = ((result & 0x3FFF) << 16) | (uc1 << 8) | uc0;
    return result;
}

__int64 Stream::ReadInt64()
{
    __int64 i;
    ReadStruct(i);
    return i;
}

unsigned __int64 Stream::ReadUInt64()
{
    unsigned __int64 ui;
    ReadStruct(ui);
    return ui;
}

float Stream::ReadSingle()
{
    float f;
    ReadStruct(f);
    return f;
}

double Stream::ReadDouble()
{
    double d;
    ReadStruct(d);
    return d;
}

wstring Stream::ReadUtf8StringZ()
{
    string str;
    char c;
    while ((c = ReadChar()) != '\0')
    {
        str += c;
    }
    
    return StringUtil::ToUTF16(str);
}

wstring Stream::ReadUtf8String(int length)
{
    string str;
    str.resize(length);
    ReadBytes(const_cast<char*>(str.data()), length);
    return StringUtil::ToUTF16(str);
}

wstring Stream::ReadVarUtf8String()
{
    return ReadUtf8String(ReadVarInt32());
}

wstring Stream::ReadUtf16StringZ()
{
    wstring wstr;
    wchar_t c;
    while ((c = ReadWChar()) != L'\0')
    {
        wstr += c;
    }
    return wstr;
}

wstring Stream::ReadUtf16String(int length)
{
    wstring wstr;
    wstr.resize(length);
    ReadBytes(const_cast<wchar_t*>(wstr.data()), length * sizeof(wchar_t));
    return wstr;
}

wstring Stream::ReadVarUtf16String()
{
    return ReadUtf16String(ReadVarInt32());
}

void Stream::Write(const vector<byte>& data)
{
    Write(data.data(), data.size());
}

void Stream::Write(char value)
{
    WriteStruct(value);
}

void Stream::Write(wchar_t value)
{
    WriteStruct(value);
}

void Stream::Write(byte value)
{
    WriteStruct(value);
}

void Stream::Write(short value)
{
    WriteStruct(value);
}

void Stream::Write(ushort value)
{
    WriteStruct(value);
}

void Stream::Write(int value)
{
    WriteStruct(value);
}

void Stream::Write(uint value)
{
    WriteStruct(value);
}

void Stream::WriteVar(int value)
{
    if (value <= 0x7F)
    {
        Write((byte)value);
    }
    else if (value <= 0x3FFF)
    {
        Write((byte)(0x80 | (value >> 8)));
        Write((byte)value);
    }
    else
    {
        Write((byte)(0xC0 | (value >> 24)));
        Write((byte)(value >> 16));
        Write((byte)(value >> 8));
        Write((byte)value);
    }
}

void Stream::Write(__int64 value)
{
    WriteStruct(value);
}

void Stream::Write(unsigned __int64 value)
{
    WriteStruct(value);
}

void Stream::Write(float value)
{
    WriteStruct(value);
}

void Stream::Write(double value)
{
    WriteStruct(value);
}

void Stream::WriteUtf8StringZ(const wstring& wstr)
{
    string str = StringUtil::ToUTF8(wstr);
    Write(str.data(), str.size());
    Write('\0');
}

void Stream::WriteVarUtf8String(const wstring& wstr)
{
    string str = StringUtil::ToUTF8(wstr);
    WriteVar(str.size());
    Write(str.data(), str.size());
}

void Stream::WriteUtf16StringZ(const wstring& wstr)
{
    Write(wstr.data(), wstr.size() * sizeof(wchar_t));
    Write(L'\0');
}

void Stream::WriteVarUtf16String(const wstring& wstr)
{
    WriteVar(wstr.size());
    Write(wstr.data(), wstr.size () * sizeof(wchar_t));
}

void Stream::Write(Stream& stream)
{
    int bytesLeft = stream.Size() - stream.GetPosition();
    byte buffer[0x1000];
    while (bytesLeft > 0)
    {
        int chunkSize = min(bytesLeft, (int)sizeof(buffer));
        stream.ReadBytes(buffer, chunkSize);
        Write(buffer, chunkSize);
        bytesLeft -= chunkSize;
    }
}
