#pragma once

class CompilerHelper
{
public:
    static void         Init                        ();

    static void**       FindVTable                  (const std::string& className);
    static void**       FindVTable                  (HMODULE hModule, CompilerType compilerType, const std::string& className);

    static inline CompilerType CompilerType{};

    template<typename TResult, void** TFuncPtrPtr, typename... TArgs>
    static TResult      CallStaticMethod            (TArgs... args)
    {
        switch (CompilerType)
        {
            case CompilerType::Borland:
                return BorlandRegToCdeclAdapter<TResult (TArgs...), TFuncPtrPtr>::Call(args...);

            case CompilerType::Msvc:
                return static_cast<TResult (*)(TArgs...)>(*TFuncPtrPtr)(args...);

            default:
                throw std::exception("Unsupported compiler type");
        }
    }

    template<typename TResult, void** TFuncPtrPtr, typename... TArgs>
    static TResult      CallInstanceMethod          (TArgs... args)
    {
        switch (CompilerType)
        {
            case CompilerType::Borland:
                return BorlandRegToCdeclAdapter<TResult (TArgs...), TFuncPtrPtr>::Call(args...);

            case CompilerType::Msvc:
                return ThiscallToCdeclAdapter<TResult (TArgs...), TFuncPtrPtr>::Call(args...);

            default:
                throw std::exception("Unsupported compiler type");
        }
    }

    template<auto* TFuncPtr>
    static void*        WrapAsStaticMethod          ()
    {
        static void* FuncPtr = TFuncPtr;
        switch (CompilerType)
        {
            case CompilerType::Borland:
                return &CdeclToBorlandRegAdapter<decltype(TFuncPtr), &FuncPtr>::Call;

            case CompilerType::Msvc:
                return TFuncPtr;

            default:
                throw std::exception("Unsupported compiler type");
        }
    }

    template<template<::CompilerType> typename TCallbackClass>
    static void*        WrapAsStaticMethod          ()
    {
        switch (CompilerType)
        {
            case CompilerType::Borland:
                return WrapAsStaticMethod<&TCallbackClass<CompilerType::Borland>::Call>();

            case CompilerType::Msvc:
                return WrapAsStaticMethod<&TCallbackClass<CompilerType::Msvc>::Call>();

            default:
                throw std::exception("Unsupported compiler type");
        }
    }

    template<auto* TFuncPtr>
    static void*         WrapAsInstanceMethod       ()
    {
        static void* FuncPtr = TFuncPtr;
        switch (CompilerType)
        {
            case CompilerType::Borland:
                return &CdeclToBorlandRegAdapter<decltype(TFuncPtr), &FuncPtr>::Call;

            case CompilerType::Msvc:
                return &CdeclToThiscallAdapter<decltype(TFuncPtr), &FuncPtr>::Call;

            default:
                throw std::exception("Unsupported compiler type");
        }
    }

    template<template<::CompilerType> typename TCallbackClass>
    static void*        WrapAsInstanceMethod        ()
    {
        switch (CompilerType)
        {
            case CompilerType::Borland:
                return WrapAsInstanceMethod<&TCallbackClass<CompilerType::Borland>::Call>();

            case CompilerType::Msvc:
                return WrapAsInstanceMethod<&TCallbackClass<CompilerType::Msvc>::Call>();

            default:
                throw std::exception("Unsupported compiler type");
        }
    }

    template<auto... TFuncPtrs>
    static void         ApplyWrappedVTable      (void* pObj)
    {
        *(void**)pObj = WrapVTable<TFuncPtrs...>(*(void**)pObj);
    }

    template<auto... TFuncPtrs>
    static void*        WrapVTable              (void* pVTable)
    {
        switch (CompilerType)
        {
            case CompilerType::Borland:
                return VTableAdapter<TFuncPtrs...>::AdaptToBorlandReg(pVTable);

            case CompilerType::Msvc:
                return pVTable;

            default:
                throw std::exception("Unsupported compiler type");
        }
    }

    struct SpecialVirtualFunction
    {
    };

    static inline SpecialVirtualFunction NoChange{};
    static inline SpecialVirtualFunction VirtualDestructor{};

private:
    template<auto... TFuncPtrs>
    class VTableAdapter
    {
    public:
        static void*        AdaptToBorlandReg               (void *pOrigVTable)
        {
            return AdaptThiscallToBorlandReg(pOrigVTable, std::make_index_sequence<sizeof...(TFuncPtrs)>());
        }

    private:
        template<size_t... TIndexes>
        static void*        AdaptThiscallToBorlandReg       (void* pOrigVTable, std::index_sequence<TIndexes...> indexes)
        {
            static void* origVTable[sizeof...(TFuncPtrs)];
            if (origVTable[0] == nullptr)
                memcpy(origVTable, pOrigVTable, sizeof(origVTable));

            static void* newVTable[] = { CallingConventionAdapter<origVTable, TIndexes>::AdaptThiscallToBorlandReg(TFuncPtrs)... };
            return &newVTable;
        }
    };

    template<void** TFuncPtrPtr, int TFuncPtrIndex>
    class CallingConventionAdapter
    {
    public:
        template<typename TResult, typename TClass, typename... TArgs>
        static constexpr void*  AdaptThiscallToBorlandReg       (TResult (TClass::*pFunc)(TArgs...))
        {
            return &ThiscallToBorlandRegAdapter<TResult (TClass::*)(TArgs...), TFuncPtrPtr, TFuncPtrIndex>::Call;
        }

        static constexpr void*  AdaptThiscallToBorlandReg       (SpecialVirtualFunction* pFunc)
        {
            if (pFunc == &NoChange)
            {
                void** ppFunc = TFuncPtrPtr + TFuncPtrIndex;
                return *ppFunc;
            }

            if (pFunc == &VirtualDestructor)
                return &ThiscallToBorlandRegAdapter<void (void*, int), TFuncPtrPtr, TFuncPtrIndex>::Call;

            throw std::exception("Unsupported special member function");
        }
    };

    static bool         HasBorlandTypeDescriptor    (void** pVTable, const std::string& className, void* pModuleStart, void* pModuleEnd);
    static bool         HasMsvcTypeDescriptor       (void** pVTable, const std::string& className, void* pModuleStart, void* pModuleEnd);
};
