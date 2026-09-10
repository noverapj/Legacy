#pragma once

#include "../io3DEngine/ioMovingWnd.h"

#include "FSMStateMgr.h"

class io2DCircleGauge;
class ioPlayStage;
class ioUIRenderImage;
class ioUIRenderFrame;
class ioUI3DRender;
class ioCreateChar;
class ioBaseChar;
class ioINILoader;
struct CHARACTER;


// 모든 유저 정보
struct BattleModeUserInfo
{
	TeamType m_Team;
	ioHashString m_NickName;
	int m_iGrade;
	ioUIRenderImage *m_pUserSlotBg;
	ioUIRenderImage *m_pUserHpBg;
	ioUIRenderImage *m_pUserHp;
	ioUIRenderImage *m_pUserSoldierSlot;
	ioUIRenderImage *m_pUserEntryGaugeBg;
	ioUIRenderImage *m_pUserEntryGauge;

	// 태그 요청, 난입 알람 이미지
	ioUIRenderImage *m_pAlarmIcon;
	ioUIRenderImage *m_pAlarmText;

	ioUIRenderImage *m_pHP_Text;

	ioUIRenderImage* m_pSoldierIcon;
	ioUIRenderImage *m_pSoldierIconBack;
	ioUIRenderImage* m_pTagReq;
	ioUIRenderImage* m_pIntrude;
	bool m_bTagReq;
	bool m_bIntrude;
	//bool m_bIntrude; 출전 유무 필요한가?

	int m_iWaitPosIdx;

	BattleModeUserInfo()
	{
		m_Team = TEAM_NONE;
		m_NickName.Clear();
		m_iGrade = 0;
		m_pUserSlotBg		= NULL;
		m_pUserHpBg		= NULL;
		m_pUserHp		= NULL;
		m_pUserSoldierSlot = NULL;
		m_pUserEntryGaugeBg = NULL;
		m_pUserEntryGauge = NULL;

		m_pAlarmIcon = NULL;
		m_pAlarmText = NULL;

		m_pHP_Text = NULL;

		m_pSoldierIcon		= NULL;
		m_pSoldierIconBack = NULL;
		m_pTagReq		= NULL;
		m_pIntrude		= NULL;
		m_bTagReq		= false;
		m_bIntrude		= false;
		m_iWaitPosIdx	= 0;
	}
	~BattleModeUserInfo()
	{
		m_NickName.Clear();
		SAFEDELETE( m_pUserSlotBg );
		SAFEDELETE( m_pUserHpBg );
		SAFEDELETE( m_pUserHp );
		SAFEDELETE( m_pUserSoldierSlot );
		SAFEDELETE( m_pUserEntryGaugeBg );
		SAFEDELETE( m_pUserEntryGauge );

		SAFEDELETE( m_pAlarmIcon );
		SAFEDELETE( m_pAlarmText );

		SAFEDELETE( m_pHP_Text );

		SAFEDELETE( m_pSoldierIcon );
		SAFEDELETE( m_pSoldierIconBack );
		SAFEDELETE( m_pTagReq );
		SAFEDELETE( m_pIntrude );
	}
};

class BattleMode3DChar
{
public:
	enum
	{
		VIEW_WIDTH = 350,
		VIEW_HEIGHT= 210,
	};

private:
	ioUI3DRender *m_pUI3DRender;
	ioCreateChar *m_pAvata;

protected:
	int m_iOffsetXPos;
	int m_iOffsetYPos;

public:
	void Initialize();
	void SetChar( ioBaseChar *pChar, bool bLeftChar );
	void FaceChange( const char *szTexPostfix, int iDuration );
	void SetHelmChange( ioBaseChar *pChar );

	void Update( float fTimePerSec );
	void Render( int iXPos, int iYPos );

public:
	BattleMode3DChar();
	~BattleMode3DChar();
};

class BattleModeTimeGaugeWnd : public ioMovingWnd
{

public:
	enum
	{
		MAX_FIGHT_NPC_WIN_COUNT = 5,
	};

public:
	enum ShowAniType 
	{
		SAT_NONE         = 0,
		SAT_START_ROUND  = 1,
		SAT_DELAY        = 2,
		SAT_UP           = 3,  // 가속
		SAT_DOWN         = 4,  // 감속
		SAT_UP_STOP      = 5,  // 가속
		SAT_STOP         = 6,
		SAT_END          = 7,
	};

	enum WarningAniType
	{
		WAT_NONE        = 0,
		WAT_HIDING      = 1,
		WAT_DONT_RENDER = 2,
		WAT_RETIMER		= 4,
	};

	enum ScoreChangeType
	{
		SCT_NONE      = 0,
		SCT_WHITE     = 1,
		SCT_REDUCTION = 2,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

	enum 
	{
		TIME_GAUGE_POS_X = 200,
	};
	
protected:
	// 상단 중앙 시간 원형 게이지
	io2DCircleGauge *m_pCircleGauge;
	io2DCircleGauge *m_pCircleSuddenDeathGauge;

	ioUIRenderImage *m_pTimeIcon;
	ioUIRenderImage *m_pTimeNumber;

	ioUIRenderImage *m_pBack;
	ioUIRenderImage *m_pMold;
	ioUIRenderImage *m_pWarningBack;
	/////////////////////////////////////

	ioUIRenderImage *m_pTimeText;
	ioUIRenderImage *m_pTimeTextRed;
	ioUIRenderImage *m_pTimeNumText;
	ioUIRenderImage *m_pTimeNumTextRed;
	ioUIRenderImage *m_pTimeBack;
	ioUIRenderImage *m_pTimeBackRed;

	ioUIRenderImage *m_pPowerBackEffectL;
	ioUIRenderImage *m_pPowerBackGaugeL;
	ioUIRenderImage *m_pPowerGaugeL;
	ioUIRenderImage *m_pPowerGaugeEffectL;
	ioUIRenderImage *m_pPowerGaugeMaxL;
	ioUIRenderImage *m_pPowerBackEffectR;
	ioUIRenderImage *m_pPowerBackGaugeR;
	ioUIRenderImage *m_pPowerGaugeR;
	ioUIRenderImage *m_pPowerGaugeEffectR;
	ioUIRenderImage *m_pPowerGaugeMaxR;

	ioUIRenderImage *m_pBlueTeamText;
	ioUIRenderImage *m_pLeftBackLine;
	ioUIRenderImage *m_pLeftBackLineLight;
	ioUIRenderImage *m_pLeftGaugeBack;
	ioUIRenderImage *m_pLeftGauge100;
	ioUIRenderImage *m_pLeftGaugeGlow;
	ioUIRenderImage *m_pLeftGaugeRed;
	ioUIRenderImage *m_pLeftBlueNameBar;
	ioUIRenderImage *m_pLeftRedNameBar;

	ioUIRenderImage *m_pRedTeamText;
	ioUIRenderImage *m_pRightBackLine;
	ioUIRenderImage *m_pRightBackLineLight;
	ioUIRenderImage *m_pRightGaugeBack;
	ioUIRenderImage *m_pRightGauge100;
	ioUIRenderImage *m_pRightGaugeGlow;
	ioUIRenderImage *m_pRightGaugeRed;
	ioUIRenderImage *m_pRightBlueNameBar;
	ioUIRenderImage *m_pRightRedNameBar;

	ioUIRenderImage *m_pScoreColon;
	ioUIRenderImage *m_pBlueScoreNum;
	ioUIRenderImage *m_pRedScoreNum;
	//ioUIRenderImage *m_pOwnerReserveChar;
	//ioUIRenderImage *m_pOwnerReserveSubChar;

	ioUIRenderImage *m_pCheerBack;
	ioUIRenderImage *m_pChampCheerPoint;
	ioUIRenderImage *m_pChampCheerNumber;
	ioUIRenderImage *m_pChampCheerText;
	ioUIRenderImage *m_pChallCheerPoint;
	ioUIRenderImage *m_pChallCheerNumber;
	ioUIRenderImage *m_pChallCheerText;
	ioUIRenderImage *m_pCheerTimeBack;
	
	io2DCircleGauge *m_pCircleGaugeNor;
	io2DCircleGauge *m_pCircleGaugeDan;

	ioUIRenderImage *m_pAcceptBack;
	ioUIRenderImage *m_pAcceptPoint;

	ioUIRenderImage *m_pBattleWin;

	typedef std::map< DWORD, ioUIRenderImage *> ComGradeMap;
	ComGradeMap m_ComGradeList;

	int m_iBlindOffSetY;
	int m_iPreBlindOffSetY;

	FSMStateMgr m_ShowAniState;
	
	int m_iDefaultX;
	int m_iDefaultY;
	
	static bool m_bPosToDefualt;

	ioPlayStage *m_pPlayStage;

	// 스코어 값
	int m_iBlueTeamScore;
	int m_iRedTeamScore;

protected:
	int		m_iRemainSecond;
	bool	m_bTimeAlert;
	DWORD   m_dwTimeAlertTime;
	float   m_fAlphaValue;

protected:
	struct CharSeqData
	{
		ioHashString m_kName;
		int          m_iSeq;
		int          m_iLevel;
		int          m_iCheer;

		CharSeqData()
		{
			m_iSeq = m_iLevel = m_iCheer = 0;
		}
	};
	typedef std::vector< CharSeqData > CharSeqDataVec;
	class CharSeqDataSort : public std::binary_function< const CharSeqData&, const CharSeqData&, bool >
	{
	public:
		bool operator()( const CharSeqData &lhs , const CharSeqData &rhs ) const
		{
			if( lhs.m_iSeq < rhs.m_iSeq )
			{
				return true;
			}	
			return false;
		}
	};
	CharSeqDataVec m_CharSeqData;

protected:
	ioHashString    m_szLeftUserName;
	float			m_fLeftCurrentHPRate;
	float           m_fLeftUserHPRate;
	int             m_iLeftUserGrade;
	bool            m_bLeftGaugeStart;
	DWORD           m_dwLfetGaugeTime;
	BattleMode3DChar *m_pLeftCharIcon;

protected:
	ioHashString    m_szRightUserName;
	ioHashString    m_szRightUserViewName;

	float			m_fRightCurrentHPRate;
	float           m_fRightUserHPRate;
	int             m_iRightUserGrade;
	bool            m_bRightGaugeStart;
	DWORD           m_dwRightGaugeTime;
	BattleMode3DChar *m_pRightCharIcon;

protected:
	int             m_iChampionWinCount;
	float           m_fChampionWinEffectRate;
	DWORD           m_dwChampionWinEffectTime;

protected:
	int             m_iOwnerReserveCharType;

protected:
	ioHashString    m_MouseOverName;
	bool            m_bMouseLBtnDown;

protected:          // 파워!!
	struct PowerGaugeData
	{
		float m_fLimitValue;
		int   m_iPointValue;
	};
	typedef std::vector< PowerGaugeData > PowerGaugeDataVec;
	PowerGaugeDataVec m_TimePowerData;
	PowerGaugeDataVec m_HealthPowerData;
	int               m_iMaxPowerValue;
	
	DWORD             m_dwLeftPowerGaugeTime;
	float             m_fLeftPowerRate;
	float             m_fLeftPowerCurrentRate;

	DWORD             m_dwRightPowerGaugeTime;
	float             m_fRightPowerRate;
	float             m_fRightPowerCurrentRate;

	DWORD             m_dwPowerGaugeTime;
	float             m_fPowerGaugeRate;
	bool			  m_bPowerGaugeRateReverse;

protected:           // 종료
	DWORD             m_dwWarningSecond;
	ioHashString      m_szWarningTickSnd;

protected:
	int               m_iCheerRemainTime;
	int               m_iCheerFullTime;
	int               m_iCheerLimitTime;
	DWORD             m_dwCheerSendTime;
	bool              m_bCheerPossible;

protected:
	DWORD			  m_dwCheerTabID;

// 
protected:
	int m_iCurFightNPCStage;

	enum AcceptPintState
	{
		APS_RIGHT,
		APS_LEFT,
	};

	int m_iAcceptX;

	AcceptPintState m_AcceptPointState;
	DWORD m_dwAcceptPointTime;
	DWORD m_dwAcceptPointCheckTime;

	// 제한 시간
	int m_iFullSecond;
	//int m_iRemainSecond;

	//Sudden Death
	bool m_bSuddenDeadth;

	int m_iSuddnDeathFullTime;
	int m_iSuddnDeathRemainTime;

	FSMStateMgr m_WarningAniState;
	FSMStateMgr m_ScoreChangeState;
	FSMStateMgr m_SuddenDeathAniState;

protected:
	void InitShowAniState();
	void InitWarningAniState();
	void UpdateAniState();
	bool UpdateShowAni();
	void UpdateDefaultPos();	

protected:
	ioUIRenderImage *GetComGradeRes( int iGrade );
	
public:
	void SetBlindOffSet( int iYOffSet );
	void ChangeShowAniState( ShowAniType eShowAniType );
	int  GetCurShowAniState();

	void InitAniState();

	int GetMainDefaultXPos() { return m_iDefXPos; }
	int GetMainDefaultYPos() { return m_iDefYPos; }
	
	DWORD GetCheerTabID()	{ return m_dwCheerTabID; }

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

protected:
	void OnTimeProcess();
	void OnLeftHealthProcess( float fTimePerSec );
	void OnRightHealthProcess( float fTimePerSec );
	void OnLeftPowerProcess( float fTimePerSec );
	void OnRightPowerProcess( float fTimePerSec );
	void OnPowerGaugeProcess( float fTimePerSec );
	void OnMouseProcess();
	bool IsMouseCheck( RECT &rkRect, ioHashString &rkName, const ioHashString &rkPrevName, const bool &rkPrevBtnDown );

protected:
	void OnTimeRender( int iXPos, int iYPos );
	void OnRenderBlueTeamParticipant( int iXPos, int iYPos );				// 상단에 출전하는 사람 정보 표시 (블루팀)
	void OnRenderRedTeamParticipant( int iXPos, int iYPos );		// 상단에 출전하는 사람 정보 표시 (레드팀)
	void OnRenderScore( int iXPos, int iYPos );
	void OnSeqUserListRenter( int iXPos, int iYPos );
	void OnPowerGaugeLeftRender( int iXPos, int iYPos );
	void OnPowerGaugeRightRender( int iXPos, int iYPos );
	
	void RenderTimeGauge( int iXPos, int iYPos );

public:   // 룸생성 후 들어오는 유저들에 대한 순서
	void ClearCharSeqInfo();	
	void RemoveCharSeqInfo( const ioHashString &rkName );

public:
	void SetPlayStage( ioPlayStage *pPlayStage );
	void SetChampionUser( ioBaseChar *pChar );
	void SetBattleModeUser( const ioHashString &rkName, TeamType eTeam );
	void SetFaceChange( const ioHashString &rkName, const char *szTexPostfix, int iDuration );
	void SetCharChange( ioBaseChar *pChar );
	void SetHelmChange( ioBaseChar *pChar );
	void UpdateTime( int iRemainTime, int iMaxTime );
	void UpdateSuddenDeathTime( DWORD dwRemainTime, DWORD dwFullTime );
	void UpdateGauge();

public:	
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	static bool GetPosToDefualt() { return m_bPosToDefualt; }
	static void SetPosToDefualt(bool PosToDefualt) { m_bPosToDefualt = PosToDefualt; }

public:
	const ioHashString &GetChampionUserName(){ return m_szLeftUserName; }
	const ioHashString &GetChallengerUserName(){ return m_szRightUserName; }

public:
	int GetMeSeqCount();
	int GetMeCheerType();

// 

protected:
	void OnProcessFightNPC();

	void OnChallengerNPCRender( int iXPos, int iYPos );
	void OnFightNPCRender( int iXPos, int iYPos );
	
public:
	BattleModeTimeGaugeWnd();
	virtual ~BattleModeTimeGaugeWnd();
};
//////////////////////////////////////////////////////////////////////////
class BattleModeFunctionWnd : public ioWnd
{
public:
	enum
	{	
		ID_INTRUDE		= 1,	// 배틀모드 대기자 기능 1 - 난입
		ID_CHAR_CHANGE	= 2,	// 배틀모드 대기자 기능 2 - 용병 변경
		ID_TAG_REQUEST	= 3,	// 배틀모드 대기자 기능 3 - 태그 요청
		ID_TAG_CANCEL	= 4,	// 배틀모드 대기자 기능 4 - 태그 취소
	};

	
protected:
	ioUIRenderFrame *m_pBackFrm;
	ioUIRenderImage *m_pBackFrmLine;

	ioPlayStage* m_pPlayStage;

protected:
	// 태그, 난입, 용병 변경 버튼 Show / Hide 시키는 버튼
	bool m_bShowFunctionButton;
	DWORD m_dwTagBtnDisableTime;

	bool m_bTag;						// 태그 요청 On / Off
public:	// sound
	//ioHashString m_szChallengerSnd;

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	void InitValue();

public:	
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	//void OnProcessChallenger();

protected:
	virtual void OnRender();

	void OnRenderChallenger( int iXPos, int iYPos );

	void SetFunctionButtonTitle();
	
public:
	void SetPlayStage( ioPlayStage *pPlayStage );
	void SetChallenger();
	void ShowFunctionButton( bool bShow );
	void ShowTagRequestButton( bool bShow );
	void ActiveTagBtn();

	void ResetEntryButtonTitle( int iEntryCount );

	DWORD GetTagBtnDisableDuration() { return m_dwTagBtnDisableTime; }

	// 버튼 이벤트
	void SwitchingTag( bool bFuncKey );
	void SelectSoldier();
	void Entry();
public:
	BattleModeFunctionWnd();
	virtual ~BattleModeFunctionWnd();
};
//////////////////////////////////////////////////////////////////////////
class BattleModeBlueSideUserInfoWnd : public ioWnd
{
public:

protected:

	enum ALARM_TYPE
	{
		NONE_ALARM		= 0,
		TAG_ALARM		= 1,
		ENTRY_ALARM		= 2
	};

	///////////
	// 모든 유저 정보
	//struct BattleModeUserInfo
	//{
	//	TeamType m_Team;
	//	ioHashString m_NickName;
	//	int m_iGrade;
	//	ioUIRenderImage *m_pUserSlotBg;
	//	ioUIRenderImage *m_pUserHpBg;
	//	ioUIRenderImage *m_pUserHp;
	//	ioUIRenderImage *m_pUserSoldierSlot;
	//	ioUIRenderImage *m_pUserEntryGaugeBg;
	//	ioUIRenderImage *m_pUserEntryGauge;

	//	// 태그 요청, 난입 알람 이미지
	//	ioUIRenderImage *m_pAlarm;

	//	ioUIRenderImage* m_pSoldierIcon;
	//	ioUIRenderImage *m_pSoldierIconBack;
	//	ioUIRenderImage* m_pTagReq;
	//	ioUIRenderImage* m_pIntrude;
	//	bool m_bTagReq;
	//	bool m_bIntrude;
	//	//bool m_bIntrude; 출전 유무 필요한가?
	//	int m_iWaitPosIdx;

	//	BattleModeUserInfo()
	//	{
	//		m_Team = TEAM_NONE;
	//		m_NickName.Clear();
	//		m_iGrade = 0;
	//		m_pUserSlotBg		= NULL;
	//		m_pUserHpBg		= NULL;
	//		m_pUserHp		= NULL;
	//		m_pUserSoldierSlot = NULL;
	//		m_pUserEntryGaugeBg = NULL;
	//		m_pUserEntryGauge = NULL;

	//		m_pSoldierIcon		= NULL;
	//		m_pSoldierIconBack = NULL;
	//		m_pTagReq		= NULL;
	//		m_pIntrude		= NULL;
	//		m_bTagReq		= false;
	//		m_bIntrude		= false;
	//		m_iWaitPosIdx	= 0;
	//	}
	//	~BattleModeUserInfo()
	//	{
	//		m_NickName.Clear();
	//		SAFEDELETE( m_pUserSlotBg );
	//		SAFEDELETE( m_pUserHpBg );
	//		SAFEDELETE( m_pUserHp );
	//		SAFEDELETE( m_pUserSoldierSlot );
	//		SAFEDELETE( m_pUserEntryGaugeBg );
	//		SAFEDELETE( m_pUserEntryGauge );

	//		SAFEDELETE( m_pSoldierIcon );
	//		SAFEDELETE( m_pSoldierIconBack );
	//		SAFEDELETE( m_pTagReq );
	//		SAFEDELETE( m_pIntrude );
	//	}
	//};
	typedef std::vector< BattleModeUserInfo* > vBattleModeUserInfo;
	vBattleModeUserInfo m_vBattleModeUserInfo;
	///////////

	// xml 로딩 시 이미지 저장
	ioUIRenderImage *m_pSaveUserSlotBg;
	ioUIRenderImage *m_pSaveUserHpBg;
	ioUIRenderImage *m_pSaveUserHp;
	ioUIRenderImage *m_pSaveUserSoldierSlot;
	ioUIRenderImage *m_pSaveUserEntryGaugeBG;
	ioUIRenderImage *m_pSaveUserEntryGauge;

	ioUIRenderImage *m_pSaveTagState_Icon;
	ioUIRenderImage *m_pSaveTagState_Text;
	ioUIRenderImage *m_pSaveEntryState_Icon;
	ioUIRenderImage *m_pSaveEntryState_Text;

	ioUIRenderImage *m_pSaveHP_Text;

	//ioUIRenderImage* m_pTagSoldierIcon;
	//ioUIRenderImage* m_pTagSoldierIcon;
	ioUIRenderImage* m_pTagBG;
	ioUIRenderImage* m_pEntryBG;

	struct AlarmOrder
	{
		DWORD m_dwEndTime;
		ALARM_TYPE m_eAlarmType;			// 알람 타입 - 태그 : 1, 난입 : 2
		ioHashString m_NickName;

		AlarmOrder()
		{
			m_dwEndTime		= 0;
			m_eAlarmType	= NONE_ALARM;
			m_NickName.Clear();
		}
		~AlarmOrder()
		{
			m_dwEndTime		= 0;
			m_eAlarmType	= NONE_ALARM;
			m_NickName.Clear();
		}
	};

	typedef std::list< AlarmOrder > AlarmOrderList;			// 태그, 난입 알람 순서
	AlarmOrderList m_AlarmOrderList;
protected:
	ioPlayStage *m_pPlayStage;
public:
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	void ShowTag( ioHashString& UserName );
	void ShowEntry( ioHashString& UserName );

protected:
	void InitValue();

protected:
	virtual void OnProcess( float fTimePerSec );
	void OnProcessContinue();

protected:
	virtual void OnRender();
	void OnRenderUserInfo( int iXPos, int iYPos );
	void OnRenderAlarm( int iXPos, int iYPos );

public:
	// iWaitPosIdx 값 오름차순으로 m_vBattleModeUserInfo 정렬
	void SetBattleModeUser( ioBaseChar* pChar, int iWaitPosIdx );
	void SetPlayStage( ioPlayStage *pPlayStage ) { m_pPlayStage = pPlayStage; }
	void ChangeSoldierIcon( ioBaseChar* pChar );
	void SwapUserWaitPosIdx( ioBaseChar* pChar_IN, int iWaitPosIdx_IN, ioBaseChar* pChar_OUT, int iWaitPosIdx_OUT );
public:
	BattleModeBlueSideUserInfoWnd();
	virtual ~BattleModeBlueSideUserInfoWnd();
};

class BattleModeRedSideUserInfoWnd : public ioWnd
{
public:
	
protected:
	
	enum ALARM_TYPE
	{
		NONE_ALARM		= 0,
		TAG_ALARM		= 1,
		ENTRY_ALARM		= 2
	};

	///////////
	
	typedef std::vector< BattleModeUserInfo* > vBattleModeUserInfo;
	vBattleModeUserInfo m_vBattleModeUserInfo;
	///////////

	// xml 로딩 시 이미지 저장
	ioUIRenderImage *m_pSaveUserSlotBg;
	ioUIRenderImage *m_pSaveUserHpBg;
	ioUIRenderImage *m_pSaveUserHp;
	ioUIRenderImage *m_pSaveUserSoldierSlot;
	ioUIRenderImage *m_pSaveUserEntryGaugeBG;
	ioUIRenderImage *m_pSaveUserEntryGauge;

	ioUIRenderImage *m_pSaveTagState_Icon;
	ioUIRenderImage *m_pSaveTagState_Text;
	ioUIRenderImage *m_pSaveEntryState_Icon;
	ioUIRenderImage *m_pSaveEntryState_Text;

	ioUIRenderImage *m_pSaveHP_Text;

	//ioUIRenderImage* m_pTagSoldierIcon;
	//ioUIRenderImage* m_pTagSoldierIcon;
	ioUIRenderImage* m_pTagBG;
	ioUIRenderImage* m_pEntryBG;

	struct AlarmOrder
	{
		DWORD m_dwEndTime;
		ALARM_TYPE m_eAlarmType;			// 알람 타입 - 태그 : 1, 난입 : 2
		ioHashString m_NickName;

		AlarmOrder()
		{
			m_dwEndTime		= 0;
			m_eAlarmType	= NONE_ALARM;
			m_NickName.Clear();
		}
		~AlarmOrder()
		{
			m_dwEndTime		= 0;
			m_eAlarmType	= NONE_ALARM;
			m_NickName.Clear();
		}
	};

	typedef std::list< AlarmOrder > AlarmOrderList;			// 태그, 난입 알람 순서
	AlarmOrderList m_AlarmOrderList;
protected:
	ioPlayStage *m_pPlayStage;

public:
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	void ShowTag( ioHashString& UserName );
	void ShowEntry( ioHashString& UserName );

protected:
	void InitValue();

protected:
	virtual void OnProcess( float fTimePerSec );
	void OnProcessContinue();

protected:
	virtual void OnRender();

	void OnRenderUserInfo( int iXPos, int iYPos );

	void OnRenderAlarm( int iXPos, int iYPos );

public:
	// iWaitPosIdx 값 오름차순으로 m_vBattleModeUserInfo 정렬
	void SetBattleModeUser( ioBaseChar* pChar, int iWaitPosIdx );
	void SetPlayStage( ioPlayStage *pPlayStage ) { m_pPlayStage = pPlayStage; }
	void ChangeSoldierIcon( ioBaseChar* pChar );
	void SwapUserWaitPosIdx( ioBaseChar* pChar_IN, int iWaitPosIdx_IN, ioBaseChar* pChar_OUT, int iWaitPosIdx_OUT );
public:
	BattleModeRedSideUserInfoWnd();
	virtual ~BattleModeRedSideUserInfoWnd();
};

// m_iWaitPosIdx 오름차순 정렬
class WaitPosIdxSort : public std::binary_function< const BattleModeUserInfo* , const BattleModeUserInfo*, bool >
{
public:
	bool operator() ( const BattleModeUserInfo* lhs, const BattleModeUserInfo* rhs ) const
	{
		return lhs->m_iWaitPosIdx < rhs->m_iWaitPosIdx;
	}
};