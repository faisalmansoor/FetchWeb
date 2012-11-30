/*
"Error"      0;
"Ready"      1
"Skipped"    2  ;
"Connecting  3
"Connected   4
"Receiving   5
"Complete    6
*/

class DownloadSingleFile :public CThread
{
private:
	enum { BYTES_TO_READ =1024};
	CString			m_sURLToDownload;
	CString			m_sFileToDownloadInto;
	CString			m_sStatus;
	UINT			m_nStatusCode;
	
	HINTERNET		m_hInternetSession;
	HINTERNET		m_hHttpConnection;
	HINTERNET		m_hHttpFile;
	
	BOOL			m_bSkip;
	BOOL			m_bSafeToClose;
	BOOL			m_bDirStructure;
	
	CFile			m_FileToWrite;
	URL_COMPONENTS  m_UrlComp;
	CString			m_sError;
	CString			m_AppName;
	
	DWORD			m_dwPercentage;
	FILE_HEADER		m_FileHeader;
	DWORD			m_dwMaxSize;
	
	CHAR			m_lpszProxy[255];				//proxy;
	
	UINT			m_nTimeOut;
	UINT			m_nRetries;
	
	DWORD			m_dwTotalBytesReceived;
	BOOL			m_bError;
	
	
	
	
	void SetPercentage(int dwTotalBytesRead,BOOL bGotFileSize)
	{
		if(bGotFileSize)
			m_dwPercentage = (DWORD) (dwTotalBytesRead * 100.0 / m_FileHeader.dwFileSize);
		
	}
	virtual void ThreadCtor()
	{
		
	}
	virtual void ThreadDtor()
	{
 		DestroyUrlComponents();
		
		URLDataBase*    URLDB;
		
		if (m_bSkip)
			::DeleteFile(m_sFileToDownloadInto);
			  
		
		else if(!m_bError)
		{
			
			URLDB = URLDataBase::MakeObject();
			URLDB->IncrementNoOfFilesDownloaded();
			URLDB->EnqueURLForScanning(m_FileHeader);
			//URLDB->UpdateBytesReceived(m_FileHeader.dwFileSize);
		}
		else
		{
			URLDB = URLDataBase::MakeObject();
			URLDB->IncrementError();
			//URLDB->DecrementNoOfFilesDownloaded();

			
			ConfigFiles *cf = ConfigFiles::MakeObject();
			cf->UpdateLog(m_FileHeader.sURL,m_sStatus);
			
		}
		
		//Free up the internet handles we may be using
		if (m_hHttpFile)
		{
			::InternetCloseHandle(m_hHttpFile);
			m_hHttpFile = NULL;
		}
		if (m_hHttpConnection)
		{
			::InternetCloseHandle(m_hHttpConnection);
			m_hHttpConnection = NULL;
		}
		if (m_hInternetSession)
		{
			::InternetCloseHandle(m_hInternetSession);
			m_hInternetSession = NULL;
		}
		m_dwPercentage = 100;
	}
	
	BOOL PrepareFileToDownloadInTo(CString& sObject)
	{
		if(sObject.GetLength() == 0 )
			sObject = m_UrlComp.lpszUrlPath;
		
		if(sObject.GetLength() != 0 )
		{
			if(sObject[sObject.GetLength() -1] == '/' )
			{
				sObject+="index.html";
			}
		}
		
		while(sObject[0] == '/' )
		{
			sObject = sObject.Right(sObject.GetLength( )-1 );  
		}
		
		if(m_bDirStructure == FALSE)
		{
			if(sObject.GetLength() != 0 )
			{
				
				m_sFileToDownloadInto = sObject; 
				int nPos = m_sFileToDownloadInto.ReverseFind('/') ;
				m_sFileToDownloadInto   = m_sFileToDownloadInto.Right(m_sFileToDownloadInto.GetLength()-nPos-1);
			}
			else 
			{
				m_sFileToDownloadInto = "index.html";
			}
			
		}
		
		if(sObject.GetLength() != 0 )
		{
			m_sFileToDownloadInto = sObject ;
			
			if( CreateAllDirectories(m_sFileToDownloadInto) == 0)
			{
				m_sError.Format("Failed to Create Directory , Error:%d", ::GetLastError());
				m_sStatus = "Error";
				m_nStatusCode = 0;
				m_bError  = TRUE;
				return FALSE;
			}
		}
		else
		{
			m_sError.Format("Bad Url Format , Error:%d", ::GetLastError());
			m_sStatus = "Error";
			m_nStatusCode = 0;
			m_bError  = TRUE;
			return FALSE;
		}
		
		if (!m_FileToWrite.Open(m_sFileToDownloadInto, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
		{
			
			m_sError.Format("Failed to open the file to download into, Error : %d",::GetLastError());
			m_sStatus = "FileError";
			m_bError  = TRUE;
			return FALSE;
		}
		
		return TRUE;
		
		
	}
	
	BOOL InitServer()
	{
		//Make internet Session
		// if proxy is specified
		if( strlen(m_lpszProxy) )
			m_hInternetSession = ::InternetOpen(m_AppName, INTERNET_OPEN_TYPE_PROXY ,m_lpszProxy, NULL, 0);
		
		// if proxy is not specified
		else
			m_hInternetSession = ::InternetOpen(m_AppName, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		
		if (m_hInternetSession == NULL)
		{
			m_sError.Format("Failed to Connect, Error:%d", ::GetLastError());
			m_sStatus = "Error";
			m_nStatusCode = 0;
			m_bError  = TRUE;
			return FALSE;
		}
		
		/*
		The time-out value in milliseconds to use for Internet connection requests. 
		If a connection request takes longer than this timeout, the request is canceled.
		The default timeout is infinite. */
		DWORD dwTimeOut     = 1000* m_nTimeOut ;
		DWORD dwTimeOutSize = 32;
		
		::InternetSetOption(m_hInternetSession, INTERNET_OPTION_CONNECT_TIMEOUT | HTTP_QUERY_FLAG_NUMBER , &dwTimeOut,dwTimeOutSize);
		
		/* The delay value in milliseconds to wait between connection retries.*/
		DWORD dwBackOff     = 1000;
		DWORD dwBackOffSize = 32;
		
		/* The retry count to use for Internet connection requests. If a connection 
		attempt still fails after the specified number of tries, the request is canceled.
		The default is five. */
		DWORD dwRetries     = m_nRetries;
		DWORD dwRetriesSize = 32;
		
		::InternetSetOption(m_hInternetSession, INTERNET_OPTION_CONNECT_RETRIES |HTTP_QUERY_FLAG_NUMBER , &dwRetries,dwRetries);
		
		
		return TRUE;
	}
	BOOL CreateAllDirectories(CString strDir)
	{
		if(strDir.GetLength() == 0)
			return FALSE;
		CString currentDir;
		INT     currentPos=0;
		
		strDir.Replace('\\','/');
		
		while(strDir[0] == '/' )
		{
			strDir = strDir.Right(strDir.GetLength( )-1 );  
		}
		
		while(currentPos != -1)
		{
			currentPos = strDir.Find( '/',currentPos );  
			if(currentPos == -1)
				break;
			
			currentDir=strDir.Left(currentPos);
			
			if(GetFileAttributes(currentDir) == FILE_ATTRIBUTE_DIRECTORY) 
			{
				currentDir += "/" ;
				currentPos++;
				continue;
			}
			
			if ( CreateDirectory(currentDir,NULL) == 0)
				return FALSE;
			
			currentDir += "/" ;
			currentPos++;
			
		}
		return TRUE;
	}
	
	
	
	
public:
	virtual DWORD Run( LPVOID /* arg */ )
	{
		m_sStatus = "Ready";
		m_nStatusCode = 1;
		if(	InitServer() == FALSE )
			return 0;
		
		
		if (m_bSkip)
		{
			m_sStatus = "Skipped";
			m_nStatusCode = 2;
			m_sError  = "Cancelled By User";
			m_bError  = TRUE;
			return 0;
		}  
		
		m_sStatus  = "Connecting";
		m_nStatusCode = 3;
		//	Make the connection to the HTTP server for user          
		
		if ( strlen(m_UrlComp.lpszUserName)!= 0 )
			m_hHttpConnection = ::InternetConnect(m_hInternetSession, m_UrlComp.lpszHostName,m_UrlComp.nPort,m_UrlComp.lpszUserName, 
			m_UrlComp.lpszPassword, m_UrlComp.nScheme , 0, (DWORD) this);
		else
			m_hHttpConnection = ::InternetConnect(m_hInternetSession, m_UrlComp.lpszHostName,m_UrlComp.nPort,NULL, 
			NULL, m_UrlComp.nScheme , 0, (DWORD) this);
		if (m_hHttpConnection == NULL)
		{
			m_sError.Format("Failed to Connect, Error:%d", ::GetLastError());
			m_sStatus = "Error";
			m_nStatusCode = 0;
			m_bError  = TRUE;
			return 0;
		}
		
		if (m_bSkip)
		{
			m_sStatus = "Skipped";
			m_sError  = "Skipped";
			m_nStatusCode = 2;
			m_bError  = TRUE;
			return 0;
		}  
		
		//Issue the request to read the file
		LPCTSTR pszAcceptTypes[2];
		pszAcceptTypes[0] = _T("*/*");  //We support accepting any mime file type since this is a simple download of a file
		pszAcceptTypes[1] = NULL;
		
		m_hHttpFile = HttpOpenRequest(m_hHttpConnection, NULL,m_UrlComp.lpszUrlPath, NULL, NULL, pszAcceptTypes, INTERNET_FLAG_RELOAD | 
			INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION, (DWORD) this);
		if (m_hHttpFile == NULL)
		{
			m_sError.Format("Failed to Open Request, Error:%d", ::GetLastError());
			m_sStatus = "Error";
			m_nStatusCode = 0;
			m_bError  = TRUE;
			return 0;
		}
		
		//Issue the request
		BOOL bSend = ::HttpSendRequest(m_hHttpFile, NULL, 0, NULL, 0);
		if (!bSend)
		{
			m_sError.Format("Failed in call to HttpSendRequest, Error:%d" , ::GetLastError());
			m_sStatus = "Error";
			m_nStatusCode = 0;
			m_bError  = TRUE;
			return 0;
		}
		
		DWORD dwStatusCode = 32;
		DWORD dwSC =32;
		
		::HttpQueryInfo(m_hHttpFile,HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER , &dwStatusCode, &dwSC, NULL);
		
		
		// access was denied
		
		if(dwStatusCode == HTTP_STATUS_DENIED)
		{
			
			// if the user cancelled the dialog, return
			m_sError ="Access denied: Invalid password";
			m_sStatus = "Error";
			m_nStatusCode = 0;
			m_bError  = TRUE;
			return 0;
		}
		if(dwStatusCode == 404)
		{
			m_sError.Format("Requested URL not found");
			m_sStatus  = "Not Found";
			m_nStatusCode = 3;
			m_bError  = TRUE;
			return 0;
		}
		
		
		
		else if (dwStatusCode != HTTP_STATUS_OK)
		{
			m_sError.Format("Failed to retrieve a HTTP 200 status, Status Code:%d", dwStatusCode);
			m_sStatus  = "Error";
			m_nStatusCode = 0;
			m_bError  = TRUE;
			return 0;
		}
		
		
		TCHAR szContentType[32];
		DWORD dwContentType = 32;
		
		if (::HttpQueryInfo(m_hHttpFile, HTTP_QUERY_CONTENT_TYPE  , szContentType, &dwContentType, NULL))
		{
			
			m_FileHeader.sContentType = szContentType;					
		}
		
		// Query last modeifies date
		
		SYSTEMTIME lastModified;
		DWORD		dwLM =sizeof(lastModified);	
		if(::HttpQueryInfo(m_hHttpFile, HTTP_QUERY_LAST_MODIFIED | HTTP_QUERY_FLAG_SYSTEMTIME , &lastModified,&dwLM, NULL))
		{
			m_FileHeader.LastModified = lastModified ;
		}
		
		
		if (m_bSkip)
		{
			m_sStatus = "Skipped";
			m_nStatusCode = 2;
			m_sError  = "Skipped";
			m_bError  = TRUE;
			return 0;
		}  
		
		
		// Update Status
		m_sStatus = "Connected";
		m_nStatusCode = 4;
		
		DWORD dwInfoSize = 32;
		DWORD dwFileSize = 0;
		BOOL bGotFileSize = FALSE;
		
		if (::HttpQueryInfo(m_hHttpFile, HTTP_QUERY_CONTENT_LENGTH |HTTP_QUERY_FLAG_NUMBER , &dwFileSize, &dwInfoSize, NULL))
		{
			
			bGotFileSize = TRUE;
			m_FileHeader.dwFileSize = dwFileSize;
			
		}
		
		if(m_dwMaxSize)
		{
			if(m_FileHeader.dwFileSize > m_dwMaxSize )
			{
				m_sError  = "File Size Is More Than Limit Specified."; 
				m_sStatus = "Error";
				m_nStatusCode = 0;
				m_bError  = TRUE;
				return 0;
			}
		}
		
		//Creating file to download Into
		
		TCHAR szLocation[225];
		DWORD dwLocationSize = 225;
		DWORD dwServiceType; 
		CString strServer;
		CString sObject = "";
		INTERNET_PORT nPort; 
		
		if(::HttpQueryInfo(m_hHttpFile,HTTP_QUERY_CONTENT_LOCATION ,szLocation, &dwLocationSize,NULL))
			AfxParseURL(szLocation,dwServiceType,strServer,sObject,nPort);
		if ( PrepareFileToDownloadInTo(sObject) == FALSE )
			return 0;
		
		m_FileHeader.sFileName = m_sFileToDownloadInto;
		
		CString sFileURL,scheme = "http://";
		sFileURL = scheme + m_UrlComp.lpszHostName + "/" + m_sFileToDownloadInto;
		
		m_FileHeader.sURL = sFileURL;
		
		
		//Update the status to say that we are now downloading the file
		m_sStatus = "Receiving";
		m_nStatusCode = 5;
		
		char szReadBuf[BYTES_TO_READ];
		DWORD dwBytesToRead    = BYTES_TO_READ;
		DWORD dwTotalBytesRead = 0;
		DWORD dwBytesRead = 0;
		URLDataBase*    URLDB = URLDataBase::MakeObject();
		
		do
		{
			
			if (!::InternetReadFile(m_hHttpFile, szReadBuf, dwBytesToRead, &dwBytesRead))
			{
				m_sError.Format("Failed in call to InternetReadFile, Error:%d", ::GetLastError());
				m_sStatus  = "Error";
				m_nStatusCode = 0;
				m_bError  = TRUE;
				return 0;
			}
			else if (dwBytesRead)
			{
				//Write the data to file
				TRY
				{
					m_FileToWrite.Write(szReadBuf, dwBytesRead);
				}
				CATCH(CFileException, e);                                          
				{
					m_sError.Format("An exception occured while writing to the download file.");
					e->Delete();
					m_sStatus  = "Error";
					m_nStatusCode = 0;
					m_bError  = TRUE;
					return 0;
				}
				END_CATCH
					
					//Increment the total number of bytes read
					dwTotalBytesRead += dwBytesRead;  
				m_dwTotalBytesReceived = dwTotalBytesRead;
				
				SetPercentage(dwTotalBytesRead,bGotFileSize);
				
				if (m_bSkip)
				{
					m_sStatus = "Skipped";
					m_nStatusCode = 2;
					m_sError  = "Skipped";
					m_bError  = TRUE;
					return 0;
				}  
				
			}
			URLDB->AddBytesReceived(dwBytesRead);			
		}while (dwBytesRead);
		
		//Close File Handle
		m_FileToWrite.Close();
		
		m_sStatus = "Complete";
		m_nStatusCode = 6;
		return 0;
		
	}
	
	// Skip this file
	void Skip()
	{
		m_bSkip = TRUE;
	}
	
	void InitUrlComponents(INTERNET_PORT nPort,LPCTSTR lpszUserName,LPCTSTR lpszPassword)
	{
		
		//Initializing URL_COMPONENTS 
		
		memset(&m_UrlComp, 0, sizeof(m_UrlComp));
		
		m_UrlComp.dwStructSize = sizeof(m_UrlComp);
		
		m_UrlComp.lpszScheme = new char[100];
		
		m_UrlComp.dwSchemeLength = 100;
		
		m_UrlComp.lpszHostName = new char[255];
		
		m_UrlComp.dwHostNameLength = 255;
		
		m_UrlComp.lpszUrlPath = new char[255];
		
		m_UrlComp.dwUrlPathLength = 255;
		
		m_UrlComp.lpszExtraInfo = new char[255];
		
		m_UrlComp.dwExtraInfoLength = 255;
		
		m_UrlComp.lpszUserName = new char[255];
		
		m_UrlComp.dwUserNameLength = 255;
		
		m_UrlComp.lpszPassword = new char[255];
		
		m_UrlComp.dwPasswordLength = 255;
		
		
								
		m_sURLToDownload.Replace('\\','/');
		if (! InternetCrackUrl(m_sURLToDownload,m_sURLToDownload.GetLength(), ICU_ESCAPE, &m_UrlComp) )
		{
			//Try sticking "http://" before it
			m_sURLToDownload = _T("http://") + m_sURLToDownload;
			if (! InternetCrackUrl(m_sURLToDownload,m_sURLToDownload.GetLength(), ICU_ESCAPE, &m_UrlComp) )
			{
				m_sError="Url Incorrect";
				m_bError= TRUE;
				return ;
			}
		}
		
		m_FileHeader.sURL = m_sURLToDownload;
		
		if( strlen(m_UrlComp.lpszUrlPath) == 0 )
			strcpy( m_UrlComp.lpszUrlPath , "/" );
		
		strcpy(m_UrlComp.lpszUserName,lpszUserName);
		strcpy(m_UrlComp.lpszPassword,lpszPassword);
		
	}
	
	void DestroyUrlComponents()
	{
		delete[] m_UrlComp.lpszScheme;
		
		delete[] m_UrlComp.lpszHostName;
		
		delete[] m_UrlComp.lpszUrlPath;
		
		delete[] m_UrlComp.lpszExtraInfo;
		
		delete[] m_UrlComp.lpszUserName;
		
		delete[] m_UrlComp.lpszPassword;
		
		
	}
public:
	DownloadSingleFile()
	{
	}
	
	
	void InitFile(CString sURL,LPCTSTR lpszProxy = NULL,LPCTSTR appName = "FetchWeb",DWORD dwMaxSize = 0 ,BOOL bDirStructure = TRUE , LPCTSTR lpszUserName = "" ,LPCTSTR lpszPassword ="" ,INTERNET_PORT nPort = INTERNET_DEFAULT_HTTP_PORT,UINT ntimeOut =30,UINT nRetries = 5)
	{
		m_AppName				= appName;
		m_sURLToDownload		= sURL;
		m_FileHeader.sURL		= sURL;
		m_hInternetSession		= NULL ;
		m_hHttpConnection		= NULL;
		m_hHttpFile				= NULL;
		m_bSkip					= FALSE;
		m_dwPercentage			= 0;
		m_sStatus				= "";
		m_dwMaxSize				= dwMaxSize;
		m_bDirStructure			= bDirStructure;
		m_nTimeOut				= ntimeOut;
		m_nRetries				= nRetries;
		m_dwTotalBytesReceived  = 0;
		m_bError				= FALSE;
		
		
		
		if(lpszProxy != NULL)
			strcpy(m_lpszProxy,lpszProxy);
		else
			strcpy(m_lpszProxy,"");
		
		
		InitUrlComponents(nPort,lpszUserName,lpszPassword);
		
	}
	inline DWORD GetPercentage()
	{
		return m_dwPercentage;
	}
	CString GetStatus()
	{
		if(m_sStatus.GetLength())
			return m_sStatus;
		else 
			return "";
	}
	INT GetStatusCode()
	{
		return m_nStatusCode;
	}
	CString GetURL()
	{
		return m_sURLToDownload;
	}
	DWORD GetTotalBytesReceived()
	{
		return m_dwTotalBytesReceived;
	}
	
	~DownloadSingleFile()
	{		
	}
	BOOL IsError()
	{
		return m_bError;
	}
};




