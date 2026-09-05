#pragma once

#include "../io3DEngine/ioButton.h"
#include "../ioComplexStringPrinter.h"

#include "NewShopBuyWnd.h"
#include "InventorySelectItemWnd.h"

class ManualMgr;


struct InvenItemInfo
{
	int m_iSlotIndex;
	int m_iItemCode;
	int m_iReinforce;

	InvenItemInfo()
	{
		m_iSlotIndex = 0;
		m_iItemCode = 0;
		m_iReinforce = 0;
	}
};
typedef std::vector< InvenItemInfo > InvenItemInfoList;

/////////////////////////////////////////////////////////////////////////////////////////////////////
class CompoundItemInfoSort1 : public std::binary_function< const InvenItemInfo& , const InvenItemInfo&, bool >
{
public:
	bool operator() ( const InvenItemInfo &lhs, const InvenItemInfo &rhs ) const
	{
		if( lhs.m_iItemCode < rhs.m_iItemCode )
			return true;

		if( lhs.m_iItemCode == rhs.m_iItemCode )
		{
			if( lhs.m_iReinforce > rhs.m_iReinforce )
				return true;	
		}
		return false;
	}
};

class CompoundItemInfoSort2 : public std::binary_function< const InvenItemInfo& , const InvenItemInfo&, bool >
{
public:
	bool operator() ( const InvenItemInfo &lhs, const InvenItemInfo &rhs ) const
	{
		if( lhs.m_iItemCode < rhs.m_iItemCode )
			return true;

		if( lhs.m_iItemCode == rhs.m_iItemCode )
		{
			if( lhs.m_iReinforce < rhs.m_iReinforce )
				return true;	
		}
		return false;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SelectExtraItemBtn : public ioButton
{
protected:
	int m_iCurSelectSlot;
	DWORD m_dwCurTitleColor;

	ioHashString m_szDefaultText;
	ioHashString m_szSelectText;

public:
	void InitData();
	void SetSelectExtraItemSlot( int iSelectSlot );
	void SetSelectAccessorySlot( int iSelectSlot );
	void SetSelectMaterial( int nSelectMaterial, ioHashString szTitle );
	void SetCurTitleColor( DWORD dwTitleColor );
	inline int GetSelectExtraItemSlot() const { return m_iCurSelectSlot; }
	
protected:
	virtual void OnRender();

public:
	virtual void iwm_show();

	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	SelectExtraItemBtn();
	virtual ~SelectExtraItemBtn();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ItemCompoundWnd : public ioWnd
{
public: 
	enum 
	{
		ID_COMPOUND_BTN		= 1,

		ID_TARGETITEM_BTN	= 11,
		ID_VICTIMITEM_BTN	= 12,

		ID_TARGETITEM_WND	= 101,
		ID_VICTIMITEM_WND	= 102,

		ID_TARGET_ICON		= 201,
		ID_VICTIM_ICON		= 202,
	};

protected:
	bool m_bAllSelect;
	InvenItemInfoList m_vTotalList;

	int m_iReUseTargetSlot;
	int m_iTargetReinforce;
	int m_iVictimReinforce;
	int m_iSelectedBtnType;

	DWORD m_dwEtcItemCode;

	ManualMgr* m_pManual;

protected:
	void UpdateData( bool bCheckReinforce );
	void SendItemCompound();

	void LoadTotalList();
	void GetTargetList( vNewShopPullDownItem &rkList );
	void GetTargetSeletInfo( ExtraItemList &rkList );
	void GetVictimList( vNewShopPullDownItem &rkList );
	void GetVictimSeletInfo( ExtraItemList &rkList );

	void TargetListBtnUp( ioWnd *pOpenBtn, int iListArray );
	void VictimListBtnUp( ioWnd *pOpenBtn, int iListArray );

	void SuccessRateDescRender();
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void ShowWndWithEtcItemCode( DWORD dwCode );

	inline int GetTargetSelectReinforce() const { return m_iTargetReinforce; }
	inline int GetVictimSelectReinforce() const { return m_iVictimReinforce; }
	bool CheckWarning( ioWnd *pWnd, int iSlot );

protected:
	void TargetReload();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	ItemCompoundWnd(void);
	virtual ~ItemCompoundWnd(void);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ItemGrowthCatalystWnd : public ioWnd
{
public: 
	enum 
	{
		ID_COMPOUND_BTN		= 1,

		ID_TARGETITEM_BTN	= 11,

		ID_TARGETITEM_WND	= 101,

		ID_TARGET_ICON		= 201,
	};

protected:
	InvenItemInfoList m_vTotalList;

	int m_iTargetReinforce;

	DWORD m_dwEtcItemCode;

	ManualMgr* m_pManual;

protected:
	void UpdateData();
	void SendItemGrowthCatalyst();

	void LoadTotalList();
	void GetTargetList( vNewShopPullDownItem &rkList );
	void GetTargetSeletInfo( ExtraItemList &rkList );

	void TargetListBtnUp( ioWnd *pOpenBtn, int iListArray );

	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void ShowWndWithEtcItemCode( DWORD dwCode );

	inline int GetTargetSelectReinforce() const { return m_iTargetReinforce; }

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	ItemGrowthCatalystWnd(void);
	virtual ~ItemGrowthCatalystWnd(void);
};

//////////////////////////////////////////////////////////////////////////
class ItemLuckyCoinWnd : public ioWnd
{
public: 
	enum 
	{
		ID_RECV_BTN		= 1,
		ID_ICON_BTN     = 2,

		ID_KIND_BTN		= 11,
		ID_KIND_WND		= 101,
	};

protected:
	DWORD		m_dwEtcItemCode;
	ManualMgr	*m_pManual;

	int m_iExchangeIndex;
	int m_iExchangeManual;
	ioHashString m_szExchangeName;
	ioHashString m_szExchangeIcon;
	bool m_bExchangeCostume;

protected:
	void GetItemList( vNewShopPullDownItem &rkList );
	void GetItemList( vInventorySelectItemList &rkList, ioHashStringVec& vRadioNameList );
	void SelectListBtnUp( int iSelectIndex );
	void SendItemLuckyCoin();
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );
	void ShowInventorySelectItemWnd( InventorySelectItemWnd *pInventorySelectItemWnd );
public:
	void ShowWndWithEtcItemCode( DWORD dwCode );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	ItemLuckyCoinWnd();
	virtual ~ItemLuckyCoinWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemCompoundExWnd : public ioWnd
{
public: 
	enum 
	{
		ID_COMPOUND_BTN		= 1,
		ID_TARGETITEM_BTN	= 11,
		ID_TARGETITEM_WND	= 101,
		ID_TARGET_ICON		= 201,
	};

protected:
	InvenItemInfoList m_vTotalList;

	int m_iTargetReinforce;

	DWORD m_dwEtcItemCode;

	ManualMgr* m_pManual;

protected:
	void UpdateData();
	void SendItemCompoundEx();

	void LoadTotalList();
	void GetTargetList( vNewShopPullDownItem &rkList );
	void GetTargetSeletInfo( ExtraItemList &rkList );

	void TargetListBtnUp( ioWnd *pOpenBtn, int iListArray );

	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void ShowWndWithEtcItemCode( DWORD dwCode );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	ItemCompoundExWnd(void);
	virtual ~ItemCompoundExWnd(void);
};

//////////////////////////////////////////////////////////////////////////
class ItemRainbowMixerWnd : public ioWnd
{
public: 
	enum 
	{
		ID_RECV_BTN		= 1,

		ID_SLOT_BTN1    = 2,
		ID_SLOT_BTN2    = 3,
		ID_SLOT_BTN3    = 4,
		ID_SLOT_BTN4    = 5,
		ID_SLOT_BTN5    = 6,
		ID_SLOT_BTN6    = 7,
		ID_SLOT_BTN7    = 8,
	};

public:
	enum
	{
		MAX_PATTERN = 2,
	};

protected:
	ioUIRenderImage *m_pBackground;
	ioUIRenderImage *m_pSlotEquipIcon;
	ioUIRenderImage *m_pSlotEquipLine1;
	ioUIRenderImage *m_pSlotEquipLine2;
	ioUIRenderImage *m_pSlotEquipOver;

protected:
	ManualMgr	*m_pManual;
	DWORD		 m_dwEtcItemCode;
	bool		 m_bPattern;
	DWORD        m_dwPatternScreenTime;
	bool		 m_bPatternScreen;

	struct PatternData
	{
		POINT m_Point;
		int   m_iPatternIdx;
		ioUIRenderImage *m_pPatternLine;
		ioUIRenderImage::ReverseFormatting m_ReverseFormat;
		PatternData()
		{
			m_Point.x = m_Point.y = 0;
			m_iPatternIdx	= -1;
			m_pPatternLine	= NULL; 
			m_ReverseFormat = ioUIRenderImage::RF_NONE_REVERSE;
		}
	};
	
	struct RainbowSlotData
	{
		bool m_bEquipSlot;
		DWORD m_dwEtcItem;
		ioUIRenderImage *m_pIcon;

		PatternData m_Pattern[MAX_PATTERN];
		RainbowSlotData()
		{
			m_dwEtcItem  = 0;
			m_bEquipSlot = false;
			m_pIcon      = NULL;

			for(int i = 0;i < MAX_PATTERN;i++)
			{
				m_Pattern[i].m_iPatternIdx = -1;
				m_Pattern[i].m_pPatternLine= NULL;
				m_Pattern[i].m_Point.x = m_Pattern[i].m_Point.y = 0;
				m_Pattern[i].m_ReverseFormat = ioUIRenderImage::RF_NONE_REVERSE;
			}
		}
	};
	typedef std::vector< RainbowSlotData > vRainbowSlotList;
	vRainbowSlotList m_RainbowSlotList;

protected:
	void ClearRainbowSlot();

protected:
	void SendItemMixer();
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

protected:
	void SetEquipItem( int iSlot );
	void SetEquipPatternLine();
	void CheckEquipPattern();

public:
	void ShowWndWithEtcItemCode( DWORD dwCode );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	

protected:
	void OnRenderPattern( int iXPos, int iYPos );
	void OnRenderButtonIcon( int iXPos, int iYPos );

public:
	ItemRainbowMixerWnd();
	virtual ~ItemRainbowMixerWnd();
};

//////////////////////////////////////////////////////////////////////////
class ItemLostSagaMixerWnd : public ioWnd
{
public: 
	enum 
	{
		ID_RECV_BTN		= 1,

		ID_SLOT_BTN1    = 2,
		ID_SLOT_BTN2    = 3,
		ID_SLOT_BTN3    = 4,
		ID_SLOT_BTN4    = 5,
		ID_SLOT_BTN5    = 6,
		ID_SLOT_BTN6    = 7,
	};

protected:
	ManualMgr	*m_pManual;
	DWORD		 m_dwEtcItemCode;
	bool		 m_bPattern;
	DWORD        m_dwPatternScreenTime;
	bool		 m_bPatternScreen;

	struct LostSagaSlotData
	{
		bool  m_bEquipSlot;
		DWORD m_dwEtcItem;
		ioUIRenderImage *m_pIcon;
		ioUIRenderImage *m_pGrayIcon;

		LostSagaSlotData()
		{
			m_dwEtcItem  = 0;
			m_bEquipSlot = false;
			m_pIcon      = NULL;
			m_pGrayIcon  = NULL;
		}
	};
	typedef std::vector< LostSagaSlotData > vLostSagaSlotList;
	vLostSagaSlotList m_LostSagaSlotList;

protected:
	void ClearLostSagaSlot();

protected:
	void SendItemMixer();
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void ShowWndWithEtcItemCode( DWORD dwCode );

protected:
	void SetEquipItem( int iSlot );
	void CheckEquipPattern();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	

protected:
	void OnRenderButtonIcon( int iXPos, int iYPos );

public:
	ItemLostSagaMixerWnd();
	virtual ~ItemLostSagaMixerWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemGoldBoxWnd : public ioWnd
{
public: 
	enum 
	{
		ID_RECV_BTN		= 1,

		ID_SLOT_BTN1    = 2,
	};

public:
	enum
	{
		MAX_PATTERN = 1,
	};

protected:
	ioUIRenderImage *m_pSlotEquipOver;
	ioUIRenderImage *m_pBackground;
protected:
	ManualMgr	*m_pManual;
	DWORD		 m_dwEtcItemCode;
	bool		 m_bPattern;

	struct GoldBoxSlotData
	{
		bool m_bEquipSlot;
		DWORD m_dwEtcItem;
		ioUIRenderImage *m_pIcon;

		GoldBoxSlotData()
		{
			m_dwEtcItem  = 0;
			m_bEquipSlot = false;
			m_pIcon      = NULL;		
		}
	};
	typedef std::vector< GoldBoxSlotData > vGoldBoxSlotDataList;
	vGoldBoxSlotDataList m_GoldBoxSlotList;

protected:
	void ClearSlot();

protected:
	void SendItemMixer();
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

protected:
	void SetEquipItem( int iSlot );

public:
	void ShowWndWithEtcItemCode( DWORD dwCode );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	//virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	

protected:
	void OnRenderButtonIcon( int iXPos, int iYPos );

public:
	ItemGoldBoxWnd();
	virtual ~ItemGoldBoxWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class AccessoryCompoundWnd : public ioWnd
{
public: 
	enum 
	{
		ID_COMPOUND_BTN		= 1,

		ID_TARGET_BTN	= 11,
		ID_VICTIM1_BTN	= 12,
		ID_VICTIM2_BTN	= 13,
		ID_VICTIM3_BTN	= 14,

		ID_TARGET_ICON	= 101,
		ID_VICTIM1_ICON	= 102,
		ID_VICTIM2_ICON	= 103,
		ID_VICTIM3_ICON	= 104,
	};
	enum AccessoryIndexType
	{
		ACI_UNSELECT = -1,
		ACI_TARGET = 0,
		ACI_VICTIM1 = 1,
		ACI_VICTIM2 = 2,
		ACI_VICTOM3 = 3,

		ACI_SIZE = 4,
	};

private:
	bool m_bAllSelect;
	DWORD m_dwEtcItemCode;
	AccessoryIndexType m_iSelectedBtnType;
	int m_iSelectedAccessoryIndex[ACI_SIZE];

	ioHashString m_szCompoundErrMsg[2];
	ManualMgr* m_pManual;

public:
	void ShowWndWithEtcItemCode( DWORD dwCode );

private:
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );
	void SendCompound();

private:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	AccessoryCompoundWnd(void);
	virtual ~AccessoryCompoundWnd(void);
};