#pragma once

class CodeBuffer
{
public:
    static void     Init                    ();

    static void*    GetCallToJmpAdapter     (void* pTarget);

private:
    static void     BeginWrite              ();
    static void     WriteByte               (byte b);
    static void     WritePointer            (void* ptr);
    static void     Write                   (void* pData, int size);
    static void     EndWrite                ();

    static byte*    BufferPtr;
    static int      BufferLength;

    static byte*    Ptr;
    static int      LengthRemaining;
};
