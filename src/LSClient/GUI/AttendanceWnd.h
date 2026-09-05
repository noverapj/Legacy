#pragma once
#include "ioSprite.h"

class ioCalendarSet;

//////////////////////////////////////////////////////////////////////////

class ScaleAniTrack
{
public:
	enum ANISTATE
	{
		AS_NONE,
		AS_PLAY,
		AS_END,
	};

	enum
	{
		SCALE_ANI = 100,
	};

public:
	ANISTATE m_AniState;

	DWORD m_dwAniStartTime;
	float m_fScaleAniRate;
	float m_fAlphaAniRate;

public:
	void SetPlay();
	void SetEnd();
	void OnProcess();

public:
	ScaleAniTrack();
	~ScaleAniTrack(){};
};

class CalendarInfo
{
public:
	int m_iWeekOfDay;
	int m_iDay;
	int m_iXPos;
	int m_iYPos;
	int m_iPresentType;
	int m_iValue1;
	int m_iValue2;

	bool m_bHidden;
	bool m_bChecked;
	bool m_bReward;
	
	int m_iIconIndex;

	ScaleAniTrack m_AniTrack;

public:
	bool IsEmpty();

public:
	CalendarInfo();
	~CalendarInfo();
};

//////////////////////////////////////////////////////////////////////////

typedef struct tagCalendarToolTipInfo
{
	int m_iPresentType;
	int m_iValue1;
	int m_iValue2;
	bool m_bHidden;

	tagCalendarToolTipInfo()
	{
		Init();
	}

	void Init()
	{
		m_iPresentType = 0;
		m_iValue1	   = 0;
		m_iValue2      = 0;
		m_bHidden      = false;
	}

} CalendarToolTipInfo;


//////////////////////////////////////////////////////////////////////////

class AttendanceWnd : public ioWnd
{
public:
	enum
	{
		ID_ATTENDANCE   = 1,
		ID_EXIT			= 2,
		ID_TOOLTIP		= 3,
		ID_ATTENDANCED  = 4,
	};

	enum
	{
		ACCRUE_MAX  = 5,		
		WEEKS		= 7,
		MAX_WEEKS   = 6,
		MAX_DAY		= 31,

		WEEKS_X		   = 20,
		WEEKS_Y        = 112,
		WEEKS_X_OFFSET = 65,
		WEEKS_Y_OFFSET = 61,
	};

	enum
	{
		MAX_EFFECT = 10,
		SCALE_ANI  = 100,		
	};

	enum AniSeqState
	{
		ASS_NONE,
		ASS_CALENDAR,
		ASS_EFFECT,
		ASS_STAMP,
	};

protected:
	typedef std::map< DWORD, CalendarInfo> CalendarInfoMap;

protected:
	struct TodayAttendanceReward
	{
		int m_iPresentType;
		int m_iValue1;
		int m_iValue2;
		bool m_bHidden;

		TodayAttendanceReward()
		{
			m_iPresentType = 0;
			m_iValue1      = 0;
			m_iValue2      = 0;
			m_bHidden      = false;
		}

		const bool IsEmpty() const
		{
			if( m_iPresentType == 0 && m_iValue1 == 0 && m_iValue2 == 0 )
				return true;

			return false;
		}
	};
	typedef std::map< DWORD, TodayAttendanceReward> TodayAttendanceMap;

	struct AccrueAttendanceReward
	{
		int m_iAccureTerm;

		int m_iPresentType;
		int m_iValue1;
		int m_iValue2;

		int m_iIconIndex;
		bool m_bReward;

		AccrueAttendanceReward()
		{
			m_iAccureTerm   = 0;
			m_iPresentType  = 0;
			m_iValue1       = 0;
			m_iValue2       = 0;
			m_iIconIndex    = -1;
			m_bReward = false;
		}

		bool IsEmpty() const 
		{
			if( m_iPresentType == 0 && m_iValue1 == 0 && m_iValue2 == 0 )
				return true;

			return false;
		}
	};
	typedef std::map< DWORD, AccrueAttendanceReward> AccrueAttendanceMap;	

protected:
	typedef std::vector<ioUIRenderImage*> ioUIRenderImageList;

protected:
	ioUIRenderFrame *m_pOutLine;
	ioUIRenderFrame *m_pTodayOutLine;
	ioUIRenderImage *m_pAccrueTtitleBG;
	ioUIRenderImage *m_pCalendarItemIconBG;
	ioUIRenderImage *m_pItemIconBG;

	ioUIRenderImage *m_pHiddenIcon;
	ioUIRenderImage *m_pGetIcon;
	ioUIRenderImage *m_pAttendanceIcon;

	ioUIRenderImage* m_TodayRewardIcon;
	ioUIRenderImage* m_AccrueRewardIcon;
	ioUIRenderImage* m_pTodaySubIcon;
	ioUIRenderImage* m_pAccrueSubIcon;
	ioUIRenderImage* m_NoneIcon;
	ioUIRenderImage *m_pGetEffect[MAX_EFFECT];

	ioSprite m_TodayGetEffect;
	ioSprite m_AccrueGetEffect;
	ioUIRenderImageList m_vImageList;
	ioUIRenderImageList m_vSubImageList;

protected:
	CalendarInfoMap m_CalendarInfoMap;
	int m_iCalendarCurrFocusIndex;

protected:
	TodayAttendanceMap  m_TodayAttendanceMap;
	AccrueAttendanceMap m_AccrueAttendanceMap;

	ioHashString m_szTodayRewardText1;
	ioHashString m_szTodayRewardText2;

	ioHashString m_szAccrueRewardText1;
	ioHashString m_szAccrueRewardText2;
		
	bool m_bCurrTodayHidden;
	AniSeqState m_CurrTodayRewardState;

	bool m_bCurrAccureReward;
	AniSeqState m_CurrAccureRewardState;
	
	DWORD m_dwTodayRewardStartTime;
	DWORD m_dwAccrueRewardStartTime;

	int m_iCurrAccrueTerm;
	int m_iCurrAccrueMinTerm;
	int m_iCurrAccrueMaxTerm;

	ScaleAniTrack m_CurrAccureScaleAniTrack;
	ScaleAniTrack m_CurrTodayBigAniTrack;
	ScaleAniTrack m_CurrAccureBigAniTrack;

	int m_iCurYear;
	int m_iCurMonth;
	int m_iCurDay;

	ioHashString m_szGetSound;
	ioHashString m_szStampSound;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual bool iwm_spacebar();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	void LoadReward( int iYear, int iMonth );

	void LoadTodayAttance( ioINILoader& rkLoader, DWORD dwTtitle );
	void LoadAccrueAttance( ioINILoader& rkLoader, DWORD dwTtitle );

protected:
	CalendarInfo& FindCalendarInfo( DWORD dwKey );
	AttendanceWnd::TodayAttendanceReward& GetTodayPresentInfo( DWORD dwDateKey );
	AttendanceWnd::AccrueAttendanceReward& GetAccruePresentInfo( int iAccrueKey );
	
public:
	int GetPresentIcon( float fScale, int iPresentType, int iValue1, int iValue2 );
	int GetRenderImageStartHeight( ioUIRenderImage *pImage, int iSrcHeight, int iCur, int iMax );

	ioUIRenderImage* GetPresentIconImage( float fScale, int iPresentType, int iValue1, int iValue2 );
	ioUIRenderImage* GetPresentSubIconImage( float fScale, int iPresentType, int iValue1, int iValue2 );

	void GetEffectList( OUT ioUIRenderImageList &rkImageList );	
	void GetCurCalendarInfo( CalendarToolTipInfo& Info );

	void InitCalendar( int iCurrYear, int iCurrMonth, int iCurrDay );
	void InitCurrAccrueReward();
	void CheckAttendanceButton();

	void SetReward( int iTodayPresentType, int iTodayValue1, int iTodayValue2, int iAccruePresentType, int iAccrueValue1, int iAccrueValue2 );
	
public:
	void AddAttendanceRecord( DWORD dwKey );

public:
	void OnCalanderProcess();
	void OnAniProcess();
	void OnToolTipProcess();

public:
	void Destroy();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnTtitleRender( int iXPos, int iYPos );
	void OnCalanderRender( int iXPos, int iYPos );

	void OnAccrueTtitleRender( int iXPos, int iYPos );
	void OnAccrueRewardRender( int iXPos, int iYPos );

	void OnTodayAndAccrueRewardRender( int iXPos, int iYPos );
	void OnRenderGaugeIcon( int iXPos, int iYPos, ioUIRenderImage *pImage, int iSrcHeight, int iCur, int iMax, UIRenderType eType );

public:
	AttendanceWnd();
	virtual ~AttendanceWnd();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioAttendanceCheckButton : public ioButton
{
protected:
	bool m_bEnd;
	bool m_bOver;
	DWORD m_dwOverTimer;

protected:
	virtual void OnProcess( float fTimePerSec );	

public:
	virtual void OnRender();

public:
	void SetEnd();
	void SetStart();

public:
	ioAttendanceCheckButton();
	~ioAttendanceCheckButton();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioAttendanceCheckedButton : public ioButton
{
protected:
	bool m_bOver;
	DWORD m_dwOverTimer;

protected:
	virtual void OnProcess( float fTimePerSec );	

public:
	virtual void OnRender();

public:
	void SetStart();

public:
	ioAttendanceCheckedButton();
	~ioAttendanceCheckedButton();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CalendarToolTip : public ioWnd
{
public:
	enum
	{
		MIN_WIDTH		= 0,
		MIN_HEIGHT		= 26,
		TEXT_LINE_GAP   = 18,
		SIDE_TEXT_GAP	= 14,
	};

protected:
	int m_iPresentType;
	int m_iValue1;
	int m_iValue2;

	ioComplexStringPrinter m_szDescArray;

protected:
	void Clear();
	void ProcessPosition();

public:
	bool SetInfo( int iPresentType, int iValue1, int iValue2, bool bHidden );

public:
	virtual void iwm_show();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	CalendarToolTip();
	virtual ~CalendarToolTip();
};

