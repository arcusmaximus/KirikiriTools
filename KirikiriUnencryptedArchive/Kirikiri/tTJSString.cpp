#include "stdafx.h"

void        (__stdcall *tTJSString::_CtorStr)       (tTJSString* pThis, const tTJSString& str);
void        (__stdcall *tTJSString::_CtorPsz)       (tTJSString* pThis, const tjs_char* psz);
void        (__stdcall *tTJSString::_Dtor)          (tTJSString* pThis);
tTJSString& (__stdcall *tTJSString::_AssignStr)     (tTJSString* pThis, const tTJSString& str);
tTJSString& (__stdcall *tTJSString::_AssignPsz)     (tTJSString* pThis, const tjs_char* psz);
void        (__stdcall *tTJSString::_PlusAssignStr) (tTJSString* pThis, const tTJSString& str);
void        (__stdcall *tTJSString::_PlusAssignPsz) (tTJSString* pThis, const tjs_char* psz);
int         (__stdcall *tTJSString::_GetLen)        (const tTJSString* pThis);
bool        (__stdcall *tTJSString::_StartsWithStr) (const tTJSString* pThis, const tTJSString& str);
bool        (__stdcall *tTJSString::_StartsWithPsz) (const tTJSString* pThis, const tjs_char* psz);
tTJSString  (__stdcall *tTJSString::_PlusStr)       (const tTJSString* pThis, const tTJSString& str);
tTJSString  (__stdcall *tTJSString::_PlusPsz)       (const tTJSString* pThis, const tjs_char* psz);
void        (__stdcall *tTJSString::_Replace)       (tTJSString* pThis, const tTJSString& from, const tTJSString& to, bool forAll);

tTJSVariantString tTJSString::PreInitVariantString;

void tTJSString::Init()
{
    Kirikiri::ResolveScriptExport(L"tTJSString::tTJSString(const tTJSString &)", _CtorStr);
    Kirikiri::ResolveScriptExport(L"tTJSString::tTJSString(const tjs_char *)", _CtorPsz);
    Kirikiri::ResolveScriptExport(L"tTJSString::~ tTJSString()", _Dtor);
    Kirikiri::ResolveScriptExport(L"tTJSString & tTJSString::operator =(const tTJSString &)", _AssignStr);
    Kirikiri::ResolveScriptExport(L"tTJSString & tTJSString::operator =(const tjs_char *)", _AssignPsz);
    Kirikiri::ResolveScriptExport(L"void tTJSString::operator +=(const tTJSString &)", _PlusAssignStr);
    Kirikiri::ResolveScriptExport(L"void tTJSString::operator +=(const tjs_char *)", _PlusAssignPsz);
    Kirikiri::ResolveScriptExport(L"tjs_int tTJSString::GetLen() const", _GetLen);
    Kirikiri::ResolveScriptExport(L"bool tTJSString::StartsWith(const tTJSString &) const", _StartsWithStr);
    Kirikiri::ResolveScriptExport(L"bool tTJSString::StartsWith(const tjs_char *) const", _StartsWithPsz);
    Kirikiri::ResolveScriptExport(L"tTJSString tTJSString::operator +(const tTJSString &) const", _PlusStr);
    Kirikiri::ResolveScriptExport(L"tTJSString tTJSString::operator +(const tjs_char *) const", _PlusPsz);
    Kirikiri::ResolveScriptExport(L"void tTJSString::Replace(const tTJSString &,const tTJSString &,bool)", _Replace);
}

tTJSString::tTJSString()
{
    Ptr = nullptr;
}

tTJSString::tTJSString(const tTJSString& str)
{
    _CtorStr(this, str);
}

tTJSString::tTJSString(const tjs_char* psz)
{
    if (_CtorPsz)
    {
        _CtorPsz(this, psz);
    }
    else
    {
        if (PreInitVariantString.LongString != nullptr)
            throw "Only one pre-init tTJSString can be active at a time";

        PreInitVariantString.LongString = const_cast<tjs_char*>(psz);
        PreInitVariantString.Length = wcslen(psz);
        Ptr = &PreInitVariantString;
    }
}

tTJSString::~tTJSString()
{
    if (Ptr == &PreInitVariantString)
    {
        PreInitVariantString.LongString = nullptr;
        Ptr = nullptr;
    }
    else
    {
        _Dtor(this);
    }
}

tTJSString& tTJSString::operator=(const tTJSString& str)
{
    return _AssignStr(this, str);
}

tTJSString& tTJSString::operator=(const tjs_char* psz)
{
    return _AssignPsz(this, psz);
}

void tTJSString::operator+=(const tTJSString& str)
{
    _PlusAssignStr(this, str);
}

void tTJSString::operator+=(const tjs_char* psz)
{
    _PlusAssignPsz(this, psz);
}

int tTJSString::GetLen() const
{
    return _GetLen(this);
}

bool tTJSString::IsEmpty() const
{
    return Ptr == nullptr;
}

bool tTJSString::StartsWith(const tTJSString& str) const
{
    return _StartsWithStr(this, str);
}

bool tTJSString::StartsWith(const tjs_char* psz) const
{
    return _StartsWithPsz(this, psz);
}

const tjs_char* tTJSString::c_str() const
{
    return Ptr ? Ptr->operator const tjs_char*() : L"";
}

bool tTJSString::operator==(const tTJSString& str) const
{
    if (Ptr == str.Ptr) return true; // both empty or the same pointer
    if (!Ptr && str.Ptr) return false;
    if (Ptr && !str.Ptr) return false;
    if (Ptr->Length != str.Ptr->Length) return false;
    return !wcscmp(*Ptr, *str.Ptr);
}

bool tTJSString::operator!=(const tTJSString& str) const
{
    return !this->operator==(str);
}

tTJSString tTJSString::operator+(const tTJSString& str) const
{
    return _PlusStr(this, str);
}

void tTJSString::Replace(const tTJSString& from, const tTJSString& to, bool forAll)
{
    _Replace(this, from, to, forAll);
}
