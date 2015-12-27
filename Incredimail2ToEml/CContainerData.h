#pragma once

#include <string>
#include <vector>
#include <memory>
#include "CMailData.h"

class CContainerData
{
public:
	CContainerData(const std::wstring& sName);
	~CContainerData();

	std::wstring getName() const;
	
	std::vector<std::shared_ptr<CContainerData>> getChildren() const;
	void addChild(const std::shared_ptr<CContainerData>& pChild);

	void addMail(const std::shared_ptr<CMailData>& pMailData);
	std::vector<std::shared_ptr<CMailData>> getMailData() const;

private:
	std::wstring m_sName;
	std::vector<std::shared_ptr<CContainerData>> m_aChildren;
	std::vector<std::shared_ptr<CMailData>> m_aMailData;
};

