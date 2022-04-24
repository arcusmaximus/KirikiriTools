#include "stdafx.h"

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        Proxy::Init();

        Debugger::RegisterDllLoadHandler(
            [](const wchar_t* pwszDllPath, HMODULE hDll)
            {
                if (GetProcAddress(hDll, "V2Link") != nullptr)
                    Patcher::PatchSignatureCheck(hDll);
            }
        );

        Kirikiri::Init(
            []
            {
                CompilerHelper::Init();
                Patcher::PatchXP3StreamCreation();
                Patcher::PatchAutoPathExports();
                Patcher::PatchStorageMediaRegistration();
            }
        );
    }

    return TRUE;
}
