#pragma once

class Patcher
{
    friend CompilerHelper;

public:
    static bool                 PatchSignatureCheck                     (HMODULE hModule);

    static void                 PatchXP3StreamCreation                  ();
    static void                 PatchAutoPathExports                    ();
    static void                 PatchStorageMediaRegistration           ();

private:
    static void __stdcall       CustomTVPAddAutoPath                    (const ttstr& url);
    static void __stdcall       CustomTVPRemoveAutoPath                 (const ttstr& url);

    static void __stdcall       CustomTVPRegisterStorageMedia           (iTVPStorageMedia* pMedia);
    static void __stdcall       CustomTVPUnregisterStorageMedia         (iTVPStorageMedia* pMedia);
    static tTJSBinaryStream*    CustomStorageMediaOpen                  (iTVPStorageMedia* pMedia, const ttstr& name, tjs_uint32 flags);
    static void                 WriteStreamToFile                       (tTJSBinaryStream* pStream, const std::wstring& filePath);

    static bool                 CustomGetSignatureVerificationResult    ();

    template<CompilerType TCompilerType>
    class CustomCreateStreamByIndex
    {
    public:
        static tTJSBinaryStream* Call(tTVPXP3Archive<TCompilerType>* pArchive, tjs_uint idx)
        {
            int itemSize = ((BYTE*)pArchive->ItemVector.end() - (BYTE*)pArchive->ItemVector.begin()) / pArchive->Count;
            auto* pItem = (typename tTVPXP3Archive<TCompilerType>::tArchiveItem*)((BYTE*)pArchive->ItemVector.begin() + idx * itemSize);
            if (pItem->FileHash != 0 || !pArchive->Name.StartsWith(L"file://"))
                return CompilerHelper::CallInstanceMethod<tTJSBinaryStream*, &OriginalCreateStreamByIndex, tTVPXP3Archive<TCompilerType>*, tjs_uint>(pArchive, idx);
                
            Debugger::Log(L"Creating unencrypted XP3 stream for %s", pItem->Name.c_str());
            tTVPXP3ArchiveSegment* pSegment = pItem->Segments.begin();
            auto* pStream = new CustomTVPXP3ArchiveStream(pArchive->Name, pSegment->Start, pSegment->OrgSize, pSegment->ArcSize, pSegment->IsCompressed);
            tTJSBinaryStream::ApplyWrappedVTable(pStream);
            return pStream;
        }
    };

    static inline void* OriginalCreateStreamByIndex{};

    static inline void (__stdcall* OriginalTVPAddAutoPath)(const ttstr& path){};
    static inline void (__stdcall* OriginalTVPRemoveAutoPath)(const ttstr& path){};
    static inline void (__stdcall* OriginalTVPRegisterStorageMedia)(iTVPStorageMedia* pMedia){};
    static inline void (__stdcall* OriginalTVPUnregisterStorageMedia)(iTVPStorageMedia* pMedia){};
    static inline std::map<iTVPStorageMedia*, tTJSBinaryStream* (*)(iTVPStorageMedia* pMedia, const ttstr& name, tjs_uint32 flags)> OriginalStorageMediaOpen{};
};
