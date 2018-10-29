#pragma once

#define TJS_VS_SHORT_LEN 21

struct tTJSVariantString
{
    tjs_int RefCount;
    tjs_char* LongString;
    tjs_char ShortString[TJS_VS_SHORT_LEN + 1];
    tjs_int Length;
    tjs_uint32 HeapFlag;
    tjs_uint32 Hint;

    operator const tjs_char*() const
    {
        return LongString ? LongString : ShortString;
    }
};
