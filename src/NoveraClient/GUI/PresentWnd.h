#pragma once

#include "../io3DEngine/ioMovingWnd.h"

class ioUIRenderImage;
class ioUIRenderFrame;
class PresentAlarmWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_LINK = 2,
	};

protected:
	ioUIRenderFrame *m_pBottomFrm;
	ioUIRenderFrame *m_pStatePinkFrm;
	ioUIRenderImage *m_pPresentBack;
	ioUIRenderImage *m_pPresentIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pPresentNum;
	ioUIRenderImage *m_pPresentPlus;
	ioUIRenderImage *m_pPresentCustom;
	ioUIRenderImage *m_pCostumeMark;

protected:
	DWORD m_dwIndex;                   // 선물 인덱스.
	DWORD m_dwSlotIndex;
	ioHashString m_szSendID;           // 보낸 유저
	short   m_iPresentType;            // ( 1.용병 - 2.치장 - 3.권한 - 4.페소 - 5.장비 )
	short   m_iPresentMent;            // ( 멘트타입(클라이언트에서 정의) )
	int     m_iPresentValue1;          // 용병:(용병타입), 치장(ITEMSLOT의 m_item_type), 권한(ETCITEMSLOT의 m_iType), 페소(페소금액), 장비(장비코드)
	int     m_iPresentValue2;          // 용병:(용병기간), 치장(ITEMSLOT의 m_item_code), 권한(ETCITEMSLOT의 m_iValue1), 페소(NONE), 장비(( 장비 성장값 * 10000 ) + 장비기간)
	int     m_iPresentValue3;		   // 장비:(남성스킨)
	int     m_iPresentValue4;		   // 장비:(여성스킨)
	DWORD   m_dwLimitDate;      

protected:
	DWORD   m_dwHideDelayTime;
	bool    m_bRePosition;
	int m_nGradeType;

protected:
	void SetPresentIcon();
	bool IsSpecialAwardBlock();
	bool IsShowDelay();
	void ReSizePresentUI();

public:
	// Wnd를 출력하는 시점과 조건이 차이가 있어 이 함수들을 통해 ShowWnd...
	void ShowPresentAlarmWnd();
	void ShowPresentAlarmWnd( const ioHashString szID, short iPresentType, short iPresentMent, int iValue1, int iValue2, int iValue3, int iValue4 );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_esc();
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRenderPresent( int iXPos, int iYPos );
	void OnRenderPeso( int iXPos, int iYPos );
	void OnRenderGradeExp( int iXPos, int iYPos );

protected:
	void SetGradeType( int nPresentType, int nPresentCode );

public:
	PresentAlarmWnd();
	virtual ~PresentAlarmWnd();
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class PresentDeleteWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT  = 1,
		ID_ALL_CLEAR = 2,
		ID_CLOSE = 3,
	};

protected:
	ioUIRenderFrame *m_pStatePinkFrm;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pPresentIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pPresentNum;
	ioUIRenderImage *m_pPresentPlus;
	ioUIRenderImage *m_pPresentCustom;
	ioUIRenderImage *m_pCostumeMark;

protected:
	DWORD m_dwPresentIndex;
	DWORD m_dwPresentSlotIndex;
	ioHashString m_szSendID;           // 보낸 유저
	short   m_iPresentType;            // ( 1.용병 - 2.치장 - 3.권한 - 4.페소 - 5.장비 )
	short   m_iPresentMent;            // ( 멘트타입(클라이언트에서 정의) )
	int     m_iPresentValue1;          // 용병:(용병타입), 치장(ITEMSLOT의 m_item_type), 권한(ETCITEMSLOT의 m_iType), 페소(페소금액), 장비(장비코드)
	int     m_iPresentValue2;          // 용병:(용병기간), 치장(ITEMSLOT의 m_item_code), 권한(ETCITEMSLOT의 m_iValue1), 페소(NONE), 장비(( 장비 성장값 * 10000 ) + 장비기간)
	int     m_iPresentValue3;		   // 장비:(남성스킨)
	int     m_iPresentValue4;		   // 장비:(여성스킨)
	DWORD   m_dwLimitDate;      
	int m_nGradeType;

protected:
	DWORD   m_dwHideDelayTime;

protected:
	void SetPresentIcon();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRenderPresent( int iXPos, int iYPos );
	void OnRenderPeso( int iXPos, int iYPos );
	void OnRenderGradeExp( int iXPos, int iYPos );
	void OnRenderExtraItem( int iXPos, int iYPos );
	void OnRenderExtraItemBox( int iXPos, int iYPos );
	void OnRenderCostumeBox( int iXPos, int iYPos );

protected:
	void SetGradeType( int nPresentType, int nPresentCode );

public:
	PresentDeleteWnd();
	virtual ~PresentDeleteWnd();
};

class PresentTimeCashAlarmWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_LINK = 2,
	};

protected:
	ioUIRenderFrame *m_pBottomFrm;
	ioUIRenderFrame *m_pStatePinkFrm;
	ioUIRenderImage *m_pPresentBack;
	ioUIRenderImage *m_pPresentIcon;
	ioUIRenderImage *m_pSubIcon;
	
protected:
	ManualMgr* m_pManual;
	bool m_bRePosition;
	int m_nAddCash;
	
public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_esc();
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void ReSizePresentUI();
	void PrintManual( int nXPos, int nYPos, float fScale );

public:
	void ShowPresentTimeCashAlarmWnd( int nAddCash );

public:
	PresentTimeCashAlarmWnd();
	virtual ~PresentTimeCashAlarmWnd();
};

class PresentTimeCashReleaseWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_X	= 1,
		ID_OK	= 2,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;

protected:
	DWORD m_dwEtcItemCode;
	ioHashString m_szItemName;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_esc();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void ShowPresentTimeCashReleaseWnd( DWORD dwEtcItemCode );

public:
	PresentTimeCashReleaseWnd();
	virtual ~PresentTimeCashReleaseWnd();
};