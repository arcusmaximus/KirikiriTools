#include "stdafx.h"

using namespace std;

byte*   CodeBuffer::BufferPtr;
int     CodeBuffer::BufferLength;
byte*   CodeBuffer::Ptr;
int     CodeBuffer::LengthRemaining;

void CodeBuffer::Init()
{
    BufferLength = 0x1000;
    BufferPtr = (byte *)VirtualAlloc(nullptr, BufferLength, MEM_COMMIT, PAGE_EXECUTE);
    
    Ptr = BufferPtr;
    LengthRemaining = BufferLength;
}

void* CodeBuffer::GetCallToJmpAdapter(void* pTarget)
{
    void* pAdapter = Ptr;
    
    BeginWrite();

    // mov dword ptr [esp], pTarget
    WriteByte(0xC7);
    WriteByte(0x04);
    WriteByte(0xE4);
    WritePointer(pTarget);

    // ret
    WriteByte(0xC3);

    EndWrite();
    return pAdapter;
}

void CodeBuffer::BeginWrite()
{
    DWORD oldProtect;
    VirtualProtect(BufferPtr, BufferLength, PAGE_READWRITE, &oldProtect);
}

void CodeBuffer::WriteByte(byte b)
{
    Write(&b, sizeof(b));
}

void CodeBuffer::WritePointer(void* ptr)
{
    Write(&ptr, sizeof(ptr));
}

void CodeBuffer::Write(void* pData, int size)
{
    if (size > LengthRemaining)
        throw exception("No more space left in code buffer");

    memcpy(Ptr, pData, size);
    Ptr += size;
    LengthRemaining -= size;
}

void CodeBuffer::EndWrite()
{
    DWORD oldProtect;
    VirtualProtect(BufferPtr, BufferLength, PAGE_EXECUTE_READ, &oldProtect);
}
