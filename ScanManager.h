//m_nStatusCode = 0 Watiting
//m_nStatusCode = 1 Parsing
class ScanManager: public CThread
{
private:	
	UINT            m_nCurDepth;
	ParseSingleFile m_ParseSingle;
	BOOL            m_bStop;
	UINT			m_nDepth;
	URLDataBase*    m_URLDB;
	FILE_HEADER     m_URLHeader;
	DWORD			m_dwPercentScanned;
	UINT			m_nIdleCount;
	BOOL			bBusy;
	BOOL			m_bSkip;
	CString			m_sStatus;
	UINT			m_nStatusCode;
	CString			m_sURL;
	BOOL			bExternalServerAllowed;

public:
	CString			id;
	ScanManager(BOOL bExternalServerAllowed = FALSE)
	{
		m_URLDB = URLDataBase::MakeObject();

		m_nCurDepth = 0;
		m_nStatusCode =0;
		m_bStop     = FALSE;
		m_nDepth	= m_nCurDepth;
		m_nIdleCount= 0;
		bBusy		=FALSE;	
		m_bSkip		=FALSE;

	}
	inline void GetStatus(PROGRESS& scanProg)
	{
		scanProg.sURL		  = m_sURL;	
		scanProg.dwPercentage = m_dwPercentScanned;
		scanProg.nStatusCode  = m_nStatusCode;
					
	}
	inline CString GetStatus()
	{
		return m_sStatus;
	}
	inline CString GetStatusCode()
	{
		return m_nStatusCode;
	}
	
	void Stop()
	{
		m_bSkip = TRUE;
		m_bStop = TRUE;
	}
	void Skip()
	{
		m_bSkip = TRUE;		
	}
	
	BOOL IsBusy()
	{
		return bBusy;
	}
	
	virtual DWORD Run( LPVOID /* arg */ )
	{
	
		while(m_bStop == FALSE)
		{
			/*while(m_URLDB->IsScanningQEmpty() )
			{
				if(m_bStop)
				{
					return 0;
				}
				bBusy = FALSE;
				m_sStatus = "Waiting";
				m_nStatusCode = 0;
				m_dwPercentScanned =0;				
			}*/
									
			bool flag;
			flag = m_URLDB->DequeURLForScanning(m_URLHeader);
			if ( flag == false )
			{
				if(m_bStop)
				{
					return 0;
				}
				bBusy = FALSE;
				m_sStatus = "Waiting";
				m_nStatusCode = 0;
				m_dwPercentScanned =0;				
								
				//MessageBox(NULL,id,"locked",MB_OK);
				g_StartScanEvent.Lock();				
				//MessageBox(NULL,id,"Unlocked",MB_OK);
				flag = m_URLDB->DequeURLForScanning(m_URLHeader);
			
			}

			m_sStatus = "Parsing";
			m_nStatusCode = 1;
			m_sURL    = m_URLHeader.sURL;
			m_bSkip   = FALSE;	

			
			bBusy = TRUE;
			
			PARSE_ARGS pa(m_URLHeader.sURL,m_URLHeader.sFileName,m_dwPercentScanned,m_bSkip,m_URLHeader.nDepth,bExternalServerAllowed);
			
			m_ParseSingle.ParseFile(&pa);
			m_dwPercentScanned = 100;

			if(m_bStop)
			{
				return 0;
			}				
		}
		return 0;
	}
};



				



