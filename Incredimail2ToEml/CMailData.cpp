#include "stdafx.h"
#include "CMailData.h"

CMailData::CMailData(const std::wstring& sHeaderId, const std::wstring& sSubject, MailLocation mailLocation, int64_t nMessagePos)
{
	m_sHeaderId = sHeaderId;
	m_sSubject = sSubject;
	m_MailLocation = mailLocation;
	m_nMessagePos = nMessagePos;
}

CMailData::~CMailData()
{
}

std::wstring CMailData::getHeaderId() const
{
	return m_sHeaderId;
}

std::wstring CMailData::getSubject() const
{
	return m_sSubject;
}

CMailData::MailLocation CMailData::getMailLocation() const
{
	return m_MailLocation;
}

int64_t CMailData::getMessagePos() const
{
	return m_nMessagePos;
}