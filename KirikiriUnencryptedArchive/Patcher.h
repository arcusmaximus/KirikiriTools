#pragma once

class Patcher
{
    friend CompilerHelper;

public:
    static void                 Patch                                   ();

private:
    static void                 PatchXP3StreamCreation                  ();

    template<CompilerType TCompilerType>
    class CustomCreateStreamByIndex
    {
    public:
        static tTJSBinaryStream* Call(tTVPXP3Archive<TCompilerType>* pArchive, tjs_uint idx)
        {
            int itemSize = ((BYTE*)pArchive->ItemVector.end() - (BYTE*)pArchive->ItemVector.begin()) / pArchive->Count;
            auto* pItem = (typename tTVPXP3Archive<TCompilerType>::tArchiveItem*)((BYTE*)pArchive->ItemVector.begin() + idx * itemSize);
            if (pItem->FileHash != 0 || !pArchive->Name.StartsWith(L"file://./"))
            {
                //Debugger::Log(L"Creating regular XP3 stream for %s", pItem->Name.c_str());
                return CompilerHelper::CallInstanceMethod<tTJSBinaryStream*, &OriginalCreateStreamByIndex, tTVPXP3Archive<TCompilerType>*, tjs_uint>(pArchive, idx);
            }

            Debugger::Log(L"Creating unencrypted XP3 stream for %s", pItem->Name.c_str());
            tTVPXP3ArchiveSegment* pSegment = pItem->Segments.begin();
            auto* pStream = new CustomTVPXP3ArchiveStream(pArchive->Name, pSegment->Start, pSegment->OrgSize, pSegment->ArcSize, pSegment->IsCompressed);
            tTJSBinaryStream::ApplyWrappedVTable(pStream);
            return pStream;
        }
    };

    static void                 PatchAutoPathExports                    ();

    static void __stdcall       CustomTVPAddAutoPath                    (const ttstr& url);
    static void __stdcall       CustomTVPRemoveAutoPath                 (const ttstr& url);

    static inline void* OriginalCreateStreamByIndex{};

    static inline void (__stdcall* OriginalTVPAddAutoPath)(const ttstr& path){};
    static inline void (__stdcall* OriginalTVPRemoveAutoPath)(const ttstr& path){};
};
