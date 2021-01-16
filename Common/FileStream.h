#pragma once

class FileStream : public Stream
{
private:
    FILE* _pFile = nullptr;

public:
                            FileStream          (const std::wstring& filePath, const wchar_t* pMode);
    virtual                 ~FileStream         ();

    virtual long long       GetPosition         () const override;
    virtual void            SetPosition         (long long position) override;

    virtual void            Seek                (int offset) override;

    virtual long long       Size                () const override;

    virtual void            SkipBits            (int count) override;
    virtual void            AlignByte           () override;

    virtual BYTE            PeekByte            () const override;
    virtual void            ReadBytes           (void* pBuffer, int size) override;
    virtual DWORD           ReadBits            (int count) override;

    virtual void            Write               (const void* pData, int size) override;
    using Stream::Write;
};
