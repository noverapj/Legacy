#pragma once

enum ModeItemType
{
	MIT_NONE,
	MIT_SHUFFLE_STAR,
	MIT_BUFF,
	MIT_HUNTER_COIN,
	MIT_MAX,
};

//-------------------------------------------------------------------------------

class ModeItem
{
public:
	ModeItemType m_eType;
	DWORD m_dwModeItemIdx;
	
public:
	bool IsValid() const;
	void SetType( ModeItemType eType );
	ModeItemType GetType() const;

public:
	ModeItem();
	virtual ~ModeItem();

};
//-------------------------------------------------------------------------------

class BuffModeItem : public ModeItem
{
public:
	float m_fXPos;
	float m_fZPos;

public:
	BuffModeItem();
	virtual ~BuffModeItem();

	static BuffModeItem* ToBuffModeItem( ModeItem* pItem )
	{
		if( pItem && pItem->GetType() == MIT_BUFF )
		{
			return static_cast< BuffModeItem* >( pItem );
		}

		return NULL;
	}

	static const BuffModeItem* ToBuffModeItemConst( const ModeItem* pItem )
	{
		if( pItem && pItem->GetType() == MIT_BUFF )
		{
			return static_cast< const BuffModeItem* >( pItem );
		}

		return NULL;
	}
};

//-------------------------------------------------------------------------------

namespace ModeItemCreator
{
	ModeItem* CreateModeItemTemplate( ModeItemType eType );
}
