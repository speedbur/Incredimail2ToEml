#pragma once
#include <string>

class CMailData
{
public:
	CMailData(const std::wstring& sHeaderId, const std::wstring& sSubject);
	~CMailData();

	std::wstring getHeaderId() const;
	std::wstring getSubject() const;

private:
	std::wstring m_sHeaderId;
	std::wstring m_sSubject;
};

