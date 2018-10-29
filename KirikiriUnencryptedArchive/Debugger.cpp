#include "stdafx.h"

using namespace std;

void* Debugger::ExceptionHandlerHandle;
Debugger::Breakpoint Debugger::Breakpoints[4];
Debugger::Breakpoint* Debugger::CurrentBreakpoint;

void Debugger::AddBreakpoint(void* ptr, BreakpointSize size, BreakpointMode mode, function<void()> callback)
{
    if (FindBreakpoint(ptr) != nullptr)
        throw "A breakpoint is already active at the specified address.";

    Breakpoint* pBreakpoint = FindBreakpoint(nullptr);
    if (pBreakpoint == nullptr)
        throw "The maximum amount of breakpoints has already been placed.";

    pBreakpoint->Set(ptr, size, mode, callback);

    if (ExceptionHandlerHandle == nullptr)
        ExceptionHandlerHandle = AddVectoredExceptionHandler(true, HandleException);
}

void Debugger::RemoveBreakpoint(void* ptr)
{
    Breakpoint* pBreakpoint = FindBreakpoint(ptr);
    if (pBreakpoint == nullptr)
        throw "No breakpoint exists at the specified address.";

    pBreakpoint->Clear();

    if (GetNumActiveBreakpoints() == 0)
    {
        RemoveVectoredExceptionHandler(ExceptionHandlerHandle);
        ExceptionHandlerHandle = nullptr;
    }
}

long Debugger::HandleException(EXCEPTION_POINTERS* pException)
{
    if (pException->ExceptionRecord->ExceptionCode != STATUS_SINGLE_STEP)
        return EXCEPTION_CONTINUE_SEARCH;

    Breakpoint* pBreakpoint = nullptr;
    for (int i = 0; i < 4; i++)
    {
        if (pException->ContextRecord->Dr6 & (1 << i))
        {
            pBreakpoint = &Breakpoints[i];
            break;
        }
    }

    if (pBreakpoint)
    {
        CurrentBreakpoint = pBreakpoint;
        pException->ContextRecord->Esp -= 4;
        ((void **)pException->ContextRecord->Esp)[0] = (void *)pException->ContextRecord->Eip;
        pException->ContextRecord->Eip = (DWORD)&ExecuteBreakpointCallbackWrap;
    }

    pException->ContextRecord->Dr6 = 0;
    return EXCEPTION_CONTINUE_EXECUTION;
}

void __declspec(naked) Debugger::ExecuteBreakpointCallbackWrap()
{
    __asm
    {
        pushfd
        pushad
        call ExecuteBreakpointCallback
        popad
        popfd
        ret
    }
}

void Debugger::ExecuteBreakpointCallback()
{
    CurrentBreakpoint->Callback();
    CurrentBreakpoint = nullptr;
    CleanBreakpoints();
}

Debugger::Breakpoint* Debugger::FindBreakpoint(void* ptr)
{
    for (int i = 0; i < 4; i++)
    {
        if (Breakpoints[i].Address == ptr)
            return &Breakpoints[i];
    }
    return nullptr;
}

int Debugger::GetBreakpointIndex(Breakpoint* pBreakpoint)
{
    return pBreakpoint - Breakpoints;
}

int Debugger::GetNumActiveBreakpoints()
{
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        if (Breakpoints[i].Address != nullptr)
            count++;
    }
    return count;
}

void Debugger::CleanBreakpoints()
{
    for (int i = 0; i < 4; i++)
    {
        if (Breakpoints[i].Address == nullptr)
            Breakpoints[i].Callback = nullptr;
    }
}

void Debugger::Breakpoint::Set(void* ptr, BreakpointSize size, BreakpointMode mode, function<void()> callback)
{
    Address = ptr;
    Callback = callback;

    CONTEXT context;
    context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    GetThreadContext(GetCurrentThread(), &context);

    int index = Debugger::GetBreakpointIndex(this);
    (&context.Dr0)[index] = (DWORD)ptr;
    context.Dr7 |= 1 << (2 * index);
    context.Dr7 |= (DWORD)mode << (16 + 4 * index);
    context.Dr7 |= (DWORD)size << (18 + 4 * index);

    SetThreadContext(GetCurrentThread(), &context);
}

void Debugger::Breakpoint::Clear()
{
    Address = nullptr;
    if (Debugger::CurrentBreakpoint == nullptr)
        Callback = nullptr;

    CONTEXT context;
    context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    GetThreadContext(GetCurrentThread(), &context);

    int index = Debugger::GetBreakpointIndex(this);
    context.Dr7 &= ~(1 << (2 * index));

    SetThreadContext(GetCurrentThread(), &context);
}
