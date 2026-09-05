#ifndef __CWinhttp_h__
#define __CWinhttp_h__

// 컴파일을 위해서는 Microsoft Platform SDK for Windows Server 2003 R2 필요함

#define MAX_READ_BYTE 50

class WINHTTP_API CWinhttp
{
public:
	bool GetResultData( IN const char *szURL, IN const char *szPostData, OUT char *szReturnData, IN int iReturnSize );

public:
	CWinhttp(void);
	virtual ~CWinhttp(void);
};

#endif // __CWinhttp_h__