#include "stdafx.h"

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        Proxy::Init();
        Kirikiri::Init(
            []
            {
                CompilerHelper::Init();
                Patcher::Patch();
            });
    }

    return TRUE;
}
