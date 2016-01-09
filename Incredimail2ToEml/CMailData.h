#ifndef __CMaildata_h_
#define __CMaildata_h_

#include <string>

class CMailData
{
public:
	enum class MailLocation
	{
		ImmDatabaseFile,
		Filesystem
	};
	
	CMailData(const std::wstring& sHeaderId, const std::wstring& sSubject, MailLocation mailLocation, int64_t nMessagePos);
	~CMailData();

	std::wstring getHeaderId() const;
	std::wstring getSubject() const;

	MailLocation getMailLocation() const;

	// only valid if MailLocation is ImmDatabaseFile
	int64_t getMessagePos() const;

private:
	std::wstring m_sHeaderId;
	std::wstring m_sSubject;
	MailLocation m_MailLocation;
	int64_t m_nMessagePos;
};

#endif __CMaildata_h_