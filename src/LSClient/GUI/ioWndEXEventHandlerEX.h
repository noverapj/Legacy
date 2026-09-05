//----------------------------------------------------------------------------------
/*
Filename	: ioWndEXEventHandlerEX.h
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#ifndef _IOWNDEXEVENTTYPEHANDLEREX_H_
#define _IOWNDEXEVENTTYPEHANDLEREX_H_

#include "ioWndEXEventHandler.h"

//----------------------------------------------------------------------------------
/*
ioWndEXEventHandlerEX
*/
template < typename T, typename EOT >
class ioWndEXEventHandlerEX : public ioWndEXEventHandlerBase
{
public:

	typedef void (T::*callback)( ioWndEX*, EOT& );

public:

	ioWndEXEventHandlerEX( T* owner, callback cb )
		: ioWndEXEventHandlerBase( owner )
		, m_owner( owner )
		, m_callback( cb )
	{}

	virtual ~ioWndEXEventHandlerEX()
	{}

	virtual void	Call( ioWndEX* wnd, ioWndEXEvent& event )
	{
		if ( m_owner && m_callback ) {
			(m_owner->*m_callback)( wnd, dynamic_cast<EOT&>( event ) );
		}
	}

protected:

	T*				m_owner;
	callback		m_callback;
};

//----------------------------------------------------------------------------------
/*
ioWndEXMouseEventHandlerEX
*/
template < typename T >
class ioWndEXMouseEventHandlerEX : public ioWndEXEventHandlerEX< T, ioWndEXMouseEvent >
{
public:

	ioWndEXMouseEventHandlerEX( T* owner, callback cb )
		: ioWndEXEventHandlerEX< T, ioWndEXMouseEvent >( owner, cb )
	{}

	virtual ~ioWndEXMouseEventHandlerEX() {}
};

//----------------------------------------------------------------------------------
/*
ioWndEXRadioEventHandlerEX
*/
template < typename T >
class ioWndEXRadioEventHandlerEX : public ioWndEXEventHandlerEX< T, ioWndEXRadioEvent >
{
public:

	ioWndEXRadioEventHandlerEX( T* owner, callback cb )
		: ioWndEXEventHandlerEX< T, ioWndEXRadioEvent >( owner, cb )
	{}

	virtual ~ioWndEXRadioEventHandlerEX() {}
};

//----------------------------------------------------------------------------------
/*
ioWndEXScrollBarEventHandlerEX
*/
template < typename T >
class ioWndEXScrollBarEventHandlerEX : public ioWndEXEventHandlerEX< T, ioWndEXScrollBarEvent >
{
public:

	ioWndEXScrollBarEventHandlerEX( T* owner, callback cb )
		: ioWndEXEventHandlerEX< T, ioWndEXScrollBarEvent >( owner, cb )
	{}

	virtual ~ioWndEXScrollBarEventHandlerEX() {}
};

#endif // _IOWNDEXEVENTTYPEHANDLEREX_H_