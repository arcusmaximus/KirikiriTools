#include "stdafx.h"

void (__stdcall *tTJSVariant::Ctor)(tTJSVariant* pThis);
void (__stdcall *tTJSVariant::Dtor)(tTJSVariant* pThis);

void tTJSVariant::Init()
{
    Kirikiri::ResolveScriptExport(L"tTJSVariant::tTJSVariant()", Ctor);
    Kirikiri::ResolveScriptExport(L"tTJSVariant::~ tTJSVariant()", Dtor);
}

tTJSVariant::tTJSVariant()
{
    Ctor(this);
}

tTJSVariant::~tTJSVariant()
{
    Dtor(this);
}
