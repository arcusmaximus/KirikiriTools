#include "stdafx.h"

using namespace std;

FileStream::FileStream(const wstring& filePath, const wchar_t* pMode)
{
    _pFile = _wfopen(filePath.c_str(), pMode);
    if (_pFile == nullptr)
        throw exception("Failed to open file");
}

FileStream::~FileStream()
{
    fclose(_pFile);
    _pFile = nullptr;
}

__int64 FileStream::GetPosition() const
{
    return _ftelli64(_pFile);
}

void FileStream::SetPosition(__int64 position)
{
    _fseeki64(_pFile, position, SEEK_SET);
}

void FileStream::Seek(int offset)
{
    _fseeki64(_pFile, offset, SEEK_CUR);
}

__int64 FileStream::Size() const
{
    __int64 position = _ftelli64(_pFile);
    _fseeki64(_pFile, 0, SEEK_END);
    __int64 size = _ftelli64(_pFile);
    _fseeki64(_pFile, position, SEEK_SET);
    return size;
}

void FileStream::SkipBits(int count)
{
    throw exception("Not implemented");
}

void FileStream::AlignByte()
{
    throw exception("Not implemented");
}

BYTE FileStream::PeekByte() const
{
    assert(GetPosition() < Size());

    __int64 position = _ftelli64(_pFile);
    BYTE b;
    fread(&b, 1, 1, _pFile);
    _fseeki64(_pFile, position, SEEK_SET);
    return b;
}

void FileStream::ReadBytes(void* pBuffer, int size)
{
    assert(GetPosition() + size <= Size());
    fread(pBuffer, 1, size, _pFile);
}

DWORD FileStream::ReadBits(int count)
{
    throw exception("Not implemented");
}

void FileStream::Write(const void* pData, int size)
{
    fwrite(pData, 1, size, _pFile);
}
