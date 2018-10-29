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
#include "tTVPXP3ExtractionFilterInfo.h"
#pragma pack(pop)

class Kirikiri
{
public:
    static void                     Init                                            (std::function<void()> callback);

    static void                     (__stdcall *TVPExecuteExpression)               (const ttstr& content, tTJSVariant* pResult);
    static int                      (__stdcall *ZLIB_uncompress)                    (byte* pTarget, uint* pTargetLength, const byte* pSource, uint sourceLength);

    template<typename T>
    static void                     ResolveScriptExport                             (const tjs_char* pszName, T*& pFunction)
    {
        pFunction = (T*)*TVPExportFuncs->Find(pszName);
    }

    template<typename T>
    static void                     ReplaceScriptExport                             (const tjs_char* pszName, T* pNewFunction, T*& pOldFunction)
    {
        void** ppFunction = TVPExportFuncs->Find(pszName);
        if (ppFunction == nullptr)
            throw "Exported function not found";

        pOldFunction = (T*)*ppFunction;
        *ppFunction = GetTrampoline(pNewFunction);
    }

    struct Range
    {
        Range()
        {
            Start = nullptr;
            Length = 0;
        }

        Range(void* pStart, int length)
        {
            Start = pStart;
            Length = length;
        }

        void*   Start;
        int     Length;
    };

    static HMODULE ModuleHandle;
    static Range TextSection;
    static Range DataSection;

private:
    static void                         PostInit                                        ();

    static void*                        FindExportFunctionListEnd                       ();
    static tTJSHashTable<ttstr, void*>* FindExportHashTable                             ();

    static void*                        GetTrampoline                                   (void* pTarget);

    struct FindImportContext
    {
        void*       pFunction;
        void**      pImport;
    };
    static BOOL __stdcall               MatchWin32Import                                (PVOID pContext, DWORD ordinal, LPCSTR pszName, PVOID* ppFunc);

    static void                         FindSections                                    ();
    static Range                        FindSection                                     (const char* pName);
    static std::vector<void*>           GetUnusedWin32Functions                         ();

    static tTJSHashTable<ttstr, void*>* TVPExportFuncs;
    static byte ExportHashTableData[2048];
    static byte ExportHashTableMask[2048];
};