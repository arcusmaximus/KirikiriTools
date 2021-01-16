#pragma once

struct tTVPXP3ArchiveSegment
{
	tjs_uint64 Start;
	tjs_uint64 Offset;
	tjs_uint64 OrgSize;
	tjs_uint64 ArcSize;
	bool IsCompressed;
};

template<CompilerType compilerType>
class tTVPXP3Archive : public tTVPArchive
{
public:
	ttstr Name;

	struct tArchiveItem
	{
		ttstr Name;
		tjs_uint32 FileHash;
		tjs_uint64 OrgSize;
		tjs_uint64 ArcSize;
		CompilerSpecificVector<compilerType, tTVPXP3ArchiveSegment> Segments;
	};

	tjs_int Count;

	CompilerSpecificVector<compilerType, tArchiveItem> ItemVector;
};
