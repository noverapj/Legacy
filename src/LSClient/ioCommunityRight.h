#pragma once

#include "ioItemRight.h"

class ioCommunityRight : public ioItemRight
{
public:
	virtual void ExecuteFirstFunc( ioWnd *pWnd ) const;
	virtual void ExecuteSecondFunc( ioWnd *pWnd ) const;
	virtual RightType GetType() const;

public:
	ioCommunityRight();
	virtual ~ioCommunityRight();
};

inline ioCommunityRight* ToCommunityRight( ioItemRight *pRight )
{
	if( !pRight || pRight->GetType() != ioItemRight::RT_COMMUNITY )
		return NULL;

	return dynamic_cast< ioCommunityRight* >( pRight );
}

