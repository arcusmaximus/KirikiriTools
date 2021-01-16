#include "stdafx.h"

PageUnprotector::PageUnprotector(void* base, int size)
{
	_base = base;
	_size = size;
	VirtualProtect(_base, _size, PAGE_READWRITE, &_originalProtect);
}

PageUnprotector::~PageUnprotector()
{
	DWORD dummy;
	VirtualProtect(_base, _size, _originalProtect, &dummy);
}
