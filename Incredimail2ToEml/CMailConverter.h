#ifndef _CMailConverter_h_
#define _CMailConverter_h_
#include <string>
#include <vector>

class CMailConverter
{
public:
	static bool convert(const std::wstring& sInFilename, const std::wstring& sOutFilename);
	static bool extractMailFromImmFile(const std::wstring& sImmFilename, int64_t nOffset, int64_t nLength, const std::wstring& sOutFilename);

private:
	static std::vector<unsigned char> convertToCharArray(const std::string& s);
	static std::string decryptBoundaryString(const std::string& sIncredimailBoundary);

	// delete result with delete[]
	static bool resolveAttachmentsAndWriteFile(const std::wstring& sAttachmentFolder, const char* pBuffer, const std::wstring& sOutFilename);

	static std::wstring extractBaseFolder(const std::wstring& sFilename);

	static int64_t getFileSize(const std::wstring& sFilename);
	static std::wstring convertFilename(const std::string& sFilename);

	static std::wstring CPtoWString(const std::string& sInput, int nCodepage);
};

#endif // _CMailConverter_h_