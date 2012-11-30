#include<Afxmt.h>
class ConfigFiles
{
private:
	
	static ConfigFiles *m_pConfigFiles;
	CMutex  m_LogMutex;

	CFile	m_hLogFile;
	CString m_sLogFileName;
	
	CFile	m_hListFile;
	CString m_sListFileName;
	
	ConfigFiles()
	{
		CreateDirectory("fweb-cache",NULL);
		m_sLogFileName="fweb-cache\\log.txt";
		m_sListFileName = "fweb-cache\\list.txt";
	}


public:
	static ConfigFiles* MakeObject()
	{
		if(m_pConfigFiles == NULL)
		{
			m_pConfigFiles = new ConfigFiles;
			return m_pConfigFiles;
		}
		else 
			return m_pConfigFiles;
	}
	~ConfigFiles()
	{
		delete m_pConfigFiles;
	}
	void UpdateLog(CString& str)
	{
		CSingleLock downloadSingleLock(&m_LogMutex);
		downloadSingleLock.Lock();
		
		m_hLogFile.Open(m_sLogFileName,CFile::modeNoTruncate | CFile::modeCreate );
		m_hLogFile.Write(str,str.GetLength());
		m_hLogFile.Close();
	}
	void UpdateList(vector<CString>& List)
	{
		m_hLogFile.Open(m_sListFileName,CFile::modeNoTruncate | CFile::modeCreate );
		vector<CString>::iterator _iter;
		for(_iter = List.begin();_iter!=List.end();_iter++)
		{
			(*_iter)+="\n";
			m_hListFile.Write( (*_iter),_iter->GetLength());
		}
		m_hListFile.Close();
	}

};
ConfigFiles* ConfigFiles::m_pConfigFiles = NULL;




