#pragma once

struct BorlandTypeDescriptor
{
	int tpDtt;
	short tpMask;
	short tpName;
	int bParent;
	int tpcFlags;
	short Size;
	short ExpDim;
	int mfnDel;
	short mfnMask;
	short mfnMaskArr;
	int mfnDelArr;
	int DtorCount;
	int DtorAltCount;
	void* DtorAddr;
	short DtorMask;
	short DtorMemberOff;
	char Name[1];
};
