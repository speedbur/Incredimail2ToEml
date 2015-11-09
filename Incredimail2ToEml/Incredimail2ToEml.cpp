#include "stdafx.h"
#include "Incredimail2ToEml.h"
#include "CMainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CIncredimail2ToEmlApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CIncredimail2ToEmlApp::CIncredimail2ToEmlApp()
{
}

CIncredimail2ToEmlApp theApp;

BOOL CIncredimail2ToEmlApp::InitInstance()
{
	CWinApp::InitInstance();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	//CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	CMainDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	// Delete the shell manager created above.
	/*if (pShellManager != NULL)
	{
		delete pShellManager;
	}*/

	return FALSE;
}

