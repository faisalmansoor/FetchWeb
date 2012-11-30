#if !defined(AFX_DOWNLOADING_H__CE179ED1_7801_4FD0_A628_FD176718F0FB__INCLUDED_)
#define AFX_DOWNLOADING_H__CE179ED1_7801_4FD0_A628_FD176718F0FB__INCLUDED_

#include "Utility.h"	// Added by ClassView


#include "ConfigFiles.h"
#include "Thread.h"
#include "UrlDataBase.h"
#include "DownloadSingleFile.h"
#include "ParseSingleFile.h"
#include "DownloadManager.h"
#include "ScanManager.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Downloading.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDownloading dialog

class CDownloading : public CDialog
{
// Construction
public:
	~CDownloading();
	DownloadManager DM;
	CTimeSpan m_LastTime;
	CTimeSpan elapsedTime;
	void InitializeDownloadingDlg(SITE_CONFIG& SiteConfig);
	CDownloading(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDownloading)
	enum { IDD = IDD_DOWNLOADING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDownloading)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDownloading)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg void OnSkip0();
	afx_msg void OnSkip1();
	afx_msg void OnSkip2();
	afx_msg void OnSkip3();
	afx_msg void OnSkip4();
	afx_msg void OnSkip5();
	afx_msg void OnSkip6();
	afx_msg void OnSkip7();
	afx_msg void OnButtonScan1();
	afx_msg void OnButtonScan2();
	afx_msg void OnButtonScan3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bActive;
	INT  FormatStatusCode(INT nStatusCode);
	BOOL DownloadComplete();
	void DisplayScan();
	void DisplayDownloads();
	void DisplayBMP(UINT nBMPID);
	void UpdateInformation();
	void DisplaySummary();
	ScanManager SM[3];
	UINT m_nBusyCount;
	void Stop();
	//CTime currentTime;
	CTime m_StartTime;
	void StartDownloading();
	SITE_CONFIG m_SiteConfig;
	PROGRESS m_DPROG[8];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOWNLOADING_H__CE179ED1_7801_4FD0_A628_FD176718F0FB__INCLUDED_)
