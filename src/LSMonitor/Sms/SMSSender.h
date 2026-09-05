#pragma once

#define MAX_PARAMS 11

#import "msxml4.dll" 
//using namespace MSXML2;

//"C:\Program Files\Common Files\MSSoap\Binaries\mssoap30.dll" 컴파일 안될때 풀네임을 기록하니 컴파일됨.
#import "mssoap30.dll" exclude("IErrorInfo", "IStream", "ISequentialStream", "_LARGE_INTEGER", "_ULARGE_INTEGER","tagSTATSTG", "_FILETIME")

using namespace MSSOAPLib30;

class CSMSSender
{
protected:
	enum PARAMTYPE
	{
		USER_DATA3 = 0,
		USER_DATA2 = 1,
		USER_DATA1 = 2,
		RESERVED_DATE = 3,
		MERGE_NAME = 4,
		SMS_CONTENTS = 5,
		RECIEVE_NUMBER = 6,
		SEND_NUMBER = 7,
		DNS_NAME = 8,
		PASSWORD = 9,
		ID = 10
	};
protected:
	ISoapClientPtr m_pSoapClient;

protected:
	bool ChangeType( VARIANTARG  &params, const CString &ParamText );

public:
	bool Init();
	bool SendSMS(const CString &szID, const CString &szPassWord,const CString &szDNSName,const CString &szSendNumber
		,const CString &szRecieveNumber,const CString &szContents);

public:
	CSMSSender(void);
	~CSMSSender(void);

};
