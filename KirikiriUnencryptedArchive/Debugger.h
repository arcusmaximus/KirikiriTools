#pragma once

enum class BreakpointMode
{
    Execute   = 0b00,
    Write     = 0b01,
    ReadWrite = 0b11
};

enum class BreakpointSize
{
    Byte    = 0b00,
    Word    = 0b01,
    Dword   = 0b11,
    Qword   = 0b10
};

class Debugger
{
public:
    

    static void             AddBreakpoint                   (void* ptr, BreakpointSize size, BreakpointMode mode, std::function<void ()> callback);
    static void             RemoveBreakpoint                (void* ptr);

private:
    struct Breakpoint
    {
        void* Address;
        std::function<void()> Callback;

        void                    Set                             (void* ptr, BreakpointSize size, BreakpointMode mode, std::function<void()> callback);
        void                    Clear                           ();
    };

    static long __stdcall   HandleException                 (EXCEPTION_POINTERS* pException);
    static void __stdcall   ExecuteBreakpointCallbackWrap   ();
    static void __stdcall   ExecuteBreakpointCallback       ();

    static Breakpoint*      FindBreakpoint                  (void* ptr);
    static int              GetBreakpointIndex              (Breakpoint* pBreakpoint);
    static int              GetNumActiveBreakpoints         ();
    static void             CleanBreakpoints                ();

    static void* ExceptionHandlerHandle;
    static Breakpoint Breakpoints[4];
    static Breakpoint* CurrentBreakpoint;
};
