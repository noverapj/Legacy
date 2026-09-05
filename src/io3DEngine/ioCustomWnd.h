//----------------------------------------------------------------------------------
/*
Filename	: ioCustomWnd.h
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#ifndef _IOCUSTOMWND_H_
#define _IOCUSTOMWND_H_

#include "ioWnd.h"
#include "ioXMLElement.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioCustomWnd : xml파일을 작업자가 직접 조작하였을경우 그 내용을 보장하기 위해 이 클래스가 필요하다.
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class __EX ioCustomWnd : public ioWnd
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioCustomWnd();
	virtual ~ioCustomWnd();

	//----------------------------------------------------------------------------------
	/*
	ioWnd override
	*/
	virtual void			ParseXML( ioXMLElement& xml );
	virtual bool			Save( ioXMLElement& xml );

	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( CustomWnd, ioCustomWnd )
	virtual ioWnd*			_Clone() { return new ioCustomWnd(); }

protected:

	ioXMLElement*	m_xml;
};


#endif // _IOCUSTOMWND_H_