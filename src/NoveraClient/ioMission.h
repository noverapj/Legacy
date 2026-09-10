#pragma once

enum MissionClasses
{
	MISSION_CLASS_NONE				= 0,
	MISSION_CLASS_LOGINTIME_CHECK,
	MISSION_CLASS_FISHING,
	MISSION_CLASS_PESO_COLLECT,
	MISSION_CLASS_MONSTER_KILL,
	MISSION_CLASS_ITEM_BUY,
	MISSION_CLASS_CLOVER,
	MISSION_CLASS_EXCAVATION,
	MISSION_CLASS_ITEM_USE,
	MISSION_CLASS_ITEMREINFORCE,
	MISSION_CLASS_MODEPLAY,
	MISSION_CLASS_DAILY_COMPLETE,		//주간미션. 일간미션을 몇개 했는지 체크
	MISSION_CLASS_DAILY_ALL_CLEAR,		//일간미션. 당일 일간미션을 올클리어하면 준다.
	MISSION_CLASS_WEEKLY_COMPLETE,		//월간미션. 주간미션을 몇해 했는지 체크
	MISSION_CLASS_WEEKLY_ALL_CLEAR,		// 주간 미션 : 주간미션을 올클리어하면 준다.
	MISSION_CLASS_MONTHLY_ALL_CLEAR,	// 월간 미션 : 월간미션을 올클리어하면 준다.
	MISSION_CLASS_ADDITIVE_USE,
};

enum MissionBackFrameType
{
	MBFT_NORMAL,
	MBFT_YELLOW,
};

enum MissionAttainShowStyle
{
	MASS_VALUE,	// CurValue / MaxValue Style
	MASS_GAUGE, // Gauege Style
};

//sort 우선순위
//1순위 : 일일,주간,월간 미션 올클리어 미션
//2순위 : 달성했지만 보상을 받지 않은 미션
//3순위 : 달성중인 미션
//4순위 : 달성해 보상을 받은 미션
enum MissionSortType
{
	MSORT_NONE,
	MSORT_ALL_CLEAR,
	MSORT_ATTAIN,
	MSORT_PROGRESS,
	MSORT_COMPLETE,
};

class ioMission
{
protected:
	struct ColorText
	{
		DWORD m_dwColor;
		ioHashString m_szText;

		ColorText()
		{
			m_dwColor = 0;
		}

		void Clear()
		{
			m_dwColor = 0;
			m_szText.Clear();
		}
	};
public:
	typedef std::vector< ColorText > vTitleLine;
	typedef std::vector< vTitleLine > vTitleText;
protected:
	DWORD					m_dwStartDay;
	// 서버와 연동되는 고유한 미션 코드
	int						m_iMagicCode;

	int						m_iMissionType;
	// 각각의 값은 미션의 타입마다 다르다.
	int						m_iValue1;
	int						m_iValue2;
	int						m_iValue3;
	int						m_iValue4;
	ioHashString			m_IconName;
	BYTE					m_byClearMissionType;
	int						m_iCurAchieveValue;
	int						m_iPresentNum;

	//UI
	MissionBackFrameType	m_BackFrameType;
	MissionAttainShowStyle	m_AttainShowStyle;
	ioHashStringVec			m_szRewardTextVec;
	vTitleText				m_vTitle;
	MissionSortType			m_MissionSortType;
	bool					m_bShowToolTipReward;
	bool					m_bIsCostume;

protected:
	void								LoadTitleInfo( const char* szINIKeyName, ioINILoader& kLoader );

	//Get Func
public:
	const int&							GetMagicCode()			const { return m_iMagicCode; }
	const int&							GetMissionType()		const { return m_iMissionType; }
	const ioHashString&					GetIconName()			const { return m_IconName; }
	const BYTE&							GetClearMissionType()	const { return m_byClearMissionType; }
	const int&							GetPresentNumber()		const { return m_iPresentNum; }
	const MissionBackFrameType&			GetBackFrameType()		const { return m_BackFrameType; }
	const MissionAttainShowStyle&		GetAttinShowStyle()		const { return m_AttainShowStyle; }
	const ioHashStringVec&				GetRewardTextVec()		const { return m_szRewardTextVec; }
	const int&							GetCurAchieveValue()	const { return m_iCurAchieveValue; }
	const vTitleText&					GetTitlePrinter()		const { return m_vTitle; }
	const MissionSortType&				GetMissionSortType()	const { return m_MissionSortType; }
	const bool&							GetShowToolTipReward()	const { return m_bShowToolTipReward; }
	const bool							IsMissionCostume()		const { return m_bIsCostume; }
	
	//Set Func
public:
	void SetMissionState( int iCurAchieveValue, BYTE byCurAchieveValue );
protected:
	void								SetCurAchieveValue( int iCurAchieveValue );
	void								SetClearMissionType( BYTE byCurAchieveValue );
	void								SetSortType();

//Virtual Func
public:
	//각 해당 클래스의 타입마다 달성치 Value가 다르기 때문에( 어떤것은 Value1이고 어떤것은 Value2이고)
	//순수 가상함수로 상속받는아서 결정한다.
	virtual const int&					GetMaxAchieveValue() const = 0;
	
	virtual void						Init( const char* szINIKeyName, ioINILoader &kLoader );
	virtual bool						Process();
	virtual ioMission*					Clone() = 0;
public:
	ioMission(void);
	ioMission( const ioMission &rhs );
	~ioMission(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioLoginTimeMission : public ioMission
{
protected:
	CTime				m_NowTime;
	int					m_iLoginwSecond;
	DWORD				m_dwProcessTime;
	int					m_iGapMinute;
public:
	virtual void		Init( const char* szINIKeyName, ioINILoader &kLoader );
	// 로그인 목표 접속 시간(분 단위)
	virtual const int&  GetMaxAchieveValue() const { return m_iValue1; }
	virtual bool		Process();
	virtual ioMission*	Clone();
public:
	ioLoginTimeMission(void);
	ioLoginTimeMission( const ioLoginTimeMission &rhs );
	virtual ~ioLoginTimeMission(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioFishingMission : public ioMission
{
public:
	// 낚시 전체/성공/실패 횟수
	virtual const int&  GetMaxAchieveValue() const { return m_iValue1; }
	virtual ioMission*	Clone();
public:
	ioFishingMission(void);
	ioFishingMission( const ioFishingMission &rhs );
	virtual ~ioFishingMission(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioPesoCollectMission : public ioMission
{
public:
	// 페소 획득
	virtual const int&  GetMaxAchieveValue() const { return m_iValue1; }
	virtual ioMission*	Clone();
public:
	ioPesoCollectMission(void);
	ioPesoCollectMission( const ioPesoCollectMission &rhs );
	virtual ~ioPesoCollectMission(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioMonsterKillMission : public ioMission
{
public:
	// NPC 사냥 ( 몬던 및 해골 ) 목표치
	virtual const int&  GetMaxAchieveValue() const { return m_iValue1; }
	virtual ioMission*	Clone();
public:
	ioMonsterKillMission(void);
	ioMonsterKillMission( const ioMonsterKillMission &rhs );
	virtual ~ioMonsterKillMission(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioItemBuyMission : public ioMission
{
public:
	// 아이템(장비보급/ETC) 구입/획득 목표 달성치
	virtual const int&  GetMaxAchieveValue() const { return m_iValue3; }
	virtual ioMission*	Clone();
public:
	ioItemBuyMission(void);
	ioItemBuyMission( const ioItemBuyMission &rhs );
	virtual ~ioItemBuyMission(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioCloverMission : public ioMission
{
public:
	//클로버 보내기/받기 횟수
	virtual const int&  GetMaxAchieveValue() const { return m_iValue1; }
	virtual ioMission*	Clone();
public:
	ioCloverMission(void);
	ioCloverMission( const ioCloverMission &rhs );
	virtual ~ioCloverMission(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioExcavationMission : public ioMission
{
public:
	//탐사 전체/성공/실패 횟수
	virtual const int&  GetMaxAchieveValue() const { return m_iValue1; }
	virtual ioMission*	Clone();
public:
	ioExcavationMission(void);
	ioExcavationMission( const ioExcavationMission &rhs );
	virtual ~ioExcavationMission(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioItemUseMission : public ioMission
{
public:
	//특정 아이템 사용/소모 ( 장비보급/ETC(가챠,수퍼가챠)/포션류 )
	virtual const int&  GetMaxAchieveValue() const { return m_iValue3; }
	virtual ioMission*	Clone();
public:
	ioItemUseMission(void);
	ioItemUseMission( const ioItemUseMission &rhs );
	virtual ~ioItemUseMission(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioItemReinforceMission : public ioMission
{
public:
	// 강화 전체/성공/실패 횟수
	virtual const int&  GetMaxAchieveValue() const { return m_iValue1; }
	virtual ioMission*	Clone();
public:
	ioItemReinforceMission(void);
	ioItemReinforceMission( const ioItemReinforceMission &rhs );
	virtual ~ioItemReinforceMission(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioModePlayMission : public ioMission
{
public:
	//특정 모드 플레이 횟수
	virtual const int&  GetMaxAchieveValue() const { return m_iValue1; }
	virtual ioMission*	Clone();
public:
	ioModePlayMission(void);
	ioModePlayMission( const ioModePlayMission &rhs );
	virtual ~ioModePlayMission(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioDailyCompleteMisson : public ioMission
{
public:
	//일일 미션 달성 체크
	virtual const int&  GetMaxAchieveValue() const { return m_iValue1; }
	virtual ioMission*	Clone();
public:
	ioDailyCompleteMisson(void);
	ioDailyCompleteMisson( const ioDailyCompleteMisson &rhs );
	virtual ~ioDailyCompleteMisson(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioDailyAllClearMisson : public ioMission
{
public:
	//당일 일일미션 모두 완료 갯수
	virtual const int&  GetMaxAchieveValue() const { return m_iValue1; }
	virtual ioMission*	Clone();
public:
	ioDailyAllClearMisson(void);
	ioDailyAllClearMisson( const ioDailyAllClearMisson	 &rhs );
	virtual ~ioDailyAllClearMisson(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioWeeklyCompleteMission : public ioMission
{
public:
	//주간 미션 달성 체크
	virtual const int&  GetMaxAchieveValue() const { return m_iValue1; }
	virtual ioMission*	Clone();
public:
	ioWeeklyCompleteMission(void);
	ioWeeklyCompleteMission( const ioWeeklyCompleteMission &rhs );
	virtual ~ioWeeklyCompleteMission(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioWeeklyAllClearMission : public ioMission
{
public:
	//해당 주간 미션 모두 완료 갯수
	virtual const int&  GetMaxAchieveValue() const { return m_iValue1; }
	virtual ioMission*	Clone();
public:
	ioWeeklyAllClearMission(void);
	ioWeeklyAllClearMission( const ioWeeklyAllClearMission &rhs );
	virtual ~ioWeeklyAllClearMission(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioMonthlyAllClearMission : public ioMission
{
public:
	//월간 미션 모두 완료 갯수
	virtual const int&  GetMaxAchieveValue() const { return m_iValue1; }
	virtual ioMission*	Clone();
public:
	ioMonthlyAllClearMission(void);
	ioMonthlyAllClearMission( const ioMonthlyAllClearMission &rhs );
	virtual ~ioMonthlyAllClearMission(void);
};

////////////////////////////////////////////////////////////////////////////////
class ioAdditiveUseMission : public ioMission
{
public:
	//차원 조각 사용
	virtual const int&  GetMaxAchieveValue() const { return m_iValue1; }
	virtual ioMission*	Clone();
public:
	ioAdditiveUseMission(void);
	ioAdditiveUseMission( const ioAdditiveUseMission &rhs );
	virtual ~ioAdditiveUseMission(void);
};

////////////////////////////////////////////////////////////////////////////////