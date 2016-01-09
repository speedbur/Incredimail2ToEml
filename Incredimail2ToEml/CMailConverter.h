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


};

#endif // _CMailConverter_h_