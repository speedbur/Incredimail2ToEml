#include "stdafx.h"
#include "CContainerData.h"


CContainerData::CContainerData(const std::wstring& sName, const std::wstring& sFilename)
{
	m_sName = sName;
	m_sFilename = sFilename;
}

CContainerData::~CContainerData()
{
}

std::wstring CContainerData::getName() const
{
	return m_sName;
}

std::wstring CContainerData::getFilename() const
{
	return m_sFilename;
}

std::vector<std::shared_ptr<CContainerData>> CContainerData::getChildren() const
{
	return m_aChildren;
}

void CContainerData::addChild(const std::shared_ptr<CContainerData>& pChild)
{
	m_aChildren.push_back(pChild);
}

void CContainerData::addMail(const std::shared_ptr<CMailData>& pMailData)
{
	m_aMailData.push_back(pMailData);
}

std::vector<std::shared_ptr<CMailData>> CContainerData::getMailData() const
{
	return m_aMailData;
}
