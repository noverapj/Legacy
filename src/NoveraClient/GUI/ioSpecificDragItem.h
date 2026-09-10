#pragma once

#include "../io3DEngine/ioDragItem.h"
#include "../CharacterInfo.h"
#include "../TournamentNode.h"

struct ITEM_DATA;

enum
{
	// 0 은 Unspecified..
	SDI_INVEN_ITEM			  = 1,  //인벤토리 아이템.
	SDI_EQUIP_ITEM			  = 2,  //장착 중인 아이템.
	SDI_SOLDIER_ITEM		  = 3,  //고용중인 용병 아이템.
	SDI_SHOP_SOLDIER_ITEM	  = 4,  //상점,아이템 내용병 선택 아이템.
	SDI_ALCHEMIC_ITEM		  = 5,  //조합 아이템
	SDI_TOUR_UNALLOCATE_ITEM  = 6,
	SDI_TOUR_ALLOCATED_ITEM   = 7,
};

class ioInvenDragItem : public ioDragItem
{
protected:
	DWORD	m_dwType;
	int		m_iItemIdx;
	DWORD	m_dwItemCode;
	POINT	m_ptIconOffset;

public:
	void SetItemData( int iItemDBIndex, DWORD dwItemCode );
	void SetIconOffset( int iXPos, int iYPos );

public:
	int	  GetItemDBIndex() const { return m_iItemIdx; }
	DWORD GetItemCode() const { return m_dwItemCode; }

	int GetIconOffsetX() const { return m_ptIconOffset.x; }
	int GetIconOffsetY() const { return m_ptIconOffset.y; }

public:
	virtual int GetTypeID() const;
	virtual const ioHashString& GetTypeName() const;

public:
	ioInvenDragItem( DWORD dwType );
	virtual ~ioInvenDragItem();
};

inline ioInvenDragItem* ToInvenDragItem( ioDragItem *pItem )
{
	if( !pItem || ( pItem->GetTypeID() != SDI_INVEN_ITEM && 
					pItem->GetTypeID() != SDI_EQUIP_ITEM ) )
		return NULL;

	return dynamic_cast< ioInvenDragItem* >( pItem );
}
//////////////////////////////////////////////////////////////////////////
class ioSoldierDragItem : public ioDragItem
{
protected:
	DWORD m_dwType;
	DWORD m_dwIndex;
	CHARACTER m_charInfo;
	ITEM_DATA m_equipInfo[MAX_INVENTORY];

public:
	void SetSoldierInfo( DWORD dwIndex );

public:
	const DWORD GetIndex() const { return m_dwIndex; }
	const CHARACTER &GetCharInfo();
	const ITEM_DATA &GetEquipInfo( int iArray );

public:
	virtual int GetTypeID() const;
	virtual const ioHashString &GetTypeName() const;

public:
	ioSoldierDragItem( DWORD dwType );
	virtual ~ioSoldierDragItem();
};

inline ioSoldierDragItem* ToSoldierDragItem( ioDragItem *pItem )
{
	if( !pItem || ( pItem->GetTypeID() != SDI_SOLDIER_ITEM ) )
		return NULL;
	return dynamic_cast< ioSoldierDragItem * >( pItem );
}
/////////////////////////////////////////////////////////////////////////
#define SHOP_MY_SOLDIER_BTN_SCALE 0.60f

class ioShopSoldierDragItem : public ioDragItem
{
protected:
	DWORD	m_dwType;
	int     m_iSoldierArray;
	POINT	m_ptIconOffset;

public:
	void SetSoldierArray( int iSoldierArray );
	void SetIconOffset( int iXPos, int iYPos );

public:
	const int GetSoldierArray() const { return m_iSoldierArray; }
	const int GetIconOffsetX() const { return m_ptIconOffset.x; }
	const int GetIconOffsetY() const { return m_ptIconOffset.y; }

public:
	virtual int GetTypeID() const;
	virtual const ioHashString &GetTypeName() const;

public:
	ioShopSoldierDragItem( DWORD dwType );
	virtual ~ioShopSoldierDragItem();
};

inline ioShopSoldierDragItem *ToShopSoldierDragItem( ioDragItem *pItem )
{
	if( !pItem || ( pItem->GetTypeID() != SDI_SHOP_SOLDIER_ITEM ) )
		return NULL;
	return dynamic_cast< ioShopSoldierDragItem* > ( pItem );
}
/////////////////////////////////////////////////////////////////////////
class ioUnAllocateButtonDragItem : public ioDragItem
{
protected:
	DWORD m_dwType;

	ioUIRenderFrame* m_pRenderFrame;
	TournamentNode::AllocateTeamData* m_pCustomTeamData;

public:
	virtual int GetTypeID() const;
	virtual const ioHashString &GetTypeName() const;

public:
	inline void SetCustomTeamData( TournamentNode::AllocateTeamData* pCustomTeamData ){ m_pCustomTeamData = pCustomTeamData; }
	inline void SetRenderFrame( ioUIRenderFrame* pFrameName ){ m_pRenderFrame = pFrameName; }
	inline ioUIRenderFrame* GetRenderFrame(){ return m_pRenderFrame; }
	inline TournamentNode::AllocateTeamData* GetCustomTeamData(){ return m_pCustomTeamData; }

public:
	ioUnAllocateButtonDragItem( DWORD dwType );
	virtual ~ioUnAllocateButtonDragItem();
};

inline ioUnAllocateButtonDragItem *ToUnAllocateButtonDragItem( ioDragItem *pItem )
{
	if( !pItem || ( pItem->GetTypeID() != SDI_TOUR_UNALLOCATE_ITEM ) )
		return NULL;
	return dynamic_cast< ioUnAllocateButtonDragItem* > ( pItem );
}
/////////////////////////////////////////////////////////////////////////
class ioAllocatedButtonDragItem : public ioDragItem
{
protected:
	DWORD m_dwType;
	bool m_bClickRemove;

	ioUIRenderFrame* m_pRenderFrame;
	TournamentNode::AllocateTeamData* m_pCustomTeamData;

public:
	virtual int GetTypeID() const;
	virtual const ioHashString &GetTypeName() const;
	
public:
	inline void SetCustomTeamData( TournamentNode::AllocateTeamData* pCustomTeamData ){ m_pCustomTeamData = pCustomTeamData; }
	inline void SetRenderFrame( ioUIRenderFrame* pFrameName ){ m_pRenderFrame = pFrameName; }
	inline ioUIRenderFrame* GetRenderFrame(){ return m_pRenderFrame; }
	inline TournamentNode::AllocateTeamData* GetCustomTeamData(){ return m_pCustomTeamData; }

	inline void SetClickRemove( bool bClick ){ m_bClickRemove = bClick; }
	inline bool GetClickRemove(){ return m_bClickRemove; }
	
public:
	ioAllocatedButtonDragItem( DWORD dwType );
	virtual ~ioAllocatedButtonDragItem();
};

inline ioAllocatedButtonDragItem *ToAllocatedButtonDragItem( ioDragItem *pItem )
{
	if( !pItem || ( pItem->GetTypeID() != SDI_TOUR_ALLOCATED_ITEM ) )
		return NULL;
	return dynamic_cast< ioAllocatedButtonDragItem* > ( pItem );
}