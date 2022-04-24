#include "stdafx.h"

using namespace std;

ProxyFunctionExporter::ProxyFunctionExporter(iTVPFunctionExporter* pInnerExporter)
{
    _pInnerExporter = pInnerExporter;
}

void ProxyFunctionExporter::Hook(const wchar_t* pwszName, void** ppOrigFunc, void* pNewFunc)
{
    if (!_pInnerExporter->QueryFunctions(&pwszName, ppOrigFunc, 1))
    {
        Debugger::Log(L"Failed to resolve %s", pwszName);
        throw std::exception("Failed to resolve function");
    }

    _hooks[pwszName] = pNewFunc;
}

bool TJS_INTF_METHOD ProxyFunctionExporter::QueryFunctions(const tjs_char** name, void** function, tjs_uint count)
{
    bool success = true;
    for (int i = 0; i < count; i++)
    {
        auto it = _hooks.find(name[i]);
        if (it != _hooks.end())
            function[i] = it->second;
        else
            success &= _pInnerExporter->QueryFunctions(&name[i], &function[i], 1);
    }
    return success;
}

bool TJS_INTF_METHOD ProxyFunctionExporter::QueryFunctionsByNarrowString(const char** name, void** function, tjs_uint count)
{
    bool success = true;
    for (int i = 0; i < count; i++)
    {
        auto it = _hooks.find(StringUtil::ToUTF16(name[i]));
        if (it != _hooks.end())
            function[i] = it->second;
        else
            success &= _pInnerExporter->QueryFunctionsByNarrowString(&name[i], &function[i], 1);
    }
    return success;
}
