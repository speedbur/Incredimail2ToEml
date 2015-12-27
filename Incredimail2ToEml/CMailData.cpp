#include "stdafx.h"
#include "CMailData.h"

CMailData::CMailData(const std::wstring& sHeaderId, const std::wstring& sSubject)
{
	m_sHeaderId = sHeaderId;
	m_sSubject = sSubject;
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
