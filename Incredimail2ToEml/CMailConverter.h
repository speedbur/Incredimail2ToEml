#ifndef _CMailConverter_h_
#define _CMailConverter_h_
#include <string>
#include <vector>

class CMailConverter
{
public:
	CMailConverter();
	~CMailConverter();

private:
	static std::vector<unsigned char> convertToCharArray(const std::string& s);
	static std::string decryptBoundaryString(const std::string& sIncredimailBoundary);
};

#endif // _CMailConverter_h_