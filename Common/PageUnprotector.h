#pragma once

class PageUnprotector
{
public:
	PageUnprotector(void* base, int size);
	~PageUnprotector();
	
private:
	void* _base;
	int _size;
	DWORD _originalProtect;
};
