#include "CreateAllDirectories.h"
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
