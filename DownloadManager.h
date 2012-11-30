extern CEvent g_StartDownloadEvent;
extern CEvent g_StartScanEvent;

typedef vector<DownloadSingleFile*>::iterator DITER;
class DownloadManager: public CThread
{
private:
	ParseSingleFile m_ParseSingle;
	BOOL            m_bStop;
	URLDataBase*    m_URLDB;
	FILE_HEADER     m_URLHeader;
	UINT			m_nConnections;
	CString			m_sStatus;
	SITE_CONFIG		m_Config;
	UINT			m_nIdleCount;
	DWORD			m_dwCurrentDownloads;
		
	vector<DownloadSingleFile*> m_Downloads;
	vector<PROGRESS>   m_DownloadsInProgress;
	
public:
	void InitializeDownloadManager(SITE_CONFIG& config)
	{
		m_bStop	 = FALSE;	
		m_Config = config;
		m_URLDB = URLDataBase::MakeObject();
		if(config.nConnections<9)
			m_nConnections = config.nConnections;
		else
			m_nConnections = 1;
		
		for(int nCount = 0 ;nCount<m_nConnections ;nCount++)
		{
			m_Downloads.push_back(new DownloadSingleFile);
		}
		m_sStatus = "Downloading";
	}
	~DownloadManager()
	{
		DownloadSingleFile* temp;
		while( m_Downloads.size()!=0 )
		{
			temp = 	m_Downloads.back();
			delete temp;
			m_Downloads.pop_back();
		}
		m_sStatus = "Downloaded";
	}
	CString GetStatus()
	{
		return m_sStatus;
	}
	void GetStatusOfAll(vector<PROGRESS>& InProgress)
	{
		PROGRESS current;
		
		vector<DownloadSingleFile*>::iterator _iter;
		for(_iter = m_Downloads.begin() ; _iter!=m_Downloads.end();_iter++)
		{			
			if((*_iter)->IsRunning())
			{
				
				current.dwPercentage = (*_iter)->GetPercentage();
				current.nStatusCode  = (*_iter)->GetStatusCode();
				current.sURL		 = (*_iter)->GetURL();
				
				InProgress.push_back(current);
			}
		}
	}
	void SkipThis(CString& URL)
	{
		PROGRESS current;
		CString temp;
		
		vector<DownloadSingleFile*>::iterator _iter;
		for(_iter = m_Downloads.begin() ; _iter!=m_Downloads.end();_iter++)
		{			
			if((*_iter)->GetURL().Compare(URL) == 0)
			{
				(*_iter)->Skip();

			}
		}
	}
	void Stop()
	{
		m_bStop = TRUE;
		m_sStatus = "Stopped";
	}
	DITER FindIdle()
	{
		DITER _iter;
		for(_iter = m_Downloads.begin() ; _iter!=m_Downloads.end();_iter++)
		{			
			if((*_iter)->IsRunning() == FALSE)
			{
				return _iter;
			}
			
		}
		return NULL;
	}
	BOOL IsBusy()
	{
		DITER _iter;
		for(_iter = m_Downloads.begin() ; _iter!=m_Downloads.end();_iter++)
		{			
			if((*_iter)->IsRunning() == TRUE)
			{
				return TRUE;
			}
			
		}
		return FALSE;
	}

	void StopAll()
	{
		DITER _iter;
		for(_iter = m_Downloads.begin() ; _iter!=m_Downloads.end();_iter++)
		{			
			(*_iter)->Stop(TRUE);				
		}
	}
	
	virtual DWORD Run( LPVOID /* arg */ )
	{
		
		while(m_bStop == FALSE)
		{
			/*if( m_URLDB->IsDownloadingQEmpty() )
			{
				//m_URLDB->downloadEvent.Lock();
				g_StartDownloadEvent.Lock();
			}*/
			bool flag ;
			flag = m_URLDB->DequeURLForDownloading(m_URLHeader);
			while ( flag == false )
			{
				g_StartDownloadEvent.Lock();
				flag = m_URLDB->DequeURLForDownloading(m_URLHeader);
			}			

									
			DITER currentD;
			
			do
			{
				currentD = FindIdle();
				
				vector<PROGRESS> InProgress;
			GetStatusOfAll(InProgress);
			
			}while(currentD== NULL);
			
			(*currentD)->InitFile(m_URLHeader.sURL,m_Config.lpszProxy,
				m_Config.lpszAppName,
				m_Config.dwMaxSize,
				m_Config.bDirStructure,
				m_Config.lpszUserName,
				m_Config.lpszPassword,
				m_Config.nPort,
				m_Config.ntimeOut,
				m_Config.nRetries);
			
			(*currentD)->Start();
			
			if(m_bStop)
			{
				StopAll();
			}
		}
		StopAll();
		m_bStop = TRUE;
		return 0;
	}
};