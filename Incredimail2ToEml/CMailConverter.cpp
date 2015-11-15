#include "stdafx.h"
#include "CMailConverter.h"
#include <vector>
#include <fstream>
#include <regex>

CMailConverter::CMailConverter()
{

}

CMailConverter::~CMailConverter()
{	

}

bool CMailConverter::convert(const std::wstring& sInFilename, const std::wstring& sOutFilename)
{	
	// read the whole mail... could be big i know, but its easy ;)
	std::ifstream inStream;
	inStream.open(sInFilename.c_str());
	inStream.seekg(0, std::ios::end);
	int64_t nLength = inStream.tellg();
	char* pBuffer = new char[nLength];
	inStream.seekg(0, std::ios::beg);
	inStream.read(pBuffer, nLength);
	inStream.close();

	// find line with "imbndary" tag
	int nImbndaryLen = strlen("imbndary");
	for (auto i = 0; i < nLength - nImbndaryLen; i++)
	{
		if (memcmp(&pBuffer[i], "imbndary", nImbndaryLen) == 0)
		{
			// replace string with boundary... this string is always smaller, then the source string, so we can replace it inline
			char* pBeginningOfImbndarySection = &pBuffer[i];
			char* pBeginingOfImbndarySectionData = nullptr;
			char* pEndingOfImbndarySection = pBeginningOfImbndarySection;
			int nEndingCounter = 0;
			while (nEndingCounter < 2 && pEndingOfImbndarySection != &pBuffer[nLength])
			{
				pEndingOfImbndarySection++;
				if (*pEndingOfImbndarySection == '"')
				{
					if (pBeginingOfImbndarySectionData == nullptr)
						pBeginingOfImbndarySectionData = pEndingOfImbndarySection;
					nEndingCounter++;
				}
			}

			std::string sBoundaryData(pBeginingOfImbndarySectionData + 1, pEndingOfImbndarySection);
			std::string sDecryptedBoundary = decryptBoundaryString(sBoundaryData);
			
			std::string sNewData = "boundary=\"" + sDecryptedBoundary + "\"";
			memcpy_s(pBeginningOfImbndarySection, pEndingOfImbndarySection - pBeginningOfImbndarySection, sNewData.c_str(), sNewData.length());
			memset(pBeginningOfImbndarySection + sNewData.length(), ' ', pEndingOfImbndarySection - pBeginningOfImbndarySection - sNewData.length() + 1);
			break;
		}
	}

	// write the new file
	std::ofstream outStream(sOutFilename.c_str(), std::ios::out);
	outStream.write(pBuffer, nLength);
	outStream.close();

	delete[] pBuffer;
	return true;
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
