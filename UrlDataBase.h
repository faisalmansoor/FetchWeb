#ifndef URL_DATABASE_H
#define URL_DATABASE_H

#include<list>
#include<queue>

using namespace std;

extern CEvent g_StartDownloadEvent;
extern CEvent g_StartScanEvent;


typedef vector<CString> URLLIST; 

class URLDataBase
{
private:
	queue<FILE_HEADER>			 m_Download;  // Operatd by Scanner
	queue<FILE_HEADER>			 m_Scan;	   // Operatd by Downloader
	URLLIST						 m_URLList;
	
	CCriticalSection	 m_DownloadCS;
	CCriticalSection	 m_ScanCS;
	CCriticalSection	 m_URLListCS;


	CCriticalSection  m_dwDownloadedCS;
	CCriticalSection  m_TotalFilesOnListCS;
	CCriticalSection  m_TotalBytesReceivedCS;
	CCriticalSection  m_TotalErrorsCS;


	
	
	DWORD  m_dwDownloaded;
	DWORD  m_TotalBytesReceived;
	DWORD  m_TotalErrors;
	DWORD  m_dwCurrentDownloads;
	
	vector<CString> m_AllowedTypes;
	
	static URLDataBase* m_pURLDataBase;
	
	BOOL IsPresent(CString& URL)
	{
		if(IsSafe() == FALSE)
			return FALSE;
			
		URLLIST::iterator cur;
		
		for(cur = m_URLList.begin(); cur != m_URLList.end(); cur++)
			if(*cur == URL)
				return TRUE;			
			return FALSE;
		
	}
	URLDataBase()
	{
		m_dwDownloaded        = 0;
		m_TotalBytesReceived  = 0;
		m_TotalErrors		  = 0;	
		
		
		m_AllowedTypes.push_back("text/html");
		m_AllowedTypes.push_back("application/x-javascript");
	}
	
public:
	CEvent downloadEvent;
	static BOOL IsSafe()
	{
		if(m_pURLDataBase == NULL)
			return FALSE;
		else 
			return TRUE;
	}

	static URLDataBase* MakeObject()
	{
		CCriticalSection cS;
		cS.Lock();
		
		if(m_pURLDataBase == NULL)
		{
			m_pURLDataBase = new URLDataBase;
			return m_pURLDataBase;
		}
		else
		{	
			return m_pURLDataBase;
		}
		cS.Unlock();
		
	}
	static void RemoveObject()
	{
		if(m_pURLDataBase != NULL)
			delete m_pURLDataBase;
		m_pURLDataBase = NULL;

	}	
	// Operated  by Scanner
	void EnqueURLForDownloading(FILE_HEADER& URLHeader)
	{
		
		if(IsSafe() == FALSE)
			return;
				
		if( IsPresent(URLHeader.sURL) == TRUE)
			return;
		
		else
		{
			{
				m_DownloadCS.Lock();
				m_Download.push(URLHeader);
				m_DownloadCS.Unlock();
			}

			{
				m_URLListCS.Lock();
				m_URLList.push_back(URLHeader.sURL);
				m_URLListCS.Unlock();				
			}
		}
		g_StartDownloadEvent.SetEvent();			
	}
	DWORD LinksScanned()
	{		
		if(IsSafe() == FALSE)
			return 0;
		
		m_URLListCS.Lock();
		DWORD size = m_URLList.size();
		m_URLListCS.Unlock();
		return size;
		
	}
	void EnqueURLForScanning(FILE_HEADER& URLHeader)
	{		
		if(IsSafe() == FALSE)
			return ;
				
		m_ScanCS.Lock();
		
		vector<CString>::iterator _iter;
		for(_iter = m_AllowedTypes.begin() ; _iter != m_AllowedTypes.end() ;_iter++)
		{
			if(URLHeader.sContentType.CompareNoCase(*_iter) == 0)
			{
				m_Scan.push(URLHeader);		
				break;
			}
		}	
		
		m_ScanCS.Unlock();
		//MessageBox(NULL,"Unlocking","download",MB_OK);
	    //g_StartScanEvent.PulseEvent();		
		g_StartScanEvent.SetEvent ();		
	}
	bool DequeURLForDownloading(FILE_HEADER& URLHeader)
	{
		
		if(IsSafe() == FALSE)
			return false;
		
		m_DownloadCS.Lock();  

		if( m_Download.empty() == true )
		{
			m_DownloadCS.Unlock();
			return false;
		}


		URLHeader = m_Download.front();
		m_Download.pop();				
		m_DownloadCS.Unlock();

		return true;
		
	}
	bool DequeURLForScanning(FILE_HEADER& URLHeader)
	{
		
		if(IsSafe() == FALSE)
			return false;

		if(m_pURLDataBase == NULL)
			return false;

		m_ScanCS.Lock();  	
		if( m_Scan.empty() == true )
		{
			m_ScanCS.Unlock();
			return false;
		}	
		
		URLHeader = m_Scan.front();
		m_Scan.pop();
		
		m_ScanCS.Unlock();

		return true;		
	}
	BOOL IsDownloadingQEmpty()
	{
		
		if(IsSafe() == FALSE)
			return TRUE;

		m_DownloadCS.Lock();
		BOOL bEmpty = m_Download.empty();
		m_DownloadCS.Unlock();
		
		return bEmpty;
	}
	void IncrementError()
	{
		
		if(IsSafe() == FALSE)
			return ;

		m_TotalErrorsCS.Lock();    	
		m_TotalErrors++;
		
		m_TotalErrorsCS.Unlock();
		
	}
	BOOL IsScanningQEmpty()
	{
		
		if(IsSafe() == FALSE)
			return TRUE;
		
		m_ScanCS.Lock();   	
		BOOL bEmpty = m_Scan.empty();
		
		m_ScanCS.Unlock();
		
		return bEmpty;
	}
	DWORD GetNoOfFilesLeft()
	{
		
		if(IsSafe() == FALSE)
			return 0;

		m_URLListCS.Lock();
		DWORD dwFilesLeft = m_URLList.size() - (GetNoOfFilesDownloaded()+ GetNoOfErrors());
		m_URLListCS.Unlock();	
		return dwFilesLeft;
		
	}
	DWORD GetTotalBytesReceived()
	{
		
		if(IsSafe() == FALSE)
			return 0;

		m_TotalBytesReceivedCS.Lock();
		DWORD TotalBytesReceived = m_TotalBytesReceived;		
		m_TotalBytesReceivedCS.Unlock();		
		return TotalBytesReceived;
	}
	void AddBytesReceived(DWORD dwBytes)
	{
		
		if(IsSafe() == FALSE)
			return ;

		m_TotalBytesReceivedCS.Lock();    			
		m_TotalBytesReceived+=dwBytes;
		
		m_TotalBytesReceivedCS.Unlock();
		
		
	}
	void IncrementNoOfFilesDownloaded()
	{
		
		if(IsSafe() == FALSE)
			return ;

		m_dwDownloadedCS.Lock();    	
		m_dwDownloaded++;
		
		m_dwDownloadedCS.Unlock();
		
	}
	void DecrementNoOfFilesDownloaded()
	{
		
		if(IsSafe() == FALSE)
			return ;

		m_dwDownloadedCS.Lock();    	
		
		if(m_dwDownloaded != 0)
			m_dwDownloaded--;
		
		m_dwDownloadedCS.Unlock();
		
	}
	
	DWORD GetNoOfFilesDownloaded()
	{
		
		if(IsSafe() == FALSE)
			return 0;

		m_dwDownloadedCS.Lock();    	
		DWORD dwDownloaded = m_dwDownloaded;
		
		m_dwDownloadedCS.Unlock();
		
		return dwDownloaded;
		
	}
	DWORD GetNoOfErrors()
	{
		
		if(IsSafe() == FALSE)
			return 0;

		m_TotalErrorsCS.Lock();    	
		DWORD TotalErrors = m_TotalErrors;		
		
		m_TotalErrorsCS.Unlock();
		
		return m_TotalErrors;
	}
	void MakeList()
	{
		
		if(IsSafe() == FALSE)
			return ;

		ConfigFiles *cf = ConfigFiles::MakeObject();
		cf->UpdateList(m_URLList);
	}
};

#endif





