#ifndef __ioHttpMng_h__
#define __ioHttpMng_h__

#if defined( USE_GA )

#pragma once

class ioHttpMng : public Singleton< ioHttpMng >
{
protected:
	enum GaPageView
	{
		GA_PV_NONE,				// 일반 PageView
		GA_PV_SS_START,			// Log in. Session start
		GA_PV_SS_END,			// Log out, Session end, cm1=total duration game time
		GA_PV_SS_ALIVE,			// Session Alive
		GA_PV_GOLD_CHARGE,		// Gold Charge
		GA_PV_NEW_USER,			// Account create, New user
		GA_PV_ERR_POS,			// Error, Client end, cm1=total duration game time
		GA_PV_BATTLE_RESULT,	// Battle result, Product custom metric setting
		GA_PV_PCROOM,			// PC Room check
		GA_PV_HERO_CHANGE,		// Hero change, Hero play time
		GA_PV_UNMOUSE_BUSY,		// Compulsion unmouse busy
		GA_PV_MAX,
	};

	enum GaEvent
	{
		GA_EV_NONE,				// 일반 Event Tracking
		GA_EV_ERR_POS,			// Error, Client end
		GA_EV_UNMOUSE_BUSY,		// Compulsion unmouse busy
		GA_EV_MAX,
	};

public:
	ioHttpMng(void);
	~ioHttpMng(void);

	void	LoadINI();
	void	InitGALocal();

	void	GA_PageVIewTracking( const DWORD dwUID
								, const char *szPostData
								, int iPVType = 0
								, const char *szErrPos = ""
								, int iGold = 0
								, const char *szOS = ""
								, const char *szResolution = "" );
	void	GA_EventTracking( const DWORD dwUID
							, const char *szCategory
							, const char *szAction
							, const char *szLabel
							, int iValue
							, const char *szPostData
							, int iEVType = 0 );
	void	GA_TransactionHitTracking( const DWORD dwUID
							, const char *szTID
							, const char *szName
							, int iPrice
							, int iQuanity
							, int iCode
							, const char *szCategory
							, const char *szPostData
							, bool bCash = false );
	void	GA_ItemHitTracking( const DWORD dwUID
							, const char *szName
							, int iPrice
							, int iQuanity
							, int iCode
							, const char *szCategory
							, const char *szPostData
							, bool bCash = false );
	
	void	AddReqGA( const char *szPostData );

	int		GetGAGameDataSampling(){ return m_iGameDataSampling; }	

	int		GetGAAccSampling(){ return m_iAccSampling; }	

	// 각 모드 승,킬,데스
	void	SetWin( bool bWin ){ m_bWin = bWin; }
	bool	GetWin(){ return m_bWin; }
	void	SetKill( int iKill ){ m_iKill = iKill; }
	int		GetKill(){ return m_iKill; }
	void	SetDeath( int iDeath ){ m_iDeath = iDeath; }
	int		GetDeath(){ return m_iDeath; }

	// alive check
	void	GA_AliveCheck();
	int		GetAliveTime(){ return m_iAliveTime; }

	// 각 모드 별 시작 시각, 종료 시각
	void	SetBattleStartTime( int iTime ){ m_iBattleStartTime = iTime; }
	int		GetBattleStartTime(){ return m_iBattleStartTime; }
	void	SetBattleEndTime( int iTime ){ m_iBattleEndTime = iTime; }
	int		GetBattleEndTime(){ return m_iBattleEndTime; }

	// 각 용병 별 플레이 시각, 종료 시각
	void			SetHeroIndex( int index ){ m_iHeroIndex = index; }
	int				GetHeroIndex(){ return m_iHeroIndex; }
	void			SetHeroStartTime( int iTime ){ m_iHeroStartTime = iTime; }
	int				GetHeroStartTime(){ return m_iHeroStartTime; }
	void			SetHeroEndTime( int iTime ){ m_iHeroEndTime = iTime; }
	int				GetHeroEndTime(){ return m_iHeroEndTime; }

	// 선택된 전투 타입 : 일반전투(BATTLE), 레더전투(LADDER), 진영전투(CAMP)
	void			SetBattleType( const char *szType ){ m_szBattleType = szType; }
	const char*		GetBattleType(){ return m_szBattleType.c_str(); }

	// 선택된 전투 모드 : POWERSTONE, PRISONER, HIDENCROWN...
	void			SetBattleMode( const char *szMode ){ m_szBattleMode = szMode; }
	const char*		GetBattleMode(){ return m_szBattleMode.c_str(); }

	// 용병 교체 데이터를 전송하는 모드인지
	void			SetSendHeroChange( bool bSend ){ m_bSendHeroChange = bSend; }
	bool			GetSendHeroChange(){ return m_bSendHeroChange; }

	// GA Fail data, FTP Log 전송 여부 flag
	void			SetGAFailSend( bool bSend ){ m_bGAFailSend = bSend; }
	bool			GetGAFailSend(){ return m_bGAFailSend; }

	// Tutorial 시작하는 유저인지 flag
	void			SetTutorialStart( bool bStart ){ m_bTutorialStart = bStart; }
	bool			GetTutorialStart(){ return m_bTutorialStart; }

	// 선물 아이템 인덱스
	void			SetGiftIndex( int index ){ m_iGiftIndex = index; }
	int				GetGiftIndex(){ return m_iGiftIndex; }

	// 선물 아이템 케쉬 가격
	void			SetGiftCash( int iCash ){ m_iGiftCash = iCash; }
	int				GetGiftCash(){ return m_iGiftCash; }

	// Subscription(청약철회) 용도 케릭터 인덱스
	void			SetScripHeroIndex( int index ){ m_iScripHeroIndex = index; }
	int				GetScripHeroIndex(){ return m_iScripHeroIndex; }
	

	// Subscription(청약철회) 용도 케릭터 남은 시간 값
	void			SetScripHeroLimitDate( int iLimitDate ){ m_iScripHeroLimitDate = iLimitDate; }
	int				GetScripHeroLimitDate(){ return m_iScripHeroLimitDate; }

	// 용병 구매 시 기간제 용병 시간값
	void			SetHeroLimitDate( int iLimitDate ){ m_iHeroLimitDate = iLimitDate; }
	int				GetHeroLimitDate(){ return m_iHeroLimitDate; }

	// Error code sub index
	void			SetSubErrCode( int iCode ){ m_iSubErrCode = iCode; }
	int				GetSubErrCode(){ return m_iSubErrCode; } 


protected:
	const char		*m_szCountryCode;
	const char		*m_szISO;
	const char		*m_szPesoISO;
	const char		*m_szGoldISO;

	// Account, Ecommerce 전용 TID
	ioHashString	m_szAcc_EcomTID;

	// 그 밖에 게임 데이터 전용 TID
	ioHashString	m_szGameDataTID;

	// Alive check 전용 TID
	ioHashString	m_szAliveTID;

	// Game data Sampling 상수
	int				m_iGameDataSampling;

	// Alive check 상수
	int				m_iAliveTime;

	// 케쉬 충전 시 실제 국가 금액과의 교환 비율
	float			m_fExchangeRate;

	// Account 전용 Sampling 상수
	int				m_iAccSampling;

	// 채널링 타입( 국내는 사용, 해외는 일단 Country code 로 통합 )
	const char		*m_szChanneling;

	bool			m_bWin;
	int				m_iKill;
	int				m_iDeath;

	// 각 모드 별 시작 시각, 종료 시각
	int				m_iBattleStartTime;
	int				m_iBattleEndTime;

	// 각 용병 별 플레이 시각, 종료 시각
	int				m_iHeroIndex;
	int				m_iHeroStartTime;
	int				m_iHeroEndTime;

	// 선택된 전투 타입 : 일반전투(BATTLE), 레더전투(LADDER), 진영전투(CAMP)
	std::string		m_szBattleType;

	// 선택된 전투 모드 : POWERSTONE, PRISONER, HIDENCROWN...
	std::string		m_szBattleMode;

	// 용병 교체 데이터를 전송하는 모드인지
	bool			m_bSendHeroChange;

	// GA Fail data, FTP Log 전송 여부 flag
	bool			m_bGAFailSend;

	// Tutorial 시작하는 유저인지 flag
	bool			m_bTutorialStart;

	// 선물 아이템 인덱스
	int				m_iGiftIndex;

	// 선물 아이템 케쉬 가격
	int				m_iGiftCash;

	// Subscription(청약철회) 용도 케릭터 인덱스
	int				m_iScripHeroIndex;

	// Subscription(청약철회) 용도 케릭터 시간값
	int				m_iScripHeroLimitDate;

	// 용병 구매 시 기간제 용병 시간값
	int				m_iHeroLimitDate;

	// Error code sub index
	int				m_iSubErrCode;
};

#define g_HttpMng ioHttpMng::GetSingleton()

#endif	// USE_GA

#endif	// __ioHttpMng_h__