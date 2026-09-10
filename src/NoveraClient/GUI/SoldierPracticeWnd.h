#pragma once

#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioMovingWnd.h"

class ioUIRenderImage;
class ioUIRenderFrame;
class PracticeHelpWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_NEXT = 2,
		ID_CLOSE= 3,
		ID_PREV = 4,
	};

protected:
	ioUIRenderImage *m_pDevK;
	ioUIRenderImage *m_pDevKBack;

protected:
	struct PracticeHelp
	{
		ioHashString m_szTitle;
		ioHashString m_szHelp;
	};
	typedef std::vector< PracticeHelp > vPracticeHelp;
	vPracticeHelp m_vPracticeHelp;

protected:
	DWORD     m_dwTextTickTime;
	DWORD     m_dwCurTextTickTime;
	int       m_iCurTextTickCount;
	int       m_iCurTextLineCount;
	ioHashString m_szTextTickSnd;
	ioHashString m_szIoString;

	struct StringTick
	{
		char m_szCopyDesc[MAX_PATH];
		StringTick()
		{
			memset( m_szCopyDesc, 0, sizeof( m_szCopyDesc ) );
		}
	};
	typedef std::vector< StringTick > vStringTick;
	vStringTick m_vStringTickList;

protected:
	StringVector m_DescList;
	int m_iStep;
	int m_iClassType;
	int m_iSelectCharArray;
	ioPlayStage *m_pPlayStage;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual bool iwm_spacebar();

protected:
	void SplitDescription( const ioHashString &szDesc );
	void SetStep( int iStep );
	bool SetNextAction();

public:
	void SetPracticeHelp( int iClassType, int iSelectCharArray );
	void ClearSelectCharArray()	{ m_iSelectCharArray = -1; }
	void SetPlayStage( ioPlayStage *pPlayStage );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	void ProcessTextTick();
	void ProcessIoStringTick();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	PracticeHelpWnd();
	virtual ~PracticeHelpWnd();
};
//////////////////////////////////////////////////////////////////////////
struct SoldierGradeInfo
{
	int m_iClassType;
	int m_iGradeType;

	SoldierGradeInfo()
	{
		Init();
	}
	void Init()
	{
		m_iClassType = -1;
		m_iGradeType = 0;
	}
	int GetSortIndex();
};
typedef std::vector< SoldierGradeInfo > SoldierGradeInfoVec;

class SoldierPracticeSelectBtn : public ioButton
{
protected:
	ioUIRenderFrame *m_pBaseBG;
	ioUIRenderFrame *m_pNoneBG;
	ioUIRenderFrame *m_pBlankBG;
	ioUIRenderFrame *m_pStateBlueFrm;
	ioUIRenderFrame *m_pCustomOverAdd;

	ioUIRenderImage *m_pNone;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pSoldierIcon;
	ioUIRenderImage *m_pSoldierSubIcon;
	ioHashString m_szHaveText;
	ioHashString m_szClassName;

protected:
	bool m_bHaveClass;
	SoldierGradeInfo m_cInfo;

public:
	void SetClassType( SoldierGradeInfo cInfo );
	int  GetClassType(){ return m_cInfo.m_iClassType; }

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();
	
public:
	SoldierPracticeSelectBtn();
	virtual ~SoldierPracticeSelectBtn();
};

class SoldierPracticeSelectWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_LEFT_PAGE = 2,
		ID_RIGHT_PAGE= 3,

		ID_TAB1 = 11,
		ID_TAB6 = 16,

		ID_SOLDIER_BTN1 = 101,
		ID_SOLDIER_BTN18= 118,
	};
	enum
	{
		TAB_COUNT = 6,
		MAX_SOLDIER_BTN = 18,
		MAX_HORZ = 6,
		MAX_VERT = 3,
	};
	struct TabInfo
	{
		ioHashString m_szText;
		int m_iIndex;
	};
	typedef std::vector< TabInfo > TabInfoVec;

protected:
	ioPlayStage *m_pPlayStage;
	ioUIRenderImage *m_pTabLine;

	int m_iCurPage;
	int m_iMaxPage;
	int m_iCurTab;
	IntVec m_vExtraClassVec;
	TabInfoVec m_vTabInfoVec;
	SoldierGradeInfoVec m_CharList;

public:
	void SetPlayStage( ioPlayStage *pPlayStage );

protected:
	void SetBtnPos();
	void CreateList();
	void UpdateSetPage( int iCurPage );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_wheel( int zDelta );

protected:
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	bool QuestGuide( int iClassType, float &rfXPos, float &rfYPos );

public:
	SoldierPracticeSelectWnd();
	virtual ~SoldierPracticeSelectWnd();
};

class SoldierPracticeSort : public std::binary_function< SoldierGradeInfo, SoldierGradeInfo, bool >
{
public:
	bool operator()( SoldierGradeInfo lhs , SoldierGradeInfo rhs ) const
	{
		if( lhs.GetSortIndex() < rhs.GetSortIndex() )
			return true;
		if( lhs.GetSortIndex() == rhs.GetSortIndex() )
		{
			if( lhs.m_iClassType < rhs.m_iClassType )
				return true;
		}
		return false;
	}
};