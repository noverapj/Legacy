#pragma once

#include "ioItemRight.h"

class ioTrainingRight : public ioItemRight
{
public:
	virtual void ExecuteFirstFunc( ioWnd *pWnd ) const;
	virtual void ExecuteSecondFunc( ioWnd *pWnd ) const;
	virtual RightType GetType() const;

public:
	ioTrainingRight();
	virtual ~ioTrainingRight();
};

inline ioTrainingRight* ToTrainingRight( ioItemRight *pRight )
{
	if( !pRight || pRight->GetType() != ioItemRight::RT_TRAINING )
		return NULL;

	return dynamic_cast< ioTrainingRight* >( pRight );
}

