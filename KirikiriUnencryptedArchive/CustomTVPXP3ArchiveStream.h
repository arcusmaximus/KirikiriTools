#pragma once

class CustomTVPXP3ArchiveStream : public tTJSBinaryStream
{
public:
								CustomTVPXP3ArchiveStream	(const ttstr& archiveUrl, tjs_uint64 offset, tjs_uint64 originalSize, tjs_uint64 archiveSize, bool compressed);
								~CustomTVPXP3ArchiveStream	() override;

	tjs_uint64 TJS_INTF_METHOD	Seek						(tjs_int64 offset, tjs_int whence) override;
	tjs_uint TJS_INTF_METHOD	Read						(void* buffer, tjs_uint read_size) override;
	tjs_uint TJS_INTF_METHOD	Write						(const void* buffer, tjs_uint write_size) override;
	tjs_uint64 TJS_INTF_METHOD	GetSize						() override;
	void TJS_INTF_METHOD		SetEndOfStorage				() override;
	
	std::vector<BYTE> _data;
	tjs_uint64 _position;
};
