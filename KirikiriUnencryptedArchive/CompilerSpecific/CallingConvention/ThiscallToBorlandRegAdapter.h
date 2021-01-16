#pragma once

#pragma once

template<typename TResult, void** TFuncPtrPtr, int TFuncPtrIndex, typename... TArgs>
class ThiscallToBorlandRegAdapterBase
{
    static_assert(sizeof...(TArgs) > 0);

public:
    static void Call();

private:
    static constexpr auto FuncPtrPtr = TFuncPtrPtr + TFuncPtrIndex;

    static constexpr int NumThiscallStackArgs = sizeof...(TArgs) - 1;
    static constexpr int ThiscallStackArgsSize = NumThiscallStackArgs * 4;

    static constexpr int NumBorlandStackArgs = sizeof...(TArgs) > 3 ? sizeof...(TArgs) - 3 : 0;
    static constexpr int BorlandStackArgsSize = NumBorlandStackArgs * 4;
};

template<typename TResult, void** TFuncPtrPtr, int TFuncPtrIndex, typename... TArgs>
__declspec(naked) void ThiscallToBorlandRegAdapterBase<TResult, TFuncPtrPtr, TFuncPtrIndex, TArgs...>::Call()
{
    __asm
    {
        push esi
        push edi
    }
    if constexpr (sizeof...(TArgs) > 3)
    {
        __asm
        {
            lea esi, [esp+8+4]
            mov edi, NumBorlandStackArgs

        nextStackArg:
            push dword ptr [esi]
            add esi, 4
            dec edi
            jnz nextStackArg
        }
    }
    if constexpr (sizeof...(TArgs) > 2)
    {
        __asm push ecx
    }
    if constexpr (sizeof...(TArgs) > 1)
    {
        __asm push edx
    }
    __asm
    {
        mov ecx, eax
        mov edi, FuncPtrPtr
        call dword ptr [edi]
        pop edi
        pop esi
    }
    if constexpr (sizeof...(TArgs) > 3)
    {
        __asm
        {
            pop edx
            add esp, BorlandStackArgsSize
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
class ThiscallToBorlandRegAdapter;

template<typename TResult, void** TFuncPtrPtr, int TFuncPtrIndex, typename... TArgs>
class ThiscallToBorlandRegAdapter<TResult (TArgs...), TFuncPtrPtr, TFuncPtrIndex> : public ThiscallToBorlandRegAdapterBase<TResult, TFuncPtrPtr, TFuncPtrIndex, TArgs...>
{
};

template<typename TResult, typename TClass, void** TFuncPtrPtr, int TFuncPtrIndex, typename... TArgs>
class ThiscallToBorlandRegAdapter<TResult (TClass::*)(TArgs...), TFuncPtrPtr, TFuncPtrIndex> : public ThiscallToBorlandRegAdapterBase<TResult, TFuncPtrPtr, TFuncPtrIndex, TClass*, TArgs...>
{
};
