#include "stdafx.h"

void* MemoryUtil::FindByte(void* pStart, int length, byte value)
{
    byte* pByte = (byte*)pStart;
    byte* pEnd = pByte + length;
    for (; pByte < pEnd; pByte++)
    {
        if (*pByte == value)
            return pByte;
    }
    return nullptr;
}

void** MemoryUtil::FindAlignedPointer(void* pStart, int length, void* value)
{
    void** pPointer = (void **)pStart;
    void** pEnd = pPointer + length / sizeof(void*);
    for (; pPointer < pEnd; pPointer++)
    {
        if (*pPointer == value)
            return pPointer;
    }
    return nullptr;
}

void* MemoryUtil::FindData(void* pHaystack, int haystackLength, void* pNeedle, int needleLength)
{
    byte* pTest = (byte*)pHaystack;
    byte* pEnd = pTest + haystackLength - needleLength;
    for (; pTest <= pEnd; pTest++)
    {
        if (memcmp(pTest, pNeedle, needleLength) == 0)
            return pTest;
    }
    return nullptr;
}

void* MemoryUtil::FindData(void* pHaystack, int haystackLength, void* pNeedle, void* pNeedleMask, int needleLength)
{
    byte* pTest = (byte*)pHaystack;
    byte* pEnd = pTest + haystackLength - needleLength;
    for (; pTest <= pEnd; pTest++)
    {
        int offset = 0;
        bool failed = false;
        for (; offset < (needleLength & ~3); offset += 4)
        {
            uint test   = *(uint*)(pTest + offset);
            uint needle = *(uint*)((byte*)pNeedle + offset);
            uint mask   = *(uint*)((byte*)pNeedleMask + offset);
            if ((test & mask) != needle)
            {
                failed = true;
                break;
            }
        }
        if (failed)
            continue;

        for (; offset < needleLength; offset++)
        {
            byte test   = pTest[offset];
            byte needle = *((byte*)pNeedle + offset);
            byte mask   = *((byte*)pNeedleMask + offset);
            if ((test & mask) != needle)
            {
                failed = true;
                break;
            }
        }
        if (failed)
            continue;

        return pTest;
    }
    return nullptr;
}

void MemoryUtil::WritePointer(void** ptr, void* value)
{
    DWORD oldProtect;
    VirtualProtect(ptr, sizeof(value), PAGE_READWRITE, &oldProtect);
    *ptr = value;
    VirtualProtect(ptr, sizeof(value), oldProtect, &oldProtect);
}
