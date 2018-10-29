#pragma once

struct tTVPXP3ExtractionFilterInfo
{
    const tjs_uint SizeOfSelf;  // structure size of tTVPXP3ExtractionFilterInfo itself
    const tjs_uint64 Offset;    // offset of the buffer data in uncompressed stream position
    void* Buffer;               // target data buffer
    const tjs_uint BufferSize;  // buffer size in bytes pointed by "Buffer"
    const tjs_uint32 FileHash;  // hash value of the file (since inteface v2)
};

typedef void (_stdcall* tTVPXP3ArchiveExtractionFilter)(tTVPXP3ExtractionFilterInfo *info);
