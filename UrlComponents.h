class UrlComponents
{
public:
	UrlComponents()
	{
		typedef struct {
    DWORD dwStructSize;
    LPTSTR lpszScheme;
    DWORD dwSchemeLength;
    INTERNET_SCHEME nScheme;
    LPTSTR lpszHostName;
    DWORD dwHostNameLength;
    INTERNET_PORT nPort;
    LPTSTR lpszUserName;
    DWORD dwUserNameLength;
    LPTSTR lpszPassword;
    DWORD dwPasswordLength;
    LPTSTR lpszUrlPath;
    DWORD dwUrlPathLength;
    LPTSTR lpszExtraInfo;
    DWORD dwExtraInfoLength;
	} URL_COMPONENTS, *LPURL_COMPONENTS;
	}
private:
