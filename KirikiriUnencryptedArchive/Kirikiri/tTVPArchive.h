#pragma once

class tTVPArchive
{
public:
	virtual ~tTVPArchive() = 0;
	virtual tjs_uint GetCount() = 0;
	virtual ttstr GetName(tjs_uint idx) = 0;
	virtual tTJSBinaryStream* CreateStreamByIndex(tjs_uint idx) = 0;

protected:
	tjs_uint RefCount;
	tTJSHashTable<ttstr, tjs_uint, tTJSHashFunc<ttstr>, 1024> Hash;
	bool Init;
	ttstr ArchiveName;
};
