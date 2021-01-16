#pragma once

class MemoryUtil
{
public:
    static void*            FindByte                (const void* pStart, int length, BYTE value);
    static void**           FindAlignedPointer      (const void* pStart, int length, void* value);
    static void*            FindData                (const void* pHaystack, int haystackLength, const void* pNeedle, int needleLength);
    static void*            FindData                (const void* pHaystack, int haystackLength, const void* pNeedle, const void* pNeedleMask, int needleLength);
    static void             WritePointer            (void** ptr, void* value);
};
