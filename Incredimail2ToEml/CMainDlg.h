#ifndef __CMainDlg_h_
#define __CMainDlg_h_

#include <string>
#include <vector>
#include <memory>
#include "CMailData.h"
#include "sqlite/sqlite3.h"

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

	std::vector<std::shared_ptr<CMailData>> fetchAllMailData(sqlite3* pDatabase);

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