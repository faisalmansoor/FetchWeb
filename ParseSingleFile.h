class ParseSingleFile : CThread
{
private:
	CString m_sUrl;
	CFile   m_FileToRead;

	CString m_sError;
	CString m_sStatus;

	CString m_sData;

	DWORD	m_dwFileSize;
	DWORD   m_dwServiceType;

	CString m_sTag;
	CString m_sTagName;
	CString m_sBase;

	DWORD   m_dwStartTag,
			m_dwEndTag  ,
			m_dwPos ,
			m_p	        ;
	UINT	m_nFoundCount;

	DWORD m_dwParentDepth;

	BOOL    m_bInTag;
	BOOL    m_bInComment;
	BOOL    m_bInScript;
	BOOL    m_bInScriptTag;
	BOOL    m_bExternalServerAllowed;


	CHAR    cCurr;

	CString DS;

	vector<CString> m_HtmlDetect;
	vector<CString> m_rawUrls;
	vector<CString> m_HtmlDetectURL;
	vector<CString> m_FileType;

	vector<CString> m_DoNotDetect;
	vector<CString> m_Dynamic;
	vector<CString> m_HtmlType;
	vector<CString> m_KnownTypes;
	


	INTERNET_PORT  m_nPort;

	vector <CString> m_UrlList;

	URLDataBase*    m_URLDB;

	inline BOOL ReadFile(CString& sFileName)
	{

		if (! m_FileToRead.Open(sFileName, CFile::modeRead))
		{

			m_sError.Format("Failed to open the file to download into, Error : %d",::GetLastError());
			m_sStatus = "FileError";
			return FALSE;
		}
		m_dwFileSize = m_FileToRead.GetLength();
		char *pBuff = new char[m_dwFileSize];
		m_FileToRead.Read(pBuff,m_dwFileSize);
		m_FileToRead.Close();
		m_sData = pBuff;
		m_sData.TrimLeft();
		m_sData.TrimRight();		
		return TRUE;

	}
	CString GetExtension(CString& URL)
	{
		RemoveHashQMark(URL);	
		INT nDot;  
		INT nSlash; 
		
		 nDot= URL.ReverseFind('.');
		 if(nDot == -1)
			 return "";
		 nSlash= URL.ReverseFind('/');
		if(nDot < nSlash)
			return "";
		else
		{
			CString ext = URL.Right(URL.GetLength() - nDot-1);
			return ext;
		}
	}
	
	BOOL IsURLValid(CString& URL)
	{
		if (URL.IsEmpty() == TRUE)
			return FALSE;
		else if ( (URL.Find('*') != -1)
					||
				   (URL.Find('<') != -1)
				   ||
				   (URL.Find('>') !=-1)
				 )
			return FALSE;
		else 
			return TRUE;
		
	}
	BOOL IsDynamic(CString& ext)
	{
		vector<CString>::iterator _iter;
		for(_iter = m_Dynamic.begin() ; _iter!=m_Dynamic.end() ;_iter++)
		{
			if( (*_iter).CompareNoCase(ext) == 0)				
				return TRUE;
		}
		return FALSE;
	}
	BOOL IsHtmlType(CString& ext)
	{
		vector<CString>::iterator _iter;
		for(_iter = m_HtmlType.begin() ; _iter!=m_HtmlType.end() ;_iter++)
		{
			if( (*_iter).CompareNoCase(ext) == 0)				
				return TRUE;
		}
		return FALSE;
	}
	BOOL IsKnownType(CString& ext)
	{
		vector<CString>::iterator _iter;
		for(_iter = m_KnownTypes.begin() ; _iter!=m_KnownTypes.end() ;_iter++)
		{
			if( (*_iter).CompareNoCase(ext) == 0)				
				return TRUE;
		}
		return FALSE;
	}
	BOOL IsURLOk(CString& URL)
	{
		if ( (URL.Left(7)).CompareNoCase("http://") == 0 )
			return TRUE;


		CString ext = GetExtension(URL);
		
		if(ext.IsEmpty() == TRUE)
			return FALSE;
		
		if(IsHtmlType(ext) == TRUE)
			return TRUE;

		if(IsDynamic(ext) == TRUE )
			return TRUE;
		
		if(IsKnownType(ext) == TRUE)
			return TRUE;
		
		DWORD dwAtPos = URL.Find('@');
		if(dwAtPos != -1)
		{
			DWORD dwSlash = URL.Find('/',dwAtPos);
			URL.Left(URL.GetLength() - dwAtPos);
			return TRUE;
		}
		else 
			return FALSE;
	}
	
	
	BOOL IsHttp(CString& sURL)
	{
		if ( (sURL.Left(6)).CompareNoCase("ftp://") == 0 )
			return FALSE;
		if ( (sURL.Left(7)).CompareNoCase("http://") == 0 )
			return TRUE;
		if(IsEmailAdd(sURL) == TRUE)
			return FALSE;
				
		 DWORD dwServiceType=0;
		 CString strServer;
		 CString strObject;
		 INTERNET_PORT nPort;
		if( AfxParseURL(sURL,dwServiceType,strServer,strObject,nPort ) )
		{
			if(INTERNET_SERVICE_HTTP == dwServiceType)
				return TRUE;
			else 
				return FALSE;
		}
		return TRUE;
	}
	BOOL IsEmailAdd(CString& URL)
	{
		
		if ( (URL.Left(7)).CompareNoCase("mailto:") == 0 )
			return TRUE;

		DWORD dwAtPos = URL.Find('@');
		if(dwAtPos != -1)
		{
			DWORD dwSlash = URL.Find('/',dwAtPos);
			URL.Left(URL.GetLength() - dwAtPos);
			return FALSE;
		}
		else 
			return FALSE;
	}	
	BOOL GetNextTag(CString& sTag)
	{

		DWORD dwStartPos =0,
			  dwEndPos =0;

		if( (dwStartPos = m_sData.Find('<',m_dwPos)) == -1)
			return FALSE;

		if( (dwEndPos = m_sData.Find('>',dwStartPos))== -1)
			return FALSE;

		sTag = m_sData.Mid(dwStartPos+1,dwEndPos - dwStartPos-1);

		m_dwPos = dwEndPos;

		return TRUE;

	}
	void AdvanceIfNotInAnyTag()
	{
		if ( (!m_bInTag)         /* Not in tag */
			&& (!m_bInScript)      /* Not in (java)script */
			&& (!m_bInComment)     /* Not in comment (<!--) */
			&& (!m_bInScriptTag)  /* Not in tag with script inside */
			)
		{
			/* Not at the end */
			if( m_dwPos < m_sData.GetLength() )
			{
				/* Not on a starting tag yet */
				if (m_sData.GetAt(m_dwPos) != '<')
					m_dwPos = m_sData.Find('<',m_dwPos);
			}


		}

	}
	//Checking <!==
	BOOL CheckIfInComment()
	{

		if (m_sData.GetAt(m_dwPos+1) == '!')
		{
			if (m_sData.GetAt(m_dwPos+2) == '-')
			{
				if (m_sData.GetAt(m_dwPos+3) =='-')
				{
					m_bInTag =FALSE;
					m_bInComment = TRUE;
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	void InitTags()
	{

		//  for html tags having form href = "mmmmmmm" or src = "mmmmmm"
		m_HtmlDetect.push_back("href");
		m_HtmlDetect.push_back("src");
		m_HtmlDetect.push_back("lowsrc");
		m_HtmlDetect.push_back("backgroung");
		m_HtmlDetect.push_back("dynsrc");
		m_HtmlDetect.push_back("data");
		m_HtmlDetect.push_back("profile");
		m_HtmlDetect.push_back("swurl");
		m_HtmlDetect.push_back("url");
		m_HtmlDetect.push_back("usemap");
		m_HtmlDetect.push_back("longdesc");
		m_HtmlDetect.push_back("vrml");


		m_HtmlDetectURL.push_back("content");

		m_DoNotDetect.push_back("accept-charset");
		m_DoNotDetect.push_back("accesskey");
		m_DoNotDetect.push_back("action");
		m_DoNotDetect.push_back("align");
		m_DoNotDetect.push_back("alt");
		m_DoNotDetect.push_back("axes");
		m_DoNotDetect.push_back("axis");
		m_DoNotDetect.push_back("char");
		m_DoNotDetect.push_back("charset");
		m_DoNotDetect.push_back("cite");
		m_DoNotDetect.push_back("class");
		m_DoNotDetect.push_back("classid");
		m_DoNotDetect.push_back("code");
		m_DoNotDetect.push_back("color");
		m_DoNotDetect.push_back("datetime");
		m_DoNotDetect.push_back("dir");
		m_DoNotDetect.push_back("prompt");
		m_DoNotDetect.push_back("scheme");
		m_DoNotDetect.push_back("size");
		m_DoNotDetect.push_back("style");
		m_DoNotDetect.push_back("target");
		m_DoNotDetect.push_back("title");
		m_DoNotDetect.push_back("type");
		m_DoNotDetect.push_back("valign");
		m_DoNotDetect.push_back("version");
		m_DoNotDetect.push_back("width");
		m_DoNotDetect.push_back("name");
		m_DoNotDetect.push_back("method");
		m_DoNotDetect.push_back("lang");
		m_DoNotDetect.push_back("language");
		m_DoNotDetect.push_back("id");
		m_DoNotDetect.push_back("height");
		m_DoNotDetect.push_back("face");
		m_DoNotDetect.push_back("enctype");
		m_DoNotDetect.push_back("media" );	
		
		m_Dynamic.push_back("php3");
		m_Dynamic.push_back("php");
		m_Dynamic.push_back("php4");
		m_Dynamic.push_back("php2");
		m_Dynamic.push_back("cgi");
		m_Dynamic.push_back("asp");
		m_Dynamic.push_back("jsp");
		m_Dynamic.push_back("pl");
		m_Dynamic.push_back("cfm");
		
		m_HtmlType.push_back("htm");
		m_HtmlType.push_back("shtml");
		m_HtmlType.push_back("shtm");
		m_HtmlType.push_back("phtm");
		m_HtmlType.push_back("htmlx");
		m_HtmlType.push_back("phtml");
		m_HtmlType.push_back("htmx");
		
/*		m_KnownTypes.push_back("vda");
		m_KnownTypes.push_back("cco");
		m_KnownTypes.push_back("stp");
		m_KnownTypes.push_back("stl");
		m_KnownTypes.push_back("set");
		m_KnownTypes.push_back("sol");
		m_KnownTypes.push_back("smi");
		m_KnownTypes.push_back("gzip");
		m_KnownTypes.push_back("ppt");
		m_KnownTypes.push_back("step");
		m_KnownTypes.push_back("csh");
		m_KnownTypes.push_back("dir");
		m_KnownTypes.push_back("dcr");
		m_KnownTypes.push_back("dxr");
		m_KnownTypes.push_back("cdf");
		m_KnownTypes.push_back("dvi");
		m_KnownTypes.push_back("gz");
		m_KnownTypes.push_back("hdf");
		m_KnownTypes.push_back("nc");
		m_KnownTypes.push_back("js");
		m_KnownTypes.push_back("skp");
		m_KnownTypes.push_back("skd");
		m_KnownTypes.push_back("skt");
		m_KnownTypes.push_back("skm");
		m_KnownTypes.push_back("latex");
		m_KnownTypes.push_back("rtf");
		m_KnownTypes.push_back("part");
		m_KnownTypes.push_back("prt");
		m_KnownTypes.push_back("pdf");
		m_KnownTypes.push_back("oda");
		m_KnownTypes.push_back("dwg");
		m_KnownTypes.push_back("arj");
		m_KnownTypes.push_back("ccad");
		m_KnownTypes.push_back("drw");
		m_KnownTypes.push_back("dxf");
		m_KnownTypes.push_back("xl");
		m_KnownTypes.push_back("unv");
		m_KnownTypes.push_back("unv");
		m_KnownTypes.push_back("iges");
		m_KnownTypes.push_back("hqx");
		m_KnownTypes.push_back("cpt");
		m_KnownTypes.push_back("doc");
		m_KnownTypes.push_back("w6w");
		m_KnownTypes.push_back("word");
		m_KnownTypes.push_back("wri");
		m_KnownTypes.push_back("sml");
		m_KnownTypes.push_back("smil");
		m_KnownTypes.push_back("eps");
		m_KnownTypes.push_back("ai");
		m_KnownTypes.push_back("ps");
		m_KnownTypes.push_back("class");
		m_KnownTypes.push_back("aas");
		m_KnownTypes.push_back("aab");
		m_KnownTypes.push_back("mif");
		m_KnownTypes.push_back("shar");
		m_KnownTypes.push_back("tcl");
		m_KnownTypes.push_back("tex");
		m_KnownTypes.push_back("texinfo");
		m_KnownTypes.push_back("texi");
		m_KnownTypes.push_back("t");
		m_KnownTypes.push_back("tr");
		m_KnownTypes.push_back("roff");
		m_KnownTypes.push_back("man");
		m_KnownTypes.push_back("ms");
		m_KnownTypes.push_back("src");
		m_KnownTypes.push_back("zip");
		m_KnownTypes.push_back("sit");
		m_KnownTypes.push_back("aam");
		m_KnownTypes.push_back("bcpio");
		m_KnownTypes.push_back("aifc");
		m_KnownTypes.push_back("rm");
		m_KnownTypes.push_back("ram");
		m_KnownTypes.push_back("ra");
		m_KnownTypes.push_back("rpm");
		m_KnownTypes.push_back("rpm");
		m_KnownTypes.push_back("pdb");
		m_KnownTypes.push_back("xyz");
		m_KnownTypes.push_back("dwf");
		m_KnownTypes.push_back("gif");
		m_KnownTypes.push_back("ief");
		m_KnownTypes.push_back("jpg");
		m_KnownTypes.push_back("jpe");
		m_KnownTypes.push_back("jpeg");
		m_KnownTypes.push_back("pict");
		m_KnownTypes.push_back("ppm");
		m_KnownTypes.push_back("rgb");
		m_KnownTypes.push_back("xbm");
		m_KnownTypes.push_back("xpm");
		m_KnownTypes.push_back("xwd");
		m_KnownTypes.push_back("msh");
		m_KnownTypes.push_back("mesh");
		m_KnownTypes.push_back("css");
		m_KnownTypes.push_back("htm");
		m_KnownTypes.push_back("txt");
		m_KnownTypes.push_back("html");
		m_KnownTypes.push_back("gzip");
		m_KnownTypes.push_back("zip");
		m_KnownTypes.push_back("silo");
		m_KnownTypes.push_back("tif");
		m_KnownTypes.push_back("ras");
		m_KnownTypes.push_back("fh4");
		m_KnownTypes.push_back("fh7");
		m_KnownTypes.push_back("fh5");
		m_KnownTypes.push_back("pgm");
		m_KnownTypes.push_back("fh");
		m_KnownTypes.push_back("pnm");
		m_KnownTypes.push_back("wrl");
		m_KnownTypes.push_back("aif");
		m_KnownTypes.push_back("au");
		m_KnownTypes.push_back("tiff");
		m_KnownTypes.push_back("png");
		m_KnownTypes.push_back("aiff");
		m_KnownTypes.push_back("snd");
		m_KnownTypes.push_back("vcd");
		m_KnownTypes.push_back("cpio");
		m_KnownTypes.push_back("tgz");
		m_KnownTypes.push_back("gtar");
		m_KnownTypes.push_back("shar");
		m_KnownTypes.push_back("swf");
		m_KnownTypes.push_back("swf");
		m_KnownTypes.push_back("sv4crc");
		m_KnownTypes.push_back("tar");
		m_KnownTypes.push_back("ustar");
		m_KnownTypes.push_back("hlp");
		m_KnownTypes.push_back("mid");
		m_KnownTypes.push_back("midi");
		m_KnownTypes.push_back("kar");
		m_KnownTypes.push_back("mp3");
		m_KnownTypes.push_back("mpga");
		m_KnownTypes.push_back("mp2");
		m_KnownTypes.push_back("c");
		m_KnownTypes.push_back("cc");
		m_KnownTypes.push_back("hh");
		m_KnownTypes.push_back("m");
		m_KnownTypes.push_back("f90");
		m_KnownTypes.push_back("rtx");
		m_KnownTypes.push_back("tsv");
		m_KnownTypes.push_back("etx");
		m_KnownTypes.push_back("sgml");
		m_KnownTypes.push_back("sgm");
		m_KnownTypes.push_back("xml");
		m_KnownTypes.push_back("xml");
		m_KnownTypes.push_back("mpeg");
		m_KnownTypes.push_back("mpg");
		m_KnownTypes.push_back("mpe");
		m_KnownTypes.push_back("qt");
		m_KnownTypes.push_back("mov");
		m_KnownTypes.push_back("avi");
		m_KnownTypes.push_back("h");
		m_KnownTypes.push_back("g");
		m_KnownTypes.push_back("movie");
		m_KnownTypes.push_back("ice");
		m_KnownTypes.push_back("cgi");
*/		

	}

	DWORD AdvanceSpace()
	{
		DWORD dwLen = 0;
		if(m_sData.GetAt(m_dwPos) == ' ')
		{
			m_dwPos++;
			dwLen++;
		}
		return dwLen;
	}
	BOOL CheckBase(CString& Url)
	{
		if(m_sTagName.CompareNoCase("base") == 0)
		{
			m_sBase = Url;
			return TRUE;
		}
		else return FALSE;
	}
	BOOL CompareTag(CString sTagToCompare)
	{
		CString sTag = m_sData.Mid(m_dwPos,sTagToCompare.GetLength());

		sTag.MakeLower();
		sTagToCompare.MakeLower();

		if( sTag.CollateNoCase(sTagToCompare) == 0)
			return TRUE;

		else return FALSE;
	}
	BOOL CompareTag(DWORD dwSource,CString sTagToCompare)
	{
		CString sTag = m_sData.Mid(dwSource,sTagToCompare.GetLength());

		sTag.MakeLower();
		sTagToCompare.MakeLower();

		if( sTag.CollateNoCase(sTagToCompare) == 0)
			return TRUE;

		else return FALSE;
	}
	
	
	void GotContentTags()
	{
		DWORD start=0 , end =0;
		DWORD dwRawLen = 0;

		dwRawLen+=AdvanceSpace();
		start = m_sData.Find('"',m_dwPos);
		end   = m_sData.Find('"',start+1);

		dwRawLen = end - m_dwPos;
		CString sTag = m_sData.Mid(start+1,end-start-1);
		CString URL;
		char c;
		
		for(int nPos = 0;nPos<sTag.GetLength();nPos++)
		{
			if( (c=sTag[nPos]) == 'u' || sTag[nPos]=='U')
			{
				if(sTag[nPos+1] == 'r' || sTag[nPos+1]=='R')
				{
					if(sTag[nPos+2] == 'l' || sTag[nPos+2]=='L')
					{
						start = sTag.Find('=',nPos+2);
						
						URL   = sTag.Right(sTag.GetLength() - (start+1));
						if( RemoveHashQMark(URL) == TRUE)
						{
							if (URL.Left(7)!="http://")
							{
								if (IsURLValid(URL))
								{
									Format(URL);
									if( IsHttp(URL) )
									{
										
										FILE_HEADER URLHeader;
										URLHeader.sURL = URL;
										URLHeader.nDepth = m_dwParentDepth + 1;
										m_URLDB->EnqueURLForDownloading(URLHeader);
									}
									
									//m_rawUrls.push_back(URL);
								}
								
							}
							else if(m_bExternalServerAllowed)
							{
								if(IsURLValid(URL))
								{
									
									Format(URL);
									if( IsHttp(URL) )
									{
										FILE_HEADER URLHeader;
										URLHeader.sURL = URL;
										URLHeader.nDepth = m_dwParentDepth + 1;
										m_URLDB->EnqueURLForDownloading(URLHeader);
									}									
									//	m_rawUrls.push_back(URL);
								}								
							}
							return;
						}
					}
				}
			}
		}
		
	}
	void GotHtmlTags()
	{
		DWORD dwStartUrl=0 , dwEndUrl=0;
		CString  Url;
		DWORD dwRawLen = 0;

		dwRawLen+=AdvanceSpace();
		
//		if(m_sData.GetAt(m_dwPos)== '=')
//		{m_dwPos++;dwRawLen++;}

		dwStartUrl = m_sData.Find('"',m_dwPos);
		dwEndUrl   = m_sData.Find('"',dwStartUrl+1);

		Url = m_sData.Mid(dwStartUrl+1,dwEndUrl-dwStartUrl-1);
		
		dwRawLen = dwEndUrl - m_dwPos;

		if( RemoveHashQMark(Url) == TRUE)
		{
			if (Url.Left(7)!="http://")
			{
				if(IsURLValid(Url))
				{
					if( IsHttp(Url) )
					{
						
						Format(Url);
						if(CheckBase(Url) == FALSE)
						{
							FILE_HEADER URLHeader;
							URLHeader.sURL = Url;
							URLHeader.nDepth = m_dwParentDepth + 1;
							m_URLDB->EnqueURLForDownloading(URLHeader);
							m_dwPos+=dwRawLen;
							//ADS();
						}
						
					}
				}
				//m_rawUrls.push_back(Url);					
					return;
			}
			else if(m_bExternalServerAllowed)
			{
				if(IsURLValid(Url))
				{
					if( IsHttp(Url) )
					{
						Format(Url);
						
						{
							FILE_HEADER URLHeader;
							URLHeader.sURL = Url;
							URLHeader.nDepth = m_dwParentDepth + 1;
							m_URLDB->EnqueURLForDownloading(URLHeader);
						}
					}
				}
				
				return;
			}
		}
	}
	
	/*void ADS()
	{
		DS = m_sData.Right(m_sData.GetLength() - m_dwPos);
	}*/
	void SaveTag()
	{
		DWORD dwEndTagName,dwSpacePos = 0 ,dwBPos = 0;
		dwSpacePos = m_sData.Find(' ',m_dwPos);
		dwBPos     = m_sData.Find('>',m_dwPos);

		if(dwSpacePos < dwBPos)
			dwEndTagName = dwSpacePos;

		else
			dwEndTagName = dwBPos;

		m_sTagName = m_sData.Mid(m_dwPos +1 ,dwEndTagName - m_dwPos -1);

	}
	void SetPercentage(DWORD& dwPercentage)
	{
		dwPercentage = (DWORD) (m_dwPos * 100.0 / m_dwFileSize);
	}
	BOOL RemoveHashQMark(CString& URL)
	{
		
		DWORD p = URL.Find('#');
		if(p==0)	// URL contains only # part #Somewhere
			return FALSE;
		else if(p==-1)
		{
			DWORD q = URL.Find('?');
			if(q != -1)
			{
				URL = URL.Left(q);
			}
			return TRUE;
		}
		else
			URL = URL.Left(p);
		DWORD q = URL.Find('?');
		if(q != -1)
		{
			URL = URL.Left(q);
		}
		return TRUE;
		
	}
	void Initialize(CString& Url)
	{
		m_URLDB = URLDataBase::MakeObject();

		m_sUrl = Url;

		m_dwStartTag = 0,
		m_dwEndTag   = 0,
		m_dwPos		 = 0,

		m_bInTag		= FALSE;
		m_bInComment    = FALSE;
		m_bInScript	    = FALSE;
		m_bInScriptTag  = FALSE;
	}
	void Format(CString& curr)
	{
		curr.Replace('\\','/');
		
		
		CString prefix=m_sUrl.Left(m_sUrl.ReverseFind('/')+1);
		if(prefix.IsEmpty())
			prefix = m_sUrl + "/";
		
		if(prefix.GetAt(prefix.GetLength() - 1) != '/')
			prefix = m_sUrl + "/";
		
		if(prefix.CompareNoCase("http://") == 0)
			prefix = m_sUrl;
		
		
		
		// if not full url
		if (curr.Left(7)!="http://")
		{
			
			// if format of curr is like "../../"
			if (curr.Left(3)=="../")
			{
				CString sub_prefix=prefix.Left(prefix.GetLength()-1);
				while (curr.Left(3)=="../")
				{
					sub_prefix=sub_prefix.Left(sub_prefix.ReverseFind('/'));
					curr=curr.Right(curr.GetLength()-3);
				}
				if(curr.GetAt(curr.GetLength() - 1) != '/')
					curr=sub_prefix+ "/" +curr;
				else
					curr=sub_prefix+curr;
				
			}
			// if format of curr is like "./"
			else if (curr.Left(2)=="./")
			{
				curr=curr.Right(curr.GetLength()-2);
				curr=prefix+curr;
			}
			// like /g
			else if (curr.Left(1)=="/")
			{
				DWORD dwServiceType=0;
				CString strServer;
				CString strObject;
				INTERNET_PORT nPort;
				if( AfxParseURL(m_sUrl,dwServiceType,strServer,strObject,nPort ) )
				{
					curr="http://" +strServer+curr;
						}			
				
			}
			else
				curr = prefix + curr;
		}
		
	}
	BOOL GoJavaTags(CString& Url)
	{		
		if( RemoveHashQMark(Url) == TRUE)
		{
			if (Url.Left(7)!="http://")
			{
				if(IsURLValid(Url))
				{
					if( IsHttp(Url) )
					{						
						Format(Url);
						FILE_HEADER URLHeader;
						URLHeader.sURL = Url;
						URLHeader.nDepth = m_dwParentDepth + 1;
						m_URLDB->EnqueURLForDownloading(URLHeader);
						return TRUE;										
					}
				}
				//m_rawUrls.push_back(Url);					
				return FALSE;
			}
			else if(m_bExternalServerAllowed)
			{
				if(IsURLValid(Url))
				{
					if( IsHttp(Url) )
					{
						Format(Url);
						
						{
							FILE_HEADER URLHeader;
							URLHeader.sURL = Url;
							URLHeader.nDepth = m_dwParentDepth + 1;
							m_URLDB->EnqueURLForDownloading(URLHeader);
							return TRUE;
						}
					}
				}
				
				return FALSE;
			}
		}
		return FALSE;	
	}

public:
	BOOL ParseFile(LPVOID pf)
	{
		PARSE_ARGS* pa = static_cast<PARSE_ARGS*>(pf);

		CString& sURL = pa->sURL;
		CString& sFileName = pa->sFileName;
		DWORD&  dwPercentage = pa->dwPercentage;
		BOOL &bSkip = pa->bSkip;
		UINT dwParentDepth = pa->dwParentDepth;
		BOOL bExternalServerAllowed = pa->bExternalServerAllowed = FALSE;
		
		CString S ;
		
		m_dwParentDepth = dwParentDepth;
		m_bExternalServerAllowed = bExternalServerAllowed;
		Initialize(sURL);

		if(!ReadFile(sFileName) )
			return FALSE;

		InitTags();
		m_nFoundCount = 0;

		do
		{
			if(bSkip == TRUE)
				return FALSE;

			// Case when we are at the start of Tag
			if(m_sData.GetAt(m_dwPos) == '<')
			{
				m_bInTag = TRUE;
				
				//checking <!--
				if( CheckIfInComment() == TRUE)
				{
					m_bInComment = TRUE;
				}
				//Saving Tag
				else SaveTag();

			}
			else if(m_sData.GetAt(m_dwPos) == '>')
			{
				m_nFoundCount = 0;
				// we are not in script and the tag is an ending tag
				m_bInTag = FALSE;

				if(m_sTag.CompareNoCase("/script") == 0)
					m_bInScript = FALSE;

				if(m_bInComment)
					m_bInComment = FALSE;
			}
			else if(m_bInTag || m_bInScript)
			{
				vector<CString>::iterator pTags;

				for(pTags = m_HtmlDetect.begin() ; pTags!=m_HtmlDetect.end() ; pTags++)
				{
					if( CompareTag(*pTags) == TRUE )
					{
						m_nFoundCount++;
						GotHtmlTags();
						break;
					}
				}
				//Content tag
				for(pTags = m_HtmlDetectURL.begin() ; pTags!=m_HtmlDetectURL.end() ; pTags++)
				{
					if( CompareTag(*pTags) == TRUE )
					{
						m_nFoundCount++;
						GotContentTags();
						break;
					}
				}
				if(CompareTag("JavaScript"))
					m_bInScript = TRUE;
				
				if(CompareTag("JavaScript:"))
					m_bInScriptTag = TRUE;

//////////////////////////////////////////////////////////////////////////////////////////////
				
				if(m_bInScript)
				{
					BOOL bFound ;
					char  cExpected     = '=';
					char * sExpectedEnd = ";";
					DWORD   dwTagLen = 0;
					
					if (m_bInScriptTag)
						sExpectedEnd=";\"\'";            //  href="javascript:doc.location='foo'"
					
					dwTagLen = 4;
					bFound = CompareTag(".src");// nom.src="image";
					
					if (!bFound){ bFound = CompareTag(".location");dwTagLen = 9;};  // document.location="doc"
					
					if (!bFound){ bFound = CompareTag(".href"); dwTagLen = 5;}  // document.location="doc"
					
					if (!bFound)
					{
						bFound = CompareTag(".open");
						if (bFound)  // window.open("doc",..
						{
							cExpected ='(';    // parenthèse
							sExpectedEnd="),";  // fin: virgule ou parenthèse
							dwTagLen = 5;
						}
					}
					if (!bFound)
					{
						if ( (bFound = CompareTag(".replace")) )  // window.replace("url")
						{
							cExpected='(';    // parenthèse
							sExpectedEnd=")";  // fin: parenthèse
							dwTagLen = 8;
						}
					}
					if (!bFound)
					{
						if ( (bFound = CompareTag(".link")) )  // window.link("url")
						{
							cExpected='(';    // parenthèse
							sExpectedEnd=")";  // fin: parenthèse
							dwTagLen = 5;
						}
					}
					if(bFound)
					{
						CString U;
						DWORD dwCurPos = m_dwPos + dwTagLen;
						DWORD  dwEndPos;
						/**/S = m_sData.Right(m_sData.GetLength() - dwCurPos);
						while(m_sData.GetAt(dwCurPos) == ' ')dwCurPos++;
						/**/S = m_sData.Right(m_sData.GetLength() - dwCurPos);
						
						if(m_sData.GetAt(dwCurPos) == cExpected)
						{
							dwCurPos++;
							/**/S = m_sData.Right(m_sData.GetLength() - dwCurPos);
							while(m_sData.GetAt(dwCurPos) == ' ')dwCurPos++;
							/**/S = m_sData.Right(m_sData.GetLength() - dwCurPos);
							if(m_sData.GetAt(dwCurPos) == '"' || m_sData.GetAt(dwCurPos) == '\'')
							{
								dwCurPos++;
								/**/S = m_sData.Right(m_sData.GetLength() - dwCurPos);
								dwEndPos = dwCurPos;
								char cur = m_sData.GetAt(dwEndPos);
						
								while(cur != '"' && cur != '\'' && cur != '\0')
								{
									dwEndPos++;
									cur = m_sData.GetAt(dwEndPos);
								}
								U = m_sData.Mid(dwCurPos,dwEndPos - dwCurPos);
								if(m_bInScriptTag)
									dwCurPos = dwEndPos; 
								else
									dwCurPos = dwEndPos +1; 
								
								/**/S = m_sData.Right(m_sData.GetLength() - dwCurPos);
								
								while(m_sData.GetAt(dwCurPos) == ' ')dwCurPos++;
								cur =m_sData.GetAt(dwCurPos);


								if ( strchr(sExpectedEnd,m_sData.GetAt(dwCurPos)) || (m_sData.GetAt(dwCurPos) =='\n') || (m_sData.GetAt(dwCurPos)=='\r'))
								{
									dwCurPos +=U.GetLength();
									if(GoJavaTags(U))								
										m_dwPos = dwCurPos;
									//ADS();

								}
							}
						}
					}
				}
				
///////////////////////////////////////////////////////////////////////////////////////////////
				if(!m_nFoundCount || m_bInScript)
				{
					if(m_sData.GetAt(m_dwPos) == '\'' || m_sData.GetAt(m_dwPos) == '\"')
					{
						
						DWORD dwLastCharPos = m_dwPos-1;
						while(m_sData.GetAt(dwLastCharPos) == ' ')
							dwLastCharPos--;
						
						if(strchr( "=(," , m_sData.GetAt(dwLastCharPos) ) )
						{
							DWORD dwStartPos = m_dwPos +1;
							DWORD dwEndPos   = m_sData.Find('\"',dwStartPos);
							
							CString sTempURL = m_sData.Mid(dwStartPos,dwEndPos - dwStartPos);
							
							DWORD dwNextCharPos = dwEndPos+1;
							while(m_sData.GetAt(dwNextCharPos) == ' ')
								dwNextCharPos++;							
							
							BOOL bPass = FALSE;
							if (strchr("),;>/+\r\n",m_sData.GetAt(dwNextCharPos)))      // exemple: ..img.gif";// le / est pour funct("img.gif" /* URL */);
							{
								if(IsURLValid(sTempURL))
								{
									if( IsURLOk(sTempURL))
									{									
										if(!m_bInComment)
										{											
											for(pTags = m_DoNotDetect.begin() ; pTags!=m_DoNotDetect.end() ; pTags++)
											{
												if( CompareTag(*pTags) == TRUE )
												{
													bPass = FALSE;
													break;
												}
												else bPass = TRUE;
											}
										}
									}
									if(bPass)
									{
										m_nFoundCount =0;
										GotHtmlTags();
									}
								}
							}
						}
					}
					
				}
///////////////////////////////////////////////////////////////////////////////////////////////
				

			}
			m_dwPos++;
			//ADS();
			SetPercentage(dwPercentage);

			// Optimize skip all non tag or script characters
			if( !m_bInTag && !m_bInComment && !m_bInScript &&  !m_bInScriptTag)
			{
				if( m_dwPos < m_sData.GetLength() )
				{
					DWORD dwNextPos=0;
					if( m_sData.GetAt(m_dwPos) != '<')
					{
						dwNextPos = m_sData.Find('<',m_dwPos);
						if( dwNextPos == -1)    // Cant find start tag till the end
						{
							m_dwPos = m_sData.GetLength();
						}
						else
						{
							m_dwPos = dwNextPos;
						}
					}
				}
			}


		}while(m_dwPos < m_sData.GetLength() );
	return TRUE;
	}

};

