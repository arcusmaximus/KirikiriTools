#pragma once

class tTJSVariant;
class iTJSDispatch2;

class iTJSNativeInstance
{
public:
    virtual tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant** param, iTJSDispatch2* tjs_obj) = 0;
    virtual void TJS_INTF_METHOD Invalidate() = 0;
    virtual void TJS_INTF_METHOD Destruct() = 0;
};
