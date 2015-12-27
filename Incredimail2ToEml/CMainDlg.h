#ifndef __CMainDlg_h_
#define __CMainDlg_h_

#include <string>
#include <vector>
#include <memory>
#include "CMailData.h"
#include "sqlite/sqlite3.h"
#include "CContainerData.h"

class CMainDlg : public CDialogEx
{
public:
	CMainDlg(CWnd* pParent = NULL);	// standard constructor

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INCREDIMAIL2TOEML_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	HICON m_hIcon;

	void fetchAllMailData(sqlite3* pDatabase, const std::wstring& sContainerId, const std::shared_ptr<CContainerData>& pContainer);
	std::shared_ptr<CContainerData> fetchContainerTree(sqlite3* pDatabase);
	void fetchSubElement(sqlite3* pDatabase, const std::wstring& sId, const std::shared_ptr<CContainerData>& pContainer);

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	std::wstring openFolderDialog();
	void setFolderForId(int nId);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowseIncredimailDirectory();
	afx_msg void OnBnClickedBrowseOutputFolder();
	afx_msg void OnBnClickedExecute();
};

#endif