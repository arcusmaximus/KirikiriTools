#include "stdafx.h"

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DetourRestoreAfterWith();
        CodeBuffer::Init();
        Kirikiri::Init([] { PluginPatcher::Patch(); });
    }

    return TRUE;
}

HRESULT __stdcall V2Link(iTVPFunctionExporter* pExporter)
{
    return 0;
}
