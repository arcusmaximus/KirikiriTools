#include "stdafx.h"

using namespace std;

bool ImportHooker::Hook(HMODULE hModule, const char* pName, void* pNewFunc)
{
    Context context
    {
        .Name = pName,
        .NewFunc = pNewFunc,
        .Patched = false
    };
    DetourEnumerateImportsEx(hModule, (void*)&context, nullptr, PatchGameImport);
    return context.Patched;
}

BOOL ImportHooker::PatchGameImport(void* pContext, DWORD nOrdinal, LPCSTR pszFunc, void** ppvFunc)
{
    if (pszFunc == nullptr || ppvFunc == nullptr)
        return true;

    Context* pPatchContext = (Context*)pContext;
    if (strcmp(pszFunc, pPatchContext->Name) == 0)
    {
        PageUnprotector unprotect(ppvFunc, 4);
        *ppvFunc = pPatchContext->NewFunc;
        pPatchContext->Patched = true;
    }

    return true;
}
