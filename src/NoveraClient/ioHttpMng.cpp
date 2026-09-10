#include "StdAfx.h"
#include "ioHttpMng.h"

#if defined( USE_GA )

#include "../../extra/LS_HTTP/LS_HTTP/ioHTTP.h"

template<> ioHttpMng* Singleton< ioHttpMng >::ms_Singleton = 0;

class ioHttpReqTask : public IThreadTask
{

protected:
	struct GAReqData
	{
		char m_szURL[MAX_PATH];			// google analytics url
		char m_szPostData[MAX_PATH];	// send parameter

		GAReqData()
		{
			sprintf_s( m_szURL, "%s", "http://www.google-analytics.com/collect" );
			memset( m_szPostData, 0, sizeof( m_szPostData ) );
		}
	}
	m_stGAReqData;

public:
	virtual void Process();
	virtual int  GetPriority() const { return 1; }
	virtual void Done();

public:
	ioHttpReqTask( const char *szPostData );
	virtual ~ioHttpReqTask();

};

extern CLog g_GALog;

ioHttpReqTask::ioHttpReqTask( const char *szPostData )
{
	sprintf_s( m_stGAReqData.m_szPostData, "%s", szPostData );
}


ioHttpReqTask::~ioHttpReqTask()
{
}


void ioHttpReqTask::Process()
{
	if( g_App.GetGAStart() == false )
		return;

	ioHTTP Winhttp;

	char szFullURL[MAX_PATH]		= {0,};
	char szPostData[MAX_PATH]		= {0,};
	char szReturnValue[MAX_PATH]	= {0,};

	if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
	{
		sprintf_e( szFullURL, "%s", m_stGAReqData.m_szURL );
		sprintf_e( szPostData, "%s", m_stGAReqData.m_szPostData );
	}
	else
	{
		SafeSprintf( szFullURL, sizeof(szFullURL), "%1", m_stGAReqData.m_szURL );
		SafeSprintf( szPostData, sizeof(szPostData), "%1", m_stGAReqData.m_szPostData );
	}	

	// Google send
	if( !Winhttp.GetResultData( szFullURL, szPostData, szReturnValue, MAX_PATH*2 ) )
	{
		if( g_HttpMng.GetGAFailSend() == false )
		{
			// 전송 실패 로그부분 주석 처리
//			g_GALog.OpenLog( 0, "info/Ga.log");
//			g_GALog.PrintTimeAndLog( 0, "%s", szPostData );
			g_HttpMng.SetGAFailSend( true );
		}
		else
		{
//			g_GALog.PrintTimeAndLog( 0, "%s", szPostData );
		}
	}
}


void ioHttpReqTask::Done()
{
}


ioHttpMng::ioHttpMng(void):
m_bWin( false ),
m_iKill( 0 ),
m_iDeath( 0 ),
m_iAccSampling( 1 ),
m_iGameDataSampling( 1 ),
m_iAliveTime( 300000 ),
m_fExchangeRate( 0.0f ),
m_iBattleStartTime( 0 ),
m_iBattleEndTime( 0 ),
m_iHeroIndex( 0 ),
m_iHeroStartTime( 0 ),
m_iHeroEndTime( 0 ),
m_bSendHeroChange( false ),
m_bGAFailSend( false ),
m_bTutorialStart( false ),
m_iGiftIndex( 0 ),
m_iGiftCash( 0 ),
m_iScripHeroIndex( 0 ),
m_iScripHeroLimitDate( 0 ),
m_iHeroLimitDate( 0 ),
m_iSubErrCode( 0 )
{
}

ioHttpMng::~ioHttpMng(void)
{
	m_bWin					= false;
	m_iKill					= 0;
	m_iDeath				= 0;
	m_iAccSampling			= 1;
	m_iGameDataSampling		= 1;
	m_iAliveTime			= 3000000;
	m_fExchangeRate			= 0.0f;
	m_iBattleStartTime		= 0;
	m_iBattleEndTime		= 0;
	m_iHeroStartTime		= 0;
	m_iHeroEndTime			= 0;
	m_bSendHeroChange		= false;
	m_bGAFailSend			= false;
	m_iGiftIndex			= 0;
	m_iGiftCash				= 0;
	m_iScripHeroIndex		= 0;
	m_iScripHeroLimitDate	= 0;
	m_iHeroLimitDate		= 0;
	m_iSubErrCode			= 0;
}


void ioHttpMng::LoadINI()
{
	ioINILoader_e kLoader( "config/sp2_GA.ini" );

	kLoader.SetTitle_e( "Common" );
	int iGAOn = kLoader.LoadInt_e( "GA_On_Off", 0 );
	if( iGAOn == 0 )
		g_App.SetGAStart( false );
	else
		g_App.SetGAStart( true );

	kLoader.SetTitle_e( "Local" );
	char szBuf[MAX_PATH] = {0,};

	kLoader.LoadString_e( "Account_EcommerceTID", "", szBuf, MAX_PATH );
	m_szAcc_EcomTID = szBuf;

	kLoader.LoadString_e( "GameData_TID", "", szBuf, MAX_PATH );
	m_szGameDataTID = szBuf;

	kLoader.LoadString_e( "AliveCheck_TID", "", szBuf, MAX_PATH );
	m_szAliveTID = szBuf;

	m_iAccSampling = kLoader.LoadInt_e( "Account_Sampling", 1 );
	if( m_iAccSampling == 0 )
		m_iAccSampling = 1;

	m_iGameDataSampling = kLoader.LoadInt_e( "GameData_Sampling", 100 );
	if( m_iGameDataSampling == 0 )
		m_iGameDataSampling = 1;

	m_iAliveTime = kLoader.LoadInt_e( "AliveTime", 300000 );

	m_fExchangeRate = kLoader.LoadFloat( "ExchageRate", FLOAT1 );
}

void ioHttpMng::InitGALocal()
{
	// Country code, Peso, Gold name setting
	int iLocalType = (int)ioLocalManager::GetLocalType();
	switch( iLocalType )
	{
	case ioLocalManager::LCT_US:
		{
			// GP, ZP
			m_szCountryCode	= "US";
			m_szPesoISO		= "USD";
			m_szGoldISO		= "USD";
			m_szChanneling	= "US";
		}
		break;

	case ioLocalManager::LCT_INDONESIA:
		{
			// Peso, GCash
			m_szCountryCode	= "ID";
			m_szPesoISO		= "IDR";
			m_szGoldISO		= "IDR";
			m_szChanneling	= "ID";
		}
		break;

	case ioLocalManager::LCT_TAIWAN:
		{
			// Peso, Cash
			m_szCountryCode	= "TW";
			m_szPesoISO		= "TWD";
			m_szGoldISO		= "TWD";
			m_szChanneling	= "TW";
		}
		break;

	case ioLocalManager::LCT_THAILAND:
		{
			// Peso, Cash
			m_szCountryCode	= "TH";
			m_szPesoISO		= "THB";
			m_szGoldISO		= "THB";
			m_szChanneling	= "TH";
		}
		break;

	case ioLocalManager::LCT_KOREA:
		{
			// Peso, Gold
			m_szCountryCode	= "KR";
			m_szPesoISO		= "KRW";
			m_szGoldISO		= "KRW";		

			int iChannelingType = (int)g_MyInfo.GetChannelingType();
			switch( iChannelingType )
			{
			case CNT_WEMADEBUY:
				{
					m_szChanneling	= "Wemade";
				}
				break;

			case CNT_MGAME:
				{
					m_szChanneling	= "MGame";
				}
				break;

			case CNT_DAUM:
				{
					m_szChanneling	= "Daum";
				}
				break;

			case CNT_NAVER:
				{
					m_szChanneling	= "Naver";
				}
				break;

			case CNT_TOONILAND:
				{
					m_szChanneling	= "TooniLand";
				}
				break;

			case CNT_NEXON:
				{
					m_szChanneling	= "Nexon";
				}
				break;

			default:
				{
					m_szChanneling	= "None";
				}
				break;
			}
		}
		break;

	default:
		{
			m_szCountryCode	= "None";
			m_szPesoISO		= "None";
			m_szGoldISO		= "None";
			m_szChanneling	= "None";
		}
		break;
	}
}


// alive check
void ioHttpMng::GA_AliveCheck()
{
	// ACCOUNT_ALIVE
	GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FACCOUNT%2FALIVE", 3 );
}


void ioHttpMng::GA_PageVIewTracking( const DWORD dwUID, const char *szPostData, int iPVType, const char *szErrPos, int iGold, const char *szOS, const char *szResolution )
{
	// GA On, Off 체크
	if( g_App.GetGAStart() == false )
		return;

	// 사이즈 체크
	if( strlen(szPostData) > MAX_PATH )
		return;

	char strPostData[MAX_PATH]	= {0,};
	char chLevel[8]				= {0,};

	if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		sprintf_e( chLevel, "%d", g_MyInfo.GetGradeLevel() );
	else
		SafeSprintf( chLevel, sizeof(chLevel), "%1", g_MyInfo.GetGradeLevel() );

	switch( iPVType )
	{
	// 일반 PageView
	case GA_PV_NONE:
		{
			// 일반 page view 는 sampling 을 한다
			if( g_App.GetGAUser() == false )
				return;

			_snprintf_s( strPostData
				, sizeof( strPostData ) - 1
				, "v=1&tid=%s&cid=%s&t=pageview&dh=lostsaga.com&dp=%s&dt=LOST_SAGA&uid=%d&cd1=%s&cd2=%s&cd4=%s"
				, m_szGameDataTID.c_str()
				, m_szCountryCode
				, szPostData
				, dwUID
				, m_szChanneling
				, chLevel
				, m_szCountryCode );

			AddReqGA( strPostData );
		}
		break;

	// Log in. Session start
	case GA_PV_SS_START:
		{
			// GA Accont Data Sampling. ex> ( User Index % 100 == 0 ), 1% 만 sampling
			int iUID = (int)g_MyInfo.GetUserIndex();
			if( iUID % GetGAAccSampling() != 0 )
				return;

			char strPostDataStart[MAX_PATH] = {0,};

			_snprintf_s( strPostDataStart
				, sizeof(strPostDataStart) - 1
				, "v=1&tid=%s&cid=%s&t=pageview&dh=lostsaga.com&dp=%s&dt=LOST_SAGA&uid=%d&sc=start&ua=%s&sr=%s&cd1=%s&cd2=%s&cd4=%s"
				, m_szAcc_EcomTID.c_str()
				, m_szCountryCode
				, szPostData
				, dwUID
				, szOS
				, szResolution
				, m_szChanneling
				, chLevel
				, m_szCountryCode );

			AddReqGA( strPostDataStart );
		}
		break;

	// Log out, Session end
	case GA_PV_SS_END:
		{
			// total game play time
			int iGameTime = (int)REALGETTIME() / 1000;

			_snprintf_s( strPostData
				, sizeof(strPostData) - 1
				, "v=1&tid=%s&cid=%s&t=pageview&dh=lostsaga.com&dp=%s&dt=LOST_SAGA&uid=%d&sc=end&cd1=%s&cd2=%s&cd4=%s&cm1=%d"
				, m_szGameDataTID.c_str()
				, m_szCountryCode
				, szPostData
				, dwUID
				, m_szChanneling
				, chLevel
				, m_szCountryCode
				, iGameTime );

			AddReqGA( strPostData );
		}
		break;

	// Session alive check
	case GA_PV_SS_ALIVE:
		{
			//////////////////////////////////////////////////////////////////////////
			// Global TID
			_snprintf_s( strPostData
				, sizeof(strPostData) - 1
				, "v=1&tid=%s&cid=%s&t=pageview&dh=lostsaga.com&dp=%s&dt=LOST_SAGA_%s&uid=%s_%d&cd1=%s&cd2=%s&cd4=%s"
				, m_szAliveTID.c_str()
				, m_szCountryCode
				, szPostData
				, m_szCountryCode
				, m_szCountryCode
				, dwUID
				, m_szChanneling
				, chLevel
				, m_szCountryCode );

			AddReqGA( strPostData );
		}
		break;

	// Gold Charge
	case GA_PV_GOLD_CHARGE:
		{
			float fChargeMoney = iGold * m_fExchangeRate;
			char chGold[128]	= {0,};
			sprintf_s( chGold, sizeof(chGold), "%d", fChargeMoney );

			_snprintf_s( strPostData
				, sizeof(strPostData) - 1
				, "v=1&tid=%s&cid=%s&t=pageview&dh=lostsaga.com&dp=%s&dt=LOST_SAGA&uid=%d&cd1=%s&cd2=%s&cd3=%s&cd4=%s&cm8=%f"
				, m_szAcc_EcomTID.c_str()
				, m_szCountryCode
				, szPostData
				, dwUID
				, m_szChanneling
				, chLevel
				, chGold
				, m_szCountryCode
				, fChargeMoney );

			AddReqGA( strPostData );
		}
		break;

	// Account create, New user
	case GA_PV_NEW_USER:
		{
			// GA Accont Data Sampling. ex> ( User Index % 100 == 0 ), 1% 만 sampling
			int iUID = (int)g_MyInfo.GetUserIndex();
			if( iUID % GetGAAccSampling() != 0 )
				return;

			_snprintf_s( strPostData
				, sizeof(strPostData) - 1
				, "v=1&tid=%s&cid=%s&t=pageview&dh=lostsaga.com&dp=%s&dt=LOST_SAGA&uid=%d&cd1=%s&cd2=%s&cd4=%s"
				, m_szAcc_EcomTID.c_str()
				, m_szCountryCode
				, szPostData
				, dwUID
				, m_szChanneling
				, chLevel
				, m_szCountryCode );

			AddReqGA( strPostData );
		}
		break;

	// Error, Client end, cm1=total duration game time
	case GA_PV_ERR_POS:
		{
			// total game play time
			int iGameTime = (int)REALGETTIME() / 1000;

			// Sub Error code index
			char chErrCode[32] = {0,};
			if( m_iSubErrCode != 0 )
				sprintf_s( chErrCode, "%d", m_iSubErrCode );

			_snprintf_s( strPostData
				, sizeof(strPostData) - 1
				, "v=1&tid=%s&cid=%s&t=pageview&dh=lostsaga.com&dp=%s&dt=LOST_SAGA&uid=%d&sc=end&cd1=%s&cd2=%s&cd4=%s&cd5=%s&cd7=%s&cm1=%d"
				, m_szGameDataTID.c_str()
				, m_szCountryCode
				, szPostData
				, dwUID
				, m_szChanneling
				, chLevel
				, m_szCountryCode
				, szErrPos
				, chErrCode
				, iGameTime );

			AddReqGA( strPostData );

			m_iSubErrCode = 0;
		}
		break;

	// Battle result, Product custom metric setting
	case GA_PV_BATTLE_RESULT:
		{
			int iPlayTime	= m_iBattleEndTime - m_iBattleStartTime;
			int iWin		= 0;
			int iLose		= 0;

			if( m_bWin == true )
				iWin		= 1;
			else
				iLose		= 1;
			
			_snprintf_s( strPostData
				, sizeof(strPostData) - 1
				, "v=1&tid=%s&cid=%s&t=pageview&dh=lostsaga.com&dp=%s&dt=LOST_SAGA&uid=%d&cd1=%s&cd2=%s&cd4=%s&cm1=%d&cm3=%d&cm4=%d&cm5=%d&cm6=%d"
				, m_szGameDataTID.c_str()
				, m_szCountryCode
				, szPostData
				, dwUID
				, m_szChanneling
				, chLevel
				, m_szCountryCode
				, iPlayTime
				, iWin
				, iLose
				, m_iKill
				, m_iDeath );

			AddReqGA( strPostData );
		}
		break;

	// PC Room check
	case GA_PV_PCROOM:
		{
			_snprintf_s( strPostData
				, sizeof( strPostData ) - 1
				, "v=1&tid=%s&cid=%s&t=pageview&dh=lostsaga.com&dp=%s&dt=LOST_SAGA&uid=%d&cd1=%s&cd2=%s&cd4=%s"
				, m_szGameDataTID.c_str()
				, m_szCountryCode
				, szPostData
				, dwUID
				, m_szChanneling
				, chLevel
				, m_szCountryCode );

			AddReqGA( strPostData );
		}
		break;

	// Hero change, Hero play time
	case GA_PV_HERO_CHANGE:
		{
			// 용병 교체는 sampling 을 한다
			if( g_App.GetGAUser() == true )
			{
				char chHeroIndex[8]	= {0,};
				int iHeroPlayTime	= m_iHeroEndTime - m_iHeroStartTime;

				if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
					sprintf_e( chHeroIndex, "%d", m_iHeroIndex );
				else
					SafeSprintf( chHeroIndex, sizeof(chHeroIndex), "%1", m_iHeroIndex );

				_snprintf_s( strPostData
					, sizeof(strPostData) - 1
					, "v=1&tid=%s&cid=%s&t=pageview&dh=lostsaga.com&dp=%s&dt=LOST_SAGA&uid=%d&cd1=%s&cd2=%s&cd4=%s&cd6=%s&cm7=%d"
					, m_szGameDataTID.c_str()
					, m_szCountryCode
					, szPostData
					, dwUID
					, m_szChanneling
					, chLevel
					, m_szCountryCode
					, chHeroIndex
					, iHeroPlayTime );

				AddReqGA( strPostData );
			}

			m_iHeroEndTime		= 0;
			m_iHeroStartTime	= 0;
		}
		break;

	// Compulsion unmouse busy
	case GA_PV_UNMOUSE_BUSY:
		{
			// total game play time
			int iGameTime = (int)REALGETTIME() / 1000;

			_snprintf_s( strPostData
				, sizeof( strPostData ) - 1
				, "v=1&tid=%s&cid=%s&t=pageview&dh=lostsaga.com&dp=%s&dt=LOST_SAGA&uid=%d&cd1=%s&cd2=%s&cd4=%s&cm1=%d"
				, m_szGameDataTID.c_str()
				, m_szCountryCode
				, szPostData
				, dwUID
				, m_szChanneling
				, chLevel
				, m_szCountryCode
				, iGameTime );

			AddReqGA( strPostData );
		}
		break;
	}		
}


void ioHttpMng::GA_EventTracking( const DWORD dwUID, const char *szCategory, const char *szAction, const char *szLabel, int iValue, const char *szPostData, int iEVType )
{
	// GA 초기화 완료된 유저 인 경우만
	if( g_App.GetGAStart() == false )
		return;

	// 사이즈 체크
	if( strlen(szPostData) > MAX_PATH )
		return;

	char strPostData[MAX_PATH] = {0,};

	switch( iEVType )
	{
	// 일반 Event Tracking
	case GA_EV_NONE:
		{
			// 일반 Event tracking 은 sampling 을 하도록 한다.
			if( g_App.GetGAUser() == true )
			{
				_snprintf_s( strPostData
					, sizeof(strPostData) - 1
					, "v=1&tid=%s&cid=%s&t=event&ec=%s&ea=%s&el=%s&ev=%d&dp=%s&uid=%d"
					, m_szGameDataTID.c_str()
					, m_szCountryCode
					, szCategory
					, szAction
					, szLabel
					, iValue
					, szPostData
					, dwUID );

				// 일반 event tracking 임시 해제
				// AddReqGA( strPostData );
			}			
		}
		break;

	// Error, Client end
	case GA_EV_ERR_POS:
		{
			// Sub Error code index
			char chErrCode[32] = {0,};
			if( m_iSubErrCode != 0 )
				sprintf_s( chErrCode, "%d", m_iSubErrCode );

			_snprintf_s( strPostData
				, sizeof(strPostData) - 1
				, "v=1&tid=%s&cid=%s&t=event&ec=%s&ea=%s&el=%s&ev=%d&dp=%s&uid=%d&cd7=%s"
				, m_szGameDataTID.c_str()
				, m_szCountryCode
				, szCategory
				, szAction
				, szLabel
				, iValue
				, szPostData
				, dwUID
				, chErrCode );

			AddReqGA( strPostData );
		}
		break;

	// Compulsion unmouse busy
	case GA_EV_UNMOUSE_BUSY:
		{
			// total game play time
			int iGameTime = (int)REALGETTIME() / 1000;

			_snprintf_s( strPostData
				, sizeof(strPostData) - 1
				, "v=1&tid=%s&cid=%s&t=event&ec=%s&ea=%s&el=%s&ev=%d&dp=%s&uid=%d&cm1=%d"
				, m_szGameDataTID.c_str()
				, m_szCountryCode
				, szCategory
				, szAction
				, szLabel
				, iValue
				, szPostData
				, dwUID
				, iGameTime );

			AddReqGA( strPostData );
		}
		break;
	}
}


void ioHttpMng::GA_TransactionHitTracking( const DWORD dwUID, const char *szTID, const char *szName, int iPrice, int iQuanity, int iCode, const char *szCategory, const char *szPostData, bool bCash )
{
	// GA On, Off 체크
	if( g_App.GetGAStart() == false )
		return;

	// 사이즈 체크
	if( strlen(szPostData) > MAX_PATH )
		return;

	// Cash 사용 여부를 통해 화폐 단위 세팅
	if( bCash )
		m_szISO = m_szGoldISO;
	else
		m_szISO = m_szPesoISO;

	char chLevel[8]					= {0,};
	char strPostData[MAX_PATH]		= {0,};

	if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		sprintf_e( chLevel, "%d", g_MyInfo.GetGradeLevel() );
	else
		SafeSprintf( chLevel, sizeof(chLevel), "%1", g_MyInfo.GetGradeLevel() );
	
	_snprintf_s( strPostData
		, sizeof(strPostData) - 1
		, "v=1&tid=%s&cid=%s&t=transaction&ti=%s&ta=%s&tr=%d&cu=%s&dp=%s&uid=%d&cd1=%s&cd2=%s&cd4=%s"
		, m_szAcc_EcomTID.c_str()
		, m_szCountryCode
		, szTID
		, szName
		, iPrice
		, m_szISO
		, szPostData
		, dwUID
		, m_szChanneling
		, chLevel
		, m_szCountryCode );

	AddReqGA( strPostData );
}


void ioHttpMng::GA_ItemHitTracking( const DWORD dwUID, const char *szName, int iPrice, int iQuanity, int iCode, const char *szCategory, const char *szPostData, bool bCash )
{
	// GA On, Off 체크
	if( g_App.GetGAStart() == false )
		return;

	// Sampling 체크, Cash 사용 체크. ( Cash 사용은 Sampling 하지 않도록 한다. )
	if( g_App.GetGAUser() == false && bCash == false )
		return;

	// 사이즈 체크
	if( strlen(szPostData) > MAX_PATH )
		return;

	// Time stamp setting
	time_t cur;
	struct tm* ptm;
	char chTime[128] = {0,};

	cur = time( NULL );
	ptm = localtime(&cur);
	strftime( chTime, sizeof(chTime), "%Y%m%d%H%M%S", ptm );

	if( bCash )
		m_szISO = m_szGoldISO;
	else
	{
		m_szISO = m_szPesoISO;
		return;	// in game money 정보는 보내지 않도록 한다.
	}

	char chLevel[8]			= {0,};
	if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		sprintf_e( chLevel, "%d", g_MyInfo.GetGradeLevel() );
	else
		SafeSprintf( chLevel, sizeof(chLevel), "%1", g_MyInfo.GetGradeLevel() );

	// Transaction tracking 용 Post data 세팅
	char chTranData[128]	= {0,};
	char chSearch[32]		= "USE";
	char chChange[32]		= "TRAN";
	char *pParse			= NULL;
	int iLen				= 0;

	sprintf_s( chTranData, "%s", szPostData );

	for (;;) 
	{
		pParse = strstr( chTranData, chSearch );
		if ( pParse == NULL ) 
			break;

		iLen = strlen( chSearch );
		memmove( pParse, pParse+iLen, strlen( pParse+iLen )+1 );

		iLen=strlen( chChange );
		memmove( pParse + iLen, pParse, strlen( pParse )+1 );
		memcpy( pParse, chChange, iLen);
	}

	// Item tracking 으로 보내기 전 Transaction tracking 을 항상 보내주도록 한다.
	GA_TransactionHitTracking( dwUID, chTime, szName, iPrice, iQuanity, iCode, szCategory, chTranData, bCash );	

	char strPostData[MAX_PATH] = {0,};
	_snprintf_s( strPostData
		, sizeof(strPostData) - 1
		, "v=1&tid=%s&cid=%s&t=item&ti=%s&in=%s&ip=%d&iq=%d&ic=%d&iv=%s&cu=%s&dp=%s&uid=%d&cd1=%s&cd2=%s&cd4=%s"
		, m_szAcc_EcomTID.c_str()
		, m_szCountryCode
		, chTime
		, szName
		, iPrice
		, iQuanity
		, iCode
		, szCategory
		, m_szISO
		, szPostData
		, dwUID
		, m_szChanneling
		, chLevel
		, m_szCountryCode );

	AddReqGA( strPostData );

	SetScripHeroIndex( 0 );
	SetScripHeroLimitDate( 0 );
	SetHeroLimitDate( 0 );
}


void ioHttpMng::AddReqGA( const char *szPostData )
{
	if( strlen(szPostData) > MAX_PATH )
		return;

	ioHttpReqTask *pTask = new ioHttpReqTask( szPostData );
	if( ioThreadTaskManager::GetInstancePtr() != NULL && pTask )
	{
		g_ThreadTaskMgr.AddTask( pTask );
	}
}

#endif	// USE_GA