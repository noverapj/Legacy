#pragma once

#include "NewShopBuyWnd.h"
#include "ItemReinforceDownWnd.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//강화 아이템 리스트 선택 (메인 윈도우)
class ItemMaterialCompoundWnd : public ioWnd
{
public: 
	enum 
	{
		ID_COMPOUND_BTN		= 1,

		ID_TARGETITEM_BTN	= 11,
		ID_TARGET_ICON		= 201,

		ID_MATERIALITEM_BTN	 = 12,
		ID_MATERIALITEM_WND	 = 102,
		ID_MATERIALITEM_ICON = 202,	

		ID_FAIL_MANUAL_BTN	 = 13,
		ID_FAIL_TOOL_TIP	 = 103,
	};

protected:
	ioUIRenderImage *m_pExtraSubBack;
	ioUIRenderImage *m_pManualMark;
	ioUIRenderImage *m_pGaugeMainBack;
	ioUIRenderImage *m_pGaugeBack;
	ioUIRenderImage *m_pGauge;
	ioUIRenderImage *m_pMagicCircle;
	ioUIRenderImage *m_pItemBack;

	InvenItemNewInfoList m_vTotalItemList;

	int m_nReUseSlot;
	int m_nCurReinforce;
	float m_fSuccessRate;
	bool m_bIsSelectItem;
	bool m_bIsSelectMaterial;
	bool m_bSelectAdditive;
	int m_nMaxFailExp;
	int m_nCurFailExp;
	float m_fFailExpRate;
	int m_nExtraSubManualIdx;
	int m_nHelpTipIdx;

	DWORD m_dwEtcItemCode;

	ManualMgr* m_pManual;

	DWORD m_dwMagicEffectCreateTime;
	vMagicEffectInfo m_vMagicEffectInfoList;

	ioHashString m_szSelectItemMsg;
	ioHashString m_szLackMaterialMsg;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

	void CreateMagicCircle();
	void ProcessMagicCircle();
	void OnRenderMagicCircle();

protected:
	//성공율 렌더
	void SuccessRateDescRender();
	//실패경험치 렌더
	void RenderFailExp( int iXPos, int iYPos );
	//매뉴얼 출력
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	//아이템 코드 설정
	void ShowWndWithEtcItemCode( DWORD dwCode );
	void SetMaterialCode( int nMaterialCode );

protected:
	//초기화
	void InitData();
	//모든 영구 아이템 얻기
	void LoadTotalItemList();
	//강화 아이템 초기화
	void TargetItemInit();

protected:
	//리스트 얻기
	void GetTargetList( vNewShopPullDownItem &rkList );
	void GetTargetSeletInfo( ExtraItemList &rkList );
	//슬롯 재설정
	bool SlotReload();
	//강화 아이템 갱신
	void UpdateItemData();	
	//강화 패킷 전송
	void SendItemCompound();

protected:
	void GetMaterialList( vNewShopPullDownItem &rkList );
	void MaterialListBtnUp( ioWnd *pOpenBtn, int iListArray );

protected:
	//재료별 패널티 적용
	void ApplySuccessRate();

public:
	bool GetCurAlchemicItemInfo( OUT AlchemicToolTipInfo &rkInfo, IN DWORD dwItemInfoID );

public:
	ItemMaterialCompoundWnd(void);
	virtual ~ItemMaterialCompoundWnd(void);
};
