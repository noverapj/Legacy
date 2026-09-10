#pragma once

#include "../io3DEngine/ioButton.h"
#include "../CharacterInfo.h"
#include "NewShopBuyWnd.h"

class FirstSoldierSelectBtn : public ioButton
{
protected:
	int m_iMagicCode;

public:
	inline int GetMagicCode() const { return m_iMagicCode; }
	void       SetMagicCode(int iMagicCode) { m_iMagicCode = iMagicCode; }
	
public:
	FirstSoldierSelectBtn();
	virtual ~FirstSoldierSelectBtn();
};

///////////////////////////////////////////////////////////////////////////
#define FSS_DECO_ICON_SCALE 0.60f
#define FSS_BTN_TITLE_SIZE  188.0f
#define FSS_CHAR_SCALE      1.20f

class FirstSoldierSelectWnd : public ioWnd
{
public:
	enum
	{
		ID_NEXT_BTN              = 1,

		ID_CHAR_SELECT_BTN	     = 101,
		ID_KINDRED_SELECT_BTN    = 102,
		ID_HAIR_SELECT_BTN	     = 103,
		ID_HAIR_COLOR_SELECT_BTN = 104,
		ID_FACE_SELECT_BTN       = 105,
		ID_SKIN_COLOR_SELECT_BTN = 106,
		ID_UNDERWEAR_SELECT_BTN  = 107,

		ID_SOLDIER_WND           = 1001,
		ID_PULLDOWN_WND          = 1002,

		MAX_SELECT_BTN           = 7,
	};

	enum FSSWndType
	{
		WT_START  = 1,
		WT_SELECT = 2,
		WT_END    = 3,
	};

	enum
	{
		MAX_DESC             = 37,
		MAX_START_DESC       = 16,
		START_FIRST_ARRAY    = 0,
		MAX_SELECT_DESC      = 5,
		SELECT_FIRST_ARRAY   = 16,
		MAX_END_DESC         = 8,
		END_FIRST_ARRAY      = 21,
		START_TITLE_ARRAY    = 29,
		SELECT_TITLE_ARRAY   = 30,
		END_TITLE_ARRAY      = 31,
		START_SELECT_BTN_ARRAY = 32,
		END_BTN_ARRAY          = 33,
		SELECT_MENT1_ARRAY     = 34,
		SELECT_MENT2_ARRAY     = 35,
		SELECT_CHECK_ARRAY     = 36,
	};

	enum
	{
		MAX_DECO              = 6,
		DECO_KINDRED_ARRAY    = 0,
		DECO_HAIR_ARRAY       = 1,
		DECO_HAIR_COLOR_ARRAY = 2,
		DECO_FACE_ARRAY       = 3,
		DECO_SKIN_COLOR_ARRAY = 4,
		DECO_UNDERWEAR_ARRAY  = 5,
	};

protected:
	struct DecoVectorInfo
	{
		int       m_iClassType; 
		int       m_iSexType;   // RDT_HUMAN_MAN, RDT_WOHUMAN_MAN
		int       m_iDecoType;  // UID_HAIR..
		IntVec    m_vDecoCode; 

		DecoVectorInfo()
		{
			m_iSexType    = 0; //RDT_HUMAN_MAN;
			m_iDecoType   = 2; //UID_HAIR;
			m_iClassType  = -1;
			m_vDecoCode.clear();
		}
	};
	typedef std::vector<DecoVectorInfo*> vDecoVectorInfo;

protected:
	FSSWndType      m_eWndType;
	
	ioUIRenderFrame *m_pBottomFrm;
	ioUIRenderImage *m_pStripGrayDark;
	ioUIRenderImage *m_pStripGrayLight;
	ioUIRenderImage *m_pStartImg;
	ioUIRenderImage *m_pEndTextImg;
	ioUIRenderImage *m_pEndImg;

	ioUIRenderImage *m_pDecoBack;
	ioUIRenderImage *m_pDeco[MAX_DECO];
	ioUIRenderImage *m_pDot;

	ioHashStringVec m_vDesc;
	vDecoVectorInfo m_vDecoVectorInfo;

	CHARACTER        m_CharInfo;
	CHARACTER        m_OverCharInfo;

protected:
	void ChangeWnd( FSSWndType eWndType );

	void OnRenderStart();
	void OnRenderSelect();
	void OnRenderSelectAfterChild();
	void OnRenderEnd();

	void SetChildTitleText( DWORD dwID, const char *szTitle );
	void SetCharInfo( OUT CHARACTER &rkCharInfo, IN DWORD dwID, IN int iMagicCode );
	void SetAllDefaultTitleAndCharInfo( OUT CHARACTER &rkCharInfo, IN int iClassType, IN int iSexType, IN bool bChangeOnlyCharInfo );

	void SetDecoImg( DWORD dwID, int iSexType, int iDecoCode );
	void SetChildMagicCode( DWORD dwID , int iMagicCode );
	void SetAllDefaultTitleAndCharInfoByCharKindred( OUT CHARACTER &rkCharInfo, IN DWORD dwID,IN int iMagicCode, IN bool bChangeOnlyCharInfo = false );
	void SetTitleAndCharInfo( OUT CHARACTER &rkCharInfo, IN DWORD dwID, IN int iMagicCode );
	void SetNextWnd();

	void SetPullDownMenu( ioWnd *pOpenBtn );
	void SetPullDownItemList( IN DWORD dwID, OUT vNewShopPullDownItem &rkList );

	int  GetDecoType( DWORD dwID );
	int  GetFirstClassType();
	int  GetFirstDecoCode( int iClassType, int iSexType, int iDecoType );
	void GetMagicCodeVector( OUT IntVec &rvMagicCode, IN DWORD dwID, IN int iClassType, int iSexType );
	void GetClassTypeVector( OUT IntVec &rvClassType );
	void GetDecoCodeVector( OUT IntVec &rvDecoCode, IN int iClassType, IN int iSexType, IN int iDecoType );
	void GetMagicName( OUT ioHashString &rsName, IN DWORD dwID, IN int iSexType, IN int iMagicCode );
	int  GetDecoImgArray( DWORD dwID );

	void PullDownBtnOver( ioWnd *pOpenBtn, int iListArray );
	void PullDownBtnUP( ioWnd *pOpenBtn, int iListArray );
	
	void SendFirstChar();

	void LoadINI();

	void Clear();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void OnRenderAfterChild();

public:
	FirstSoldierSelectWnd(void);
	virtual ~FirstSoldierSelectWnd(void);
};

