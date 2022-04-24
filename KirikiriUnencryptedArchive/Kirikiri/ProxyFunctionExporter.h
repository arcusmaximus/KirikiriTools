#pragma once

class ProxyFunctionExporter : public iTVPFunctionExporter
{
public:
                                    ProxyFunctionExporter           (iTVPFunctionExporter* pInnerExporter);

    void                            Hook                            (const wchar_t* pwszName, void** ppOrigFunc, void* pNewFunc);

    virtual bool TJS_INTF_METHOD    QueryFunctions                  (const tjs_char** name, void** function, tjs_uint count) override;
    virtual bool TJS_INTF_METHOD    QueryFunctionsByNarrowString    (const char** name, void** function, tjs_uint count) override;

private:
    iTVPFunctionExporter* _pInnerExporter{};
    std::map<std::wstring, void*> _hooks{};
};
