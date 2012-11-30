// Downloading.cpp : implementation file
//

#include "stdafx.h"
#include "FetchWeb.h"
#include "Downloading.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDownloading dialog


CDownloading::CDownloading(CWnd* pParent /*=NULL*/)
: CDialog(CDownloading::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDownloading)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDownloading::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDownloading)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDownloading, CDialog)
//{{AFX_MSG_MAP(CDownloading)
ON_WM_TIMER()
ON_WM_PAINT()
ON_BN_CLICKED(IDC_SKIP0, OnSkip0)
ON_BN_CLICKED(IDC_SKIP1, OnSkip1)
ON_BN_CLICKED(IDC_SKIP2, OnSkip2)
ON_BN_CLICKED(IDC_SKIP3, OnSkip3)
ON_BN_CLICKED(IDC_SKIP4, OnSkip4)
ON_BN_CLICKED(IDC_SKIP5, OnSkip5)
ON_BN_CLICKED(IDC_SKIP6, OnSkip6)
ON_BN_CLICKED(IDC_SKIP7, OnSkip7)
ON_BN_CLICKED(IDC_BUTTON_SCAN1, OnButtonScan1)
ON_BN_CLICKED(IDC_BUTTON_SCAN2, OnButtonScan2)
	ON_BN_CLICKED(IDC_BUTTON_SCAN3, OnButtonScan3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDownloading message handlers

BOOL CDownloading::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	
	
	StartDownloading();
	
	m_nBusyCount = 0;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDownloading::InitializeDownloadingDlg(SITE_CONFIG &SiteConfig)
{
	m_SiteConfig = SiteConfig;
}

void CDownloading::StartDownloading()
{
	SYSTEMTIME DUMMY;
	GetSystemTime(&DUMMY); 
	
	FILE_HEADER startURLHeader;
	startURLHeader.sURL			= m_SiteConfig.sStartURL;
	startURLHeader.sContentType = "";
	startURLHeader.LastModified = DUMMY;
	startURLHeader.dwFileSize   = 0;
	
	URLDataBase *URLDB;
	URLDB = URLDataBase::MakeObject();
	URLDB->EnqueURLForDownloading(startURLHeader);
	
	ConfigFiles *cf = ConfigFiles::MakeObject();
	cf->SetDirectory(m_SiteConfig.sStartURL,m_SiteConfig.sProjectName);
	cf->MakeIndex(m_SiteConfig.sStartURL);
	
	
	DM.InitializeDownloadManager(m_SiteConfig);	
	DM.Start();
	
	
	SM[0].id = "Sacnner 0";
	SM[1].id = "Sacnner 1";
	SM[2].id = "Sacnner 2";
	
	
	SM[0].Start();
	SM[1].Start();
	SM[2].Start();
	
	this->SetTimer(1,1000,NULL);
	m_StartTime = CTime::GetCurrentTime();
	
	
}

void CDownloading::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CTime curTime = CTime::GetCurrentTime();
	
	CTimeSpan elapsedTime = curTime-m_StartTime;
	
	CString sCurTime = elapsedTime.Format( "%H:%M:%S" );
	this->SetDlgItemText(IDC_TIME,sCurTime);
	
	
	DisplayDownloads();
	DisplayScan();	
	
	if(DownloadComplete() == TRUE)
		Stop();	
	
	
	//BitMap
	if(m_bActive)
	{
		DisplayBMP(IDB_BITMAP_SEARCH);
		m_bActive = false;
	}
	else
	{
		DisplayBMP(IDB_BITMAP_DOWN);
		m_bActive = true;
	}
	

	//Update Information
	UpdateInformation();
	
	CDialog::OnTimer(nIDEvent);
}


void CDownloading::Stop()
{
		KillTimer(1);
		DisplaySummary();	
		
		URLDataBase* URLDB = URLDataBase::MakeObject();
		URLDB->MakeList();
		
		DM.Stop();
		SM[0].Stop();
		//SM[1].Stop();
		//SM[2].Stop();
		
		CDialog::OnCancel();
	
}

void CDownloading::OnCancel() 
{
	
	// TODO: Add extra cleanup here
	if(MessageBox("Are You Sure You Want To Cancel FetchWeb","FetchWeb",MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		KillTimer(1);
		DisplaySummary();	
		
		URLDataBase* URLDB = URLDataBase::MakeObject();
		URLDB->MakeList();
		
		DM.Stop();
		SM[0].Stop();
		SM[1].Stop();
		SM[2].Stop();

		CDialog::OnCancel();
	}
}

void CDownloading::DisplaySummary()
{	
	//Calculating Time
	CTime curTime = CTime::GetCurrentTime();
	CTimeSpan elapsedTime = curTime-m_StartTime;
	CString sTime = elapsedTime.Format( "%H:%M:%S" );
	
	URLDataBase *URLDB = URLDataBase::MakeObject();
	
	char buff[255];
	_ltoa(URLDB->GetTotalBytesReceived(),buff,10);
	CString sTBytes = buff;
	
	_ltoa(URLDB->GetNoOfFilesDownloaded(),buff,10);
	CString sNoOfFilesWritten = buff;
	
	_ltoa(URLDB->GetNoOfErrors(),buff,10);
	CString sNoOfError = buff;
	
	_ltoa(URLDB->GetNoOfFilesDownloaded(),buff,10);
	CString sFilesLeft = buff;

	
	
	CString empty = "";
	CString sOut =  empty +
		"\nProject Name 	      : " + m_SiteConfig.sProjectName + "\n"
		"StartUrl     	      : " + m_SiteConfig.sStartURL   + "\n"
		"Total Time    	      : " + sTime					+ "\n"
		"Total Bytes Written        : " + sTBytes		+ "\n"
		"Total Errors	      : " +sNoOfError				+ "\n"
		"Total Files Written          : "  +sFilesLeft				+"\n\n\n"+
		"  Refer To Log File For More Details\n";
	if(URLDB->GetNoOfFilesDownloaded() == 0)
	{
		sOut +="\n\n\tFatal Error\n\n  Base URL is Incorrect \n Or Internet connection is not avaliable";
	}
	MessageBox(sOut);
	
	
}

void CDownloading::UpdateInformation()
{
	URLDataBase *URLDB = URLDataBase::MakeObject();
	
	char buff[255];
	_ltoa(URLDB->GetTotalBytesReceived(),buff,10);
	this->SetDlgItemText(IDC_BYTES_SAVED,buff);
	
	_ltoa(URLDB->GetNoOfFilesDownloaded(),buff,10);
	this->SetDlgItemText(IDC_FILES_DOWNLOADED,buff);
	
	_ltoa(URLDB->GetNoOfErrors(),buff,10);
	this->SetDlgItemText(IDC_ERRORS,buff);
	
	_ltoa(URLDB->GetNoOfFilesLeft(),buff,10);
	this->SetDlgItemText(IDC_FILES_LEFT,buff);
	
	_ltoa(URLDB->LinksScanned(),buff,10);
	this->SetDlgItemText(IDC_LINKS_SCANNED,buff);
	
}

CDownloading::~CDownloading()
{
	ConfigFiles *cf = ConfigFiles::MakeObject();
	cf->SetOrigionalDirectory();
	ConfigFiles::RemoveObject();
	URLDataBase::RemoveObject();
}

void CDownloading::DisplayBMP(UINT nBMPID)
{
	
	CBitmap bmp;
	bmp.LoadBitmap(nBMPID);
	
	CClientDC DC(this);
	CDC memDC;
	
	memDC.CreateCompatibleDC(&DC);
	
	memDC.SelectObject(&bmp);
	DC.BitBlt(150,440,150+30,440+28,&memDC,0,0,SRCCOPY);
	
}


void trunUrl(CString& sUrl)
{
	
	int delLen = sUrl.GetLength() - 40;
	sUrl.Delete(20,delLen);
	sUrl.Insert(20,".....");
	
}

void CDownloading::DisplayDownloads()
{
	vector<PROGRESS> DownloadInProgress;
	vector<PROGRESS>::iterator _iterDIP;
	
	vector<PROGRESS> ScanInProgress;
	
	DM.GetStatusOfAll(DownloadInProgress);
	
	
	
	int nCount = 0;
	int nStatus;
	CString sStatus;
	UINT IDURL = IDC_URL0;
	UINT IDSTATUS = IDC_STATUS0;
	UINT IDPROG = IDC_PROGRESS0;
	for(_iterDIP = DownloadInProgress.begin() ; _iterDIP != DownloadInProgress.end() ; _iterDIP++,IDURL++,IDSTATUS++,IDPROG++,nCount++)
	{
		m_DPROG[nCount] = (*_iterDIP);
		//this->SetDlgItemText(IDSTATUS, (*_iterDIP).sStatus) ;
		nStatus = FormatStatusCode((*_iterDIP).nStatusCode);
		if(nStatus != -1)
			sStatus.LoadString(nStatus);
		else
			sStatus = "";
		this->SetDlgItemText(IDSTATUS,sStatus) ;
		
		//If the length of the url is > then 40 the space 
		//in the display url the truncate it

		if((*_iterDIP).sURL.GetLength() > 40)
			trunUrl((*_iterDIP).sURL);
		this->SetDlgItemText(IDURL,(*_iterDIP).sURL);
		CProgressCtrl *pProgress = static_cast<CProgressCtrl *>(this->GetDlgItem(IDPROG)); 
		pProgress->SetPos((*_iterDIP).dwPercentage);
	}
	for(int rep = nCount ;rep<8 ;rep++)
	{
		m_DPROG[nCount].sStatus = "";
		m_DPROG[nCount].sURL    = "";
		m_DPROG[nCount].dwPercentage = 0;
		this->SetDlgItemText(IDSTATUS,"") ;
		this->SetDlgItemText(IDURL,"");
		CProgressCtrl *pProgress = static_cast<CProgressCtrl *>(this->GetDlgItem(IDPROG)); 
		pProgress->SetPos(0);
		IDURL++;
		IDSTATUS++;
		IDPROG++;	
	}
	DownloadInProgress.erase(DownloadInProgress.begin(), DownloadInProgress.end());
}

void CDownloading::DisplayScan()
{
	//First Scan Manager
	PROGRESS scanProg;
	SM[0].GetStatus(scanProg);
	CString status;

	if(scanProg.nStatusCode == 0)
	{
		status.LoadString(IDS_SWAITING);
		scanProg.sURL = "";
		scanProg.dwPercentage = 0;
	}

	else
		status.LoadString(IDS_SPARSING);
		
	this->SetDlgItemText(IDC_SCAN_STATUS1,status);
	
	if(scanProg.sURL.GetLength() > 40)
			trunUrl(scanProg.sURL);		
	
	this->SetDlgItemText(IDC_SCAN_URL1,scanProg.sURL);
	CProgressCtrl *pProgress = static_cast<CProgressCtrl *>(this->GetDlgItem(IDC_SCAN_PROGRESS1)); 
	pProgress->SetPos(scanProg.dwPercentage);


	// Second Scan Manager
	SM[1].GetStatus(scanProg);
	
	if(scanProg.nStatusCode == 0)
	{
		status.LoadString(IDS_SWAITING);
		scanProg.sURL = "";
		scanProg.dwPercentage = 0;
	}

	else
		status.LoadString(IDS_SPARSING);
		
	this->SetDlgItemText(IDC_SCAN_STATUS2,status);
	
	if(scanProg.sURL.GetLength() > 40)
			trunUrl(scanProg.sURL);		
	
	this->SetDlgItemText(IDC_SCAN_URL2,scanProg.sURL);
	pProgress = static_cast<CProgressCtrl *>(this->GetDlgItem(IDC_SCAN_PROGRESS2)); 
	pProgress->SetPos(scanProg.dwPercentage);
	
	// Third Scan Manager
	SM[2].GetStatus(scanProg);
	
	if(scanProg.nStatusCode == 0)
	{
		status.LoadString(IDS_SWAITING);
		scanProg.sURL = "";
		scanProg.dwPercentage = 0;
	}

	else
		status.LoadString(IDS_SPARSING);
		
	this->SetDlgItemText(IDC_SCAN_STATUS3,status);
	
	if(scanProg.sURL.GetLength() > 40)
			trunUrl(scanProg.sURL);		
	
	this->SetDlgItemText(IDC_SCAN_URL3,scanProg.sURL);
	pProgress = static_cast<CProgressCtrl *>(this->GetDlgItem(IDC_SCAN_PROGRESS3)); 
	pProgress->SetPos(scanProg.dwPercentage);
	
}

BOOL CDownloading::DownloadComplete()
{
	
	CTime curTime = CTime::GetCurrentTime();
	CTimeSpan elapsedTime = curTime-m_StartTime;
	LONG lTotalSec = elapsedTime.GetTotalSeconds( );
	if(m_SiteConfig.lMaxTime)
		if(lTotalSec>m_SiteConfig.lMaxTime)
			return TRUE;
		
	BOOL Busy;
	URLDataBase *URLDB = URLDataBase::MakeObject();
	
	Busy = (DM.IsBusy() || SM[0].IsBusy() || SM[1].IsBusy()|| SM[2].IsBusy()|| !(URLDB->IsScanningQEmpty()) || !(URLDB->IsDownloadingQEmpty()) );
	Busy = (DM.IsBusy() || SM[0].IsBusy() || SM[1].IsBusy()|| SM[2].IsBusy()|| !(URLDB->IsScanningQEmpty()) || !(URLDB->IsDownloadingQEmpty()) );
	Busy = (DM.IsBusy() || SM[0].IsBusy() || SM[1].IsBusy()|| SM[2].IsBusy()|| !(URLDB->IsScanningQEmpty()) || !(URLDB->IsDownloadingQEmpty()) );
	Busy = (DM.IsBusy() || SM[0].IsBusy() || SM[1].IsBusy()|| SM[2].IsBusy()|| !(URLDB->IsScanningQEmpty()) || !(URLDB->IsDownloadingQEmpty()) );
	if(Busy == FALSE)
	{
		m_nBusyCount++;
		Busy = (DM.IsBusy() || SM[0].IsBusy() || SM[1].IsBusy()|| SM[2].IsBusy()|| !(URLDB->IsScanningQEmpty()) || !(URLDB->IsDownloadingQEmpty()) );
		Busy = (DM.IsBusy() || SM[0].IsBusy() || SM[1].IsBusy()|| SM[2].IsBusy()|| !(URLDB->IsScanningQEmpty()) || !(URLDB->IsDownloadingQEmpty()) );
		Busy = (DM.IsBusy() || SM[0].IsBusy() || SM[1].IsBusy()|| SM[2].IsBusy()|| !(URLDB->IsScanningQEmpty()) || !(URLDB->IsDownloadingQEmpty()) );
		Busy = (DM.IsBusy() || SM[0].IsBusy() || SM[1].IsBusy()|| SM[2].IsBusy()|| !(URLDB->IsScanningQEmpty()) || !(URLDB->IsDownloadingQEmpty()) );
	}
	else
		m_nBusyCount = 0;
	
	if(m_nBusyCount == 10)
		return TRUE;
	else
		return FALSE;
	
}

void CDownloading::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages
	DisplayBMP(IDB_BITMAP_DOWN);
}

void CDownloading::OnSkip0() 
{
	// TODO: Add your control notification handler code here
	
	if(m_DPROG[0].sURL != "")
		DM.SkipThis(m_DPROG[0].sURL);
	
	m_DPROG[0].sURL = "";
	m_DPROG[0].dwPercentage = 0;
	m_DPROG[0].sStatus      = "";
	
}

void CDownloading::OnSkip1() 
{
	// TODO: Add your control notification handler code here
	if(m_DPROG[1].sURL != "")
		DM.SkipThis(m_DPROG[1].sURL);
	
	m_DPROG[1].sURL = "";
	m_DPROG[1].dwPercentage = 0;
	m_DPROG[1].sStatus      = "";
	
}

void CDownloading::OnSkip2() 
{
	// TODO: Add your control notification handler code here
	if(m_DPROG[2].sURL != "")
		DM.SkipThis(m_DPROG[2].sURL);
	
	m_DPROG[2].sURL = "";
	m_DPROG[2].dwPercentage = 0;
	m_DPROG[2].sStatus      = "";
	
}

void CDownloading::OnSkip3() 
{
	// TODO: Add your control notification handler code here
	if(m_DPROG[3].sURL != "")
		DM.SkipThis(m_DPROG[3].sURL);
	
	m_DPROG[3].sURL = "";
	m_DPROG[3].dwPercentage = 0;
	m_DPROG[3].sStatus      = "";
	
}

void CDownloading::OnSkip4() 
{
	// TODO: Add your control notification handler code here
	if(m_DPROG[4].sURL != "")
		DM.SkipThis(m_DPROG[4].sURL);
	
	m_DPROG[4].sURL = "";
	m_DPROG[4].dwPercentage = 0;
	m_DPROG[4].sStatus      = "";
	
}

void CDownloading::OnSkip5() 
{
	// TODO: Add your control notification handler code here
	if(m_DPROG[5].sURL != "")
		DM.SkipThis(m_DPROG[5].sURL);
	
	m_DPROG[5].sURL = "";
	m_DPROG[5].dwPercentage = 0;
	m_DPROG[5].sStatus      = "";
}

void CDownloading::OnSkip6() 
{
	// TODO: Add your control notification handler code here
	if(m_DPROG[6].sURL != "")
		DM.SkipThis(m_DPROG[6].sURL);
	
	m_DPROG[6].sURL = "";
	m_DPROG[6].dwPercentage = 0;
	m_DPROG[6].sStatus      = "";
	
}

void CDownloading::OnSkip7() 
{
	// TODO: Add your control notification handler code here
	if(m_DPROG[7].sURL != "")
		DM.SkipThis(m_DPROG[7].sURL);
	
	m_DPROG[7].sURL = "";
	m_DPROG[7].dwPercentage = 0;
	m_DPROG[7].sStatus      = "";
	
}

INT CDownloading::FormatStatusCode(INT nStatusCode)
{
	switch(nStatusCode)
	{
	case 0:
		return IDS_DERROR;
		break;
	case 1:
		return IDS_DREADY;
		break;
	case 2:
		return IDS_DSKIP;
		break;
	case 3:
		return IDS_DCONNECTING;
		break;
	case 4:
		return IDS_DCONNECTED;
		break;
	case 5:
		return IDS_DRECEIVING;
		break;
	case 6:
		return IDS_DCOMPLETE;
		break;
	default:
			return -1;
			
	}
}


void CDownloading::OnButtonScan1() 
{

	// TODO: Add your control notification handler code here
	SM[0].Skip();
	
}

void CDownloading::OnButtonScan2() 
{
	// TODO: Add your control notification handler code here
		SM[1].Skip();
	
}

void CDownloading::OnButtonScan3() 
{
	// TODO: Add your control notification handler code here
		SM[2].Skip();
	
}
