#pragma once

class iTVPStorageLister
{
public:
	virtual void TJS_INTF_METHOD Add(const ttstr& file) = 0;
};

class iTVPStorageMedia
{
public:
	virtual void TJS_INTF_METHOD AddRef() = 0;
	virtual void TJS_INTF_METHOD Release() = 0;

	virtual void TJS_INTF_METHOD GetName(ttstr& name) = 0;

	virtual void TJS_INTF_METHOD NormalizeDomainName(ttstr& name) = 0;
	virtual void TJS_INTF_METHOD NormalizePathName(ttstr& name) = 0;

	virtual bool TJS_INTF_METHOD CheckExistentStorage(const ttstr& name) = 0;

	virtual tTJSBinaryStream* TJS_INTF_METHOD Open(const ttstr& name, tjs_uint32 flags) = 0;

	virtual void TJS_INTF_METHOD GetListAt(const ttstr& name, iTVPStorageLister* lister) = 0;
	virtual void TJS_INTF_METHOD GetLocallyAccessibleName(ttstr& name) = 0;
};
