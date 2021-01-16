#pragma once

template<CompilerType type, typename T>
struct CompilerSpecificVector;

template<typename T>
struct CompilerSpecificVector<CompilerType::Msvc, T>
{
public:
    T* begin()
    {
        return _start;
    }

    T* end()
    {
        return _finish;
    }

private:
    T* _start;
    T* _finish;
    T* _end_of_storage;
};

template<typename T>
struct CompilerSpecificVector<CompilerType::Borland, T>
{
public:
    T* begin()
    {
        return _start;
    }

    T* end()
    {
        return _finish;
    }

private:
    int _buffer_size;
    T* _start;
    T* _finish;
    int paddingC[3];
    T* _end_of_storage;
    int padding1C;
};
