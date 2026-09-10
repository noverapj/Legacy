#pragma once

#include "ioItemRight.h"

class ioHideBloodRight : public ioItemRight
{
public:
	virtual void ExecuteFirstFunc( ioWnd *pWnd ) const;
	virtual void ExecuteSecondFunc( ioWnd *pWnd ) const;
	virtual RightType GetType() const;

public:
	ioHideBloodRight();
	virtual ~ioHideBloodRight();
};

inline ioHideBloodRight* ToHideBloodRight( ioItemRight *pRight )
{
	if( !pRight || pRight->GetType() != ioItemRight::RT_HIDE_BLOOD )
		return NULL;

	return dynamic_cast< ioHideBloodRight* >( pRight );
}

