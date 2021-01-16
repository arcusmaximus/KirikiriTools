#pragma once

template<typename TResult, void** TFuncPtrPtr, int TFuncPtrIndex, typename... TArgs>
class CdeclToThiscallAdapterBase
{
    static_assert(sizeof...(TArgs) > 0);

public:
    static void Call();

private:
    static constexpr void** FuncPtrPtr = TFuncPtrPtr + TFuncPtrIndex;

    static constexpr int NumThiscallStackArgs = sizeof...(TArgs) - 1;
    static constexpr int ThiscallStackArgsSize = NumThiscallStackArgs * 4;

    static constexpr int NumCdeclStackArgs = sizeof...(TArgs);
    static constexpr int CdeclStackArgsSize = NumCdeclStackArgs * 4;
};

template<typename TResult, void** TFuncPtrPtr, int TFuncPtrIndex, typename... TArgs>
__declspec(naked) void CdeclToThiscallAdapterBase<TResult, TFuncPtrPtr, TFuncPtrIndex, TArgs...>::Call()
{
    if constexpr (sizeof...(TArgs) > 1)
    {
        __asm
        {
            mov eax, NumThiscallStackArgs
            lea edx, [esp+4+4*eax]

        nextStackArg:
            sub edx, 4
            push dword ptr [edx]
            dec eax
            jnz nextStackArg
        }
    }
    __asm
    {
        push ecx
        mov eax, FuncPtrPtr
        call dword ptr [eax]
        add esp, CdeclStackArgsSize
    }

    if constexpr (sizeof...(TArgs) > 1)
    {
        __asm
        {
            pop edx
            add esp, ThiscallStackArgsSize
            jmp edx
        }
    }
    else
    {
        __asm
        {
            ret
        }
    }
}

template<typename TSignature, void** TFuncPtrPtr, int TFuncPtrIndex = 0>
class CdeclToThiscallAdapter;

template<typename TResult, void** TFuncPtrPtr, int TFuncPtrIndex, typename... TArgs>
class CdeclToThiscallAdapter<TResult (TArgs...), TFuncPtrPtr, TFuncPtrIndex> : public CdeclToThiscallAdapterBase<TResult, TFuncPtrPtr, TFuncPtrIndex, TArgs...>
{
};

template<typename TResult, void** TFuncPtrPtr, int TFuncPtrIndex, typename... TArgs>
class CdeclToThiscallAdapter<TResult (*)(TArgs...), TFuncPtrPtr, TFuncPtrIndex> : public CdeclToThiscallAdapterBase<TResult, TFuncPtrPtr, TFuncPtrIndex, TArgs...>
{
};
