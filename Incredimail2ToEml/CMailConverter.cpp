#include "stdafx.h"
#include "CMailConverter.h"
#include <vector>

CMailConverter::CMailConverter()
{
}


CMailConverter::~CMailConverter()
{
}

std::vector<unsigned char> CMailConverter::convertToCharArray(const std::string& s)
{
	static std::string CHAR_TABLE = "0123456789abcdef";

	std::vector<unsigned char> aResult;
	const char* pData = s.c_str();
	int nCount = s.length() / 2;
	for (int i = 0; i < nCount; i++)
	{
		unsigned char nHigh = static_cast<unsigned char>(CHAR_TABLE.find(tolower(*pData)));
		pData++;
		unsigned char nLow = static_cast<unsigned char>(CHAR_TABLE.find(tolower(*pData)));
		pData++;

		aResult.push_back(nHigh << 4 | nLow);
	}
	
	return aResult;
}

std::string CMailConverter::decryptBoundaryString(const std::string& sIncredimailBoundary)
{
	// bit mapping table
	// incredi    ascii
	// 0       -> 4
	// 1       -> 7
	// 2       -> 0
	// 3       -> 2
	// 4       -> 1
	// 5       -> 6
	// 6       -> 3
	// 7       -> 5

	if (sIncredimailBoundary.length() % 2 != 0)
		throw std::exception("incredimail boundary must have an even length");

	std::vector<unsigned char> aBytes = convertToCharArray(sIncredimailBoundary);
	for (auto& byte : aBytes)
	{
		byte = (byte & 0x01) << 5   // 0 -> 4 
			| (byte & 0x02) << 6    // 1 -> 7
			| (byte & 0x04) >> 2    // 2 -> 0 
			| (byte & 0x08) >> 1    // 3 -> 2
			| (byte & 0x10) >> 3    // 4 -> 1
			| (byte & 0x20) << 1    // 5 -> 6 
			| (byte & 0x40) >> 3    // 6 -> 3
			| (byte & 0x80) >> 3;    // 7 -> 5
	}
	aBytes.push_back(0);
	std::string sResult = reinterpret_cast<char*>(&aBytes[0]);
	return sResult;
}
