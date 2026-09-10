#pragma once

#include "ioItemRight.h"

class ioINILoader;
class ioSetItemInfo;

class ioSetItemBuyRight : public ioItemRight
{
protected:
	DWORD m_dwSetItemCode;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual void ExecuteFirstFunc( ioWnd *pWnd ) const;
	virtual void ExecuteSecondFunc( ioWnd *pWnd ) const;
	virtual RightType GetType() const;
	
public:
	const ioSetItemInfo* GetSetItemInfo() const;
	inline DWORD GetSetItemCode() const { return m_dwSetItemCode; }

public:
	ioSetItemBuyRight();
	virtual ~ioSetItemBuyRight();
};

inline ioSetItemBuyRight* ToSetItemBuyRight( ioItemRight *pRight )
{
	if( !pRight || pRight->GetType() != ioItemRight::RT_SET_ITEM_BUY )
		return NULL;

	return dynamic_cast< ioSetItemBuyRight* >( pRight );
}

inline const ioSetItemBuyRight* ToSetItemBuyRightConst( const ioItemRight *pRight )
{
	if( !pRight || pRight->GetType() != ioItemRight::RT_SET_ITEM_BUY )
		return NULL;

	return dynamic_cast< const ioSetItemBuyRight* >( pRight );
}

