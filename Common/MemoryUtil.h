#pragma once

class MemoryUtil
{
public:
    static void*            FindByte                (void* pStart, int length, byte value);
    static void**           FindAlignedPointer      (void* pStart, int length, void* value);
    static void*            FindData                (void* pHaystack, int haystackLength, void* pNeedle, int needleLength);
    static void*            FindData                (void* pHaystack, int haystackLength, void* pNeedle, void* pNeedleMask, int needleLength);
    static void             WritePointer            (void** ptr, void* value);
};
