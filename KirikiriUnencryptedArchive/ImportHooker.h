#pragma once

class ImportHooker
{
public:
    static bool Hook(HMODULE hModule, const char* pName, void* pNewFunc);

private:
    struct Context
    {
        const char* Name;
        void* NewFunc;
        bool Patched;
    };

    static BOOL __stdcall PatchGameImport(void* pContext, DWORD nOrdinal, LPCSTR pszFunc, void** ppvFunc);
};
