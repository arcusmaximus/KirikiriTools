#include "stdafx.h"

using namespace std;

CustomTVPXP3ArchiveStream::CustomTVPXP3ArchiveStream(const ttstr& archiveUrl, tjs_uint64 offset, tjs_uint64 originalSize, tjs_uint64 archiveSize, bool compressed)
{
	_data.resize(originalSize);
	_position = 0;

	ttstr archivePath = archiveUrl;
	archivePath.Replace(L"file://./", L"", true);
	archivePath.Replace(L"/", L"\\", true);

	wstring archivePathStr(archivePath.c_str());
	archivePathStr.insert(1, L":");
	
	FileStream stream(archivePathStr, L"rb");
	stream.SetPosition(offset);
	if (compressed)
	{
		vector<BYTE> compressedData;
		compressedData.resize(archiveSize);
		stream.ReadBytes(compressedData.data(), compressedData.size());

		int uncompressedSize = _data.size();
		Kirikiri::ZLIB_uncompress(_data.data(), &uncompressedSize, compressedData.data(), compressedData.size());
	}
	else
	{
		stream.ReadBytes(_data.data(), _data.size());
	}
}

CustomTVPXP3ArchiveStream::~CustomTVPXP3ArchiveStream()
{
}

tjs_uint64 CustomTVPXP3ArchiveStream::Seek(tjs_int64 offset, tjs_int whence)
{
	switch (whence)
	{
		case SEEK_SET:
			_position = offset;
			break;

		case SEEK_CUR:
			_position += offset;
			break;

		case SEEK_END:
			_position = _data.size() + offset;
			break;
	}

	return _position;
}

tjs_uint CustomTVPXP3ArchiveStream::Read(void* buffer, tjs_uint read_size)
{
	if (read_size > _data.size() - _position)
		read_size = _data.size() - _position;

	memcpy(buffer, _data.data() + _position, read_size);
	_position += read_size;
	return read_size;
}

tjs_uint64 CustomTVPXP3ArchiveStream::GetSize()
{
	return _data.size();
}

tjs_uint CustomTVPXP3ArchiveStream::Write(const void* buffer, tjs_uint write_size)
{
	throw exception("Not implemented");
}

void CustomTVPXP3ArchiveStream::SetEndOfStorage()
{
	throw exception("Not implemented");
}
