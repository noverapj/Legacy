#include "../stdafx.h"
#include "./thailanduser.h"
#include "../EtcHelpFunc.h"
#include "../Local\ioLocalThailand.h"


extern CLog LOG;

ThailandUser::ThailandUser(void)
{
	Clear();
}

ThailandUser::~ThailandUser(void)
{
}

void ThailandUser::SetData( DWORD dwCreateTime, const ioHashString &rsBillingGUID, const ioHashString &rsPrivateID, const ioHashString &rsEncodePW, const ioHashString &rsIP, DWORD dwReturnMsgType, const ioHashString &rsServerIP, const int iServerPort )
{
	m_dwCreateTime    = dwCreateTime;
	m_sBillingGUID    = rsBillingGUID;
	m_sPrivateID      = rsPrivateID;	
	m_sIP             = rsIP;
	m_dwReturnMsgType = dwReturnMsgType;
	m_sServerIP       = rsServerIP;
	m_iServerPort     = iServerPort;

	SetDomainAndID( m_sPrivateID );
	SetPW( rsEncodePW );
}

void ThailandUser::Clear()
{
	m_dwCreateTime = 0;

	m_sPID.Clear();
	m_sPrivateID.Clear();
	m_sThailandID.Clear();
	m_sPW.Clear(); 
	m_sDomain.Clear();
	m_sIP.Clear();
	m_sBillingGUID.Clear();
	m_sOTPID.Clear();
	m_sOTPPW.Clear();

	m_socket = 0;
	m_bAlive = false;

	m_sServerIP.Clear();
	m_iServerPort = 0;
	m_dwReturnMsgType = 0;

	m_iChannelingType = 0;
	m_dwUserIndex = 0;
	m_bMouseBusy  = false;

	m_iItemType = 0;
	for (int i = 0; i < MAX_ITEM_VALUE ; i++)
		m_iItemValueList[i] = 0;
	m_iPayAmt = 0;
}

void ThailandUser::SetDomainAndID( const ioHashString &rsPrivateID )
{
	char szDomain[MAX_PATH]="";
	enum { MAX_DOMAIN = 4 };
	if( rsPrivateID.Length() > MAX_DOMAIN )
		StringCbCopyN( szDomain, sizeof( szDomain ), rsPrivateID.c_str(), MAX_DOMAIN );
	m_sDomain = szDomain;
	m_sDomain.MakeUpper();

	char szThaillandID[MAX_PATH]="";
	int iCnt = 0;
	int iSize = rsPrivateID.Length();
	iSize--; // 로그인 ID 구분자 | 제거
		
	for (int i = 0; i < iSize; i++)
	{
		if( i >= MAX_DOMAIN+1 ) // +1 THPP.tester001에서 .을 제거하기 위해서
		{
			szThaillandID[iCnt] = rsPrivateID.At(i);
			iCnt++;
			if( iCnt >= MAX_PATH )
				break;
		}
	}

	m_sThailandID = szThaillandID;
}

void ThailandUser::SetPW( const ioHashString &rsEncodePW )
{
	enum 
	{ 
		LOGINPW_ARRAY = 0, 
		OTPIDPW_ARRAY = 1,

		OTPID_ARRAY = 0,
		OTPPW_ARRAY = 1,
	};

	if( rsEncodePW.IsEmpty() )
		return;

	ioHashStringVec vParseStr;
	Help::GetParseData( rsEncodePW, vParseStr, THAILAND_TOKEN );

	if( vParseStr.size() > LOGINPW_ARRAY && !vParseStr[LOGINPW_ARRAY].IsEmpty() )
	{
		if( vParseStr[LOGINPW_ARRAY] != "0" )
		{
			char szLoginPW[MAX_PATH]="";
			ZeroMemory( szLoginPW, sizeof( szLoginPW ) );
			GetDecodeStr( vParseStr[LOGINPW_ARRAY], m_sPrivateID, szLoginPW, sizeof( szLoginPW ) );
			m_sPW = szLoginPW;
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Error 1 %s", __FUNCTION__, rsEncodePW.c_str() );
	}

	if( vParseStr.size() <= OTPIDPW_ARRAY )
		return;

	char szOTPIDPW[MAX_PATH]="";
	ZeroMemory( szOTPIDPW, sizeof( szOTPIDPW ) );
	GetDecodeStr( vParseStr[OTPIDPW_ARRAY], m_sPrivateID, szOTPIDPW, sizeof( szOTPIDPW ) );

	ioHashString sOTPIDPW = szOTPIDPW;
	Help::GetParseData( sOTPIDPW, vParseStr, THAILAND_TOKEN );
	if( vParseStr.size() > OTPPW_ARRAY && !vParseStr[OTPID_ARRAY].IsEmpty() && !vParseStr[OTPPW_ARRAY].IsEmpty() )
	{
		m_sOTPID = vParseStr[OTPID_ARRAY];
		m_sOTPPW = vParseStr[OTPPW_ARRAY];
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Error 2 %s", __FUNCTION__, sOTPIDPW.c_str() );
	}
}

void ThailandUser::GetDecodeStr( IN const ioHashString &rsEncode, IN const ioHashString &rsPrivateID, OUT char *szDecode, IN int iDecodeSize )
{
	enum { MAX_RANDOM_KEY = 20, };
	char szRandomKey[MAX_PATH]="";
	ZeroMemory( szRandomKey, sizeof( szRandomKey ) );
	if( rsEncode.Length() > MAX_RANDOM_KEY )
		StringCbCopyN( szRandomKey, sizeof( szRandomKey ), rsEncode.c_str(), MAX_RANDOM_KEY );
	char szEncode[MAX_PATH]="";
	ZeroMemory( szEncode, sizeof( szEncode ) );
	int iEncodeCnt = 0;
	int iEncodPwLength = rsEncode.Length();
	for (int i = 0; i < iEncodPwLength; i++)
	{
		if( i >= MAX_RANDOM_KEY )
		{
			szEncode[iEncodeCnt] = rsEncode.At(i);
			iEncodeCnt++;
			if( iEncodeCnt >= MAX_PATH )
				break;
		}
	}

	char szUserKey[MAX_PATH]="";
	ZeroMemory( szUserKey, sizeof( szUserKey ) );
	StringCbPrintf( szUserKey, sizeof( szUserKey ), "%s%s", rsPrivateID.c_str(), szRandomKey );
	Help::Decode( szEncode, strlen( szEncode ), szDecode, iDecodeSize, szUserKey, strlen( szUserKey ) );
}

void ThailandUser::SetExtendData( int iChannelingType, DWORD dwUserIndex, bool bMouseBusy, int iItemType, int iItemValueList[MAX_ITEM_VALUE], int iPayAmt )
{
	m_iChannelingType = iChannelingType;
	m_dwUserIndex = dwUserIndex;
	m_bMouseBusy  = bMouseBusy;
	m_iItemType   = iItemType;

	for (int i = 0; i < MAX_ITEM_VALUE ; i++)
		m_iItemValueList[i] = iItemValueList[i];

	m_iPayAmt = iPayAmt;
}

int ThailandUser::GetItemValue( int iArray ) const
{
	if( COMPARE( iArray, 0, MAX_ITEM_VALUE ))
		return m_iItemValueList[iArray];
	else
		return 0;
}