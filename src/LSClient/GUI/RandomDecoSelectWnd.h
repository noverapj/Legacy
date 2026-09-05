#pragma once

#include "../ioComplexStringPrinter.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioButton.h"

#include "ioMessageBox.h"
#include "SoldierPackageWnd.h"

class RandomDecoSelectWnd : public ioWnd
{
public:
	enum
	{
		ID_SOLDIER_BTN	= 1,
		ID_RECV			= 2,

		ID_GO			= 3,

		ID_SOLDIER_WND  = 1000,
		ID_PULLDOWN_WND	= 1001,
	};

protected:
	ioUIRenderImage *m_pPresentPreBack;
	ioUIRenderImage *m_pPresentBack;

	ioUIRenderImage *m_pPresentIcon;

protected:
	bool m_bSubscription;

	DWORD m_dwIndex;                   // 선물 인덱스.
	DWORD m_dwSlotIndex;               // 선물 슬롯 인덱스.
	ioHashString m_szSendID;           // 보낸 유저
	short   m_iPresentType;            // ( 1.용병 - 2.치장 - 3.권한 - 4.페소 - 5.장비 )
	short   m_iPresentMent;            // ( 멘트타입(클라이언트에서 정의) )
	int     m_iPresentValue1;          // 용병:(용병타입), 치장(ITEMSLOT의 m_item_type), 권한(ETCITEMSLOT의 m_iType), 페소(페소금액), 장비(장비코드)
	int     m_iPresentValue2;          // 용병:(용병기간), 치장(ITEMSLOT의 m_item_code), 권한(ETCITEMSLOT의 m_iValue1), 페소(NONE), 장비(( 장비 성장값 * 10000 ) + 장비기간)
	int     m_iPresentValue3;          // 장비:(남성스킨)
	int     m_iPresentValue4;		   // 장비:(여성스킨)
	DWORD   m_dwLimitDate;

	ioHashString m_szSubscriptionID;
	int m_iSubscriptionGold;

protected:
	int m_iSelectClassType;
	bool m_bResult;

	CHARACTER m_CharInfo;
	CHARACTER m_OverCharInfo;

	vPackageItem m_PackageItemList;

	bool m_bPullDownShow;

	ioComplexStringPrinter m_PresentInfo[2];

protected:
	void SetPresentIcon();
	void PresentRecv();
	void ReSizePresentUI();

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

	void OnRenderPresent( int iXPos, int iYPos );

protected:
	void GetClassList( vPackageItem &rkList );
	void ConvertItemList( IN vPackageItem &rkPackageList, OUT vNewShopPullDownItem &rkList );

	void SetPullDownMenu( ioWnd *pOpenBtn );

	void PullDownBtnUP( ioWnd *pOpenBtn, int iListArray );
	void PullDownBtnOver( ioWnd *pOpenBtn, int iListArray );

	void UpdateBtn();

	void DefaultDecoration( CHARACTER &rkCharInfo );
	void CheckSoldierWnd( int iClassType, CHARACTER &rkCharInfo );

public:
	void SetPresent( DWORD dwIndex, DWORD dwSlotIndex );
	void SetSubscription( DWORD dwIndex, const ioHashString szSubscriptionID );
	void OnResultState();

public:
	RandomDecoSelectWnd();
	virtual ~RandomDecoSelectWnd();
};

