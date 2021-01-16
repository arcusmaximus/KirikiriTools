#include "stdafx.h"

using namespace std;

#define TVP_XP3_INDEX_ENCODE_METHOD_MASK 0x07
#define TVP_XP3_INDEX_ENCODE_RAW      0
#define TVP_XP3_INDEX_ENCODE_ZLIB     1

#define TVP_XP3_INDEX_CONTINUE   0x80

bool CxdecHelper::IsCxdecUrl(const ttstr& url)
{
    return url.StartsWith(L"archive://");
}

ttstr CxdecHelper::CxdecUrlToXp3FilePath(const ttstr& url)
{
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, sizeof(exePath));
    wstring folderPath = Path::GetDirectoryName(exePath);

    const wchar_t* pPathStart = url.c_str() + strlen("archive://");
    const wchar_t* pPathEnd = wcschr(pPathStart, L'/');
    wstring fileName;
    if (pPathEnd != nullptr)
        fileName.assign(pPathStart, pPathEnd - pPathStart);
    else
        fileName.assign(pPathStart);

    return Path::Combine(folderPath, fileName).c_str();
}

bool CxdecHelper::IsCxdecArchive(const ttstr& path)
{
    FileStream stream(path.c_str(), L"rb");

    static BYTE expectedHeader[] = { 0x58, 0x50, 0x33, 0x0D, 0x0A, 0x20, 0x0A, 0x1A, 0x8B, 0x67, 0x01 };
    BYTE header[11];
    stream.ReadBytes(header, sizeof(header));
    if (memcmp(header, expectedHeader, sizeof(header)) != 0)
        return false;

    while (true)
    {
        __int64 indexOffset = stream.ReadInt64();
        stream.SetPosition(indexOffset);

        BYTE indexFlags = stream.ReadByte();
        int indexSize = (int)stream.ReadInt64();

        if ((indexFlags & TVP_XP3_INDEX_CONTINUE) != 0)
        {
            assert((indexFlags & TVP_XP3_INDEX_ENCODE_METHOD_MASK) == TVP_XP3_INDEX_ENCODE_RAW);
            assert(indexSize == 0);
            continue;
        }

        vector<BYTE> indexData;
        if ((indexFlags & TVP_XP3_INDEX_ENCODE_METHOD_MASK) == TVP_XP3_INDEX_ENCODE_ZLIB)
        {
            int compressedIndexSize = indexSize;
            indexSize = (int)stream.ReadInt64();

            indexData.resize(indexSize);
            vector<BYTE> compressedIndexData(compressedIndexSize);
            stream.ReadBytes(compressedIndexData.data(), compressedIndexSize);
            Kirikiri::ZLIB_uncompress(indexData.data(), &indexSize, compressedIndexData.data(), compressedIndexSize);
        }
        else if ((indexFlags & TVP_XP3_INDEX_ENCODE_METHOD_MASK) == TVP_XP3_INDEX_ENCODE_RAW)
        {
            indexData.resize(indexSize);
            stream.ReadBytes(indexData.data(), indexSize);
        }
        else
        {
            throw exception("Unknown XP3 index encoding");
        }
        return indexSize >= 4 && *((int *)indexData.data()) != 0x656C6946;
    }
}

