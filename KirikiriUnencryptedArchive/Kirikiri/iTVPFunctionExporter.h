#pragma once

struct iTVPFunctionExporter
{
    virtual bool TJS_INTF_METHOD QueryFunctions(const tjs_char** name, void** function, tjs_uint count) = 0;
    virtual bool TJS_INTF_METHOD QueryFunctionsByNarrowString(const char** name, void** function, tjs_uint count) = 0;
};
