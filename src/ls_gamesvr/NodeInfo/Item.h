// Item.h: interface for the ioItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEM_H__34CC070D_4014_4679_9F9E_B84328970B21__INCLUDED_)
#define AFX_ITEM_H__34CC070D_4014_4679_9F9E_B84328970B21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NodeHelpStructDefine.h"

class SP2Packet;
class ioItemMaker;

class ioItem
{
public:
	enum ModeCrownType
	{
		MCT_NONE		 = 0,
		MCT_HIDDEN_KING	 = 1,
		MCT_DOUBLE_CROWN = 2,
		MCT_FLAG = 3,			// 2018-03-15 by bckim, 깃발모드 추가
	};

private:
	ITEM_DATA m_ItemData;
	int       m_iGameIndex;

	ioItemMaker *m_pCreator;
	ioHashString m_ItemName;

	ioHashString m_OwnerName;
	Vector3 m_ItemPos;
	float m_fItemCurGauge;

	DWORD m_dwDropTime;
	bool m_bNotDeleteItem;

protected:
	int  m_iCrownItemType;
	int  m_iItemTeamType;

public:
	void SetItemData( const ITEM_DATA &rkData );
	void SetItemData( int iCode, int iReinforce, DWORD dwMaleCustom, DWORD dwFemaleCustom );

	void SetItemCode( int iItemCode );
	void SetGameIndex( int iIndex );

	void SetItemName( const ioHashString &rkName );
	void SetOwnerName( const ioHashString &rkName );
	void SetItemPos( const Vector3 &vPos );
	void SetDropTime( DWORD dwDropTime );

	void SetCurItemGauge( float fGauge );

public:
	bool HasOwner() const;
	void ClearOwnerNameIf( const ioHashString &rkName );
	
	bool IsRightItem( int iGameIndex, int iItemCode ) const;

public:
	void FillFieldItemInfo( SP2Packet &rkPacket );

public:
	inline int GetItemCode() const { return m_ItemData.m_item_code; }
	inline int GetItemReinforce() const { return m_ItemData.m_item_reinforce; }
	inline DWORD GetItemMaleCustom() const { return m_ItemData.m_item_male_custom; }
	inline DWORD GetItemFemaleCustom() const { return m_ItemData.m_item_female_custom; }
	inline int GetGameIndex() const { return m_iGameIndex; }

	inline DWORD GetDropTime() const { return m_dwDropTime; }

	inline const ioHashString& GetItemName() const { return m_ItemName; }
	inline const ioHashString& GetOwnerName() const { return m_OwnerName; }

	inline const Vector3& GetItemPos() const { return m_ItemPos; }

	inline void SetEnableDelete( bool bNotDeletItem ) { m_bNotDeleteItem = bNotDeletItem; }
	inline bool IsNotDeleteItem() const { return m_bNotDeleteItem; }

public:
	void SetCrown( int iCrownItemType, int iItemTeamType );
	int GetCrownItemType(){ return m_iCrownItemType; }
	int GetCrownItemTypeConst() const { return m_iCrownItemType; }
	int GetItemTeamType(){ return m_iItemTeamType; }

public:
	ioItem( ioItemMaker *pCreator );
	~ioItem();
};

typedef std::list< ioItem* > ItemList;
typedef std::vector< ioItem* > ItemVector;

#endif // !defined(AFX_ITEM_H__34CC070D_4014_4679_9F9E_B84328970B21__INCLUDED_)
