#pragma once

enum tTJSVariantType
{
    tvtVoid,
    tvtObject,
    tvtString,
    tvtOctet,
    tvtInteger,
    tvtReal
};

struct tTJSVariant
{
public:
    static void         Init            ();

                        tTJSVariant     ();
                        ~tTJSVariant    ();

private:
    union
    {
        tTJSVariantClosure Object;
        tTVInteger Integer;
        tTVReal Real;
        tTJSVariantString* String;
        tTJSVariantOctet* Octet;
    };
    tTJSVariantType vt;

    static void         (__stdcall *Ctor)       (tTJSVariant* pThis);
    static void         (__stdcall *Dtor)       (tTJSVariant* pThis);
};
