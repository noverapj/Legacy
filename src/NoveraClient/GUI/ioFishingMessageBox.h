#pragma once


#include "../ioComplexStringPrinter.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioButton.h"
#include "../io3DEngine/ioMovingWnd.h"

#include "ioMessageBox.h"

class FishingAutoSellQuestion : public ioWnd
{
public:
	enum
	{
		ID_EXIT    = 1,
		ID_OK      = 2,
		ID_CANCEL  = 3,

		ID_QUESTION_EDIT = 100,
	};

protected:
	ioWnd *m_pPreEdit;
	ioWnd *m_pOwnerWnd;

	int m_iFirstNumber;
	int m_iSecondNumber;

protected:
	void CheckQuestion();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void SetOwnerWnd( ioWnd *pWnd );

protected:
	virtual void OnRender();	

public:
	FishingAutoSellQuestion();
	virtual ~FishingAutoSellQuestion();
};
////////////////////////////////////////////////////////////////////////////////////////////////
class ioFishingMessageBox : public ioWnd
{
public:
	enum
	{
		ID_AUTO_SELL = 1,
		ID_SELL      = 2,
		ID_CANCEL	 = 3,
		ID_EXIT		 = 4,
		ID_CLOSE	 = 5,
		ID_NEXT_SELL = 6,
		ID_AUTO_SELL_STOP = 7,

		ID_VERT_SCROLL = 1000,
	};

	enum
	{
		MAX_GRADE	 = 9,
		MAX_GRADE_BAR= 4,
		BACK_ARROW   = 4,
	};

	enum
	{
		NORMAL_BTN	= 1,
		ROLLING_BTN = 2,
		RESULT_BTN	= 3,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pPrevIcon;
	ioUIRenderImage *m_pPrevGrade;
	ioUIRenderImage *m_pNextIcon;
	ioUIRenderImage *m_pGrade;
	ioUIRenderImage *m_pRedBackEffect;
	ioUIRenderImage *m_pBlueBackEffect;
	ioUIRenderImage *m_pBackArrow[BACK_ARROW];
	ioUIRenderImage *m_pGradeIconList[MAX_GRADE];
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pNoneItemIcon;
	ioUIRenderImage *m_pGradeBar[MAX_GRADE_BAR];

	ioHashStringVec m_vGradeList;	
	ioHashString    m_szRollingSound;
	
protected:
	ioWnd *m_pPrevWnd;
	
	bool  m_bAutoSell;
	DWORD m_dwAutoSellTime;
	int   m_iStartIndex;
	int   m_iAutoSellCount;
	bool  m_bResult;
	bool  m_bFinalResult;

	bool m_bAlarm;

	int m_iItemType;
	int m_iItemIndex;
	int m_iResultGrade;
	__int64 m_iResultPeso;
	int   m_iMaxItemCnt;

	int   m_iCurPage;
	int   m_iCurGrade;
	float m_fCurGradeRate;

	DWORD m_dwRollingTime;
	DWORD m_dwRollingTotalTime;

	float m_fGradeScale;
	DWORD m_dwGradeScaleTime;

	DWORD m_dwRollingStartTime;
	DWORD m_dwGradeScaleStartTime;

protected:
	struct SellDataList
	{
		int m_iItemType;
		int m_iGradeIndex;
		int m_iSellPeso;
		SellDataList()
		{
			m_iItemType = m_iGradeIndex = m_iSellPeso = 0;
		}
	};
	typedef std::vector< SellDataList > vSellDataList;
	vSellDataList m_SellDataList;

protected:
	void ClearList();

public:
	void SetPrintMessage( int iStartArray, ioWnd *pPrevWnd );
	void OnResultInfo( int iItemType, int iItemIndex, int iGrade, bool bAlarm, __int64 iPeso );
	void UpdateMaxItemCnt();

public:
	void StartAutoSell();
	void StopAutoSell();
	void AutoSellQuestionFailed();

protected:
	virtual void ReBatchButton( int iType );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll(DWORD dwID, int curPos );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

protected:
	void OnRenderPrevSellItem( int iXPos, int iYPos );
	void OnRenderNextSellItem( int iXPos, int iYPos );
	void OnRenderSellItemList( int iXPos, int iYPos );

public:
	ioFishingMessageBox();
	virtual ~ioFishingMessageBox();
};
////////////////////////////////////////////////////////////////////////////////////////////////
class ioFishingItemMessageBox : public ioWnd
{
public:
	enum
	{
		ID_YES		= 1,
		ID_NO		= 2,
		ID_X		= 3,
	};

protected:
	ioComplexStringPrinter m_Printer[2];
	ioUIRenderImage *m_pLine;

	ioWnd *m_pPrevWnd;

protected:
	void ClearList();

public:
	void SetPrintMessage( ioComplexStringPrinter *MsgPrinter, ioWnd *pPrevWnd );

protected:
	virtual void ReBatchButton( int iType );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();
	virtual bool iwm_spacebar();
	virtual bool iwm_esc();

protected:
	virtual void OnRender();

public:
	ioFishingItemMessageBox();
	virtual ~ioFishingItemMessageBox();
};
////////////////////////////////////////////////////////////////////////////////////////////////
class ioFishingLevelUpWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_CLOSE= 2,
		ID_ALPHA_WND   = 100,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pLevelUpIcon;
	ioUIRenderImage *m_pLevelUpAddSlotIcon;

	ioHashString     m_szUpSound;

	bool m_bAddSlot;
	int  m_iCurLevel;

	ioWnd *m_pPrevWnd;

public:
	void SetPrintMessage( int iCurLevel, bool bAddSlot, ioWnd *pPrevWnd );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ioFishingLevelUpWnd();
	virtual ~ioFishingLevelUpWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
class FishingBuyWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT  = 1,
		ID_SHOP  = 2,
		ID_CANCEL= 3,
	};

protected:
	ioUIRenderImage *m_pIcon;

protected:
	ioHashStringVec  m_vDescVec;


public:
	bool SetInfo( const ioHashString &rsIcon, const ioHashString &rsTitle, const ioHashStringVec &rvDescVec );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	FishingBuyWnd();
	virtual ~FishingBuyWnd();
};

////////////////////////////////////////////////////////////////////////////////////////////////
class FishingDropZoneInduceWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT    = 1,
		ID_NORMAL_CLOSE   = 2,
		ID_WIDE_CLOSE   = 3,
		ID_GO_TO_SHOP = 4,
	};

	AlarmType m_AlarmType;

	bool m_bCheckPopup;

	DWORD m_dwAlarmPopUpEndDate[ AT_MAX_ALARM_TYPE ];

	// ¨úE¢Ò¡À A¢¬AO¢¯¢® ¥ìu¢¬¡Í ¡ío¢¥U AI©öIAo¥ìe
	ioUIRenderImage *m_pNotFishingAlarmType;
	ioUIRenderImage *m_pNewFishingAlarmType;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();

	bool SetAlarmType( AlarmType eAlarmType );

protected:
	virtual void OnRender();	

public:
	FishingDropZoneInduceWnd();
	virtual ~FishingDropZoneInduceWnd();
};
