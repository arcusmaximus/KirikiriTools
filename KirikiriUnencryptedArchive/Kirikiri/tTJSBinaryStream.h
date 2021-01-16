#pragma once

class tTJSBinaryStream
{
public:
	virtual tjs_uint64 TJS_INTF_METHOD Seek(tjs_int64 offset, tjs_int whence) = 0;
	virtual tjs_uint TJS_INTF_METHOD Read(void* buffer, tjs_uint read_size) = 0;
	virtual tjs_uint TJS_INTF_METHOD Write(const void* buffer, tjs_uint write_size) = 0;
	virtual void TJS_INTF_METHOD SetEndOfStorage() = 0;
	virtual tjs_uint64 TJS_INTF_METHOD GetSize() = 0;
	virtual ~tTJSBinaryStream() { }

	template<typename T>
	static void ApplyWrappedVTable(T* pObj)
	{
		CompilerHelper::ApplyWrappedVTable<
			&CompilerHelper::NoChange,
	        &CompilerHelper::NoChange,
	        &CompilerHelper::NoChange,
	        &CompilerHelper::NoChange,
	        &CompilerHelper::NoChange,
	        &CompilerHelper::VirtualDestructor
        >(pObj);
	}
};
