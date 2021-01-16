#pragma once

struct MsvcRttiCompleteObjectLocator
{
	int Signature;
	int Offset;
	int ConstructorDisplacementOffset;
	type_info* pTypeDescriptor;
	void* pHierarchyDescriptor;
};
