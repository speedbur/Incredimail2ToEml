#ifndef __CMainDlg_h_
#define __CMainDlg_h_

class CMainDlg : public CDialogEx
{
public:
	CMainDlg(CWnd* pParent = NULL);	// standard constructor

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INCREDIMAIL2TOEML_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};

#endif