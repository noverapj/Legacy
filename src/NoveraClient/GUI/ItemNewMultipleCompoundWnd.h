#pragma once
#include "../io3DEngine/ioWnd.h"
#include "ItemCompoundWnd.h"
#include "InventorySelectItemWnd.h"

class ItemNewMultipleCompoundWnd :public ioWnd
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
	};
	enum
	{
		MAX_ITEM_BTN		= 3,
	};

protected:
	DWORD m_dwEtcItemType;
	bool m_bAllSelect;
	DWORD m_dwMaterialType;
	DWORD m_dwResultGashaponIndex;
	vInventorySelectItemList m_vTotalList;

	DWORD m_dwCurItemBtn;

	ManualMgr* m_pManual;
protected:
	void LoadTotalList();
	void LoadMedalItemList();
	void LoadCostumeItemList();
	void LoadExtraItemList();

	void UpdateData();
	void UpdateMedalData();
	void UpdateCostumeData();
	void UpdateExtraItemData();

	void GetItemList( vInventorySelectItemList& vrkList, ioHashStringVec& vRadioNameList );

	void CheckCompoundItemBtn( ioWnd *pWnd, DWORD dwID );

	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

	void SendItemCompound();
	void SendMedalItemCompound();
	void SendCostumeItemCompound();
	void SendExtraItemCompound();

	void ShowInventorySelectItemWnd( InventorySelectItemWnd *pInventorySelectItemWnd );
	
public:
	void SetInfoAndShow( DWORD dwType );
public:
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
protected:
	virtual void OnRender();
public:
	ItemNewMultipleCompoundWnd(void);
	virtual ~ItemNewMultipleCompoundWnd(void);
};

