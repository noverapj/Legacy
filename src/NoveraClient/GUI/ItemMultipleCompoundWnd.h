#pragma once

#include "../io3DEngine/ioButton.h"
#include "../ioComplexStringPrinter.h"

#include "ItemCompoundWnd.h"
#include "SoldierPackageWnd.h"

class ManualMgr;


////////////////////////////////////////////////////////////////////////////////////////////////////
struct CompoundItemInfo
{
	ioHashString m_szItemName;
	int m_iSlotIndex;

	CompoundItemInfo()
	{
		m_iSlotIndex = 0;
	}
};
typedef std::vector< CompoundItemInfo > CompoundItemInfoList;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemMultipleCompoundWnd : public ioWnd
{
public: 
	enum 
	{
		ID_COMPOUND_BTN		= 1,

		ID_ITEM1_BTN		= 11,
		ID_ITEM2_BTN		= 12,
		ID_ITEM3_BTN		= 13,

		ID_ICON1_BTN		= 21,
		ID_ICON2_BTN		= 22,
		ID_ICON3_BTN		= 23,

		ID_ITEM_LIST_WND	= 101,
	};

protected:
	DWORD m_dwEtcItemType;
	bool m_bAllSelect;
	InvenItemInfoList m_vTotalList;

	DWORD m_dwCurItemBtn;

	ManualMgr* m_pManual;

protected:
	void LoadTotalList();

	void GetCurList( DWORD dwID, vNewShopPullDownItem &rkList );
	void GetCurSeletedBtnList( DWORD dwID, ExtraItemList &rkList );

	void UpdateData();
	void CheckCompoundItemBtn( ioWnd *pWnd, DWORD dwID );
	void CheckItemListWnd( ioWnd *pWnd, int iArray );
	void CheckCurIconList();

	void SendItemCompound();

	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void SetInfoAndShow( DWORD dwType );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	ItemMultipleCompoundWnd(void);
	virtual ~ItemMultipleCompoundWnd(void);
};

class UniqueItemMultipleCompoundWnd : public ioWnd
{
public: 
	enum 
	{
		ID_COMPOUND_BTN		= 1,

		ID_ITEM1_BTN		= 11,
		ID_ITEM2_BTN		= 12,
		ID_ITEM3_BTN		= 13,

		ID_ICON1_BTN		= 21,
		ID_ICON2_BTN		= 22,
		ID_ICON3_BTN		= 23,

		ID_ITEM_LIST_WND	= 101,
	};

protected:
	DWORD m_dwEtcItemType;
	bool m_bAllSelect;
	InvenItemInfoList m_vTotalList;

	DWORD m_dwCurItemBtn;

	ManualMgr* m_pManual;

protected:
	void LoadTotalList();

	void GetCurList( DWORD dwID, vNewShopPullDownItem &rkList );
	void GetCurSeletedBtnList( DWORD dwID, ExtraItemList &rkList );

	void UpdateData();
	void CheckCompoundItemBtn( ioWnd *pWnd, DWORD dwID );
	void CheckItemListWnd( ioWnd *pWnd, int iArray );
	void CheckCurIconList();

	void SendItemCompound();

	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void SetInfoAndShow( DWORD dwType );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	UniqueItemMultipleCompoundWnd(void);
	virtual ~UniqueItemMultipleCompoundWnd(void);
};