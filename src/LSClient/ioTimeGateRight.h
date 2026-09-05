#pragma once

#include "ioItemRight.h"

class ioTimeGateRight : public ioItemRight
{
public:
	virtual void ExecuteFirstFunc( ioWnd *pWnd ) const;
	virtual void ExecuteSecondFunc( ioWnd *pWnd ) const;
	virtual RightType GetType() const;

public:
	ioTimeGateRight();
	virtual ~ioTimeGateRight();
};

inline ioTimeGateRight* ToTimeGateRight( ioItemRight *pRight )
{
	if( !pRight || pRight->GetType() != ioItemRight::RT_TIME_GATE )
		return NULL;

	return dynamic_cast< ioTimeGateRight* >( pRight );
}

