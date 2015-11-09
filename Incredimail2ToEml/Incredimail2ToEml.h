#ifndef _Incredimail2TmEml_h_
#define _Incredimail2TmEml_h_

#include "resource.h"		// main symbols

class CIncredimail2ToEmlApp : public CWinApp
{
public:
	CIncredimail2ToEmlApp();

public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CIncredimail2ToEmlApp theApp;

#endif