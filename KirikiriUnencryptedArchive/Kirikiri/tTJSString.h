#pragma once

struct tTJSString
{
public:
    static void     Init                        ();

                    tTJSString                  ();
                    tTJSString                  (const tTJSString& str);
                    tTJSString                  (const tjs_char* psz);

                    ~tTJSString                 ();

    tTJSString&     operator=                   (const tTJSString& str);
    tTJSString&     operator=                   (const tjs_char* psz);

    void            operator+=                  (const tTJSString& str);
    void            operator+=                  (const tjs_char* psz);

    int             GetLen                      () const;
    bool            IsEmpty                     () const;
    bool            StartsWith                  (const tTJSString& str) const;
    bool            StartsWith                  (const tjs_char* psz) const;

    const tjs_char* c_str                       () const;

    bool            operator==                  (const tTJSString& str) const;
    bool            operator!=                  (const tTJSString& str) const;

    tTJSString      operator+                   (const tTJSString& str) const;

    void            Replace                     (const tTJSString& from, const tTJSString& to, bool forAll);

private:
    tTJSVariantString* Ptr;

    static void         (__stdcall *_CtorStr)       (tTJSString* pThis, const tTJSString& str);
    static void         (__stdcall *_CtorPsz)       (tTJSString* pThis, const tjs_char* psz);
    static void         (__stdcall *_Dtor)          (tTJSString* pThis);
    static tTJSString&  (__stdcall *_AssignStr)     (tTJSString* pThis, const tTJSString& str);
    static tTJSString&  (__stdcall *_AssignPsz)     (tTJSString* pThis, const tjs_char* psz);
    static void         (__stdcall *_PlusAssignStr) (tTJSString* pThis, const tTJSString& str);
    static void         (__stdcall *_PlusAssignPsz) (tTJSString* pThis, const tjs_char* psz);
    static int          (__stdcall *_GetLen)        (const tTJSString* pThis);
    static bool         (__stdcall *_StartsWithStr) (const tTJSString* pThis, const tTJSString& str);
    static bool         (__stdcall *_StartsWithPsz) (const tTJSString* pThis, const tjs_char* psz);
    static tTJSString   (__stdcall *_PlusStr)       (const tTJSString* pThis, const tTJSString& str);
    static tTJSString   (__stdcall *_PlusPsz)       (const tTJSString* pThis, const tjs_char* psz);
    static void         (__stdcall *_Replace)       (tTJSString* pThis, const tTJSString& from, const tTJSString& to, bool forAll);
    
    static tTJSVariantString PreInitVariantString;
};

typedef tTJSString ttstr;
