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
#include "tTJSBinaryStream.h"
#include "tTVPArchive.h"
#include "tTVPXP3Archive.h"
#pragma pack(pop)

class Kirikiri
{
public:
    static void                     Init                                            (const std::function<void()>& callback);

    static inline void              (__stdcall *TVPExecuteExpression)               (const ttstr& content, tTJSVariant* pResult){};
    static inline int               (__stdcall *ZLIB_uncompress)                    (BYTE* pTarget, int* pTargetLength, const BYTE* pSource, int sourceLength){};

    template<typename T>
    static void                     ResolveScriptExport                             (const tjs_char* pszName, T*& pFunction)
    {
        void** ppFunction = TVPExportTable->Find(pszName);
        if (ppFunction == nullptr)
        {
	        Debugger::Log(L"Failed to resolve %s", pszName);
            throw std::exception("Failed to resolve function");
        }
    	
        Debugger::Log(L"Resolved %s", pszName);
        pFunction = (T*)*ppFunction;
    }

    template<typename T>
    static void                     ReplaceScriptExport                             (const tjs_char* pszName, T* pNewFunction, T*& pOldFunction)
    {
        void** ppFunction = TVPExportTable->Find(pszName);
        if (ppFunction == nullptr)
        {
	        Debugger::Log(L"Failed to resolve %s for replacement", pszName);
	        throw std::exception("Failed to resolve function for replacement");
        }
    	
        Debugger::Log(L"Resolved %s for replacement", pszName);
        pOldFunction = (T*)*ppFunction;
        *ppFunction = GetTrampoline(pNewFunction);
    }

    static inline HMODULE GameModuleHandle{};
    static inline int GameModuleSize{};
    static inline PE::Section GameTextSection{};
    static inline std::vector<PE::Section> PossibleGameDataSections{};

private:
	static void                         FindTextAndDataSections                     ();

    static void                         SetGameStartupMemoryBreakpoints             ();
	static bool                         IsGamePacked                                ();
	static void                         HandleMemoryBreakpointForGameStartup        (CONTEXT* pContext);
	static void                         HandleHwBreakpointForTVPExportTableInit     (CONTEXT* pContext);
    static void                         FinishInitialization                        ();

    static void*                        FindTVPExportFunctionPointersEnd            ();
    static tTJSHashTable<ttstr, void*>* FindTVPExportTable                          ();

    static void*                        GetTrampoline                               (void* pTarget);

    static inline std::function<void()> InitializationCallback{};
	
    static tTJSHashTable<ttstr, void*>* TVPExportTable;
    static BYTE ExportHashTableData[2048];
    static BYTE ExportHashTableMask[2048];
};