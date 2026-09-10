#pragma once

#include "../io3DEngine/ioButton.h"
#include "../ioComplexStringPrinter.h"

class ioXMLElement;

class TwinkleButton : public ioButton
{
protected:
	DWORD m_dwScreenTimer;
	DWORD m_dwCurTimer;
	bool  m_bScreen;
	bool  m_bTwinkle;
	
protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnDrawNormal( int iXPos, int iYPos );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	void SetTwinkle(bool bTwinkle);

public:
	TwinkleButton();
	virtual ~TwinkleButton();
};

//////////////////////////////////////////////////////////////////////
class ComplexTitleBtn : public ioButton
{
protected:
	ioComplexStringPrinter m_TitlePrinter;
	TextAlignType m_TextHorzAlign;
	int           m_iXOffset;
	int           m_iYOffset;

protected:
	virtual void OnRender();

public:
	void SetComplexTitle( ioComplexStringPrinter &rkPrinter , TextAlignType eTextHorzAlign, int iXOffset, int iYOffset );

public:
	ComplexTitleBtn();
	virtual ~ComplexTitleBtn();
};

