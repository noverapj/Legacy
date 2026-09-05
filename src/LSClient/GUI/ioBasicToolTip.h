#pragma once

#include "../ioComplexStringPrinter.h"

class ioBasicToolTip : public ioWnd
{
public:
	virtual void iwm_show();	
	virtual void iwm_hide();

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	ioBasicToolTip();
	virtual ~ioBasicToolTip();
};
