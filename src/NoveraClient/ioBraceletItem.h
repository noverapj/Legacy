#pragma once

#include "ioItem.h"

class ioBraceletItem :	public ioItem
{
private:
	float m_fLocalRandomNumber;
	int	  m_iManualNumber;
	ioHashString m_szToolTipText;

	int m_iAbilityMax;
	int m_iAbilityMin;
	int m_iAbilityGap;
	bool m_bAbilitySign;
	ioHashString m_szReinforceText;
	ioHashString m_szReinforceResultText;

public:
	virtual ItemType GetType() const;
	virtual ioItem* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void InitializeBeforeEquip( ioBaseChar *pOwner, ioPlayStage* pStage );

public:
	const int& GetManualIndex() const;
	const ioHashString& GetToolTipText() const;
	const int GetAbilityMax() const;
	const int GetAbilityGap() const					{ return m_iAbilityGap; }
	const bool GetAbilitySign() const				{ return m_bAbilitySign; }
	const ioHashString& GetReinforceText() const	{ return m_szReinforceText; }
	const ioHashString& GetReinforceResultText() const	{ return m_szReinforceResultText; }

public:
	ioBraceletItem(void);
	ioBraceletItem( const ioBraceletItem &rhs );
	virtual ~ioBraceletItem(void);
};

inline ioBraceletItem* ToBraceletItem( ioItem *pItem )
{
	if( !pItem || pItem->GetType() != ioItem::IT_BRACELET )
		return NULL;

	return dynamic_cast< ioBraceletItem* >( pItem );
}

inline const ioBraceletItem* ToBraceletItemConst( const ioItem *pItem )
{
	if( !pItem || pItem->GetType() != ioItem::IT_BRACELET )
		return NULL;

	return dynamic_cast< const ioBraceletItem* >( pItem );
}
