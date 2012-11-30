// FetchWebDlg.h : header file
//

#if !defined(AFX_FETCHWEBDLG_H__AF0ADD48_6DB5_4069_AFAD_ADEBB4689C42__INCLUDED_)
#define AFX_FETCHWEBDLG_H__AF0ADD48_6DB5_4069_AFAD_ADEBB4689C42__INCLUDED_

#include "Downloading.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFetchWebDlg dialog



class CFetchWebDlg : public CDialog
{
// Construction
public:
	
	CFetchWebDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFetchWebDlg)
	enum { IDD = IDD_FETCHWEB_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFetchWebDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFetchWebDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnFinish();
	afx_msg void OnHelp();
	afx_msg void OnBrowse();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString sAppDir;
	void DisplayBMP(UINT nBMPID);
	BOOL IsURLOK(CString URL);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FETCHWEBDLG_H__AF0ADD48_6DB5_4069_AFAD_ADEBB4689C42__INCLUDED_)
