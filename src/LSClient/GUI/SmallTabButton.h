#pragma once

#include "../io3DEngine/ioWnd.h"

class SmallTabButton : public ioRadioButton
{
protected:
	bool m_bDsiable;

public:
	void SetDisable( bool bDisable );

public:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawOveredAdd( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

public:
	SmallTabButton();
	virtual ~SmallTabButton();
};