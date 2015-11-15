#include "stdafx.h"
#include "CMailData.h"

CMailData::CMailData(const std::wstring& sHeaderId, const std::wstring& sContainerId, const std::wstring& sSubject)
{
	m_sHeaderId = sHeaderId;
	m_sContainerId = sContainerId;
	m_sSubject = sSubject;
}

CMailData::~CMailData()
{
}

std::wstring CMailData::getHeaderId() const
{
	return m_sHeaderId;
}

std::wstring CMailData::getContainerId() const
{
	return m_sContainerId;
}

std::wstring CMailData::getSubject() const
{
	return m_sSubject;
}
