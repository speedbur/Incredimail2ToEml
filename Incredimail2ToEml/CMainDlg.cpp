#include "stdafx.h"

#include "Incredimail2ToEml.h"
#include "CMainDlg.h"
#include "afxdialogex.h"
#include <memory>
#include "sqlite/sqlite3.h"
#include "CContainerData.h"
#include <algorithm>
#include <map>
#include <thread>
#include "CMailConverter.h"

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

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
	ON_BN_CLICKED(IDC_EXECUTE, &CMainDlg::OnBnClickedExecute)
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

void CMainDlg::fetchAllMailData(sqlite3* pDatabase, const std::wstring& sContainerId, const std::shared_ptr<CContainerData>& pContainer)
{
	char* pErrorMessage = nullptr;
	sqlite3_stmt* pStatement;

	std::wstring sQuery = L"select HeaderID, Subject, Location, MsgPos, LightMsgSize from Headers where ContainerId = '" + sContainerId + L"'";
	if (sqlite3_prepare16_v2(pDatabase, sQuery.c_str(), -1, &pStatement, nullptr) != SQLITE_OK)
	{
		AfxMessageBox(L"error during maildata fetch");
		return;
	}

	while (sqlite3_step(pStatement) == SQLITE_ROW)
	{
		std::wstring sHeaderId = (wchar_t*)sqlite3_column_text16(pStatement, 0);
		std::wstring sSubject = (wchar_t*)sqlite3_column_text16(pStatement, 1);
		int nMessageLocation = sqlite3_column_int(pStatement, 2);
		int64_t nMessagePos = sqlite3_column_int64(pStatement, 3);
		int64_t nLightMessageSize = sqlite3_column_int64(pStatement, 4);

		CMailData::MailLocation location;
		if (nMessageLocation == 0)
			location = CMailData::MailLocation::ImmDatabaseFile;
		else if (nMessageLocation == 1)
			location = CMailData::MailLocation::Filesystem;
		else
		{
			AfxMessageBox(L"unknown mail location");
			return;
		}

		pContainer->addMail(std::make_shared<CMailData>(sHeaderId, sSubject, location, nMessagePos, nLightMessageSize));
	}
	sqlite3_finalize(pStatement);
}

void CMainDlg::fetchSubElement(sqlite3* pDatabase, const std::wstring& sId, const std::shared_ptr<CContainerData>& pContainer)
{
	char* pErrorMessage = nullptr;
	sqlite3_stmt* pStatement;

	std::shared_ptr<CContainerData> pResult;

	// fetch all root nodes
	std::wstring sQuery = L"select ContainerID, Label, FileName from Containers where ParentContainerID = '" + sId + L"'";
	if (sqlite3_prepare16_v2(pDatabase, sQuery.c_str(), -1, &pStatement, nullptr) != SQLITE_OK)
	{
		AfxMessageBox(L"error during maildata fetch");
		return;
	}

	std::map<std::wstring, std::shared_ptr<CContainerData>> mapContainerData;
	while (sqlite3_step(pStatement) == SQLITE_ROW)
	{
		std::wstring sContainerId = (wchar_t*)sqlite3_column_text16(pStatement, 0);
		std::wstring sLabel = (wchar_t*)sqlite3_column_text16(pStatement, 1);
		std::wstring sFilename = (wchar_t*)sqlite3_column_text16(pStatement, 2);

		mapContainerData[sContainerId] = std::make_shared<CContainerData>(sLabel, sFilename);
		pContainer->addChild(mapContainerData[sContainerId]);

	}
	sqlite3_finalize(pStatement);

	for (const auto& it : mapContainerData)
	{
		fetchSubElement(pDatabase, it.first, it.second);
		fetchAllMailData(pDatabase, it.first, it.second);
	}
}

std::shared_ptr<CContainerData> CMainDlg::fetchContainerTree(sqlite3* pDatabase)
{
	std::shared_ptr<CContainerData> pRootElement = std::make_shared<CContainerData>(L"", L"");

	std::vector<std::wstring> aChildIds;
	char* pErrorMessage = nullptr;
	sqlite3_stmt* pStatement;

	// fetch all root nodes
	if (sqlite3_prepare16_v2(pDatabase, L"select ContainerID from Containers where ParentContainerID = ''", -1, &pStatement, nullptr) != SQLITE_OK)
	{
		AfxMessageBox(L"error during maildata fetch");
		return nullptr;
	}

	while (sqlite3_step(pStatement) == SQLITE_ROW)
	{
		std::wstring sContainerId = (wchar_t*)sqlite3_column_text16(pStatement, 0);
		aChildIds.push_back(sContainerId);
		
	}
	sqlite3_finalize(pStatement);

	for (const auto& sId : aChildIds)
	{
		std::shared_ptr<CContainerData> pData = std::make_shared<CContainerData>(L"", L"");
		fetchSubElement(pDatabase, sId, pData);
		fetchAllMailData(pDatabase, sId, pData);
		pRootElement->addChild(pData);
	}

	return pRootElement;
}

void CMainDlg::cleanName(std::wstring& sName) 
{
	for (wchar_t& c : sName)
		if (c < 32 || c == L'<' || c == L'>' || c == L':' || c == L'"' || c == L'/' || c == L'\\' || c == L'|' || c == L'?' || c == L'*')
			c = L'_';
}

void CMainDlg::convertAndStoreMessage(const std::wstring& sInputDir, const std::wstring& sTargetFolder, const std::shared_ptr<CContainerData>& pFolder, volatile int& nCurrentProgress)
{
	std::wstring sName = pFolder->getName();
	cleanName(sName);
	
	std::wstring sTargetSubFolder = sTargetFolder;
	if (!sName.empty())
		sTargetSubFolder += sName + L"\\";

	// create directory
	::CreateDirectoryW(sTargetSubFolder.c_str(), nullptr);

	// write mail data
	for (const auto& pMail : pFolder->getMailData()) 
	{
		nCurrentProgress++;

		std::wstring sOutFilename = pMail->getSubject();
		cleanName(sOutFilename);
		if (sOutFilename.empty())
			sOutFilename = L"No Subject";
		if (sOutFilename.length() > 100)
			sOutFilename = sOutFilename.substr(0, 100);

		// find unique name for output file
		std::wstring sCleanedOutFilename = sTargetSubFolder + sOutFilename + L".eml";
		while (PathFileExists(sCleanedOutFilename.c_str()) == TRUE)
			sCleanedOutFilename = sTargetSubFolder + sOutFilename + L" " + pMail->getHeaderId() + L".eml";

		switch (pMail->getMailLocation())
		{
			case CMailData::MailLocation::Filesystem:
			{
				std::wstring sMessageRootDirectory = sInputDir + L"\\Messages\\1\\";
				std::wstring sInFilename = sMessageRootDirectory + pMail->getHeaderId() + L"\\msg.iml";
				CMailConverter::convert(sInFilename, sCleanedOutFilename);
				break;
			}

			case CMailData::MailLocation::ImmDatabaseFile:
			{
				std::wstring sImmFilename = sInputDir + L"\\" + pFolder->getFilename() + L".imm";
				CMailConverter::extractMailFromImmFile(sImmFilename, pMail->getMessagePos(), pMail->getLightMessageSize(), sCleanedOutFilename);
				break;
			}

			default:
				throw std::exception("unknown mail location for conversion");
		}
	}

	// iterate through sub folders
	for (const auto& pSubFolder : pFolder->getChildren()) 
	{
		convertAndStoreMessage(sInputDir, sTargetSubFolder, pSubFolder, nCurrentProgress);
	}
}

int CMainDlg::getMailCount(const std::shared_ptr<CContainerData>& pContainerData)
{
	int nResult = pContainerData->getMailData().size();
	for (const auto& pChild : pContainerData->getChildren()) 
		nResult += getMailCount(pChild);
	return nResult;
}

void CMainDlg::OnBnClickedExecute()
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_INCREDIMAIL_DIRECTORY);
	CEdit* pTargetEdit = (CEdit*)GetDlgItem(IDC_OUTPUT_DIRECTORY);
	CButton* pConvertButton = (CButton*)GetDlgItem(IDC_EXECUTE);
	CButton* pCloseButton = (CButton*)GetDlgItem(IDOK);
	pConvertButton->EnableWindow(FALSE);
	pCloseButton->EnableWindow(FALSE);

	CProgressCtrl* pProgress = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
	pProgress->SetRange32(0, 1);
	pProgress->SetPos(0);

	CString sInputDir;
	CString sOutputDir;
	pEdit->GetWindowTextW(sInputDir);
	pTargetEdit->GetWindowTextW(sOutputDir);
	sOutputDir += L"\\";

	std::wstring sInputFilename = sInputDir + L"\\MessageStore.db";

	sqlite3* pDatabase;
	if (sqlite3_open16(sInputFilename.c_str(), &pDatabase))
	{
		sqlite3_close(pDatabase);
		::AfxMessageBox(L"could not open database connection");
		return;
	}
	std::shared_ptr<CContainerData> pContainerData = fetchContainerTree(pDatabase);
	sqlite3_close(pDatabase);

	::CreateDirectoryW(sOutputDir, nullptr);

	int nEmailCount = getMailCount(pContainerData);
	pProgress->SetRange32(0, nEmailCount);

	volatile bool bFinished = false;
	volatile int nCurrentProgress = 0;

	std::wstring sOutputDirW = sOutputDir;
	std::thread thread([&bFinished, sInputDir, sOutputDirW, pContainerData, &nCurrentProgress, this]()
	{
		try
		{
			std::wstring sInputDirWString = sInputDir;
			convertAndStoreMessage(sInputDirWString, sOutputDirW, pContainerData, nCurrentProgress);
		}
		catch (...)
		{

		}
		bFinished = true;
	});
	
	MSG msg;
	while (!bFinished)
	{
		pProgress->SetPos(nCurrentProgress);

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		::Sleep(1);
	}

	pConvertButton->EnableWindow(TRUE);
	pCloseButton->EnableWindow(TRUE);
}
