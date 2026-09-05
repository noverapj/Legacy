#pragma once

#include "ioItemRight.h"

class ioItemNShopRight : public ioItemRight
{
public:
	virtual void ExecuteFirstFunc( ioWnd *pWnd ) const;
	virtual void ExecuteSecondFunc( ioWnd *pWnd ) const;
	virtual RightType GetType() const;

public:
	ioItemNShopRight();
	virtual ~ioItemNShopRight();
};

inline ioItemNShopRight* ToItemNShopRight( ioItemRight *pRight )
{
	if( !pRight || pRight->GetType() != ioItemRight::RT_ITEM_SHOP )
		return NULL;

	return dynamic_cast< ioItemNShopRight* >( pRight );
}

