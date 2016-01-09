#include "stdafx.h"
#include "CMailConverter.h"
#include <vector>
#include <fstream>
#include <locale>
#include <codecvt>
#include <algorithm>
#include "base64.h"

int64_t CMailConverter::getFileSize(const std::wstring& sFilename)
{
	struct _stat32 stat_buf;
	_wstat32(sFilename.c_str(), &stat_buf);
	return stat_buf.st_size;
}

bool CMailConverter::extractMailFromImmFile(const std::wstring& sImmFilename, int64_t nOffset, int64_t nLength, const std::wstring& sOutFilename)
{
	FILE* pFile;
	_wfopen_s(&pFile, sImmFilename.c_str(), L"rb");

	char* pBuffer = new char[(size_t)nLength+1];
	pBuffer[nLength] = '\0';
	_fseeki64(pFile, nOffset, SEEK_SET);
	fread_s(pBuffer, (size_t)nLength, 1, (size_t)nLength, pFile);
	fclose(pFile);

	bool bResult = resolveAttachmentsAndWriteFile(extractBaseFolder(sImmFilename) + L"\\Attachments", pBuffer, sOutFilename);
	delete[] pBuffer;
	return bResult;
}

bool CMailConverter::convert(const std::wstring& sInFilename, const std::wstring& sOutFilename)
{	
	// read the whole mail... could be big i know, but its easy ;)
	int nLength = (int)getFileSize(sInFilename);
	FILE* pFile;
	_wfopen_s(&pFile, sInFilename.c_str(), L"rb");
	char* pBuffer = new char[static_cast<size_t>(nLength)+1];
	pBuffer[nLength] = '\0';
	fread(pBuffer, 1, nLength, pFile);
	fclose(pFile);

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

	bool bResult = resolveAttachmentsAndWriteFile(extractBaseFolder(sInFilename) + L"\\Attachments", pBuffer, sOutFilename);
	delete[] pBuffer;
	return bResult;
}

std::wstring CMailConverter::extractBaseFolder(const std::wstring& sFilename)
{
	std::wstring::size_type lastSlashPosition = sFilename.find_last_of(L'\\');
	return sFilename.substr(0, lastSlashPosition);
}

std::wstring CMailConverter::convertFilename(const std::string& sFilename) 
{
	std::string sIsoStart = "=?iso-8859-1?B?";
	if (sIsoStart.length() > sFilename.length())
		return CPtoWString(sFilename, CP_UTF8);

	if (memcmp(sIsoStart.c_str(), sFilename.c_str(), sIsoStart.length()) == 0)
	{
		//=?iso-8859-1?B?SGFuZG91dCBMeXJpay1XZXJrc3RhdHQgT3NuYWJy/GNrLmRvYw==?=
		std::string sMimeString = sFilename.substr(sIsoStart.length(), sFilename.length() - sIsoStart.length() - 2);
		return CPtoWString(base64_decode(sMimeString), CP_ACP);
	}
	
	return CPtoWString(sFilename, CP_UTF8);
}

std::wstring CMailConverter::CPtoWString(const std::string& sInput, int nCodePage)
{
	int nNeededSize = ::MultiByteToWideChar(nCodePage, 0, sInput.c_str(), -1, nullptr, 0);
	wchar_t* pOutput = new wchar_t[nNeededSize];
	::MultiByteToWideChar(nCodePage, 0, sInput.c_str(), -1, pOutput, nNeededSize);
	std::wstring sResult = pOutput;
	delete[] pOutput;
	return sResult;
}

bool CMailConverter::resolveAttachmentsAndWriteFile(const std::wstring& sAttachmentFolder, const char* pBuffer, const std::wstring& sOutFilename)
{
	int nLength = strlen(pBuffer);

	// write the new file
	FILE* pFile;
	_wfopen_s(&pFile, sOutFilename.c_str(), L"wb");
	
	// try to find attachment marker
	int nFilePathMarker = strlen("----------[%ImFilePath%]----------");
	int i = 0;
	int nWriteStart = 0;
	while (i < nLength - nFilePathMarker)
	{
		if (memcmp(&pBuffer[i], "----------[%ImFilePath%]----------", nFilePathMarker) == 0)
		{
			fwrite(&pBuffer[nWriteStart], 1, i - nWriteStart, pFile);

			i += nFilePathMarker;
			const char* pStartFilename = &pBuffer[i];
			while (pBuffer[i] != '\r' && (pBuffer[i] != '\n'))
				i++;
			
			// write base64 data
			std::string sFilenameA(pStartFilename, &pBuffer[i]);
			std::wstring sFilename = sAttachmentFolder + L"\\" + convertFilename(sFilenameA);

			int64_t nFilesize = getFileSize(sFilename);
			FILE* pAttachmentFile;
			_wfopen_s(&pAttachmentFile, sFilename.c_str(), L"rb");
			unsigned char* pAttachmentBuffer = new unsigned char[static_cast<size_t>(nFilesize)];
			fread(pAttachmentBuffer, 1, (size_t)nFilesize, pAttachmentFile);
			fclose(pAttachmentFile);
			std::string sBase64EncodedData = base64_encode(pAttachmentBuffer, (unsigned int)nFilesize);
			delete[] pAttachmentBuffer;

			int nLineWidth = 76;
			std::string::size_type nPosition = 0;
			std::string::size_type nRemaining = sBase64EncodedData.length();
			while (nRemaining > 0) 
			{
				std::string::size_type nToWrite = std::min<std::string::size_type>(nLineWidth, nRemaining);
				const char* pLine = sBase64EncodedData.c_str() + nPosition;
				fwrite(pLine, 1, nToWrite, pFile);
				fwrite("\r\n", 1, 2, pFile);
				if (nLineWidth > (int)nRemaining)
					nRemaining = 0;
				else
					nRemaining -= nLineWidth;
				nPosition += nToWrite;
			}
			nWriteStart = i;
		}
		i++;
	}

	// write remaining data
	if (i - nWriteStart > 0)
		fwrite(&pBuffer[nWriteStart], 1, i - nWriteStart + nFilePathMarker, pFile);

	fclose(pFile);

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
