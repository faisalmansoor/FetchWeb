class CThread
{
public:
		CThread()
		{ 
			m_pThreadFunc = CThread::EntryPoint; 
		}
		~CThread()
		{
			if ( m_ThreadCtx.m_hThread )
				Stop(true);
		}		
		
		BOOL IsRunning()
		{
			return GetExitCode() == STILL_ACTIVE ;
		}

		DWORD Start( void* arg = NULL )
		{
			m_ThreadCtx.m_pUserData = arg;
			m_ThreadCtx.m_hThread = CreateThread(NULL, 0, m_pThreadFunc, this, 0, &m_ThreadCtx.m_dwTID);
			m_ThreadCtx.m_dwExitCode = (DWORD)-1;

			return GetLastError();
		}

		DWORD Stop ( bool bForceKill = false )
		{
			if ( m_ThreadCtx.m_hThread )
			{
				GetExitCodeThread(m_ThreadCtx.m_hThread, &m_ThreadCtx.m_dwExitCode);

				if ( m_ThreadCtx.m_dwExitCode == STILL_ACTIVE && bForceKill )
					TerminateThread(m_ThreadCtx.m_hThread, DWORD(-1));

				m_ThreadCtx.m_hThread = NULL;
			}

			return m_ThreadCtx.m_dwExitCode;
		}

		DWORD GetExitCode() const 
		{ 
			if ( m_ThreadCtx.m_hThread )
				GetExitCodeThread(m_ThreadCtx.m_hThread, (LPDWORD)&m_ThreadCtx.m_dwExitCode);
			return m_ThreadCtx.m_dwExitCode;
		}


protected:

		static DWORD WINAPI EntryPoint( LPVOID pArg)
		{
			CThread *pParent = reinterpret_cast<CThread*>(pArg);

			pParent->ThreadCtor();

			pParent->Run( pParent->m_ThreadCtx.m_pUserData );

			pParent->ThreadDtor();

			return 0;
		}
		
		
		virtual DWORD Run( LPVOID /* arg */ )
		{ return m_ThreadCtx.m_dwExitCode; }

		virtual void ThreadCtor(){	}

virtual void ThreadDtor(){	}
		
private:
		class CThreadContext
		{
		public:
			CThreadContext(){
				memset(this, 0, sizeof(CThreadContext));
			}

		public:
			HANDLE m_hThread;					//	The Thread Handle
			DWORD  m_dwTID;						//	The Thread ID
			LPVOID m_pUserData;					//	The user data pointer
			LPVOID m_pParent;					//	The this pointer of the parent CThread object
			DWORD  m_dwExitCode;				//	The Exit Code of the thread
		};

protected:
		CThreadContext			m_ThreadCtx;	//	The Thread Context member
		LPTHREAD_START_ROUTINE	m_pThreadFunc;	//	The Worker Thread Function Pointer
};

