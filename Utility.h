#pragma warning (disable:4786)

#include<vector>
using namespace std;

#include<Afxinet.h>
#include<wininet.h>

struct PROGRESS
{
	CString sStatus;
	INT		nStatusCode;
	DWORD   dwPercentage;
	CString sURL;	
};
struct DOWNLOAD_PROGRESSID
{
	UINT   STATUSID;
	UINT   PERCENTAGEID;
	UINT   URLID;
};


struct SITE_CONFIG
{
	CString			sProjectName;
	CString			sStartURL;
	LPCTSTR			lpszProxy;
	LPCTSTR			lpszAppName;
	DWORD			dwMaxSize;
	BOOL			bDirStructure;
	LPCTSTR			lpszUserName;
	LPCTSTR			lpszPassword;
	INTERNET_PORT	nPort;
	UINT			ntimeOut;
	UINT			nRetries;
	BOOL			bExternalServerAllowed;
	UINT			nConnections;
	LONG			lMaxTime;

	SITE_CONFIG():lpszProxy(""),lpszAppName("FetchWeb"),lpszUserName(""),lpszPassword("")
	{
		nConnections = 8;
		dwMaxSize = 0;
		lMaxTime  = 0;	
		bDirStructure = TRUE;
		nPort = INTERNET_DEFAULT_HTTP_PORT;
		ntimeOut =30;
		nRetries = 5;
		bExternalServerAllowed =FALSE;
	}
};

struct FILE_HEADER
{
	FILE_HEADER()
	{
		dwFileSize = 0;
		nDepth     = 0;
	}
	CString     sFileName;
	CString     sURL;
	CString		sContentType;
	SYSTEMTIME  LastModified;
	DWORD		dwFileSize;
	UINT		nDepth;
};

struct PARSE_ARGS
{
	CString& sURL;
	CString& sFileName;
	DWORD&  dwPercentage;
	BOOL &bSkip;
	UINT dwParentDepth;
	BOOL bExternalServerAllowed;

	PARSE_ARGS(CString& URL,CString& FileName,DWORD&  pScan,BOOL &Skip,UINT Depth,BOOL bExternalServerAllowed = FALSE ):sURL(URL),sFileName(FileName),
										 dwPercentage(pScan),bSkip(Skip),
										dwParentDepth(Depth)
	{
		this->bExternalServerAllowed = FALSE;
	}
};