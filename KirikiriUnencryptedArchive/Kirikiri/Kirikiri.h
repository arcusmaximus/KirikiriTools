#pragma once

typedef __int8 tjs_int8;
typedef unsigned __int8 tjs_uint8;
typedef __int16 tjs_int16;
typedef unsigned __int16 tjs_uint16;
typedef __int32 tjs_int32;
typedef unsigned __int32 tjs_uint32;
typedef __int64 tjs_int64;
typedef unsigned __int64 tjs_uint64;
typedef int tjs_int;
typedef unsigned int tjs_uint;

typedef wchar_t tjs_char;
typedef char tjs_nchar;
typedef double tjs_real;

typedef tjs_int32 tjs_error;

typedef tjs_int64 tTVInteger;
typedef tjs_real tTVReal;

#define TJS_INTF_METHOD __cdecl

#pragma pack(push, 4)
#include "tTJSVariantString.h"
#include "tTJSVariantOctet.h"
#include "iTJSNativeInstance.h"
#include "iTJSDispatch2.h"
#include "tTJSVariantClosure.h"
#include "tTJSVariant.h"
#include "tTJSString.h"
#include "tTJSHashTable.h"
#include "iTVPFunctionExporter.h"
#include "tTJSBinaryStream.h"
#include "tTVPArchive.h"
#include "tTVPXP3Archive.h"
#include "iTVPStorageMedia.h"

#include "ProxyFunctionExporter.h"
#pragma pack(pop)

class Kirikiri
{
public:
    static void                     Init                                            (const std::function<void()>& callback);

    static inline void              (__stdcall *TVPExecuteExpression)               (const ttstr& content, tTJSVariant* pResult){};
    static inline int               (__stdcall *ZLIB_uncompress)                    (BYTE* pTarget, int* pTargetLength, const BYTE* pSource, int sourceLength){};
    static inline bool              (__stdcall *TVPIsExistentStorageNoSearch)       (const ttstr&){};
    static inline bool              (__stdcall *TVPIsExistentStorageNoSearchNoNormalize)(const ttstr&){};
    static inline void*             (__stdcall *TVPCreateIStream)                   (const ttstr& name, tjs_uint32 flags){};
    static inline tTJSBinaryStream* (__stdcall *TVPCreateBinaryStreamAdapter)       (void* pComStream){};

    template<typename T>
    static void                     ResolveScriptExport                             (const tjs_char* pszName, T*& pFunction)
    {
        if (!RealFunctionExporter->QueryFunctions(&pszName, (void**)&pFunction, 1))
        {
            Debugger::Log(L"Failed to resolve %s", pszName);
            throw std::exception("Failed to resolve function");
        }
        
        Debugger::Log(L"Resolved %s", pszName);
    }

    template<typename T>
    static void                     HookScriptExport                                (const tjs_char* pszName, T** ppOldFunction, T* pNewFunction)
    {
        ProxyFunctionExporter->Hook(pszName, (void**)ppOldFunction, GetTrampoline(pNewFunction));
    }

    static std::wstring                 FilePathToUrl                               (const std::wstring& path);

private:
    static bool                         IsKirikiriExe                               ();
    static void                         HandleDllLoaded                             (const wchar_t* pwszDllPath, HMODULE hDll);
    static void                         V2LinkHook                                  ();
    static void                         HandleV2Link                                (iTVPFunctionExporter** ppExporter);

    static void*                        GetTrampoline                               (void* pTarget);

    static inline std::function<void()> InitializationCallback{};

    static inline iTVPFunctionExporter*  RealFunctionExporter{};
    static inline ProxyFunctionExporter* ProxyFunctionExporter{};
    static inline void* OriginalV2Link{};
};