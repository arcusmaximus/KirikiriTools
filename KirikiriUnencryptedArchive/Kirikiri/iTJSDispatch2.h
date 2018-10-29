#pragma once

class tTJSVariant;
class tTJSVariantClosure;

class iTJSDispatch2
{
public:
    virtual tjs_uint TJS_INTF_METHOD AddRef() = 0;
    virtual tjs_uint TJS_INTF_METHOD Release() = 0;

    virtual tjs_error TJS_INTF_METHOD FuncCall(tjs_uint32 flag, const tjs_char* membername, tjs_uint32* hint, tTJSVariant* result, tjs_int numparams, tTJSVariant** param, iTJSDispatch2* objthis) = 0;
    virtual tjs_error TJS_INTF_METHOD FuncCallByNum(tjs_uint32 flag, tjs_int num, tTJSVariant* result, tjs_int numparams, tTJSVariant** param, iTJSDispatch2* objthis) = 0;

    virtual tjs_error TJS_INTF_METHOD PropGet(tjs_uint32 flag, const tjs_char* membername, tjs_uint32* hint, tTJSVariant* result, iTJSDispatch2* objthis) = 0;
    virtual tjs_error TJS_INTF_METHOD PropGetByNum(tjs_uint32 flag, tjs_int num, tTJSVariant* result, iTJSDispatch2* objthis) = 0;

    virtual tjs_error TJS_INTF_METHOD PropSet(tjs_uint32 flag, const tjs_char* membername, tjs_uint32* hint, const tTJSVariant* param, iTJSDispatch2* objthis) = 0;
    virtual tjs_error TJS_INTF_METHOD PropSetByNum(tjs_uint32 flag, tjs_int num, const tTJSVariant* param, iTJSDispatch2* objthis) = 0;

    virtual tjs_error TJS_INTF_METHOD GetCount(tjs_int* result, const tjs_char* membername, tjs_uint32* hint, iTJSDispatch2* objthis) = 0;
    virtual tjs_error TJS_INTF_METHOD GetCountByNum(tjs_int* result, tjs_int num, iTJSDispatch2* objthis) = 0;

    virtual tjs_error TJS_INTF_METHOD PropSetByVS(tjs_uint32 flag, tTJSVariantString* membername, const tTJSVariant* param, iTJSDispatch2* objthis) = 0;

    virtual tjs_error TJS_INTF_METHOD EnumMembers(tjs_uint32 flag, tTJSVariantClosure* callback, iTJSDispatch2* objthis) = 0;

    virtual tjs_error TJS_INTF_METHOD DeleteMember(tjs_uint32 flag, const tjs_char* membername, tjs_uint32* hint, iTJSDispatch2* objthis) = 0;
    virtual tjs_error TJS_INTF_METHOD DeleteMemberByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2* objthis) = 0;

    virtual tjs_error TJS_INTF_METHOD Invalidate(tjs_uint32 flag, const tjs_char* membername, tjs_uint32* hint, iTJSDispatch2* objthis) = 0;
    virtual tjs_error TJS_INTF_METHOD InvalidateByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2* objthis) = 0;

    virtual tjs_error TJS_INTF_METHOD IsValid(tjs_uint32 flag, const tjs_char* membername, tjs_uint32* hint, iTJSDispatch2* objthis) = 0;
    virtual tjs_error TJS_INTF_METHOD IsValidByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2* objthis) = 0;

    virtual tjs_error TJS_INTF_METHOD CreateNew(tjs_uint32 flag, const tjs_char* membername, tjs_uint32* hint, iTJSDispatch2** result, tjs_int numparams, tTJSVariant** param, iTJSDispatch2* objthis) = 0;

    virtual tjs_error TJS_INTF_METHOD CreateNewByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2** result, tjs_int numparams, tTJSVariant** param, iTJSDispatch2* objthis) = 0;

    virtual tjs_error TJS_INTF_METHOD Reserved1() = 0;

    virtual tjs_error TJS_INTF_METHOD IsInstanceOf(tjs_uint32 flag, const tjs_char* membername, tjs_uint32* hint, const tjs_char* classname, iTJSDispatch2* objthis) = 0;
    virtual tjs_error TJS_INTF_METHOD IsInstanceOfByNum(tjs_uint32 flag, tjs_int num, const tjs_char* classname, iTJSDispatch2* objthis) = 0;

    virtual tjs_error TJS_INTF_METHOD Operation(tjs_uint32 flag, const tjs_char* membername, tjs_uint32* hint, tTJSVariant* result, const tTJSVariant* param, iTJSDispatch2* objthis) = 0;
    virtual tjs_error TJS_INTF_METHOD OperationByNum(tjs_uint32 flag, tjs_int num, tTJSVariant* result, const tTJSVariant* param, iTJSDispatch2* objthis) = 0;

    virtual tjs_error TJS_INTF_METHOD NativeInstanceSupport(tjs_uint32 flag, tjs_int32 classid, iTJSNativeInstance **pointer) = 0;

    virtual tjs_error TJS_INTF_METHOD ClassInstanceInfo(tjs_uint32 flag, tjs_uint num, tTJSVariant* value) = 0;

    virtual tjs_error TJS_INTF_METHOD Reserved2() = 0;
    virtual tjs_error TJS_INTF_METHOD Reserved3() = 0;
};
