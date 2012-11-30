// FetchWebDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FetchWeb.h"
#include "FetchWebDlg.h"

//#include "Utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFetchWebDlg dialog

CFetchWebDlg::CFetchWebDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFetchWebDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFetchWebDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
	
}

void CFetchWebDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFetchWebDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFetchWebDlg, CDialog)
//{{AFX_MSG_MAP(CFetchWebDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDFINISH, OnFinish)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFetchWebDlg message handlers

BOOL CFetchWebDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Add "About..." menu item to system menu.
	
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	CComboBox *pAction=static_cast<CComboBox *>(GetDlgItem(IDC_ACTION));
	
	pAction->SetCurSel(0);

	CComboBox *pNoConnections=static_cast<CComboBox *>(GetDlgItem(IDC_MAX_CONNECTIONS));
	
	pNoConnections->SetCurSel(5);


	this->SetDlgItemText(IDC_PROJECT_NAME,"Relisoft");
	this->SetDlgItemText(IDC_START_URL,"http://www.relisoft.com/book/index.htm");
	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFetchWebDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFetchWebDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
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
		CDialog::OnPaint();
	}
	DisplayBMP(IDB_BITMAP_FETCHWEB);
	this->SetDlgItemText(IDC_NOTES,"Welcome To FetchWeb.");	
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFetchWebDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFetchWebDlg::OnFinish() 
{
	// TODO: Add extra validation here
	
	CString sProjectName;
	CString sStartURL;
	CString sProxy;
	CString sBrowserID;
	CString sUserName;
	CString sPassword;
	UINT	nAction=0;
	UINT	nPort=0;
	UINT    nDepth=0;
	UINT    nMaxSize = 0;
	UINT	nMaxTime = 0;
	UINT	nMaxConnections =0;
	UINT	nTimeOut = 0;
	UINT	nRetries = 0;
	UINT	nBrowserID = 0;
	
	UINT    nTimeOutFinal;
	LONG    lMaxTime;
	DWORD   dwMaxSize;
	
	BOOL bPass = TRUE;
	
	
	BOOL    bDirStructure=TRUE;
	BOOL    bExternalServerAllowed = FALSE;
	
	SITE_CONFIG SiteConfig;
	
	this->GetDlgItemText(IDC_PROJECT_NAME,sProjectName);
	this->GetDlgItemText(IDC_START_URL,sStartURL);
	CComboBox *pAction=static_cast<CComboBox *>(GetDlgItem(IDC_ACTION));
	nAction = pAction->GetCurSel();
	
	switch (nAction)
	{
	case 0:
		bDirStructure = TRUE;
		bExternalServerAllowed = FALSE;
		break;
	case 1:
		bDirStructure = TRUE;
		bExternalServerAllowed = TRUE;
		break;
	case 2:
		bDirStructure = FALSE;
		bExternalServerAllowed = FALSE;
		break;
	}
	
	this->GetDlgItemText(IDC_PROXY,sProxy);
	nPort = this->GetDlgItemInt(IDC_PORT,NULL,FALSE);
	
	CComboBox *pDepth=static_cast<CComboBox *>(GetDlgItem(IDC_MAX_DEPTH));
	nDepth = pDepth->GetCurSel();
	
	nMaxSize = this->GetDlgItemInt(IDC_MAX_SIZE,NULL,FALSE);
	dwMaxSize = nMaxSize;
	
	CComboBox *pMaxTime=static_cast<CComboBox *>(GetDlgItem(IDC_MAX_TIME));
	nMaxTime = pMaxTime->GetCurSel();
	
	switch(nMaxTime)
	{
	case 0:
		lMaxTime = 0;
		break;
	case 1:
		lMaxTime = 1800;
		break;	
	case 2:
		lMaxTime = 3600;
		break;	
	case 3:
		lMaxTime = 7200;
		break;	
	default:
		lMaxTime = 0;
		break;
		
	}
	
	
	CComboBox *pMaxConnections=static_cast<CComboBox *>(GetDlgItem(IDC_MAX_CONNECTIONS));
	nMaxConnections = pMaxConnections->GetCurSel()+1;
	if(nMaxConnections <1 || nMaxConnections>8)
		nMaxConnections = 2;
	
	
	
	CComboBox *pTimeOut=static_cast<CComboBox *>(GetDlgItem(IDC_TIME_OUT));
	nTimeOut = pTimeOut->GetCurSel();
	
	switch(nTimeOut)
	{
	case 0:
		nTimeOutFinal = 0;
		break;
	case 1:
		nTimeOutFinal = 30;
		break;
		
	case 2:
		nTimeOutFinal = 60;
		break;
	case 3:
		nTimeOutFinal = 120;
		break;
		
	case 4:
		nTimeOutFinal = 180;
		break;
		
	case 5:
		nTimeOutFinal = 300;
		break;
		
	case 6:
		nTimeOutFinal = 600;
		break;
		
	case 7:
		nTimeOutFinal = 1200;
		break;
	default:
		nTimeOutFinal = 1200;
		break;
	}
	CComboBox *pRetries=static_cast<CComboBox *>(GetDlgItem(IDC_RETRIES));
	nRetries = pRetries->GetCurSel();
	
	
	this->GetDlgItemText(IDC_BROWSER_ID,sBrowserID);
	
	this->GetDlgItemText(IDC_USER_NAME,sUserName);
	this->GetDlgItemText(IDC_PASSWORD,sPassword);
	
	SiteConfig.sProjectName			  = sProjectName;	
	SiteConfig.bDirStructure		  = bDirStructure;
	SiteConfig.bExternalServerAllowed = bExternalServerAllowed;
	SiteConfig.dwMaxSize			  = nMaxSize;
	SiteConfig.lpszAppName			  = sBrowserID;
	SiteConfig.lpszProxy			  = sProxy;
	SiteConfig.nConnections			  = nMaxConnections;
	SiteConfig.nPort				  = nPort;
	SiteConfig.nRetries				  = nRetries;
	SiteConfig.ntimeOut				  = nTimeOut;
	SiteConfig.sStartURL			  = sStartURL;	
	SiteConfig.lpszPassword			  = sPassword;
	SiteConfig.lpszUserName			  = sUserName;
	SiteConfig.lMaxTime				  = lMaxTime;	
	
	
	if(IsURLOK(sStartURL) == FALSE)
	{
		bPass = FALSE;
		MessageBox("Incorrect URL.");
	}
	
	if(sProjectName.IsEmpty())
	{
		bPass = FALSE;
		MessageBox("Please enter Project Name.");
	}
	
	if(bPass == TRUE)
	{
		MessageBox("Allah is Great");
		//this->ShowWindow(SW_HIDE);		
		CDownloading aDownloadingDlg;
		aDownloadingDlg.InitializeDownloadingDlg(SiteConfig);
		aDownloadingDlg.DoModal();
		//this->ShowWindow(SW_SHOWDEFAULT);
	}		
	
}

void CFetchWebDlg::OnOK() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
	
}
ConfigFiles* ConfigFiles::m_pConfigFiles = NULL;
URLDataBase* URLDataBase::m_pURLDataBase = NULL;




BOOL CFetchWebDlg::IsURLOK(CString sBaseURL)
{
	if(sBaseURL.IsEmpty() == TRUE)
		return FALSE;
	
	DWORD dwServiceType=0;
	CString strServer;
	CString strObject;
	INTERNET_PORT nPort;
	sBaseURL.Replace('\\','/');
	if(!AfxParseURL(sBaseURL,dwServiceType,strServer,strObject,nPort ) )
	{
		//Try sticking "http://" before it
		sBaseURL = _T("http://") + sBaseURL;
		if(!AfxParseURL(sBaseURL,dwServiceType,strServer,strObject,nPort ))
			return FALSE;
	}
	return TRUE;
	
}

void CFetchWebDlg::DisplayBMP(UINT nBMPID)
{
	
	CBitmap bmp;
	bmp.LoadBitmap(nBMPID);
	
	CClientDC DC(this);
	CDC memDC;
	
	memDC.CreateCompatibleDC(&DC);
	
	memDC.SelectObject(&bmp);
	DC.BitBlt(425,10,425 +94,10+400,&memDC,0,0,SRCCOPY);
	
}

void CFetchWebDlg::OnHelp() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(this->GetSafeHwnd(),NULL,"help\\index.html", NULL, NULL, SW_SHOWNORMAL);
	
}

void CFetchWebDlg::OnBrowse() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(this->GetSafeHwnd(),NULL,"MySites\\index.html", NULL, NULL, SW_SHOWNORMAL);
			
}

void CFetchWebDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if(MessageBox("Are You Sure You Want To Leave FetchWeb","FetchWeb",MB_YESNO | MB_ICONQUESTION) == IDYES)
		CDialog::OnCancel();
}
