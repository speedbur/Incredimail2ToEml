#include "stdafx.h"

#include "Incredimail2ToEml.h"
#include "CMainDlg.h"
#include "afxdialogex.h"

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_INCREDIMAIL2TOEML_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE_INCREDIMAIL_DIRECTORY, &CMainDlg::OnBnClickedBrowseIncredimailDirectory)
	ON_BN_CLICKED(IDC_BROWSE_OUTPUT_FOLDER, &CMainDlg::OnBnClickedBrowseOutputFolder)
END_MESSAGE_MAP()


BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	return TRUE; 
}

void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CMainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

std::wstring CMainDlg::openFolderDialog()
{
	IFileOpenDialog* pDialog;
	HRESULT hr = ::CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDialog));
	if (FAILED(hr))
		return L"";

	pDialog->SetOptions(FOS_PICKFOLDERS);

	hr = pDialog->Show(GetSafeHwnd());
	if (FAILED(hr)) 
	{
		pDialog->Release();
		return L"";
	}

	IShellItem* pShellItem;
	hr = pDialog->GetResult(&pShellItem);
	if (FAILED(hr)) 
	{
		pDialog->Release();
		return L"";
	}

	LPWSTR pPath;
	hr = pShellItem->GetDisplayName(SIGDN_FILESYSPATH, &pPath);
	if (FAILED(hr))
	{
		pShellItem->Release();
		pDialog->Release();
		return L"";
	}

	std::wstring sResult = pPath;
	CoTaskMemFree(pPath);

	pShellItem->Release();
	pDialog->Release();

	return sResult;
}

void CMainDlg::setFolderForId(int nId) 
{
	std::wstring sPath = openFolderDialog();
	if (!sPath.empty())
	{
		CEdit* pEdit = (CEdit*)GetDlgItem(nId);
		pEdit->SetWindowTextW(sPath.c_str());
	}
}

void CMainDlg::OnBnClickedBrowseIncredimailDirectory()
{
	setFolderForId(IDC_INCREDIMAIL_DIRECTORY);
}


void CMainDlg::OnBnClickedBrowseOutputFolder()
{
	setFolderForId(IDC_OUTPUT_DIRECTORY);
}
