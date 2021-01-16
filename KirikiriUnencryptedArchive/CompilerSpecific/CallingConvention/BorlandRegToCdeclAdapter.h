#pragma once

template<typename TResult, void** TFuncPtrPtr, int TFuncPtrIndex, typename... TArgs>
class BorlandRegToCdeclAdapterBase
{
public:
    static TResult Call(TArgs...);

private:
    static constexpr void** FuncPtrPtr = TFuncPtrPtr + TFuncPtrIndex;

    static constexpr int NumCdeclStackArgs = sizeof...(TArgs);
    static constexpr int CdeclStackArgsSize = NumCdeclStackArgs * 4;

    static constexpr int NumBorlandStackArgs = sizeof...(TArgs) > 3 ? sizeof...(TArgs) - 3 : 0;
    static constexpr int BorlandStackArgsSize = NumBorlandStackArgs * 4;
};

template<typename TResult, void** TFuncPtrPtr, int TFuncPtrIndex, typename... TArgs>
__declspec(naked) TResult BorlandRegToCdeclAdapterBase<TResult, TFuncPtrPtr, TFuncPtrIndex, TArgs...>::Call(TArgs...)
{
    __asm
    {
        push esi
        push edi
    }
    if constexpr (sizeof...(TArgs) > 0)
    {
        __asm mov eax, [esp+8+4]
    }
    if constexpr (sizeof...(TArgs) > 1)
    {
        __asm mov edx, [esp+8+8]
    }
    if constexpr (sizeof...(TArgs) > 2)
    {
        __asm mov ecx, [esp+8+0xC]
    }
    if constexpr (sizeof...(TArgs) > 3)
    {
        __asm
        {
            lea esi, [esp+8+0x10]
            mov edi, NumBorlandStackArgs

        nextStackArg:
            push dword ptr [esi]
            add esi, 4
            dec edi
            jnz nextStackArg
        }
    }
    __asm
    {
        mov edi, FuncPtrPtr
        call dword ptr [edi]
        pop edi
        pop esi
        ret
    }
}

template<typename TSignature, void** TFuncPtrPtr, int TFuncPtrIndex = 0>
class BorlandRegToCdeclAdapter;

template<typename TResult, void** TFuncPtrPtr, int TFuncPtrIndex, typename... TArgs>
class BorlandRegToCdeclAdapter<TResult (TArgs...), TFuncPtrPtr, TFuncPtrIndex> : public BorlandRegToCdeclAdapterBase<TResult, TFuncPtrPtr, TFuncPtrIndex, TArgs...>
{
};

template<typename TResult, void** TFuncPtrPtr, int TFuncPtrIndex, typename... TArgs>
class BorlandRegToCdeclAdapter<TResult (*)(TArgs...), TFuncPtrPtr, TFuncPtrIndex> : public BorlandRegToCdeclAdapterBase<TResult, TFuncPtrPtr, TFuncPtrIndex, TArgs...>
{
};
